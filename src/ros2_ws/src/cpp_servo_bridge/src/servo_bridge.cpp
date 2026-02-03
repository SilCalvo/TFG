#include "cpp_servo_bridge/servo_bridge.hpp"

#include <chrono>
#include <string>
#include <cstring>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <vector> 

using std::placeholders::_1;

// --- CONSTRUCTOR ---
ServoBridgeNode::ServoBridgeNode()
: Node("cpp_servo_bridge"), serial_port_(-1)
{
  const char * port_name = "/dev/ttyACM0"; 
  serial_port_ = open(port_name, O_RDWR);
  
  if (serial_port_ < 0) {
    RCLCPP_ERROR(this->get_logger(), "Error abriendo %s", port_name);
  } else {
    configure_serial(serial_port_);
    RCLCPP_INFO(this->get_logger(), "Conexión Serial OK");
  }

  subscription_ = this->create_subscription<std_msgs::msg::Int16MultiArray>(
    "robot_cmd", 10, std::bind(&ServoBridgeNode::robot_cmd_callback, this, _1));
}

// --- DESTRUCTOR ---
ServoBridgeNode::~ServoBridgeNode() {
  if (serial_port_ >= 0) close(serial_port_);
}

// --- CALLBACK  ---
void ServoBridgeNode::robot_cmd_callback(const std_msgs::msg::Int16MultiArray::SharedPtr msg)
{
  if (serial_port_ < 0) return;

  // Verificar que el mensaje traiga datos
  if (msg->data.size() != 5) {
    RCLCPP_WARN(this->get_logger(), "Se esperaban 5 valores, recibidos: %zu", msg->data.size());
    return; 
  }

  // Construir la cadena "val1,val2,val3,val4,val5\n"
  std::string command = "";
  for (size_t i = 0; i < msg->data.size(); ++i) {
    command += std::to_string(msg->data[i]);
    if (i < msg->data.size() - 1) {
      command += ","; 
    }
  }
  command += "\n"; 

  RCLCPP_INFO(this->get_logger(), "Enviando: %s", command.c_str()); 

  // Enviar por serial
  write(serial_port_, command.c_str(), command.size());
}


// --- CONFIGURACIÓN TÉCNICA SERIAL (LINUX/WSL) ---
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
    tcsetattr(fd, TCSANOW, &tty);
}

// --- MAIN ---
int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<ServoBridgeNode>());
  rclcpp::shutdown();
  return 0;
}