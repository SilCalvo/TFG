// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from robot_interfaces:srv/SpawnObject.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "robot_interfaces/srv/spawn_object.hpp"


#ifndef ROBOT_INTERFACES__SRV__DETAIL__SPAWN_OBJECT__BUILDER_HPP_
#define ROBOT_INTERFACES__SRV__DETAIL__SPAWN_OBJECT__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "robot_interfaces/srv/detail/spawn_object__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace robot_interfaces
{

namespace srv
{

namespace builder
{

class Init_SpawnObject_Request_z
{
public:
  explicit Init_SpawnObject_Request_z(::robot_interfaces::srv::SpawnObject_Request & msg)
  : msg_(msg)
  {}
  ::robot_interfaces::srv::SpawnObject_Request z(::robot_interfaces::srv::SpawnObject_Request::_z_type arg)
  {
    msg_.z = std::move(arg);
    return std::move(msg_);
  }

private:
  ::robot_interfaces::srv::SpawnObject_Request msg_;
};

class Init_SpawnObject_Request_y
{
public:
  explicit Init_SpawnObject_Request_y(::robot_interfaces::srv::SpawnObject_Request & msg)
  : msg_(msg)
  {}
  Init_SpawnObject_Request_z y(::robot_interfaces::srv::SpawnObject_Request::_y_type arg)
  {
    msg_.y = std::move(arg);
    return Init_SpawnObject_Request_z(msg_);
  }

private:
  ::robot_interfaces::srv::SpawnObject_Request msg_;
};

class Init_SpawnObject_Request_x
{
public:
  explicit Init_SpawnObject_Request_x(::robot_interfaces::srv::SpawnObject_Request & msg)
  : msg_(msg)
  {}
  Init_SpawnObject_Request_y x(::robot_interfaces::srv::SpawnObject_Request::_x_type arg)
  {
    msg_.x = std::move(arg);
    return Init_SpawnObject_Request_y(msg_);
  }

private:
  ::robot_interfaces::srv::SpawnObject_Request msg_;
};

class Init_SpawnObject_Request_piece_type
{
public:
  explicit Init_SpawnObject_Request_piece_type(::robot_interfaces::srv::SpawnObject_Request & msg)
  : msg_(msg)
  {}
  Init_SpawnObject_Request_x piece_type(::robot_interfaces::srv::SpawnObject_Request::_piece_type_type arg)
  {
    msg_.piece_type = std::move(arg);
    return Init_SpawnObject_Request_x(msg_);
  }

private:
  ::robot_interfaces::srv::SpawnObject_Request msg_;
};

class Init_SpawnObject_Request_package_name
{
public:
  explicit Init_SpawnObject_Request_package_name(::robot_interfaces::srv::SpawnObject_Request & msg)
  : msg_(msg)
  {}
  Init_SpawnObject_Request_piece_type package_name(::robot_interfaces::srv::SpawnObject_Request::_package_name_type arg)
  {
    msg_.package_name = std::move(arg);
    return Init_SpawnObject_Request_piece_type(msg_);
  }

private:
  ::robot_interfaces::srv::SpawnObject_Request msg_;
};

class Init_SpawnObject_Request_name
{
public:
  Init_SpawnObject_Request_name()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_SpawnObject_Request_package_name name(::robot_interfaces::srv::SpawnObject_Request::_name_type arg)
  {
    msg_.name = std::move(arg);
    return Init_SpawnObject_Request_package_name(msg_);
  }

private:
  ::robot_interfaces::srv::SpawnObject_Request msg_;
};

}  // namespace builder

}  // namespace srv

template<typename MessageType>
auto build();

template<>
inline
auto build<::robot_interfaces::srv::SpawnObject_Request>()
{
  return robot_interfaces::srv::builder::Init_SpawnObject_Request_name();
}

}  // namespace robot_interfaces


namespace robot_interfaces
{

namespace srv
{

namespace builder
{

class Init_SpawnObject_Response_success
{
public:
  Init_SpawnObject_Response_success()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  ::robot_interfaces::srv::SpawnObject_Response success(::robot_interfaces::srv::SpawnObject_Response::_success_type arg)
  {
    msg_.success = std::move(arg);
    return std::move(msg_);
  }

private:
  ::robot_interfaces::srv::SpawnObject_Response msg_;
};

}  // namespace builder

}  // namespace srv

template<typename MessageType>
auto build();

template<>
inline
auto build<::robot_interfaces::srv::SpawnObject_Response>()
{
  return robot_interfaces::srv::builder::Init_SpawnObject_Response_success();
}

}  // namespace robot_interfaces


namespace robot_interfaces
{

namespace srv
{

namespace builder
{

class Init_SpawnObject_Event_response
{
public:
  explicit Init_SpawnObject_Event_response(::robot_interfaces::srv::SpawnObject_Event & msg)
  : msg_(msg)
  {}
  ::robot_interfaces::srv::SpawnObject_Event response(::robot_interfaces::srv::SpawnObject_Event::_response_type arg)
  {
    msg_.response = std::move(arg);
    return std::move(msg_);
  }

private:
  ::robot_interfaces::srv::SpawnObject_Event msg_;
};

class Init_SpawnObject_Event_request
{
public:
  explicit Init_SpawnObject_Event_request(::robot_interfaces::srv::SpawnObject_Event & msg)
  : msg_(msg)
  {}
  Init_SpawnObject_Event_response request(::robot_interfaces::srv::SpawnObject_Event::_request_type arg)
  {
    msg_.request = std::move(arg);
    return Init_SpawnObject_Event_response(msg_);
  }

private:
  ::robot_interfaces::srv::SpawnObject_Event msg_;
};

class Init_SpawnObject_Event_info
{
public:
  Init_SpawnObject_Event_info()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_SpawnObject_Event_request info(::robot_interfaces::srv::SpawnObject_Event::_info_type arg)
  {
    msg_.info = std::move(arg);
    return Init_SpawnObject_Event_request(msg_);
  }

private:
  ::robot_interfaces::srv::SpawnObject_Event msg_;
};

}  // namespace builder

}  // namespace srv

template<typename MessageType>
auto build();

template<>
inline
auto build<::robot_interfaces::srv::SpawnObject_Event>()
{
  return robot_interfaces::srv::builder::Init_SpawnObject_Event_info();
}

}  // namespace robot_interfaces

#endif  // ROBOT_INTERFACES__SRV__DETAIL__SPAWN_OBJECT__BUILDER_HPP_
