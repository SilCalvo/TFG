// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from robot_interfaces:srv/SolveDK.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "robot_interfaces/srv/solve_dk.h"


#ifndef ROBOT_INTERFACES__SRV__DETAIL__SOLVE_DK__STRUCT_H_
#define ROBOT_INTERFACES__SRV__DETAIL__SOLVE_DK__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

// Include directives for member types
// Member 'joint_angles'
// Member 'tool_dimensions'
#include "rosidl_runtime_c/primitives_sequence.h"
// Member 'tcp_offset'
#include "geometry_msgs/msg/detail/pose__struct.h"

/// Struct defined in srv/SolveDK in the package robot_interfaces.
typedef struct robot_interfaces__srv__SolveDK_Request
{
  rosidl_runtime_c__double__Sequence joint_angles;
  geometry_msgs__msg__Pose tcp_offset;
  rosidl_runtime_c__double__Sequence tool_dimensions;
} robot_interfaces__srv__SolveDK_Request;

// Struct for a sequence of robot_interfaces__srv__SolveDK_Request.
typedef struct robot_interfaces__srv__SolveDK_Request__Sequence
{
  robot_interfaces__srv__SolveDK_Request * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} robot_interfaces__srv__SolveDK_Request__Sequence;

// Constants defined in the message

// Include directives for member types
// Member 'target_pose'
// already included above
// #include "geometry_msgs/msg/detail/pose__struct.h"

/// Struct defined in srv/SolveDK in the package robot_interfaces.
typedef struct robot_interfaces__srv__SolveDK_Response
{
  geometry_msgs__msg__Pose target_pose;
  bool success;
} robot_interfaces__srv__SolveDK_Response;

// Struct for a sequence of robot_interfaces__srv__SolveDK_Response.
typedef struct robot_interfaces__srv__SolveDK_Response__Sequence
{
  robot_interfaces__srv__SolveDK_Response * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} robot_interfaces__srv__SolveDK_Response__Sequence;

// Constants defined in the message

// Include directives for member types
// Member 'info'
#include "service_msgs/msg/detail/service_event_info__struct.h"

// constants for array fields with an upper bound
// request
enum
{
  robot_interfaces__srv__SolveDK_Event__request__MAX_SIZE = 1
};
// response
enum
{
  robot_interfaces__srv__SolveDK_Event__response__MAX_SIZE = 1
};

/// Struct defined in srv/SolveDK in the package robot_interfaces.
typedef struct robot_interfaces__srv__SolveDK_Event
{
  service_msgs__msg__ServiceEventInfo info;
  robot_interfaces__srv__SolveDK_Request__Sequence request;
  robot_interfaces__srv__SolveDK_Response__Sequence response;
} robot_interfaces__srv__SolveDK_Event;

// Struct for a sequence of robot_interfaces__srv__SolveDK_Event.
typedef struct robot_interfaces__srv__SolveDK_Event__Sequence
{
  robot_interfaces__srv__SolveDK_Event * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} robot_interfaces__srv__SolveDK_Event__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // ROBOT_INTERFACES__SRV__DETAIL__SOLVE_DK__STRUCT_H_
