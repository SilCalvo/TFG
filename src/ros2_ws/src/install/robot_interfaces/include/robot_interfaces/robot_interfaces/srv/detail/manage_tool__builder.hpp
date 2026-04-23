// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from robot_interfaces:srv/ManageTool.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "robot_interfaces/srv/manage_tool.hpp"


#ifndef ROBOT_INTERFACES__SRV__DETAIL__MANAGE_TOOL__BUILDER_HPP_
#define ROBOT_INTERFACES__SRV__DETAIL__MANAGE_TOOL__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "robot_interfaces/srv/detail/manage_tool__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace robot_interfaces
{

namespace srv
{

namespace builder
{

class Init_ManageTool_Request_offset
{
public:
  explicit Init_ManageTool_Request_offset(::robot_interfaces::srv::ManageTool_Request & msg)
  : msg_(msg)
  {}
  ::robot_interfaces::srv::ManageTool_Request offset(::robot_interfaces::srv::ManageTool_Request::_offset_type arg)
  {
    msg_.offset = std::move(arg);
    return std::move(msg_);
  }

private:
  ::robot_interfaces::srv::ManageTool_Request msg_;
};

class Init_ManageTool_Request_dimensions
{
public:
  explicit Init_ManageTool_Request_dimensions(::robot_interfaces::srv::ManageTool_Request & msg)
  : msg_(msg)
  {}
  Init_ManageTool_Request_offset dimensions(::robot_interfaces::srv::ManageTool_Request::_dimensions_type arg)
  {
    msg_.dimensions = std::move(arg);
    return Init_ManageTool_Request_offset(msg_);
  }

private:
  ::robot_interfaces::srv::ManageTool_Request msg_;
};

class Init_ManageTool_Request_type
{
public:
  explicit Init_ManageTool_Request_type(::robot_interfaces::srv::ManageTool_Request & msg)
  : msg_(msg)
  {}
  Init_ManageTool_Request_dimensions type(::robot_interfaces::srv::ManageTool_Request::_type_type arg)
  {
    msg_.type = std::move(arg);
    return Init_ManageTool_Request_dimensions(msg_);
  }

private:
  ::robot_interfaces::srv::ManageTool_Request msg_;
};

class Init_ManageTool_Request_name
{
public:
  Init_ManageTool_Request_name()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_ManageTool_Request_type name(::robot_interfaces::srv::ManageTool_Request::_name_type arg)
  {
    msg_.name = std::move(arg);
    return Init_ManageTool_Request_type(msg_);
  }

private:
  ::robot_interfaces::srv::ManageTool_Request msg_;
};

}  // namespace builder

}  // namespace srv

template<typename MessageType>
auto build();

template<>
inline
auto build<::robot_interfaces::srv::ManageTool_Request>()
{
  return robot_interfaces::srv::builder::Init_ManageTool_Request_name();
}

}  // namespace robot_interfaces


namespace robot_interfaces
{

namespace srv
{

namespace builder
{

class Init_ManageTool_Response_message
{
public:
  explicit Init_ManageTool_Response_message(::robot_interfaces::srv::ManageTool_Response & msg)
  : msg_(msg)
  {}
  ::robot_interfaces::srv::ManageTool_Response message(::robot_interfaces::srv::ManageTool_Response::_message_type arg)
  {
    msg_.message = std::move(arg);
    return std::move(msg_);
  }

private:
  ::robot_interfaces::srv::ManageTool_Response msg_;
};

class Init_ManageTool_Response_success
{
public:
  Init_ManageTool_Response_success()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_ManageTool_Response_message success(::robot_interfaces::srv::ManageTool_Response::_success_type arg)
  {
    msg_.success = std::move(arg);
    return Init_ManageTool_Response_message(msg_);
  }

private:
  ::robot_interfaces::srv::ManageTool_Response msg_;
};

}  // namespace builder

}  // namespace srv

template<typename MessageType>
auto build();

template<>
inline
auto build<::robot_interfaces::srv::ManageTool_Response>()
{
  return robot_interfaces::srv::builder::Init_ManageTool_Response_success();
}

}  // namespace robot_interfaces


namespace robot_interfaces
{

namespace srv
{

namespace builder
{

class Init_ManageTool_Event_response
{
public:
  explicit Init_ManageTool_Event_response(::robot_interfaces::srv::ManageTool_Event & msg)
  : msg_(msg)
  {}
  ::robot_interfaces::srv::ManageTool_Event response(::robot_interfaces::srv::ManageTool_Event::_response_type arg)
  {
    msg_.response = std::move(arg);
    return std::move(msg_);
  }

private:
  ::robot_interfaces::srv::ManageTool_Event msg_;
};

class Init_ManageTool_Event_request
{
public:
  explicit Init_ManageTool_Event_request(::robot_interfaces::srv::ManageTool_Event & msg)
  : msg_(msg)
  {}
  Init_ManageTool_Event_response request(::robot_interfaces::srv::ManageTool_Event::_request_type arg)
  {
    msg_.request = std::move(arg);
    return Init_ManageTool_Event_response(msg_);
  }

private:
  ::robot_interfaces::srv::ManageTool_Event msg_;
};

class Init_ManageTool_Event_info
{
public:
  Init_ManageTool_Event_info()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_ManageTool_Event_request info(::robot_interfaces::srv::ManageTool_Event::_info_type arg)
  {
    msg_.info = std::move(arg);
    return Init_ManageTool_Event_request(msg_);
  }

private:
  ::robot_interfaces::srv::ManageTool_Event msg_;
};

}  // namespace builder

}  // namespace srv

template<typename MessageType>
auto build();

template<>
inline
auto build<::robot_interfaces::srv::ManageTool_Event>()
{
  return robot_interfaces::srv::builder::Init_ManageTool_Event_info();
}

}  // namespace robot_interfaces

#endif  // ROBOT_INTERFACES__SRV__DETAIL__MANAGE_TOOL__BUILDER_HPP_
