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
    // --- Estados del Juego  ---
    enum Estado { INICIALIZANDO, ESPERANDO_HUMANO, ANALIZANDO_IA, PENSANDO_JUGADA, MOVIENDO_ROBOT, FIN_JUEGO };
    Estado estado_actual_;
    bool first_time_;

    // --- ROS 2: Subs, Servs y Clients ---
    rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr color_sub_;
    rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr depth_sub_;
    rclcpp::Subscription<sensor_msgs::msg::CameraInfo>::SharedPtr info_sub_;
    
    rclcpp::Client<robot_interfaces::srv::SpawnObject>::SharedPtr spawn_client_;
    rclcpp::Client<std_srvs::srv::Trigger>::SharedPtr ia_client_;
    rclcpp::Client<robot_interfaces::srv::MoveJoint>::SharedPtr go_home_client_;
    rclcpp::Service<robot_interfaces::srv::PutPiece>::SharedPtr put_piece_service_;
    
    // --- Clientes de accion delmovieminto ---
    rclcpp_action::Client<nav2_msgs::action::NavigateToPose>::SharedPtr client_moveJ_;
    rclcpp_action::Client<nav2_msgs::action::NavigateToPose>::SharedPtr client_moveL_;

    rclcpp::TimerBase::SharedPtr game_timer_;
    std::unique_ptr<tf2_ros::Buffer> tf_buffer_;
    std::shared_ptr<tf2_ros::TransformListener> tf_listener_;

    // --- Datos de Sensores ---
    sensor_msgs::msg::Image last_color_image_;
    sensor_msgs::msg::Image last_depth_image_;
    sensor_msgs::msg::CameraInfo last_camera_info_;
    bool color_received_ = false, depth_received_ = false, info_received_ = false;

    // --- Lógica de Juego ---
    char tablero_[3][3];
    int cantidad_fichas_humanas_;
    int turnos_jugados_;
    rclcpp::Time tiempo_inicio_turno_;
    int ultimo_segundo_impreso_;
    std::vector<cv::Point2f> ultimas_esquinas_tablero_;

    // --- Funciones Principales ---
    void game_loop();
    void reiniciar_juego();
    void ia_response_callback(rclcpp::Client<std_srvs::srv::Trigger>::SharedFuture future);
    void handle_put_piece(const std::shared_ptr<robot_interfaces::srv::PutPiece::Request> request,
                          std::shared_ptr<robot_interfaces::srv::PutPiece::Response> response);
    
    // --- Lógica del Robot ---
    void ejecutar_turno_robot();
    char comprobar_ganador(char tab[3][3]);
    bool hay_empate(char tab[3][3]);
    int minimax(char tab[3][3], int profundidad, bool es_maximizador);
    std::pair<int, int> encontrar_mejor_movimiento();
    bool send_move_action(double x, double y, double z, std::string action_name);
    bool call_go_home();

    // --- Visión y Transformaciones ---
    Point3D get_pixel_xyz_in_robot_frame(int u, int v, const std::string& camera_frame, const std::string& robot_frame);
    cv::Mat extraer_tablero_robusto(cv::Mat& img);
    std::vector<cv::Mat> dividir_tablero(cv::Mat& img);
    std::vector<cv::Point2f> ordenar_puntos(const std::vector<cv::Point2f>& puntos);
    void imprimir_tablero_debug();

    // --- Callbacks Sensores ---
    void color_callback(const sensor_msgs::msg::Image::SharedPtr msg);
    void depth_callback(const sensor_msgs::msg::Image::SharedPtr msg);
    void info_callback(const sensor_msgs::msg::CameraInfo::SharedPtr msg);
};

#endif