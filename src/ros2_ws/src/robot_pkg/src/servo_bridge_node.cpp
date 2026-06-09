#include "robot_pkg/servo_bridge_node.hpp"

#include <chrono>
#include <string>
#include <cstring>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <vector> 
#include <cstdio> 
#include <sstream>

#include <tf2/LinearMath/Quaternion.h>
#include <tf2/LinearMath/Matrix3x3.h>

using std::placeholders::_1;
using namespace std::chrono_literals;

ServoBridgeNode::ServoBridgeNode()
: Node("servo_bridge_node"), serial_port_(-1)
{
  const char * port_name = "/dev/ttyACM0"; 
  
  serial_port_ = open(port_name, O_RDWR | O_NOCTTY | O_NDELAY);
  
  if (serial_port_ < 0) {
    RCLCPP_ERROR(this->get_logger(), "Fatal error: cannot open %s", port_name);
  } else {
    fcntl(serial_port_, F_SETFL, 0); 
    configure_serial(serial_port_);
    RCLCPP_INFO(this->get_logger(), "Arduino connected on %s", port_name);
  }

  this->declare_parameter("num_joints", 5);
  num_joints_ = this->get_parameter("num_joints").as_int();

  rclcpp::QoS qos_sub(1);
  qos_sub.transient_local();
  subscription_ = this->create_subscription<std_msgs::msg::Int16MultiArray>(
    "robot_cmd", 
    qos_sub, 
    std::bind(&ServoBridgeNode::robot_cmd_callback, this, _1)
  );
  
  rclcpp::QoS qos_profile(1); 
  qos_profile.transient_local();  
  qos_profile.reliable();        
  publisher_ = this->create_publisher<std_msgs::msg::Int16MultiArray>(
    "robot_cmd", 
    qos_profile
  );

  add_wall_client_ = this->create_client<robot_interfaces::srv::AddObstacle>("/add_wall");
  dk_client_ = this->create_client<robot_interfaces::srv::SolveDK>("/solve_dk");

  timer_ = this->create_wall_timer(
    20ms, std::bind(&ServoBridgeNode::timer_callback, this));

  RCLCPP_INFO(this->get_logger(), "Servo bridge working");
}

ServoBridgeNode::~ServoBridgeNode() {
  if (serial_port_ >= 0) close(serial_port_);
}

// ROS2 -> Arduino
void ServoBridgeNode::robot_cmd_callback(const std_msgs::msg::Int16MultiArray::SharedPtr msg)
{
  if (serial_port_ < 0) return;
  if (robot_mode != 1 ) return;

  if (msg->data.size() != num_joints_) { 
    RCLCPP_WARN(this->get_logger(), "Ignoring command: expected %zu angles", num_joints_);
    return; 
  }

  std::string command = "D{";
  for (size_t i = 0; i < msg->data.size(); ++i) {
    command += std::to_string(msg->data[i] + 90);
    if (i < msg->data.size() - 1) command += ","; 
  }
  
  command += "}\n";
  RCLCPP_INFO(this->get_logger(), "Sending angles: %s", command.c_str());

  write(serial_port_, command.c_str(), command.size());
}

// Arduino -> ROS2
void ServoBridgeNode::timer_callback()
{
  if (serial_port_ < 0) return;

  char buffer[256];
  int n = read(serial_port_, buffer, sizeof(buffer) - 1);

  if (n > 0) {
    buffer[n] = '\0'; 
    read_buffer_ += buffer; 

    if (read_buffer_.size() > 1000) {
      RCLCPP_WARN(this->get_logger(), "Buffer overflow! Discarding stale data.");
      read_buffer_.clear();
      return;
    }

    size_t pos = read_buffer_.find('\n');
    while (pos != std::string::npos) {
      std::string line = read_buffer_.substr(0, pos);
      read_buffer_.erase(0, pos + 1); 
      if (!line.empty() && line.back() == '\r') {
        line.pop_back();
      }

      if (line.find("MANUAL") != std::string::npos) {
        RCLCPP_INFO(this->get_logger(), "Arduino: MANUAL MODE ACTIVATED");
        robot_mode = 0;
      }
      else if (line.find("AUTO") != std::string::npos) {
        RCLCPP_INFO(this->get_logger(), "Arduino: AUTOMATIC MODE ACTIVATED");
        robot_mode = 1;
      }
      else if (robot_mode == 0) {
        
        bool is_wall = false;
        std::string data_str = line;

        // Parse WALL prefix if present
        if (line.find("WALL(") == 0) {
          is_wall = true;
          data_str = line.substr(5, line.length() - 6); 
        }
        data_str.erase(std::remove(data_str.begin(), data_str.end(), '('), data_str.end());
        data_str.erase(std::remove(data_str.begin(), data_str.end(), ')'), data_str.end());

        std::vector<int> v;
        std::stringstream ss(data_str);
        std::string token;

        while (std::getline(ss, token, ',')) {
          try {
            v.push_back(std::stoi(token) - 90);
          } catch (...) {
            RCLCPP_WARN(this->get_logger(), "Invalid token: '%s'", token.c_str());
            continue;
          }
        }

        std::stringstream debug;
        for (size_t i = 0; i < v.size(); i++) {
            debug << v[i];
            if (i < v.size() - 1) {
              debug << ", ";
            }
        }
        RCLCPP_INFO(this->get_logger(), "size: %zu | values: [%s]", v.size(), debug.str().c_str());

        if (v.size() == num_joints_) {
          if (is_wall) {
            RCLCPP_INFO(this->get_logger(), "Wall point received");
            add_wall_point(v); 
          }
          std_msgs::msg::Int16MultiArray msg_out;
          msg_out.data.assign(v.begin(), v.end());
          publisher_->publish(msg_out);
        } else {
          RCLCPP_WARN(this->get_logger(), "Unexpected format or joint count mismatch: %s", line.c_str());
        }
      }
      else {
        RCLCPP_INFO(this->get_logger(), "Arduino: UNKNOWN COMMAND");
      } 

      pos = read_buffer_.find('\n');
    }
  }
}


// Async wall-building logic
void ServoBridgeNode::add_wall_point(const std::vector<int>& v) { 
  
  std::string degrees_str = "";
  for (int val : v) {
    degrees_str += std::to_string(val) + " ";
  }
  RCLCPP_INFO(this->get_logger(), "Degrees received: [ %s]", degrees_str.c_str());

  // Convert degrees to radians
  std::vector<double> angles;
  std::string radians_str = "";
  for (int angle_deg : v) {
    double rad = angle_deg * M_PI / 180.0;
    angles.push_back(rad);
    radians_str += std::to_string(rad) + " ";
  }
  RCLCPP_INFO(this->get_logger(), "Radians sent to DK: [ %s]", radians_str.c_str());

  Tool_Config default_cfg;
  default_cfg.name = "default";
  default_cfg.type = 1; 
  default_cfg.dimensions = {0.01, 0.01}; 
  geometry_msgs::msg::Pose default_pose; 
  default_cfg.offset = default_pose;

  auto request_dk = std::make_shared<robot_interfaces::srv::SolveDK::Request>();
  request_dk->joint_angles = angles;
  request_dk->tcp_offset = default_cfg.offset;
  request_dk->tool_dimensions = default_cfg.dimensions;

  if (!dk_client_->wait_for_service(std::chrono::seconds(2))) {
    RCLCPP_ERROR(this->get_logger(), "DK service not available. Aborting wall.");
    return;
  }

  // Send async DK request
  dk_client_->async_send_request(request_dk, 
    [this](rclcpp::Client<robot_interfaces::srv::SolveDK>::SharedFuture future_dk) {
      
      auto response_dk = future_dk.get();

      if (response_dk && response_dk->success) {
        
        // Extract RPY from quaternion
        double roll, pitch, yaw;
        tf2::Quaternion q(
          response_dk->target_pose.orientation.x,
          response_dk->target_pose.orientation.y,
          response_dk->target_pose.orientation.z,
          response_dk->target_pose.orientation.w
        );
        tf2::Matrix3x3(q).getRPY(roll, pitch, yaw);

        RCLCPP_INFO(this->get_logger(), "DK result -> Position (X: %.3f, Y: %.3f, Z: %.3f)", 
                    response_dk->target_pose.position.x, 
                    response_dk->target_pose.position.y, 
                    response_dk->target_pose.position.z);
        RCLCPP_INFO(this->get_logger(), "          -> Rotation (Roll: %.3f, Pitch: %.3f, Yaw: %.3f)", 
                    roll, pitch, yaw);

        tf2::Vector3 new_point(
            response_dk->target_pose.position.x,
            response_dk->target_pose.position.y,
            response_dk->target_pose.position.z
        );

        current_wall_points_.push_back(new_point);
        RCLCPP_INFO(this->get_logger(), "Wall point %zu/3 registered.", current_wall_points_.size());

        // Check if 3 points collected
        if (current_wall_points_.size() == 3) {
            RCLCPP_INFO(this->get_logger(), "3 points collected, computing plane...");

            tf2::Vector3 p1 = current_wall_points_[0];
            tf2::Vector3 p2 = current_wall_points_[1];
            tf2::Vector3 p3 = current_wall_points_[2];

            // Compute plane vectors, normal and wall coordinate system
            tf2::Vector3 v1 = p2 - p1;
            tf2::Vector3 v2 = p3 - p1;
            tf2::Vector3 normal = v1.cross(v2).normalized();

            tf2::Vector3 x_axis = v1.normalized();
            tf2::Vector3 y_axis = normal.cross(x_axis).normalized();

            tf2::Matrix3x3 rot_matrix(
                x_axis.x(), y_axis.x(), normal.x(),
                x_axis.y(), y_axis.y(), normal.y(),
                x_axis.z(), y_axis.z(), normal.z()
            );

            double r_wall, p_wall, y_wall;
            rot_matrix.getRPY(r_wall, p_wall, y_wall);

            tf2::Vector3 center = (p1 + p2 + p3) / 3.0;

            // Build AddObstacle request
            auto request_wall = std::make_shared<robot_interfaces::srv::AddObstacle::Request>();
            request_wall->name = "pared_arduino_" + std::to_string(wall_count_);
            request_wall->x = center.x();
            request_wall->y = center.y();
            request_wall->z = center.z();
            request_wall->roll = r_wall;
            request_wall->pitch = p_wall;
            request_wall->yaw = y_wall;
            request_wall->width = 1.0;
            request_wall->height = 1.0;
            request_wall->depth = 0.025;

            RCLCPP_INFO(this->get_logger(), "Sending wall -> Center(X: %.3f, Y: %.3f, Z: %.3f) | RPY(R: %.3f, P: %.3f, Y: %.3f)", 
                        request_wall->x, request_wall->y, request_wall->z, 
                        request_wall->roll, request_wall->pitch, request_wall->yaw);

            if (!add_wall_client_->wait_for_service(std::chrono::seconds(2))) {
              RCLCPP_ERROR(this->get_logger(), "Service /add_wall not available.");
              current_wall_points_.clear();
              return;
            }

            add_wall_client_->async_send_request(request_wall, 
              [this](rclcpp::Client<robot_interfaces::srv::AddObstacle>::SharedFuture future_wall) {
                auto response = future_wall.get();
                if (response && response->success) {
                  RCLCPP_INFO(this->get_logger(), "Wall %d added via 3 points.", wall_count_);
                  wall_count_++; 
                } else {
                  RCLCPP_ERROR(this->get_logger(), "Failed to add wall in simulator.");
                }
              }
            );
            current_wall_points_.clear();
        }
      } else {
        RCLCPP_ERROR(this->get_logger(), "DK service response failed.");
      }
    }
  );
}

// Serial port configuration
void ServoBridgeNode::configure_serial(int fd) {
  struct termios tty;
  if(tcgetattr(fd, &tty) != 0) return;
  
  cfsetospeed(&tty, B57600);
  cfsetispeed(&tty, B57600);
  
  tty.c_cflag &= ~PARENB; 
  tty.c_cflag &= ~CSTOPB; 
  tty.c_cflag &= ~CSIZE;
  tty.c_cflag |= CS8;     
  tty.c_cflag |= CREAD | CLOCAL; 

  tty.c_lflag &= ~ICANON;
  tty.c_lflag &= ~ECHO;
  tty.c_lflag &= ~ECHOE;
  tty.c_lflag &= ~ISIG;
  
  tty.c_cc[VMIN]  = 0; 
  tty.c_cc[VTIME] = 0; 

  tcsetattr(fd, TCSANOW, &tty);
  
  int flags = fcntl(fd, F_GETFL, 0);
  fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

// --- MAIN ---
int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<ServoBridgeNode>());
  rclcpp::shutdown();
  return 0;
}
