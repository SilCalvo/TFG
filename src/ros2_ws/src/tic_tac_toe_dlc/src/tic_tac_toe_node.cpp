#include "tic_tac_toe_dlc/tic_tac_toe_node.hpp" 
#include <algorithm>
#include <opencv2/imgproc.hpp>

TicTacToeNode::TicTacToeNode() : Node("tic_tac_toe_node") {
    // 1. Inicialización de TF2
    tf_buffer_ = std::make_unique<tf2_ros::Buffer>(this->get_clock());
    tf_listener_ = std::make_shared<tf2_ros::TransformListener>(*tf_buffer_);

    // 2. Suscriptores de Sensores
    color_sub_ = this->create_subscription<sensor_msgs::msg::Image>(
        "/camera/camara_tablero/color/image_raw", 10, std::bind(&TicTacToeNode::color_callback, this, std::placeholders::_1));
    depth_sub_ = this->create_subscription<sensor_msgs::msg::Image>(
        "/camera/camara_tablero/depth/image_raw", 10, std::bind(&TicTacToeNode::depth_callback, this, std::placeholders::_1));
    info_sub_ = this->create_subscription<sensor_msgs::msg::CameraInfo>(
        "/camera/camara_tablero/camera_info", 10, std::bind(&TicTacToeNode::info_callback, this, std::placeholders::_1));
        
    // 3. Clientes y Servicios
    spawn_client_ = this->create_client<robot_interfaces::srv::SpawnObject>("/add_object");
    ia_client_ = this->create_client<std_srvs::srv::Trigger>("/analizar_tablero");

    put_piece_service_ = this->create_service<robot_interfaces::srv::PutPiece>(
        "put_piece_virtual", 
        std::bind(&TicTacToeNode::handle_put_piece, this, std::placeholders::_1, std::placeholders::_2));

    // CREACIÓN DE LOS DOS CLIENTES DE ACCIÓN PERMANENTES
    client_moveJ_ = rclcpp_action::create_client<nav2_msgs::action::NavigateToPose>(this, "moveJ");
    client_moveL_ = rclcpp_action::create_client<nav2_msgs::action::NavigateToPose>(this, "moveL");
    estado_actual_ = INICIALIZANDO;
    first_time_ = true;
    tiempo_inicio_turno_ = this->get_clock()->now();

    // 5. Timer principal
    game_timer_ = this->create_wall_timer(std::chrono::milliseconds(100), std::bind(&TicTacToeNode::game_loop, this));
    
    RCLCPP_INFO(this->get_logger(), ">>> Nodo Tic-Tac-Toe iniciado.");
}

void TicTacToeNode::reiniciar_juego() {
    for(int i=0; i<3; i++) {
        for(int j=0; j<3; j++) tablero_[i][j] = '-';
    }
    cantidad_fichas_humanas_ = 0;
    turnos_jugados_ = 0;
    ultimo_segundo_impreso_ = -1;
    tiempo_inicio_turno_ = this->get_clock()->now();
    RCLCPP_WARN(this->get_logger(), "=== JUEGO REINICIADO ===");
}

void TicTacToeNode::game_loop() {
    if (!color_received_ || !info_received_ || !depth_received_) return;

    switch (estado_actual_) {
        case INICIALIZANDO :{
            if (first_time_) {
                reiniciar_juego();
                first_time_=false;
            } else if ((this->get_clock()->now() - tiempo_inicio_turno_ ).seconds() > 5){
                estado_actual_ = ESPERANDO_HUMANO;
                first_time_ = true;
                tiempo_inicio_turno_ = this->get_clock()->now();
            }
            break;
        }
        case ESPERANDO_HUMANO: {
            auto ahora = this->get_clock()->now();
            double segs = (ahora - tiempo_inicio_turno_).seconds();
            int resta = 15 - static_cast<int>(segs);

            if (resta >= 0 && resta % 5 == 0 and resta!= ultimo_segundo_impreso_) {
                RCLCPP_INFO(this->get_logger(), ">>> TURNO HUMANO: %ds", resta);
                ultimo_segundo_impreso_ = resta;
            }

            if (segs >= 7.0) {
                RCLCPP_INFO(this->get_logger(), "Analizando tablero...");
                
                cv::Mat img_rgba(last_color_image_.height, last_color_image_.width, CV_8UC4, 
                                const_cast<unsigned char*>(last_color_image_.data.data()), last_color_image_.step);
                cv::Mat img_opencv;
                cv::cvtColor(img_rgba, img_opencv, cv::COLOR_RGBA2BGR);

                cv::Mat tablero_img = extraer_tablero_robusto(img_opencv);
                
                if (tablero_img.empty()) {
                    RCLCPP_WARN(this->get_logger(), "Tablero no detectado. Reintentando...");
                    tiempo_inicio_turno_ = this->get_clock()->now();
                    ultimo_segundo_impreso_ = -1;
                    return;
                }
                
                std::vector<cv::Mat> piezas = dividir_tablero(tablero_img);
                for (size_t i = 0; i < piezas.size(); ++i) {
                    std::string filename = "/tmp/casilla_" + std::to_string(i) + ".jpg";
                    cv::imwrite(filename, piezas[i]);
                }
                
                auto req = std::make_shared<std_srvs::srv::Trigger::Request>();
                ia_client_->async_send_request(req, std::bind(&TicTacToeNode::ia_response_callback, this, std::placeholders::_1));
                estado_actual_ = ANALIZANDO_IA;
            }
            break;
        }
        case ANALIZANDO_IA: 
            break; 
            
        case PENSANDO_JUGADA: {
            // Cambiamos estado para que este bloque no se repita
            estado_actual_ = MOVIENDO_ROBOT; 

            // Lanzamos el turno del robot en un hilo independiente
            std::thread(&TicTacToeNode::ejecutar_turno_robot, this).detach();
            break;
        }
        case MOVIENDO_ROBOT:
            // Esperando pacientemente en segundo plano...
            break;

        case FIN_JUEGO: 
            break;
    }
}

void TicTacToeNode::handle_put_piece(const std::shared_ptr<robot_interfaces::srv::PutPiece::Request> request,
                                     std::shared_ptr<robot_interfaces::srv::PutPiece::Response> response) {
    int f = request->fila, c = request->columna;
    if (f<0 || f>2 || c<0 || c>2 || tablero_[f][c] != '-' || estado_actual_ != ESPERANDO_HUMANO) {
        response->success = false; response->message = "Movimiento invalido."; return;
    }

    if (ultimas_esquinas_tablero_.size() == 4) {
        float ur = (c + 0.5f) / 3.0f, vr = (f + 0.5f) / 3.0f;
        cv::Point2f p0 = ultimas_esquinas_tablero_[0], p1 = ultimas_esquinas_tablero_[1];
        cv::Point2f p2 = ultimas_esquinas_tablero_[2], p3 = ultimas_esquinas_tablero_[3];
        cv::Point2f cp = (1-ur)*(1-vr)*p0 + ur*(1-vr)*p1 + ur*vr*p2 + (1-ur)*vr*p3;

        Point3D pm = get_pixel_xyz_in_robot_frame(cp.x, cp.y, "camera_camara_tablero_optical_frame", "base");

        auto spawn_piece = std::make_shared<robot_interfaces::srv::SpawnObject::Request>();
        spawn_piece->name = "ficha_o_" + std::to_string(turnos_jugados_);
        spawn_piece->package_name = "tic_tac_toe_dlc"; 
        spawn_piece->piece_type = "ficha_o";
        spawn_piece->x = pm.x; spawn_piece->y = pm.y; spawn_piece->z = pm.z + 0.002;
        spawn_client_->async_send_request(spawn_piece);

        response->success = true;
    } else { 
        response->success = false; response->message = "Camara no lista."; 
    }
}

void TicTacToeNode::ia_response_callback(rclcpp::Client<std_srvs::srv::Trigger>::SharedFuture future) {
    auto res = future.get();
    std::string s = res->message; 
    int nuevas = 0; char temp[3][3]; int idx = 0;
    for (int i=0; i<3; i++) {
        for (int j=0; j<3; j++) {
            char c = s[idx*2]; 
            temp[i][j] = c;
            if (c == 'o') nuevas++;
            idx++; 
        }
    }
    if (nuevas > cantidad_fichas_humanas_) {
        RCLCPP_INFO(this->get_logger(), "¡O detectada!");
        cantidad_fichas_humanas_ = nuevas;
        for(int i=0; i<3; i++) for(int j=0; j<3; j++) tablero_[i][j] = temp[i][j];
        turnos_jugados_++;
        estado_actual_ = PENSANDO_JUGADA;
        imprimir_tablero_debug();
    } else {
        estado_actual_ = ESPERANDO_HUMANO; 
        tiempo_inicio_turno_ = this->get_clock()->now(); 
        ultimo_segundo_impreso_ = -1;
        imprimir_tablero_debug();
    }
}

bool TicTacToeNode::send_move_action(double x, double y, double z, std::string action_name) {
    rclcpp_action::Client<nav2_msgs::action::NavigateToPose>::SharedPtr client;
    
    // Seleccionamos el cliente correcto pre-creado
    if (action_name == "moveJ") client = client_moveJ_;
    else if (action_name == "moveL") client = client_moveL_;
    else return false;

    if (!client->wait_for_action_server(std::chrono::seconds(5))) {
        RCLCPP_ERROR(this->get_logger(), "Servidor %s no disponible", action_name.c_str());
        return false;
    }

    auto goal_msg = nav2_msgs::action::NavigateToPose::Goal();
    goal_msg.pose.header.frame_id = "base";
    goal_msg.pose.header.stamp = this->now();
    goal_msg.pose.pose.position.x = x;
    goal_msg.pose.pose.position.y = y;
    goal_msg.pose.pose.position.z = z;

    goal_msg.pose.pose.orientation.x = 0.0;
    goal_msg.pose.pose.orientation.y = 1.0; 
    goal_msg.pose.pose.orientation.z = 0.0;
    goal_msg.pose.pose.orientation.w = 0.0; 

    auto send_goal_future = client->async_send_goal(goal_msg);
    
    // Al usar .get(), el hilo actual se detiene hasta recibir respuesta
    auto goal_handle = send_goal_future.get();
    if (!goal_handle) {
        RCLCPP_ERROR(this->get_logger(), "Meta %s rechazada por el servidor", action_name.c_str());
        return false;
    }

    // El hilo actual se detiene hasta que el brazo termine físicamente el movimiento
    auto result_future = client->async_get_result(goal_handle);
    auto result = result_future.get();
    
    return result.code == rclcpp_action::ResultCode::SUCCEEDED;
}

void TicTacToeNode::ejecutar_turno_robot() {
    auto mov = encontrar_mejor_movimiento();
    int f = mov.first, c = mov.second;
    if (f == -1) {
        estado_actual_ = FIN_JUEGO;
        return;
    }
    
    tablero_[f][c] = 'x'; 
    turnos_jugados_++;
    
    float ur = (c + 0.5f) / 3.0f, vr = (f + 0.5f) / 3.0f;
    cv::Point2f cp = (1-ur)*(1-vr)*ultimas_esquinas_tablero_[0] + ur*(1-vr)*ultimas_esquinas_tablero_[1] + 
                     ur*vr*ultimas_esquinas_tablero_[2] + (1-ur)*vr*ultimas_esquinas_tablero_[3];
    
    Point3D pm = get_pixel_xyz_in_robot_frame(cp.x, cp.y, "camera_camara_tablero_optical_frame", "base");

    // Lógica con comprobación de errores. Si alguno falla, reinicia todo de cero.
    RCLCPP_INFO(this->get_logger(), "Moviendo brazo a aproximación...");
    if (!send_move_action(pm.x, pm.y, pm.z + 0.1, "moveJ")) {
        RCLCPP_ERROR(this->get_logger(), "Error físico al ir al punto de aproximación. Abortando turno.");
        estado_actual_ = INICIALIZANDO;

        return;
    }
    
    RCLCPP_INFO(this->get_logger(), "Bajando al tablero...");
    if (!send_move_action(pm.x, pm.y, pm.z, "moveL")) {
        RCLCPP_ERROR(this->get_logger(), "Error físico al bajar. Abortando turno.");
        estado_actual_ = INICIALIZANDO;
        return;
    }

    auto spawn_piece = std::make_shared<robot_interfaces::srv::SpawnObject::Request>();
    spawn_piece->name = "ficha_x_" + std::to_string(turnos_jugados_);
    spawn_piece->package_name = "tic_tac_toe_dlc"; 
    spawn_piece->piece_type = "ficha_x";
    spawn_piece->x = pm.x; spawn_piece->y = pm.y; spawn_piece->z = pm.z + 0.002;
    spawn_client_->async_send_request(spawn_piece);

    RCLCPP_INFO(this->get_logger(), "Retirando brazo...");
    if (!send_move_action(pm.x, pm.y, pm.z + 0.1, "moveL")) {
        RCLCPP_ERROR(this->get_logger(), "Error físico al retirar el brazo. Abortando turno.");
        estado_actual_ = INICIALIZANDO;
        return;
    }

    imprimir_tablero_debug();

    // Comprobamos resultados al final del turno
    char g = comprobar_ganador(tablero_);
    if (g != '-') { 
        RCLCPP_INFO(this->get_logger(), "GANADOR: %c", g); 
        estado_actual_ = FIN_JUEGO; 
    } else if (hay_empate(tablero_)) { 
        RCLCPP_INFO(this->get_logger(), "EMPATE"); 
        estado_actual_ = FIN_JUEGO; 
    } else { 
        // Si sigue el juego, devolvemos el turno al humano
        tiempo_inicio_turno_ = this->get_clock()->now(); 
        ultimo_segundo_impreso_ = -1;
        estado_actual_ = ESPERANDO_HUMANO; 
    }
}

int TicTacToeNode::minimax(char tab[3][3], int prof, bool isMax) {
    char g = comprobar_ganador(tab);
    if (g == 'x') return 10 - prof;
    if (g == 'o') return -10 + prof;
    if (hay_empate(tab)) return 0;

    int mejor = isMax ? -1000 : 1000;
    for(int i=0; i<3; i++) {
        for(int j=0; j<3; j++) {
            if(tab[i][j] == '-') {
                tab[i][j] = isMax ? 'x' : 'o';
                int val = minimax(tab, prof + 1, !isMax);
                tab[i][j] = '-';
                mejor = isMax ? std::max(mejor, val) : std::min(mejor, val);
            }
        }
    }
    return mejor;
}

std::pair<int, int> TicTacToeNode::encontrar_mejor_movimiento() {
    int mejorVal = -1000; std::pair<int, int> mov = {-1, -1};
    for(int i=0; i<3; i++) {
        for(int j=0; j<3; j++) {
            if(tablero_[i][j] == '-') {
                tablero_[i][j] = 'x'; int val = minimax(tablero_, 0, false); tablero_[i][j] = '-';
                if(val > mejorVal) { mejorVal = val; mov = {i, j}; }
            }
        }
    }
    return mov;
}

char TicTacToeNode::comprobar_ganador(char t[3][3]) {
    for(int i=0; i<3; i++) {
        if(t[i][0]==t[i][1] && t[i][1]==t[i][2] && t[i][0]!='-') return t[i][0];
        if(t[0][i]==t[1][i] && t[1][i]==t[2][i] && t[0][i]!='-') return t[0][i];
    }
    if(t[0][0]==t[1][1] && t[1][1]==t[2][2] && t[0][0]!='-') return t[0][0];
    if(t[0][2]==t[1][1] && t[1][1]==t[2][0] && t[0][2]!='-') return t[0][2];
    return '-';
}

bool TicTacToeNode::hay_empate(char t[3][3]) {
    for(int i=0; i<3; i++) for(int j=0; j<3; j++) if(t[i][j]=='-') return false;
    return true;
}

Point3D TicTacToeNode::get_pixel_xyz_in_robot_frame(int u, int v, const std::string& cam_f, const std::string& rob_f) {
    if (u < 0 || u >= (int)last_depth_image_.width || v < 0 || v >= (int)last_depth_image_.height) return {0,0,0};
    int idx = (v * last_depth_image_.step) + (u * sizeof(float));
    float d = *reinterpret_cast<const float*>(&last_depth_image_.data[idx]);
    
    Point3D p_c;
    p_c.z = d;
    p_c.x = (u - last_camera_info_.k[2]) * d / last_camera_info_.k[0];
    p_c.y = (v - last_camera_info_.k[5]) * d / last_camera_info_.k[4];
    
    geometry_msgs::msg::PointStamped p_in, p_out;
    p_in.header.frame_id = cam_f; p_in.header.stamp = this->get_clock()->now();
    p_in.point.x = p_c.x; p_in.point.y = p_c.y; p_in.point.z = p_c.z;
    try { 
        p_out = tf_buffer_->transform(p_in, rob_f, tf2::durationFromSec(1.0)); 
    }
    catch (const tf2::TransformException & ex) {
         return {0,0,0}; 
    }
    return {p_out.point.x, p_out.point.y, p_out.point.z};
}

cv::Mat TicTacToeNode::extraer_tablero_robusto(cv::Mat& img) {
    cv::Mat gris, bordes; cv::cvtColor(img, gris, cv::COLOR_BGR2GRAY);
    cv::GaussianBlur(gris, gris, cv::Size(5,5), 0);
    cv::Canny(gris, bordes, 50, 150);
    std::vector<std::vector<cv::Point>> conts;
    cv::findContours(bordes, conts, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    std::sort(conts.begin(), conts.end(), [](auto& a, auto& b){ return cv::contourArea(a) > cv::contourArea(b); });
    if(conts.empty() || cv::contourArea(conts[0]) < 2000) return cv::Mat();
    std::vector<cv::Point> approx;
    cv::approxPolyDP(conts[0], approx, 0.02 * cv::arcLength(conts[0], true), true);
    if(approx.size() != 4) return cv::Mat();
    std::vector<cv::Point2f> pts; for(auto p : approx) pts.push_back(cv::Point2f(p.x, p.y));
    ultimas_esquinas_tablero_ = ordenar_puntos(pts);
    cv::Mat M = cv::getPerspectiveTransform(ultimas_esquinas_tablero_, std::vector<cv::Point2f>{ {0,0},{449,0},{449,449},{0,449} });
    cv::Mat out; cv::warpPerspective(img, out, M, cv::Size(450,450));
    return out;
}

std::vector<cv::Point2f> TicTacToeNode::ordenar_puntos(const std::vector<cv::Point2f>& p) {
    std::vector<cv::Point2f> r(4); std::vector<cv::Point2f> pts = p;
    std::sort(pts.begin(), pts.end(), [](const cv::Point2f& a, const cv::Point2f& b) { return a.y < b.y; });
    r[0] = (pts[0].x < pts[1].x) ? pts[0] : pts[1]; 
    r[1] = (pts[0].x < pts[1].x) ? pts[1] : pts[0]; 
    r[2] = (pts[2].x > pts[3].x) ? pts[2] : pts[3]; 
    r[3] = (pts[2].x > pts[3].x) ? pts[3] : pts[2]; 
    return r;
}

std::vector<cv::Mat> TicTacToeNode::dividir_tablero(cv::Mat& img) {
    std::vector<cv::Mat> piezas; int sy = img.rows/3, sx = img.cols/3;
    for(int i=0; i<3; i++) for(int j=0; j<3; j++) piezas.push_back(img(cv::Rect(j*sx, i*sy, sx, sy)).clone());
    return piezas;
}

void TicTacToeNode::imprimir_tablero_debug() {
    std::string b = "\n  0 1 2\n";
    for(int i=0; i<3; i++) {
        b += std::to_string(i) + " ";
        for(int j=0; j<3; j++) { b += tablero_[i][j]; b += "  "; }
        b += "\n";
    }
    RCLCPP_INFO(this->get_logger(), "%s", b.c_str());
}

void TicTacToeNode::info_callback(const sensor_msgs::msg::CameraInfo::SharedPtr msg) { last_camera_info_ = *msg; info_received_ = true; }
void TicTacToeNode::depth_callback(const sensor_msgs::msg::Image::SharedPtr msg) { last_depth_image_ = *msg; depth_received_ = true; }
void TicTacToeNode::color_callback(const sensor_msgs::msg::Image::SharedPtr msg) { last_color_image_ = *msg; color_received_ = true; }

int main(int argc, char **argv) {
    rclcpp::init(argc, argv);
    auto node = std::make_shared<TicTacToeNode>();
    
    // Al usar std::thread para la lógica, el main vuelve a ser el tradicional y sencillo
    rclcpp::spin(node);
    
    rclcpp::shutdown();
    return 0;
}