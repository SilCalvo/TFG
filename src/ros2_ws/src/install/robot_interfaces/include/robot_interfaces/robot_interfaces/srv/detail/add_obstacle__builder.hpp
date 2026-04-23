// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from robot_interfaces:srv/AddObstacle.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "robot_interfaces/srv/add_obstacle.hpp"


#ifndef ROBOT_INTERFACES__SRV__DETAIL__ADD_OBSTACLE__BUILDER_HPP_
#define ROBOT_INTERFACES__SRV__DETAIL__ADD_OBSTACLE__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "robot_interfaces/srv/detail/add_obstacle__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace robot_interfaces
{

namespace srv
{

namespace builder
{

class Init_AddObstacle_Request_depth
{
public:
  explicit Init_AddObstacle_Request_depth(::robot_interfaces::srv::AddObstacle_Request & msg)
  : msg_(msg)
  {}
  ::robot_interfaces::srv::AddObstacle_Request depth(::robot_interfaces::srv::AddObstacle_Request::_depth_type arg)
  {
    msg_.depth = std::move(arg);
    return std::move(msg_);
  }

private:
  ::robot_interfaces::srv::AddObstacle_Request msg_;
};

class Init_AddObstacle_Request_height
{
public:
  explicit Init_AddObstacle_Request_height(::robot_interfaces::srv::AddObstacle_Request & msg)
  : msg_(msg)
  {}
  Init_AddObstacle_Request_depth height(::robot_interfaces::srv::AddObstacle_Request::_height_type arg)
  {
    msg_.height = std::move(arg);
    return Init_AddObstacle_Request_depth(msg_);
  }

private:
  ::robot_interfaces::srv::AddObstacle_Request msg_;
};

class Init_AddObstacle_Request_width
{
public:
  explicit Init_AddObstacle_Request_width(::robot_interfaces::srv::AddObstacle_Request & msg)
  : msg_(msg)
  {}
  Init_AddObstacle_Request_height width(::robot_interfaces::srv::AddObstacle_Request::_width_type arg)
  {
    msg_.width = std::move(arg);
    return Init_AddObstacle_Request_height(msg_);
  }

private:
  ::robot_interfaces::srv::AddObstacle_Request msg_;
};

class Init_AddObstacle_Request_z
{
public:
  explicit Init_AddObstacle_Request_z(::robot_interfaces::srv::AddObstacle_Request & msg)
  : msg_(msg)
  {}
  Init_AddObstacle_Request_width z(::robot_interfaces::srv::AddObstacle_Request::_z_type arg)
  {
    msg_.z = std::move(arg);
    return Init_AddObstacle_Request_width(msg_);
  }

private:
  ::robot_interfaces::srv::AddObstacle_Request msg_;
};

class Init_AddObstacle_Request_y
{
public:
  explicit Init_AddObstacle_Request_y(::robot_interfaces::srv::AddObstacle_Request & msg)
  : msg_(msg)
  {}
  Init_AddObstacle_Request_z y(::robot_interfaces::srv::AddObstacle_Request::_y_type arg)
  {
    msg_.y = std::move(arg);
    return Init_AddObstacle_Request_z(msg_);
  }

private:
  ::robot_interfaces::srv::AddObstacle_Request msg_;
};

class Init_AddObstacle_Request_x
{
public:
  explicit Init_AddObstacle_Request_x(::robot_interfaces::srv::AddObstacle_Request & msg)
  : msg_(msg)
  {}
  Init_AddObstacle_Request_y x(::robot_interfaces::srv::AddObstacle_Request::_x_type arg)
  {
    msg_.x = std::move(arg);
    return Init_AddObstacle_Request_y(msg_);
  }

private:
  ::robot_interfaces::srv::AddObstacle_Request msg_;
};

class Init_AddObstacle_Request_name
{
public:
  Init_AddObstacle_Request_name()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_AddObstacle_Request_x name(::robot_interfaces::srv::AddObstacle_Request::_name_type arg)
  {
    msg_.name = std::move(arg);
    return Init_AddObstacle_Request_x(msg_);
  }

private:
  ::robot_interfaces::srv::AddObstacle_Request msg_;
};

}  // namespace builder

}  // namespace srv

template<typename MessageType>
auto build();

template<>
inline
auto build<::robot_interfaces::srv::AddObstacle_Request>()
{
  return robot_interfaces::srv::builder::Init_AddObstacle_Request_name();
}

}  // namespace robot_interfaces


namespace robot_interfaces
{

namespace srv
{

namespace builder
{

class Init_AddObstacle_Response_success
{
public:
  Init_AddObstacle_Response_success()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  ::robot_interfaces::srv::AddObstacle_Response success(::robot_interfaces::srv::AddObstacle_Response::_success_type arg)
  {
    msg_.success = std::move(arg);
    return std::move(msg_);
  }

private:
  ::robot_interfaces::srv::AddObstacle_Response msg_;
};

}  // namespace builder

}  // namespace srv

template<typename MessageType>
auto build();

template<>
inline
auto build<::robot_interfaces::srv::AddObstacle_Response>()
{
  return robot_interfaces::srv::builder::Init_AddObstacle_Response_success();
}

}  // namespace robot_interfaces


namespace robot_interfaces
{

namespace srv
{

namespace builder
{

class Init_AddObstacle_Event_response
{
public:
  explicit Init_AddObstacle_Event_response(::robot_interfaces::srv::AddObstacle_Event & msg)
  : msg_(msg)
  {}
  ::robot_interfaces::srv::AddObstacle_Event response(::robot_interfaces::srv::AddObstacle_Event::_response_type arg)
  {
    msg_.response = std::move(arg);
    return std::move(msg_);
  }

private:
  ::robot_interfaces::srv::AddObstacle_Event msg_;
};

class Init_AddObstacle_Event_request
{
public:
  explicit Init_AddObstacle_Event_request(::robot_interfaces::srv::AddObstacle_Event & msg)
  : msg_(msg)
  {}
  Init_AddObstacle_Event_response request(::robot_interfaces::srv::AddObstacle_Event::_request_type arg)
  {
    msg_.request = std::move(arg);
    return Init_AddObstacle_Event_response(msg_);
  }

private:
  ::robot_interfaces::srv::AddObstacle_Event msg_;
};

class Init_AddObstacle_Event_info
{
public:
  Init_AddObstacle_Event_info()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_AddObstacle_Event_request info(::robot_interfaces::srv::AddObstacle_Event::_info_type arg)
  {
    msg_.info = std::move(arg);
    return Init_AddObstacle_Event_request(msg_);
  }

private:
  ::robot_interfaces::srv::AddObstacle_Event msg_;
};

}  // namespace builder

}  // namespace srv

template<typename MessageType>
auto build();

template<>
inline
auto build<::robot_interfaces::srv::AddObstacle_Event>()
{
  return robot_interfaces::srv::builder::Init_AddObstacle_Event_info();
}

}  // namespace robot_interfaces

#endif  // ROBOT_INTERFACES__SRV__DETAIL__ADD_OBSTACLE__BUILDER_HPP_
