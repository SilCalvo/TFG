#include "cpp_servo_bridge/servo_bridge.hpp"

#include <chrono>
#include <string>
#include <cstring>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <vector> 
#include <cstdio> // Para sscanf

using std::placeholders::_1;
using namespace std::chrono_literals;

// --- CONSTRUCTOR ---
ServoBridgeNode::ServoBridgeNode()
: Node("cpp_servo_bridge"), serial_port_(-1)
{
  // Configura tu puerto aquí
  const char * port_name = "/dev/ttyACM0"; 
  
  // ABRIMOS EN MODO NO BLOQUEANTE (NDELAY) PARA EL TIMER
  serial_port_ = open(port_name, O_RDWR | O_NOCTTY | O_NDELAY);
  
  if (serial_port_ < 0) {
    RCLCPP_ERROR(this->get_logger(), "Error fatal: No se puede abrir %s", port_name);
  } else {
    // Restauramos flags para que read() funcione normal pero sin bloquear
    fcntl(serial_port_, F_SETFL, 0); 
    configure_serial(serial_port_);
    RCLCPP_INFO(this->get_logger(), "Arduino conectado en %s", port_name);
  }

  // 1. SUSCRIPTOR (Recibe órdenes de movimiento)
  subscription_ = this->create_subscription<std_msgs::msg::Int16MultiArray>(
    "robot_cmd", 10, std::bind(&ServoBridgeNode::robot_cmd_callback, this, _1));

  // 2. PUBLICADOR (Envía estado real de los servos a ROS)
  publisher_ = this->create_publisher<std_msgs::msg::Int16MultiArray>("robot_status", 10);

  // 3. TIMER (Lee el puerto serial cada 20ms)
  timer_ = this->create_wall_timer(
      20ms, std::bind(&ServoBridgeNode::timer_callback, this));
}

// --- DESTRUCTOR ---
ServoBridgeNode::~ServoBridgeNode() {
  if (serial_port_ >= 0) close(serial_port_);
}

// --- ESCRITURA: ROS -> ARDUINO ---
void ServoBridgeNode::robot_cmd_callback(const std_msgs::msg::Int16MultiArray::SharedPtr msg)
{
  if (serial_port_ < 0) return;

  if (msg->data.size() != 5) {
    RCLCPP_WARN(this->get_logger(), "Ignorando comando: Se esperaban 5 angulos");
    return; 
  }

  // Formato: "90,45,0,180,90\n"
  std::string command = "";
  for (size_t i = 0; i < msg->data.size(); ++i) {
    command += std::to_string(msg->data[i]);
    if (i < msg->data.size() - 1) command += ","; 
  }
  command += "\n"; 

  // Debug (opcional)
  // RCLCPP_INFO(this->get_logger(), "Enviando: %s", command.c_str()); 

  write(serial_port_, command.c_str(), command.size());
}

// --- LECTURA: ARDUINO -> ROS (Timer) ---
void ServoBridgeNode::timer_callback()
{
  if (serial_port_ < 0) return;

  char buffer[256];
  // Leemos lo que haya en el buffer del sistema (No bloquea si está vacío gracias a fcntl/O_NDELAY)
  int n = read(serial_port_, buffer, sizeof(buffer) - 1);

  if (n > 0) {
    buffer[n] = '\0'; // Terminador null
    read_buffer_ += buffer; // Añadir al acumulador

    // Procesamos líneas completas
    size_t pos = read_buffer_.find('\n');
    while (pos != std::string::npos) {
      std::string line = read_buffer_.substr(0, pos);
      read_buffer_.erase(0, pos + 1); // Borrar lo procesado

      // PARSEO CON SSCANF
      int v[5];
      int encontrados = sscanf(line.c_str(), "%d,%d,%d,%d,%d", &v[0], &v[1], &v[2], &v[3], &v[4]);

      // Si tenemos los 5 valores, publicamos
      if (encontrados == 5) {
        std_msgs::msg::Int16MultiArray msg_out;
        msg_out.data.assign(v, v + 5); // Copia rápida array -> vector
        publisher_->publish(msg_out);
      }

      // Buscar siguiente línea
      pos = read_buffer_.find('\n');
    }
  }
}

// --- CONFIGURACIÓN SERIAL ---
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

    // MODO RAW (Importante para lectura binaria/rápida)
    tty.c_lflag &= ~ICANON;
    tty.c_lflag &= ~ECHO;
    tty.c_lflag &= ~ECHOE;
    tty.c_lflag &= ~ISIG;
    
    // Timeouts NO BLOQUEANTES
    tty.c_cc[VMIN]  = 0; 
    tty.c_cc[VTIME] = 0; 

    tcsetattr(fd, TCSANOW, &tty);
    
    // Asegurar flag no bloqueante
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