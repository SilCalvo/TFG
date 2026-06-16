#include "tic_tac_toe_dlc/tic_tac_toe_node.hpp" 
#include <algorithm>
#include <opencv2/imgproc.hpp>

TicTacToeNode::TicTacToeNode() : Node("tic_tac_toe_node") {
    // TF2 initialization
    tf_buffer_ = std::make_unique<tf2_ros::Buffer>(this->get_clock());
    tf_listener_ = std::make_shared<tf2_ros::TransformListener>(*tf_buffer_);

    // Sensor subscribers
    color_sub_ = this->create_subscription<sensor_msgs::msg::Image>(
        "/camera/camara_tablero/color/image_raw", 10, std::bind(&TicTacToeNode::color_callback, this, std::placeholders::_1));
    depth_sub_ = this->create_subscription<sensor_msgs::msg::Image>(
        "/camera/camara_tablero/depth/image_raw", 10, std::bind(&TicTacToeNode::depth_callback, this, std::placeholders::_1));
    info_sub_ = this->create_subscription<sensor_msgs::msg::CameraInfo>(
        "/camera/camara_tablero/camera_info", 10, std::bind(&TicTacToeNode::info_callback, this, std::placeholders::_1));
        
    // Clients and services
    spawn_client_ = this->create_client<robot_interfaces::srv::SpawnObject>("/add_object");
    ia_client_ = this->create_client<std_srvs::srv::Trigger>("/analizar_tablero");

    put_piece_service_ = this->create_service<robot_interfaces::srv::PutPiece>(
        "put_piece_virtual", 
        std::bind(&TicTacToeNode::handle_put_piece, this, std::placeholders::_1, std::placeholders::_2));

    // Create the two action clients
    client_moveJ_ = rclcpp_action::create_client<nav2_msgs::action::NavigateToPose>(this, "moveJ");
    client_moveL_ = rclcpp_action::create_client<nav2_msgs::action::NavigateToPose>(this, "moveL");
    // Initialize state variables and start game timer
    current_state_ = INITIALIZING;
    first_time_ = true;
    turn_start_time_ = this->get_clock()->now();

    // Main timer
    game_timer_ = this->create_wall_timer(std::chrono::milliseconds(100), std::bind(&TicTacToeNode::game_loop, this));
    
    RCLCPP_INFO(this->get_logger(), ">>> Tic-Tac-Toe node started.");
}

void TicTacToeNode::reset_game() {
    // Clear all board cells
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            board_[i][j] = '-';
        }
    }
    human_piece_count_ = 0;
    turns_played_ = 0;
    last_second_printed_ = -1;
    turn_start_time_ = this->get_clock()->now();
    RCLCPP_WARN(this->get_logger(), "=== GAME RESET ===");
}

void TicTacToeNode::game_loop() {
    if (!color_received_ || !info_received_ || !depth_received_) return;

    switch (current_state_) {
        case INITIALIZING :{
            if (first_time_) {
                reset_game();
                first_time_=false;
            } else if ((this->get_clock()->now() - turn_start_time_ ).seconds() > 5){
                current_state_ = WAITING_FOR_HUMAN;
                first_time_ = true;
                turn_start_time_ = this->get_clock()->now();
            }
            break;
        }
        case WAITING_FOR_HUMAN: {
            auto now = this->get_clock()->now();
            double elapsed = (now - turn_start_time_).seconds();
            int remaining = 15 - static_cast<int>(elapsed);

            // Log remaining time every 5 seconds
            if (remaining >= 0 && remaining % 5 == 0 && remaining != last_second_printed_) {
                RCLCPP_INFO(this->get_logger(), ">>> HUMAN TURN: %ds", remaining);
                last_second_printed_ = remaining;
            }

            // Analyze the board after 7 seconds of human turn
            if (elapsed >= 7.0) {
                RCLCPP_INFO(this->get_logger(), "Analyzing board...");
                
                // Convert ROS RGBA image to OpenCV BGR
                cv::Mat img_rgba(last_color_image_.height, last_color_image_.width, CV_8UC4, 
                                const_cast<unsigned char*>(last_color_image_.data.data()), last_color_image_.step);
                cv::Mat img_opencv;
                cv::cvtColor(img_rgba, img_opencv, cv::COLOR_RGBA2BGR);

                cv::Mat board_img = extract_board_robust(img_opencv);
                
                if (board_img.empty()) {
                    RCLCPP_WARN(this->get_logger(), "Board not detected. Retrying...");
                    turn_start_time_ = this->get_clock()->now();
                    last_second_printed_ = -1;
                    return;
                }
                
                // Save each cell image to disk for AI
                std::vector<cv::Mat> pieces = split_board(board_img);
                for (size_t i = 0; i < pieces.size(); ++i) {
                    std::string filename = "/tmp/casilla_" + std::to_string(i) + ".jpg";
                    cv::imwrite(filename, pieces[i]);
                }
                
                // Send analysis request to AI service
                auto req = std::make_shared<std_srvs::srv::Trigger::Request>();
                ia_client_->async_send_request(req, std::bind(&TicTacToeNode::ai_response_callback, this, std::placeholders::_1));
                current_state_ = ANALYZING_AI;
            }
            break;
        }
        case ANALYZING_AI: 
            // Waiting in the background
            break; 
            
        case THINKING_MOVE: {
            // Change state to prevent this block from repeating
            current_state_ = MOVING_ROBOT; 

            // Launch robot turn in an independent thread
            std::thread(&TicTacToeNode::execute_robot_turn, this).detach();
            break;
        }
        case MOVING_ROBOT:
            // Waiting in the background
            break;

        case GAME_OVER: 
            break;
    }
}

void TicTacToeNode::handle_put_piece(const std::shared_ptr<robot_interfaces::srv::PutPiece::Request> request,
                                     std::shared_ptr<robot_interfaces::srv::PutPiece::Response> response) {

    // Put piece service callback: receives the human move, checks its validity, spawns the piece and updates the board state
    RCLCPP_INFO(this->get_logger(), "Put piece service");
    int f = request->fila, c = request->columna;
    if (f<0 || f>2 || c<0 || c>2 || board_[f][c] != '-' || current_state_ != WAITING_FOR_HUMAN) {
        response->success = false;
        response->message = "Invalid move.";
        return;
    }

    // Compute pixel center of the chosen cell via bilinear interpolation
    if (last_board_corners_.size() == 4) {
        float ur = (c + 0.5f) / 3.0f, vr = (f + 0.5f) / 3.0f;
        cv::Point2f p0 = last_board_corners_[0], p1 = last_board_corners_[1];
        cv::Point2f p2 = last_board_corners_[2], p3 = last_board_corners_[3];
        cv::Point2f cp = (1-ur)*(1-vr)*p0 + ur*(1-vr)*p1 + ur*vr*p2 + (1-ur)*vr*p3;

        Point3D pm = get_pixel_xyz_in_robot_frame(cp.x, cp.y, "camera_camara_tablero_optical_frame", "base");

        // Build and send piece spawn request
        auto spawn_piece = std::make_shared<robot_interfaces::srv::SpawnObject::Request>();
        spawn_piece->name = "ficha_o_" + std::to_string(turns_played_);
        spawn_piece->package_name = "tic_tac_toe_dlc"; 
        spawn_piece->piece_type = "ficha_o";
        spawn_piece->x = pm.x;
        spawn_piece->y = pm.y;
        spawn_piece->z = pm.z + 0.002;
        spawn_client_->async_send_request(spawn_piece);
        RCLCPP_INFO(this->get_logger(), "Piece: x: %f, y: %f, z: %f", pm.x, pm.y, pm.z + 0.002);

        response->success = true;
    } else {
        response->success = false;
        response->message = "Camera not ready.";
    }
}

void TicTacToeNode::ai_response_callback(rclcpp::Client<std_srvs::srv::Trigger>::SharedFuture future) {
    // Parse AI response string into a temporary board
    auto res = future.get();
    std::string s = res->message; 
    int new_count = 0; 
    char temp[3][3]; 
    int idx = 0;

    for (int i=0; i<3; i++) {
        for (int j=0; j<3; j++) {
            char c = s[idx*2]; 
            temp[i][j] = c;
            if (c == 'o') new_count++;
            idx++; 
        }
    }
    // Check if the count of 'o' pieces has increased, indicating a valid human move
    if (new_count > human_piece_count_) {
        RCLCPP_INFO(this->get_logger(), "O detected!");
        // Copy detected board into the game state
        human_piece_count_ = new_count;
        for(int i=0; i<3; i++){
            for(int j=0; j<3; j++){
                 board_[i][j] = temp[i][j];
            }
        }   
        turns_played_++;
        current_state_ = THINKING_MOVE;
        print_board_debug();

    } else {
        current_state_ = WAITING_FOR_HUMAN; 
        turn_start_time_ = this->get_clock()->now(); 
        last_second_printed_ = -1;
        print_board_debug();
    }
}

bool TicTacToeNode::send_move_action(double x, double y, double z, std::string action_name) {
    rclcpp_action::Client<nav2_msgs::action::NavigateToPose>::SharedPtr client;
    
    // Select the correct client
    if (action_name == "moveJ") client = client_moveJ_;
    else if (action_name == "moveL") client = client_moveL_;
    else return false;

    if (!client->wait_for_action_server(std::chrono::seconds(5))) {
        RCLCPP_ERROR(this->get_logger(), "Server %s not available", action_name.c_str());
        return false;
    }

    // Build goal pose with fixed downward orientation
    auto goal_msg = nav2_msgs::action::NavigateToPose::Goal();
    goal_msg.pose.header.frame_id = "base";
    goal_msg.pose.header.stamp = this->now();
    goal_msg.pose.pose.position.x = x;
    goal_msg.pose.pose.position.y = y;
    goal_msg.pose.pose.position.z = z;

    goal_msg.pose.pose.orientation.x = 0.0;
    goal_msg.pose.pose.orientation.y = 0.999; 
    goal_msg.pose.pose.orientation.z = 0.0;
    goal_msg.pose.pose.orientation.w = 0.001; 

    auto send_goal_future = client->async_send_goal(goal_msg);
    
    // Using .get() blocks the current thread until a response is received
    auto goal_handle = send_goal_future.get();
    if (!goal_handle) {
        RCLCPP_ERROR(this->get_logger(), "Goal %s rejected by the server", action_name.c_str());
        return false;
    }

    // The current thread blocks until the arm physically finishes the movement
    auto result_future = client->async_get_result(goal_handle);
    auto result = result_future.get();
    
    return result.code == rclcpp_action::ResultCode::SUCCEEDED;
}

void TicTacToeNode::execute_robot_turn() {
    // Find best AI move; abort if no move available
    auto best_move = find_best_move();
    int f = best_move.first, c = best_move.second;
    if (f == -1) {
        current_state_ = GAME_OVER;
        return;
    }
    char g = check_winner(board_);
    if (g != '-') { 
        RCLCPP_INFO(this->get_logger(), "WINNER: %c", g); 
        current_state_ = GAME_OVER; 
    } else if (is_draw(board_)) { 
        RCLCPP_INFO(this->get_logger(), "DRAW"); 
        current_state_ = GAME_OVER; 
    }
    
    // Mark best cell in board and compute 3D pixel position
    board_[f][c] = 'x'; 
    turns_played_++;
    
    float ur = (c + 0.5f) / 3.0f, vr = (f + 0.5f) / 3.0f;
    cv::Point2f cp = (1-ur)*(1-vr)*last_board_corners_[0] + ur*(1-vr)*last_board_corners_[1] + 
                     ur*vr*last_board_corners_[2] + (1-ur)*vr*last_board_corners_[3];
    
    Point3D pm = get_pixel_xyz_in_robot_frame(cp.x, cp.y, "camera_camara_tablero_optical_frame", "base");

    
    // Logic with error checking. If any step fails, restart from scratch.
    RCLCPP_INFO(this->get_logger(), "Moving arm to approach position...");
    if (!send_move_action(pm.x, pm.y, pm.z + 0.4, "moveJ")) {
        RCLCPP_ERROR(this->get_logger(), "Physical error going to approach point. Aborting turn.");
        current_state_ = INITIALIZING;

        return;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(1000)); 
    
    RCLCPP_INFO(this->get_logger(), "Lowering to board...");
    if (!send_move_action(pm.x, pm.y, pm.z + 0.1, "moveL")) {
        RCLCPP_ERROR(this->get_logger(), "Physical error going to approach point. Aborting turn.");
        current_state_ = INITIALIZING;

        return;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(300)); 

    // Spawn 'x' piece at the computed 3D position
    auto spawn_piece = std::make_shared<robot_interfaces::srv::SpawnObject::Request>();
    spawn_piece->name = "ficha_x_" + std::to_string(turns_played_);
    spawn_piece->package_name = "tic_tac_toe_dlc"; 
    spawn_piece->piece_type = "ficha_x";
    spawn_piece->x = pm.x;
    spawn_piece->y = pm.y;
    spawn_piece->z = pm.z + 0.002;
    spawn_client_->async_send_request(spawn_piece);
    std::this_thread::sleep_for(std::chrono::milliseconds(200)); 

    RCLCPP_INFO(this->get_logger(), "Retracting arm...");
    if (!send_move_action(pm.x, pm.y, pm.z + 0.6, "moveJ")) {
        RCLCPP_ERROR(this->get_logger(), "Physical error going to approach point. Aborting turn.");
        current_state_ = INITIALIZING;
        return;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(500)); 


    print_board_debug();

    // Check results 
    g = check_winner(board_);
    if (g != '-') { 
        RCLCPP_INFO(this->get_logger(), "WINNER: %c", g); 
        current_state_ = GAME_OVER; 
    } else if (is_draw(board_)) { 
        RCLCPP_INFO(this->get_logger(), "DRAW"); 
        current_state_ = GAME_OVER; 
    } else { 
        turn_start_time_ = this->get_clock()->now(); 
        last_second_printed_ = -1;
        current_state_ = WAITING_FOR_HUMAN; 
    }
}

int TicTacToeNode::minimax(char tab[3][3], int depth, bool isMax) {
    // Base case: check for terminal state
    char g = check_winner(tab);
    if (g == 'x') return 10 - depth;
    if (g == 'o') return -10 + depth;
    if (is_draw(tab)) return 0;

    // Initialize best score for current player
    int best = isMax ? -1000 : 1000;
    // Try all empty cells recursively
    for(int i=0; i<3; i++) {
        for(int j=0; j<3; j++) {
            if(tab[i][j] == '-') {
                tab[i][j] = isMax ? 'x' : 'o';
                int val = minimax(tab, depth + 1, !isMax);
                tab[i][j] = '-';
                best = isMax ? std::max(best, val) : std::min(best, val);
            }
        }
    }
    return best;
}

std::pair<int, int> TicTacToeNode::find_best_move() {
    int best_val = -1000;
    std::pair<int, int> best_move = {-1, -1};
    // Evaluate each empty cell via minimax
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (board_[i][j] == '-') {
                board_[i][j] = 'x';
                int val = minimax(board_, 0, false);
                board_[i][j] = '-';
                // Keep track of best scoring move
                if (val > best_val) {
                    best_val = val;
                    best_move = {i, j};
                }
            }
        }
    }
    return best_move;
}

char TicTacToeNode::check_winner(char t[3][3]) {
    // Check rows and columns for a winner
    for(int i=0; i<3; i++) {
        if(t[i][0]==t[i][1] && t[i][1]==t[i][2] && t[i][0]!='-') return t[i][0];
        if(t[0][i]==t[1][i] && t[1][i]==t[2][i] && t[0][i]!='-') return t[0][i];
    }
    if(t[0][0]==t[1][1] && t[1][1]==t[2][2] && t[0][0]!='-') return t[0][0];
    if(t[0][2]==t[1][1] && t[1][1]==t[2][0] && t[0][2]!='-') return t[0][2];
    return '-';
}

bool TicTacToeNode::is_draw(char t[3][3]) {
    // Draw if no empty cells remain
    for(int i=0; i<3; i++) {
        for(int j=0; j<3; j++) {
            if(t[i][j]=='-') return false;
        }
    }
    return true;
}

Point3D TicTacToeNode::get_pixel_xyz_in_robot_frame(int u, int v, const std::string& cam_f, const std::string& rob_f) {
    // Reject pixel if out of image bounds
    if (u < 0 || u >= (int)last_depth_image_.width || v < 0 || v >= (int)last_depth_image_.height) {
        return {0,0,0};
    }
    // Read depth value at given pixel
    int idx = (v * last_depth_image_.step) + (u * sizeof(float));
    float d = *reinterpret_cast<const float*>(&last_depth_image_.data[idx]);
    
    // Unproject pixel to 3D camera coordinates
    Point3D p_c;
    p_c.z = d;
    p_c.x = (u - last_camera_info_.k[2]) * d / last_camera_info_.k[0];
    p_c.y = (v - last_camera_info_.k[5]) * d / last_camera_info_.k[4];
    
    // Transform point to robot frame via TF2
    geometry_msgs::msg::PointStamped p_in, p_out;
    p_in.header.frame_id = cam_f;
    p_in.header.stamp = this->get_clock()->now();
    p_in.point.x = p_c.x;
    p_in.point.y = p_c.y;
    p_in.point.z = p_c.z;
    try { 
        p_out = tf_buffer_->transform(p_in, rob_f, tf2::durationFromSec(1.0)); 
    }
    catch (const tf2::TransformException & ex) {
        RCLCPP_WARN(this->get_logger(), "Transform failed: %s", ex.what());
        return {0,0,0}; 
    }
    return {p_out.point.x, p_out.point.y, p_out.point.z};
}

cv::Mat TicTacToeNode::extract_board_robust(cv::Mat& img) {
    // Convert to grayscale and detect edges
    cv::Mat gray, edges;
    cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);
    cv::GaussianBlur(gray, gray, cv::Size(5,5), 0);
    cv::Canny(gray, edges, 50, 150);
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(edges, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    // Get largest contour and approximate to quadrilateral
    std::sort(contours.begin(), contours.end(), [](auto& a, auto& b){ return cv::contourArea(a) > cv::contourArea(b); });
    if(contours.empty() || cv::contourArea(contours[0]) < 2000) return cv::Mat();
    std::vector<cv::Point> approx;
    cv::approxPolyDP(contours[0], approx, 0.02 * cv::arcLength(contours[0], true), true);
    if(approx.size() != 4) return cv::Mat();
    std::vector<cv::Point2f> pts;
    for (auto p : approx) {
        pts.push_back(cv::Point2f(p.x, p.y));
    }
    // Warp perspective to a 450x450 square
    last_board_corners_ = sort_points(pts);
    cv::Mat M = cv::getPerspectiveTransform(last_board_corners_, std::vector<cv::Point2f>{ {0,0},{449,0},{449,449},{0,449} });
    cv::Mat out;
    cv::warpPerspective(img, out, M, cv::Size(450,450));
    return out;
}

std::vector<cv::Point2f> TicTacToeNode::sort_points(const std::vector<cv::Point2f>& p) {
    // Order corners: top-left, top-right, bottom-right, bottom-left
    std::vector<cv::Point2f> r(4);
    std::vector<cv::Point2f> pts = p;
    std::sort(pts.begin(), pts.end(), [](const cv::Point2f& a, const cv::Point2f& b) { return a.y < b.y; });
    r[0] = (pts[0].x < pts[1].x) ? pts[0] : pts[1]; 
    r[1] = (pts[0].x < pts[1].x) ? pts[1] : pts[0]; 
    r[2] = (pts[2].x > pts[3].x) ? pts[2] : pts[3]; 
    r[3] = (pts[2].x > pts[3].x) ? pts[3] : pts[2]; 
    return r;
}

std::vector<cv::Mat> TicTacToeNode::split_board(cv::Mat& img) {
    // Crop each of the 9 board cells
    std::vector<cv::Mat> pieces;
    int sy = img.rows / 3, sx = img.cols / 3;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            pieces.push_back(img(cv::Rect(j * sx, i * sy, sx, sy)).clone());
        }
    }
    return pieces;
}

void TicTacToeNode::print_board_debug() {
    // Build and log board state as text
    std::string b = "\n  0 1 2\n";
    for(int i=0; i<3; i++) {
        b += std::to_string(i) + " ";
        for (int j = 0; j < 3; j++) {
            b += board_[i][j];
            b += "  ";
        }
        b += "\n";
    }
    RCLCPP_INFO(this->get_logger(), "%s", b.c_str());
}

void TicTacToeNode::info_callback(const sensor_msgs::msg::CameraInfo::SharedPtr msg) {
    last_camera_info_ = *msg;
    info_received_ = true;
}
void TicTacToeNode::depth_callback(const sensor_msgs::msg::Image::SharedPtr msg) {
    last_depth_image_ = *msg;
    depth_received_ = true;
}
void TicTacToeNode::color_callback(const sensor_msgs::msg::Image::SharedPtr msg) {
    last_color_image_ = *msg;
    color_received_ = true;
}

int main(int argc, char **argv) {
    rclcpp::init(argc, argv);
    auto node = std::make_shared<TicTacToeNode>();
    
    // Using std::thread for the logic, main remains simple and traditional
    rclcpp::spin(node);
    
    rclcpp::shutdown();
    return 0;
}