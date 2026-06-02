#ifndef TIC_TAC_TOE_NODE_HPP_
#define TIC_TAC_TOE_NODE_HPP_

#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/image.hpp>
#include <sensor_msgs/msg/camera_info.hpp>
#include <geometry_msgs/msg/point_stamped.hpp>
#include <std_srvs/srv/trigger.hpp>
#include <tf2_ros/transform_listener.h>
#include <tf2_ros/buffer.h>
#include <tf2_geometry_msgs/tf2_geometry_msgs.hpp>
#include <opencv2/opencv.hpp>
#include <vector>
#include <string>
#include <memory>
#include <chrono>
#include <thread>

#include "rclcpp_action/rclcpp_action.hpp"
#include "nav2_msgs/action/navigate_to_pose.hpp"

#include "robot_interfaces/srv/spawn_object.hpp"
#include "robot_interfaces/srv/put_piece.hpp"
#include "robot_interfaces/srv/move_joint.hpp"

struct Point3D {
    double x, y, z;
};

class TicTacToeNode : public rclcpp::Node {
public:
    TicTacToeNode();

private:
    // --- Game States ---
    enum State { INITIALIZING, WAITING_FOR_HUMAN, ANALYZING_AI, THINKING_MOVE, MOVING_ROBOT, GAME_OVER };
    State current_state_;
    bool first_time_;

    // --- ROS 2: Subscriptions, Services and Clients ---
    rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr color_sub_;
    rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr depth_sub_;
    rclcpp::Subscription<sensor_msgs::msg::CameraInfo>::SharedPtr info_sub_;
    
    rclcpp::Client<robot_interfaces::srv::SpawnObject>::SharedPtr spawn_client_;
    rclcpp::Client<std_srvs::srv::Trigger>::SharedPtr ia_client_;
    rclcpp::Client<robot_interfaces::srv::MoveJoint>::SharedPtr go_home_client_;
    rclcpp::Service<robot_interfaces::srv::PutPiece>::SharedPtr put_piece_service_;
    
    // --- Movement action clients ---
    rclcpp_action::Client<nav2_msgs::action::NavigateToPose>::SharedPtr client_moveJ_;
    rclcpp_action::Client<nav2_msgs::action::NavigateToPose>::SharedPtr client_moveL_;

    rclcpp::TimerBase::SharedPtr game_timer_;
    std::unique_ptr<tf2_ros::Buffer> tf_buffer_;
    std::shared_ptr<tf2_ros::TransformListener> tf_listener_;

    // --- Sensor Data ---
    sensor_msgs::msg::Image last_color_image_;
    sensor_msgs::msg::Image last_depth_image_;
    sensor_msgs::msg::CameraInfo last_camera_info_;
    bool color_received_ = false, depth_received_ = false, info_received_ = false;

    // --- Game Logic ---
    char board_[3][3];
    int human_piece_count_;
    int turns_played_;
    rclcpp::Time turn_start_time_;
    int last_second_printed_;
    std::vector<cv::Point2f> last_board_corners_;

    // --- Main Functions ---
    void game_loop();
    void reset_game();
    void ai_response_callback(rclcpp::Client<std_srvs::srv::Trigger>::SharedFuture future);
    void handle_put_piece(const std::shared_ptr<robot_interfaces::srv::PutPiece::Request> request,
                          std::shared_ptr<robot_interfaces::srv::PutPiece::Response> response);
    
    // --- Robot Logic ---
    void execute_robot_turn();
    char check_winner(char tab[3][3]);
    bool is_draw(char tab[3][3]);
    int minimax(char tab[3][3], int depth, bool is_maximizer);
    std::pair<int, int> find_best_move();
    bool send_move_action(double x, double y, double z, std::string action_name);
    bool call_go_home();

    // --- Vision and Transforms ---
    Point3D get_pixel_xyz_in_robot_frame(int u, int v, const std::string& camera_frame, const std::string& robot_frame);
    cv::Mat extract_board_robust(cv::Mat& img);
    std::vector<cv::Mat> split_board(cv::Mat& img);
    std::vector<cv::Point2f> sort_points(const std::vector<cv::Point2f>& points);
    void print_board_debug();

    // --- Sensor Callbacks ---
    void color_callback(const sensor_msgs::msg::Image::SharedPtr msg);
    void depth_callback(const sensor_msgs::msg::Image::SharedPtr msg);
    void info_callback(const sensor_msgs::msg::CameraInfo::SharedPtr msg);
};

#endif