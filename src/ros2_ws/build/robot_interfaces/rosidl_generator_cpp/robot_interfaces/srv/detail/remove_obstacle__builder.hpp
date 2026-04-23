// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from robot_interfaces:srv/RemoveObstacle.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "robot_interfaces/srv/remove_obstacle.hpp"


#ifndef ROBOT_INTERFACES__SRV__DETAIL__REMOVE_OBSTACLE__BUILDER_HPP_
#define ROBOT_INTERFACES__SRV__DETAIL__REMOVE_OBSTACLE__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "robot_interfaces/srv/detail/remove_obstacle__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace robot_interfaces
{

namespace srv
{

namespace builder
{

class Init_RemoveObstacle_Request_name
{
public:
  Init_RemoveObstacle_Request_name()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  ::robot_interfaces::srv::RemoveObstacle_Request name(::robot_interfaces::srv::RemoveObstacle_Request::_name_type arg)
  {
    msg_.name = std::move(arg);
    return std::move(msg_);
  }

private:
  ::robot_interfaces::srv::RemoveObstacle_Request msg_;
};

}  // namespace builder

}  // namespace srv

template<typename MessageType>
auto build();

template<>
inline
auto build<::robot_interfaces::srv::RemoveObstacle_Request>()
{
  return robot_interfaces::srv::builder::Init_RemoveObstacle_Request_name();
}

}  // namespace robot_interfaces


namespace robot_interfaces
{

namespace srv
{

namespace builder
{

class Init_RemoveObstacle_Response_success
{
public:
  Init_RemoveObstacle_Response_success()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  ::robot_interfaces::srv::RemoveObstacle_Response success(::robot_interfaces::srv::RemoveObstacle_Response::_success_type arg)
  {
    msg_.success = std::move(arg);
    return std::move(msg_);
  }

private:
  ::robot_interfaces::srv::RemoveObstacle_Response msg_;
};

}  // namespace builder

}  // namespace srv

template<typename MessageType>
auto build();

template<>
inline
auto build<::robot_interfaces::srv::RemoveObstacle_Response>()
{
  return robot_interfaces::srv::builder::Init_RemoveObstacle_Response_success();
}

}  // namespace robot_interfaces


namespace robot_interfaces
{

namespace srv
{

namespace builder
{

class Init_RemoveObstacle_Event_response
{
public:
  explicit Init_RemoveObstacle_Event_response(::robot_interfaces::srv::RemoveObstacle_Event & msg)
  : msg_(msg)
  {}
  ::robot_interfaces::srv::RemoveObstacle_Event response(::robot_interfaces::srv::RemoveObstacle_Event::_response_type arg)
  {
    msg_.response = std::move(arg);
    return std::move(msg_);
  }

private:
  ::robot_interfaces::srv::RemoveObstacle_Event msg_;
};

class Init_RemoveObstacle_Event_request
{
public:
  explicit Init_RemoveObstacle_Event_request(::robot_interfaces::srv::RemoveObstacle_Event & msg)
  : msg_(msg)
  {}
  Init_RemoveObstacle_Event_response request(::robot_interfaces::srv::RemoveObstacle_Event::_request_type arg)
  {
    msg_.request = std::move(arg);
    return Init_RemoveObstacle_Event_response(msg_);
  }

private:
  ::robot_interfaces::srv::RemoveObstacle_Event msg_;
};

class Init_RemoveObstacle_Event_info
{
public:
  Init_RemoveObstacle_Event_info()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_RemoveObstacle_Event_request info(::robot_interfaces::srv::RemoveObstacle_Event::_info_type arg)
  {
    msg_.info = std::move(arg);
    return Init_RemoveObstacle_Event_request(msg_);
  }

private:
  ::robot_interfaces::srv::RemoveObstacle_Event msg_;
};

}  // namespace builder

}  // namespace srv

template<typename MessageType>
auto build();

template<>
inline
auto build<::robot_interfaces::srv::RemoveObstacle_Event>()
{
  return robot_interfaces::srv::builder::Init_RemoveObstacle_Event_info();
}

}  // namespace robot_interfaces

#endif  // ROBOT_INTERFACES__SRV__DETAIL__REMOVE_OBSTACLE__BUILDER_HPP_
