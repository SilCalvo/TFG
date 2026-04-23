// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from robot_interfaces:srv/SolveDK.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "robot_interfaces/srv/solve_dk.hpp"


#ifndef ROBOT_INTERFACES__SRV__DETAIL__SOLVE_DK__BUILDER_HPP_
#define ROBOT_INTERFACES__SRV__DETAIL__SOLVE_DK__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "robot_interfaces/srv/detail/solve_dk__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace robot_interfaces
{

namespace srv
{

namespace builder
{

class Init_SolveDK_Request_tool_dimensions
{
public:
  explicit Init_SolveDK_Request_tool_dimensions(::robot_interfaces::srv::SolveDK_Request & msg)
  : msg_(msg)
  {}
  ::robot_interfaces::srv::SolveDK_Request tool_dimensions(::robot_interfaces::srv::SolveDK_Request::_tool_dimensions_type arg)
  {
    msg_.tool_dimensions = std::move(arg);
    return std::move(msg_);
  }

private:
  ::robot_interfaces::srv::SolveDK_Request msg_;
};

class Init_SolveDK_Request_tcp_offset
{
public:
  explicit Init_SolveDK_Request_tcp_offset(::robot_interfaces::srv::SolveDK_Request & msg)
  : msg_(msg)
  {}
  Init_SolveDK_Request_tool_dimensions tcp_offset(::robot_interfaces::srv::SolveDK_Request::_tcp_offset_type arg)
  {
    msg_.tcp_offset = std::move(arg);
    return Init_SolveDK_Request_tool_dimensions(msg_);
  }

private:
  ::robot_interfaces::srv::SolveDK_Request msg_;
};

class Init_SolveDK_Request_joint_angles
{
public:
  Init_SolveDK_Request_joint_angles()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_SolveDK_Request_tcp_offset joint_angles(::robot_interfaces::srv::SolveDK_Request::_joint_angles_type arg)
  {
    msg_.joint_angles = std::move(arg);
    return Init_SolveDK_Request_tcp_offset(msg_);
  }

private:
  ::robot_interfaces::srv::SolveDK_Request msg_;
};

}  // namespace builder

}  // namespace srv

template<typename MessageType>
auto build();

template<>
inline
auto build<::robot_interfaces::srv::SolveDK_Request>()
{
  return robot_interfaces::srv::builder::Init_SolveDK_Request_joint_angles();
}

}  // namespace robot_interfaces


namespace robot_interfaces
{

namespace srv
{

namespace builder
{

class Init_SolveDK_Response_success
{
public:
  explicit Init_SolveDK_Response_success(::robot_interfaces::srv::SolveDK_Response & msg)
  : msg_(msg)
  {}
  ::robot_interfaces::srv::SolveDK_Response success(::robot_interfaces::srv::SolveDK_Response::_success_type arg)
  {
    msg_.success = std::move(arg);
    return std::move(msg_);
  }

private:
  ::robot_interfaces::srv::SolveDK_Response msg_;
};

class Init_SolveDK_Response_target_pose
{
public:
  Init_SolveDK_Response_target_pose()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_SolveDK_Response_success target_pose(::robot_interfaces::srv::SolveDK_Response::_target_pose_type arg)
  {
    msg_.target_pose = std::move(arg);
    return Init_SolveDK_Response_success(msg_);
  }

private:
  ::robot_interfaces::srv::SolveDK_Response msg_;
};

}  // namespace builder

}  // namespace srv

template<typename MessageType>
auto build();

template<>
inline
auto build<::robot_interfaces::srv::SolveDK_Response>()
{
  return robot_interfaces::srv::builder::Init_SolveDK_Response_target_pose();
}

}  // namespace robot_interfaces


namespace robot_interfaces
{

namespace srv
{

namespace builder
{

class Init_SolveDK_Event_response
{
public:
  explicit Init_SolveDK_Event_response(::robot_interfaces::srv::SolveDK_Event & msg)
  : msg_(msg)
  {}
  ::robot_interfaces::srv::SolveDK_Event response(::robot_interfaces::srv::SolveDK_Event::_response_type arg)
  {
    msg_.response = std::move(arg);
    return std::move(msg_);
  }

private:
  ::robot_interfaces::srv::SolveDK_Event msg_;
};

class Init_SolveDK_Event_request
{
public:
  explicit Init_SolveDK_Event_request(::robot_interfaces::srv::SolveDK_Event & msg)
  : msg_(msg)
  {}
  Init_SolveDK_Event_response request(::robot_interfaces::srv::SolveDK_Event::_request_type arg)
  {
    msg_.request = std::move(arg);
    return Init_SolveDK_Event_response(msg_);
  }

private:
  ::robot_interfaces::srv::SolveDK_Event msg_;
};

class Init_SolveDK_Event_info
{
public:
  Init_SolveDK_Event_info()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_SolveDK_Event_request info(::robot_interfaces::srv::SolveDK_Event::_info_type arg)
  {
    msg_.info = std::move(arg);
    return Init_SolveDK_Event_request(msg_);
  }

private:
  ::robot_interfaces::srv::SolveDK_Event msg_;
};

}  // namespace builder

}  // namespace srv

template<typename MessageType>
auto build();

template<>
inline
auto build<::robot_interfaces::srv::SolveDK_Event>()
{
  return robot_interfaces::srv::builder::Init_SolveDK_Event_info();
}

}  // namespace robot_interfaces

#endif  // ROBOT_INTERFACES__SRV__DETAIL__SOLVE_DK__BUILDER_HPP_
