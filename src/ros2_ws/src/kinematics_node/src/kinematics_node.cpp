#include "kinematics_node.hpp"

using std::placeholders::_1;

// --- CONSTRUCTOR ---
KinematicsNode::KinematicsNode()
: Node("kinematics_node")
{
  // Inicializar vector con ceros (asumiendo 4 grados de libertad por ejemplo)
  current_angles_ = {0, 0, 0, 0};

  // Suscriptor
  subscription_ = this->create_subscription<std_msgs::msg::Int16MultiArray>(
    "robot_cmd", 
    10, 
    std::bind(&KinematicsNode::servos_status_callback, this, _1)
  );

  // Publicador del resultado (TCP Pose)
  pose_publisher_ = this->create_publisher<geometry_msgs::msg::PoseStamped>("tcp_pose", 10);

  RCLCPP_INFO(this->get_logger(), "Nodo de Cinemática Iniciado esperando ángulos...");
}

// --- CALLBACK ---
void KinematicsNode::servos_status_callback(const std_msgs::msg::Int16MultiArray::SharedPtr msg)
{
  // Copiar los datos del mensaje al vector interno
  current_angles_ = msg->data;

  // Calcular la cinemática cada vez que llegan nuevos ángulos
  calculate_and_publish_fk();
}

geometry_msgs::msg::Pose KinematicsNode::calculate_fk_directly(const std::vector<double>& q)
{
  // 1. Pre-cálculo de senos y cosenos (para optimizar tus fórmulas)
  // Asumo que q[0] es theta1, q[1] es theta2, etc.
  double c1 = cos(q[0]);
  double s1 = sin(q[0]);
  double c2 = cos(q[1]);
  double s2 = sin(q[1]);
  double c23 = cos(q[1] + q[2]); // Ejemplo de suma de ángulos
  double s23 = sin(q[1] + q[2]);
  
  // Constantes de tu robot (Longitudes de eslabones)
  double L1 = 0.5;
  double L2 = 0.3;

  // --------------------------------------------------------
  // 2. TUS ECUACIONES FINALES (La Matriz 4x4 Gigante)
  // --------------------------------------------------------
  // Aquí pegas las fórmulas que hallaste a mano.
  // Matriz T_0_Final:
  // [ r11 r12 r13  px ]
  // [ r21 r22 r23  py ]
  // [ r31 r32 r33  pz ]
  // [  0   0   0    1 ]
  
  // -- ROTACIÓN (3x3) --
  // Ejemplo: r11 = c1*c23... (Pon tus fórmulas reales aquí)
  double r11 = c1 * c23; 
  double r12 = -c1 * s23;
  double r13 = s1;
  
  double r21 = s1 * c23;
  double r22 = -s1 * s23;
  double r23 = -c1;
  
  double r31 = s23;
  double r32 = c23;
  double r33 = 0.0;

  // -- TRASLACIÓN (X, Y, Z) --
  double px = c1 * (L1 * c2 + L2 * c23); 
  double py = s1 * (L1 * c2 + L2 * c23);
  double pz = L1 * s2 + L2 * s23;

  // --------------------------------------------------------
  // 3. LLENAR EL OBJETO TF2
  // --------------------------------------------------------
  
  // A. Llenar la matriz de rotación
  tf2::Matrix3x3 rotation_matrix;
  rotation_matrix.setValue(
    r11, r12, r13,
    r21, r22, r23,
    r31, r32, r33
  );

  // B. Convertir Matriz a Cuaternión (ROS lo hace automático aquí)
  tf2::Quaternion orientation_quat;
  rotation_matrix.getRotation(orientation_quat);

  // --------------------------------------------------------
  // 4. RETORNAR EL FORMATO POSE PARA ROS
  // --------------------------------------------------------
  geometry_msgs::msg::Pose pose_output;

  // Asignar Posición
  pose_output.position.x = px;
  pose_output.position.y = py;
  pose_output.position.z = pz;

  // Asignar Orientación
  pose_output.orientation.x = orientation_quat.x();
  pose_output.orientation.y = orientation_quat.y();
  pose_output.orientation.z = orientation_quat.z();
  pose_output.orientation.w = orientation_quat.w();

  return pose_output;
}
// --- MAIN ---
int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  // Esto mantiene el nodo vivo escuchando callbacks
  rclcpp::spin(std::make_shared<KinematicsNode>());
  rclcpp::shutdown();
  return 0;
}