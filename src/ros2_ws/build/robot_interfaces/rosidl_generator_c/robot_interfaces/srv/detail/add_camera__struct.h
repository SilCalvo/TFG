// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from robot_interfaces:srv/AddCamera.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "robot_interfaces/srv/add_camera.h"


#ifndef ROBOT_INTERFACES__SRV__DETAIL__ADD_CAMERA__STRUCT_H_
#define ROBOT_INTERFACES__SRV__DETAIL__ADD_CAMERA__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

// Include directives for member types
// Member 'name'
#include "rosidl_runtime_c/string.h"

/// Struct defined in srv/AddCamera in the package robot_interfaces.
typedef struct robot_interfaces__srv__AddCamera_Request
{
  rosidl_runtime_c__String name;
  double x;
  double y;
  double z;
  double target_x;
  double target_y;
  double target_z;
  int32_t width;
  int32_t height;
} robot_interfaces__srv__AddCamera_Request;

// Struct for a sequence of robot_interfaces__srv__AddCamera_Request.
typedef struct robot_interfaces__srv__AddCamera_Request__Sequence
{
  robot_interfaces__srv__AddCamera_Request * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} robot_interfaces__srv__AddCamera_Request__Sequence;

// Constants defined in the message

/// Struct defined in srv/AddCamera in the package robot_interfaces.
typedef struct robot_interfaces__srv__AddCamera_Response
{
  bool success;
} robot_interfaces__srv__AddCamera_Response;

// Struct for a sequence of robot_interfaces__srv__AddCamera_Response.
typedef struct robot_interfaces__srv__AddCamera_Response__Sequence
{
  robot_interfaces__srv__AddCamera_Response * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} robot_interfaces__srv__AddCamera_Response__Sequence;

// Constants defined in the message

// Include directives for member types
// Member 'info'
#include "service_msgs/msg/detail/service_event_info__struct.h"

// constants for array fields with an upper bound
// request
enum
{
  robot_interfaces__srv__AddCamera_Event__request__MAX_SIZE = 1
};
// response
enum
{
  robot_interfaces__srv__AddCamera_Event__response__MAX_SIZE = 1
};

/// Struct defined in srv/AddCamera in the package robot_interfaces.
typedef struct robot_interfaces__srv__AddCamera_Event
{
  service_msgs__msg__ServiceEventInfo info;
  robot_interfaces__srv__AddCamera_Request__Sequence request;
  robot_interfaces__srv__AddCamera_Response__Sequence response;
} robot_interfaces__srv__AddCamera_Event;

// Struct for a sequence of robot_interfaces__srv__AddCamera_Event.
typedef struct robot_interfaces__srv__AddCamera_Event__Sequence
{
  robot_interfaces__srv__AddCamera_Event * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} robot_interfaces__srv__AddCamera_Event__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // ROBOT_INTERFACES__SRV__DETAIL__ADD_CAMERA__STRUCT_H_
