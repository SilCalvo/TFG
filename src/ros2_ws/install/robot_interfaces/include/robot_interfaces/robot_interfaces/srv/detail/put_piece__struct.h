// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from robot_interfaces:srv/PutPiece.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "robot_interfaces/srv/put_piece.h"


#ifndef ROBOT_INTERFACES__SRV__DETAIL__PUT_PIECE__STRUCT_H_
#define ROBOT_INTERFACES__SRV__DETAIL__PUT_PIECE__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

/// Struct defined in srv/PutPiece in the package robot_interfaces.
typedef struct robot_interfaces__srv__PutPiece_Request
{
  int32_t fila;
  int32_t columna;
} robot_interfaces__srv__PutPiece_Request;

// Struct for a sequence of robot_interfaces__srv__PutPiece_Request.
typedef struct robot_interfaces__srv__PutPiece_Request__Sequence
{
  robot_interfaces__srv__PutPiece_Request * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} robot_interfaces__srv__PutPiece_Request__Sequence;

// Constants defined in the message

// Include directives for member types
// Member 'message'
#include "rosidl_runtime_c/string.h"

/// Struct defined in srv/PutPiece in the package robot_interfaces.
typedef struct robot_interfaces__srv__PutPiece_Response
{
  bool success;
  rosidl_runtime_c__String message;
} robot_interfaces__srv__PutPiece_Response;

// Struct for a sequence of robot_interfaces__srv__PutPiece_Response.
typedef struct robot_interfaces__srv__PutPiece_Response__Sequence
{
  robot_interfaces__srv__PutPiece_Response * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} robot_interfaces__srv__PutPiece_Response__Sequence;

// Constants defined in the message

// Include directives for member types
// Member 'info'
#include "service_msgs/msg/detail/service_event_info__struct.h"

// constants for array fields with an upper bound
// request
enum
{
  robot_interfaces__srv__PutPiece_Event__request__MAX_SIZE = 1
};
// response
enum
{
  robot_interfaces__srv__PutPiece_Event__response__MAX_SIZE = 1
};

/// Struct defined in srv/PutPiece in the package robot_interfaces.
typedef struct robot_interfaces__srv__PutPiece_Event
{
  service_msgs__msg__ServiceEventInfo info;
  robot_interfaces__srv__PutPiece_Request__Sequence request;
  robot_interfaces__srv__PutPiece_Response__Sequence response;
} robot_interfaces__srv__PutPiece_Event;

// Struct for a sequence of robot_interfaces__srv__PutPiece_Event.
typedef struct robot_interfaces__srv__PutPiece_Event__Sequence
{
  robot_interfaces__srv__PutPiece_Event * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} robot_interfaces__srv__PutPiece_Event__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // ROBOT_INTERFACES__SRV__DETAIL__PUT_PIECE__STRUCT_H_
