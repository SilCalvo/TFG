// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from robot_interfaces:srv/AddCamera.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "robot_interfaces/srv/add_camera.hpp"


#ifndef ROBOT_INTERFACES__SRV__DETAIL__ADD_CAMERA__BUILDER_HPP_
#define ROBOT_INTERFACES__SRV__DETAIL__ADD_CAMERA__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "robot_interfaces/srv/detail/add_camera__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace robot_interfaces
{

namespace srv
{

namespace builder
{

class Init_AddCamera_Request_height
{
public:
  explicit Init_AddCamera_Request_height(::robot_interfaces::srv::AddCamera_Request & msg)
  : msg_(msg)
  {}
  ::robot_interfaces::srv::AddCamera_Request height(::robot_interfaces::srv::AddCamera_Request::_height_type arg)
  {
    msg_.height = std::move(arg);
    return std::move(msg_);
  }

private:
  ::robot_interfaces::srv::AddCamera_Request msg_;
};

class Init_AddCamera_Request_width
{
public:
  explicit Init_AddCamera_Request_width(::robot_interfaces::srv::AddCamera_Request & msg)
  : msg_(msg)
  {}
  Init_AddCamera_Request_height width(::robot_interfaces::srv::AddCamera_Request::_width_type arg)
  {
    msg_.width = std::move(arg);
    return Init_AddCamera_Request_height(msg_);
  }

private:
  ::robot_interfaces::srv::AddCamera_Request msg_;
};

class Init_AddCamera_Request_target_z
{
public:
  explicit Init_AddCamera_Request_target_z(::robot_interfaces::srv::AddCamera_Request & msg)
  : msg_(msg)
  {}
  Init_AddCamera_Request_width target_z(::robot_interfaces::srv::AddCamera_Request::_target_z_type arg)
  {
    msg_.target_z = std::move(arg);
    return Init_AddCamera_Request_width(msg_);
  }

private:
  ::robot_interfaces::srv::AddCamera_Request msg_;
};

class Init_AddCamera_Request_target_y
{
public:
  explicit Init_AddCamera_Request_target_y(::robot_interfaces::srv::AddCamera_Request & msg)
  : msg_(msg)
  {}
  Init_AddCamera_Request_target_z target_y(::robot_interfaces::srv::AddCamera_Request::_target_y_type arg)
  {
    msg_.target_y = std::move(arg);
    return Init_AddCamera_Request_target_z(msg_);
  }

private:
  ::robot_interfaces::srv::AddCamera_Request msg_;
};

class Init_AddCamera_Request_target_x
{
public:
  explicit Init_AddCamera_Request_target_x(::robot_interfaces::srv::AddCamera_Request & msg)
  : msg_(msg)
  {}
  Init_AddCamera_Request_target_y target_x(::robot_interfaces::srv::AddCamera_Request::_target_x_type arg)
  {
    msg_.target_x = std::move(arg);
    return Init_AddCamera_Request_target_y(msg_);
  }

private:
  ::robot_interfaces::srv::AddCamera_Request msg_;
};

class Init_AddCamera_Request_z
{
public:
  explicit Init_AddCamera_Request_z(::robot_interfaces::srv::AddCamera_Request & msg)
  : msg_(msg)
  {}
  Init_AddCamera_Request_target_x z(::robot_interfaces::srv::AddCamera_Request::_z_type arg)
  {
    msg_.z = std::move(arg);
    return Init_AddCamera_Request_target_x(msg_);
  }

private:
  ::robot_interfaces::srv::AddCamera_Request msg_;
};

class Init_AddCamera_Request_y
{
public:
  explicit Init_AddCamera_Request_y(::robot_interfaces::srv::AddCamera_Request & msg)
  : msg_(msg)
  {}
  Init_AddCamera_Request_z y(::robot_interfaces::srv::AddCamera_Request::_y_type arg)
  {
    msg_.y = std::move(arg);
    return Init_AddCamera_Request_z(msg_);
  }

private:
  ::robot_interfaces::srv::AddCamera_Request msg_;
};

class Init_AddCamera_Request_x
{
public:
  explicit Init_AddCamera_Request_x(::robot_interfaces::srv::AddCamera_Request & msg)
  : msg_(msg)
  {}
  Init_AddCamera_Request_y x(::robot_interfaces::srv::AddCamera_Request::_x_type arg)
  {
    msg_.x = std::move(arg);
    return Init_AddCamera_Request_y(msg_);
  }

private:
  ::robot_interfaces::srv::AddCamera_Request msg_;
};

class Init_AddCamera_Request_name
{
public:
  Init_AddCamera_Request_name()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_AddCamera_Request_x name(::robot_interfaces::srv::AddCamera_Request::_name_type arg)
  {
    msg_.name = std::move(arg);
    return Init_AddCamera_Request_x(msg_);
  }

private:
  ::robot_interfaces::srv::AddCamera_Request msg_;
};

}  // namespace builder

}  // namespace srv

template<typename MessageType>
auto build();

template<>
inline
auto build<::robot_interfaces::srv::AddCamera_Request>()
{
  return robot_interfaces::srv::builder::Init_AddCamera_Request_name();
}

}  // namespace robot_interfaces


namespace robot_interfaces
{

namespace srv
{

namespace builder
{

class Init_AddCamera_Response_success
{
public:
  Init_AddCamera_Response_success()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  ::robot_interfaces::srv::AddCamera_Response success(::robot_interfaces::srv::AddCamera_Response::_success_type arg)
  {
    msg_.success = std::move(arg);
    return std::move(msg_);
  }

private:
  ::robot_interfaces::srv::AddCamera_Response msg_;
};

}  // namespace builder

}  // namespace srv

template<typename MessageType>
auto build();

template<>
inline
auto build<::robot_interfaces::srv::AddCamera_Response>()
{
  return robot_interfaces::srv::builder::Init_AddCamera_Response_success();
}

}  // namespace robot_interfaces


namespace robot_interfaces
{

namespace srv
{

namespace builder
{

class Init_AddCamera_Event_response
{
public:
  explicit Init_AddCamera_Event_response(::robot_interfaces::srv::AddCamera_Event & msg)
  : msg_(msg)
  {}
  ::robot_interfaces::srv::AddCamera_Event response(::robot_interfaces::srv::AddCamera_Event::_response_type arg)
  {
    msg_.response = std::move(arg);
    return std::move(msg_);
  }

private:
  ::robot_interfaces::srv::AddCamera_Event msg_;
};

class Init_AddCamera_Event_request
{
public:
  explicit Init_AddCamera_Event_request(::robot_interfaces::srv::AddCamera_Event & msg)
  : msg_(msg)
  {}
  Init_AddCamera_Event_response request(::robot_interfaces::srv::AddCamera_Event::_request_type arg)
  {
    msg_.request = std::move(arg);
    return Init_AddCamera_Event_response(msg_);
  }

private:
  ::robot_interfaces::srv::AddCamera_Event msg_;
};

class Init_AddCamera_Event_info
{
public:
  Init_AddCamera_Event_info()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_AddCamera_Event_request info(::robot_interfaces::srv::AddCamera_Event::_info_type arg)
  {
    msg_.info = std::move(arg);
    return Init_AddCamera_Event_request(msg_);
  }

private:
  ::robot_interfaces::srv::AddCamera_Event msg_;
};

}  // namespace builder

}  // namespace srv

template<typename MessageType>
auto build();

template<>
inline
auto build<::robot_interfaces::srv::AddCamera_Event>()
{
  return robot_interfaces::srv::builder::Init_AddCamera_Event_info();
}

}  // namespace robot_interfaces

#endif  // ROBOT_INTERFACES__SRV__DETAIL__ADD_CAMERA__BUILDER_HPP_
