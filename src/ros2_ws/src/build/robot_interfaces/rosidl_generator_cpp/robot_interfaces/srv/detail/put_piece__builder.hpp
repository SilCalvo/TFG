// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from robot_interfaces:srv/PutPiece.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "robot_interfaces/srv/put_piece.hpp"


#ifndef ROBOT_INTERFACES__SRV__DETAIL__PUT_PIECE__BUILDER_HPP_
#define ROBOT_INTERFACES__SRV__DETAIL__PUT_PIECE__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "robot_interfaces/srv/detail/put_piece__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace robot_interfaces
{

namespace srv
{

namespace builder
{

class Init_PutPiece_Request_columna
{
public:
  explicit Init_PutPiece_Request_columna(::robot_interfaces::srv::PutPiece_Request & msg)
  : msg_(msg)
  {}
  ::robot_interfaces::srv::PutPiece_Request columna(::robot_interfaces::srv::PutPiece_Request::_columna_type arg)
  {
    msg_.columna = std::move(arg);
    return std::move(msg_);
  }

private:
  ::robot_interfaces::srv::PutPiece_Request msg_;
};

class Init_PutPiece_Request_fila
{
public:
  Init_PutPiece_Request_fila()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_PutPiece_Request_columna fila(::robot_interfaces::srv::PutPiece_Request::_fila_type arg)
  {
    msg_.fila = std::move(arg);
    return Init_PutPiece_Request_columna(msg_);
  }

private:
  ::robot_interfaces::srv::PutPiece_Request msg_;
};

}  // namespace builder

}  // namespace srv

template<typename MessageType>
auto build();

template<>
inline
auto build<::robot_interfaces::srv::PutPiece_Request>()
{
  return robot_interfaces::srv::builder::Init_PutPiece_Request_fila();
}

}  // namespace robot_interfaces


namespace robot_interfaces
{

namespace srv
{

namespace builder
{

class Init_PutPiece_Response_message
{
public:
  explicit Init_PutPiece_Response_message(::robot_interfaces::srv::PutPiece_Response & msg)
  : msg_(msg)
  {}
  ::robot_interfaces::srv::PutPiece_Response message(::robot_interfaces::srv::PutPiece_Response::_message_type arg)
  {
    msg_.message = std::move(arg);
    return std::move(msg_);
  }

private:
  ::robot_interfaces::srv::PutPiece_Response msg_;
};

class Init_PutPiece_Response_success
{
public:
  Init_PutPiece_Response_success()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_PutPiece_Response_message success(::robot_interfaces::srv::PutPiece_Response::_success_type arg)
  {
    msg_.success = std::move(arg);
    return Init_PutPiece_Response_message(msg_);
  }

private:
  ::robot_interfaces::srv::PutPiece_Response msg_;
};

}  // namespace builder

}  // namespace srv

template<typename MessageType>
auto build();

template<>
inline
auto build<::robot_interfaces::srv::PutPiece_Response>()
{
  return robot_interfaces::srv::builder::Init_PutPiece_Response_success();
}

}  // namespace robot_interfaces


namespace robot_interfaces
{

namespace srv
{

namespace builder
{

class Init_PutPiece_Event_response
{
public:
  explicit Init_PutPiece_Event_response(::robot_interfaces::srv::PutPiece_Event & msg)
  : msg_(msg)
  {}
  ::robot_interfaces::srv::PutPiece_Event response(::robot_interfaces::srv::PutPiece_Event::_response_type arg)
  {
    msg_.response = std::move(arg);
    return std::move(msg_);
  }

private:
  ::robot_interfaces::srv::PutPiece_Event msg_;
};

class Init_PutPiece_Event_request
{
public:
  explicit Init_PutPiece_Event_request(::robot_interfaces::srv::PutPiece_Event & msg)
  : msg_(msg)
  {}
  Init_PutPiece_Event_response request(::robot_interfaces::srv::PutPiece_Event::_request_type arg)
  {
    msg_.request = std::move(arg);
    return Init_PutPiece_Event_response(msg_);
  }

private:
  ::robot_interfaces::srv::PutPiece_Event msg_;
};

class Init_PutPiece_Event_info
{
public:
  Init_PutPiece_Event_info()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_PutPiece_Event_request info(::robot_interfaces::srv::PutPiece_Event::_info_type arg)
  {
    msg_.info = std::move(arg);
    return Init_PutPiece_Event_request(msg_);
  }

private:
  ::robot_interfaces::srv::PutPiece_Event msg_;
};

}  // namespace builder

}  // namespace srv

template<typename MessageType>
auto build();

template<>
inline
auto build<::robot_interfaces::srv::PutPiece_Event>()
{
  return robot_interfaces::srv::builder::Init_PutPiece_Event_info();
}

}  // namespace robot_interfaces

#endif  // ROBOT_INTERFACES__SRV__DETAIL__PUT_PIECE__BUILDER_HPP_
