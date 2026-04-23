#include "digital_twin/digital_twin.hpp" 

DigitalTwinNode::DigitalTwinNode() : Node("digital_twin_node") {
    // 1. Conectamos con los motores que definimos en el URDF
    pub_j1_ = this->create_publisher<std_msgs::msg::Float64>("/webots/j1/cmd_pos", 10);
    pub_j2_ = this->create_publisher<std_msgs::msg::Float64>("/webots/j2/cmd_pos", 10);
    pub_j3_ = this->create_publisher<std_msgs::msg::Float64>("/webots/j3/cmd_pos", 10);
    pub_j4_ = this->create_publisher<std_msgs::msg::Float64>("/webots/j4/cmd_pos", 10);
    pub_j5_ = this->create_publisher<std_msgs::msg::Float64>("/webots/j5/cmd_pos", 10);

    // 2. Nos suscribimos al tópico general del que tu sistema ya lee para RViz
    sub_joint_states_ = this->create_subscription<sensor_msgs::msg::JointState>(
        "/joint_states", 10, 
        std::bind(&DigitalTwinNode::joint_states_callback, this, std::placeholders::_1));

    RCLCPP_INFO(this->get_logger(), "Gemelo Digital (Webots) inicializado. Escuchando comandos...");
}

void DigitalTwinNode::joint_states_callback(const sensor_msgs::msg::JointState::SharedPtr msg) {
    // Escaneamos el mensaje de joint_states y se lo pasamos al motor de Webots correspondiente
    for (size_t i = 0; i < msg->name.size(); ++i) {
        std_msgs::msg::Float64 cmd_msg;
        cmd_msg.data = msg->position[i];

        if (msg->name[i] == "joint_base_c1") {
            pub_j1_->publish(cmd_msg);
        } else if (msg->name[i] == "joint_c1_c2") {
            pub_j2_->publish(cmd_msg);
        } else if (msg->name[i] == "joint_c2_c3") {
            pub_j3_->publish(cmd_msg);
        } else if (msg->name[i] == "joint_c3_c4") {
            pub_j4_->publish(cmd_msg);
        } else if (msg->name[i] == "joint_c4_c5") {
            pub_j5_->publish(cmd_msg);
        }
    }
}

int main(int argc, char **argv) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<DigitalTwinNode>());
    rclcpp::shutdown();
    return 0;
}