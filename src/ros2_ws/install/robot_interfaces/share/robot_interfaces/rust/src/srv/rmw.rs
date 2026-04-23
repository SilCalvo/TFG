#[cfg(feature = "serde")]
use serde::{Deserialize, Serialize};



#[link(name = "robot_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__robot_interfaces__srv__SolveIK_Request() -> *const std::ffi::c_void;
}

#[link(name = "robot_interfaces__rosidl_generator_c")]
extern "C" {
    fn robot_interfaces__srv__SolveIK_Request__init(msg: *mut SolveIK_Request) -> bool;
    fn robot_interfaces__srv__SolveIK_Request__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<SolveIK_Request>, size: usize) -> bool;
    fn robot_interfaces__srv__SolveIK_Request__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<SolveIK_Request>);
    fn robot_interfaces__srv__SolveIK_Request__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<SolveIK_Request>, out_seq: *mut rosidl_runtime_rs::Sequence<SolveIK_Request>) -> bool;
}

// Corresponds to robot_interfaces__srv__SolveIK_Request
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct SolveIK_Request {

    // This member is not documented.
    #[allow(missing_docs)]
    pub target_pose: geometry_msgs::msg::rmw::Pose,


    // This member is not documented.
    #[allow(missing_docs)]
    pub tcp_offset: geometry_msgs::msg::rmw::Pose,


    // This member is not documented.
    #[allow(missing_docs)]
    pub tool_dimensions: rosidl_runtime_rs::Sequence<f64>,


    // This member is not documented.
    #[allow(missing_docs)]
    pub tool_type: i32,

}



impl Default for SolveIK_Request {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !robot_interfaces__srv__SolveIK_Request__init(&mut msg as *mut _) {
        panic!("Call to robot_interfaces__srv__SolveIK_Request__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for SolveIK_Request {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { robot_interfaces__srv__SolveIK_Request__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { robot_interfaces__srv__SolveIK_Request__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { robot_interfaces__srv__SolveIK_Request__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for SolveIK_Request {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for SolveIK_Request where Self: Sized {
  const TYPE_NAME: &'static str = "robot_interfaces/srv/SolveIK_Request";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__robot_interfaces__srv__SolveIK_Request() }
  }
}


#[link(name = "robot_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__robot_interfaces__srv__SolveIK_Response() -> *const std::ffi::c_void;
}

#[link(name = "robot_interfaces__rosidl_generator_c")]
extern "C" {
    fn robot_interfaces__srv__SolveIK_Response__init(msg: *mut SolveIK_Response) -> bool;
    fn robot_interfaces__srv__SolveIK_Response__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<SolveIK_Response>, size: usize) -> bool;
    fn robot_interfaces__srv__SolveIK_Response__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<SolveIK_Response>);
    fn robot_interfaces__srv__SolveIK_Response__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<SolveIK_Response>, out_seq: *mut rosidl_runtime_rs::Sequence<SolveIK_Response>) -> bool;
}

// Corresponds to robot_interfaces__srv__SolveIK_Response
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct SolveIK_Response {

    // This member is not documented.
    #[allow(missing_docs)]
    pub joint_angles: rosidl_runtime_rs::Sequence<f64>,


    // This member is not documented.
    #[allow(missing_docs)]
    pub success: bool,

}



impl Default for SolveIK_Response {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !robot_interfaces__srv__SolveIK_Response__init(&mut msg as *mut _) {
        panic!("Call to robot_interfaces__srv__SolveIK_Response__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for SolveIK_Response {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { robot_interfaces__srv__SolveIK_Response__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { robot_interfaces__srv__SolveIK_Response__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { robot_interfaces__srv__SolveIK_Response__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for SolveIK_Response {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for SolveIK_Response where Self: Sized {
  const TYPE_NAME: &'static str = "robot_interfaces/srv/SolveIK_Response";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__robot_interfaces__srv__SolveIK_Response() }
  }
}


#[link(name = "robot_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__robot_interfaces__srv__AddObstacle_Request() -> *const std::ffi::c_void;
}

#[link(name = "robot_interfaces__rosidl_generator_c")]
extern "C" {
    fn robot_interfaces__srv__AddObstacle_Request__init(msg: *mut AddObstacle_Request) -> bool;
    fn robot_interfaces__srv__AddObstacle_Request__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<AddObstacle_Request>, size: usize) -> bool;
    fn robot_interfaces__srv__AddObstacle_Request__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<AddObstacle_Request>);
    fn robot_interfaces__srv__AddObstacle_Request__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<AddObstacle_Request>, out_seq: *mut rosidl_runtime_rs::Sequence<AddObstacle_Request>) -> bool;
}

// Corresponds to robot_interfaces__srv__AddObstacle_Request
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct AddObstacle_Request {

    // This member is not documented.
    #[allow(missing_docs)]
    pub name: rosidl_runtime_rs::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub x: f64,


    // This member is not documented.
    #[allow(missing_docs)]
    pub y: f64,


    // This member is not documented.
    #[allow(missing_docs)]
    pub z: f64,


    // This member is not documented.
    #[allow(missing_docs)]
    pub width: f64,


    // This member is not documented.
    #[allow(missing_docs)]
    pub height: f64,


    // This member is not documented.
    #[allow(missing_docs)]
    pub depth: f64,

}



impl Default for AddObstacle_Request {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !robot_interfaces__srv__AddObstacle_Request__init(&mut msg as *mut _) {
        panic!("Call to robot_interfaces__srv__AddObstacle_Request__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for AddObstacle_Request {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { robot_interfaces__srv__AddObstacle_Request__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { robot_interfaces__srv__AddObstacle_Request__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { robot_interfaces__srv__AddObstacle_Request__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for AddObstacle_Request {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for AddObstacle_Request where Self: Sized {
  const TYPE_NAME: &'static str = "robot_interfaces/srv/AddObstacle_Request";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__robot_interfaces__srv__AddObstacle_Request() }
  }
}


#[link(name = "robot_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__robot_interfaces__srv__AddObstacle_Response() -> *const std::ffi::c_void;
}

#[link(name = "robot_interfaces__rosidl_generator_c")]
extern "C" {
    fn robot_interfaces__srv__AddObstacle_Response__init(msg: *mut AddObstacle_Response) -> bool;
    fn robot_interfaces__srv__AddObstacle_Response__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<AddObstacle_Response>, size: usize) -> bool;
    fn robot_interfaces__srv__AddObstacle_Response__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<AddObstacle_Response>);
    fn robot_interfaces__srv__AddObstacle_Response__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<AddObstacle_Response>, out_seq: *mut rosidl_runtime_rs::Sequence<AddObstacle_Response>) -> bool;
}

// Corresponds to robot_interfaces__srv__AddObstacle_Response
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct AddObstacle_Response {

    // This member is not documented.
    #[allow(missing_docs)]
    pub success: bool,

}



impl Default for AddObstacle_Response {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !robot_interfaces__srv__AddObstacle_Response__init(&mut msg as *mut _) {
        panic!("Call to robot_interfaces__srv__AddObstacle_Response__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for AddObstacle_Response {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { robot_interfaces__srv__AddObstacle_Response__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { robot_interfaces__srv__AddObstacle_Response__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { robot_interfaces__srv__AddObstacle_Response__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for AddObstacle_Response {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for AddObstacle_Response where Self: Sized {
  const TYPE_NAME: &'static str = "robot_interfaces/srv/AddObstacle_Response";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__robot_interfaces__srv__AddObstacle_Response() }
  }
}


#[link(name = "robot_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__robot_interfaces__srv__RemoveObstacle_Request() -> *const std::ffi::c_void;
}

#[link(name = "robot_interfaces__rosidl_generator_c")]
extern "C" {
    fn robot_interfaces__srv__RemoveObstacle_Request__init(msg: *mut RemoveObstacle_Request) -> bool;
    fn robot_interfaces__srv__RemoveObstacle_Request__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<RemoveObstacle_Request>, size: usize) -> bool;
    fn robot_interfaces__srv__RemoveObstacle_Request__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<RemoveObstacle_Request>);
    fn robot_interfaces__srv__RemoveObstacle_Request__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<RemoveObstacle_Request>, out_seq: *mut rosidl_runtime_rs::Sequence<RemoveObstacle_Request>) -> bool;
}

// Corresponds to robot_interfaces__srv__RemoveObstacle_Request
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct RemoveObstacle_Request {

    // This member is not documented.
    #[allow(missing_docs)]
    pub name: rosidl_runtime_rs::String,

}



impl Default for RemoveObstacle_Request {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !robot_interfaces__srv__RemoveObstacle_Request__init(&mut msg as *mut _) {
        panic!("Call to robot_interfaces__srv__RemoveObstacle_Request__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for RemoveObstacle_Request {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { robot_interfaces__srv__RemoveObstacle_Request__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { robot_interfaces__srv__RemoveObstacle_Request__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { robot_interfaces__srv__RemoveObstacle_Request__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for RemoveObstacle_Request {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for RemoveObstacle_Request where Self: Sized {
  const TYPE_NAME: &'static str = "robot_interfaces/srv/RemoveObstacle_Request";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__robot_interfaces__srv__RemoveObstacle_Request() }
  }
}


#[link(name = "robot_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__robot_interfaces__srv__RemoveObstacle_Response() -> *const std::ffi::c_void;
}

#[link(name = "robot_interfaces__rosidl_generator_c")]
extern "C" {
    fn robot_interfaces__srv__RemoveObstacle_Response__init(msg: *mut RemoveObstacle_Response) -> bool;
    fn robot_interfaces__srv__RemoveObstacle_Response__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<RemoveObstacle_Response>, size: usize) -> bool;
    fn robot_interfaces__srv__RemoveObstacle_Response__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<RemoveObstacle_Response>);
    fn robot_interfaces__srv__RemoveObstacle_Response__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<RemoveObstacle_Response>, out_seq: *mut rosidl_runtime_rs::Sequence<RemoveObstacle_Response>) -> bool;
}

// Corresponds to robot_interfaces__srv__RemoveObstacle_Response
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct RemoveObstacle_Response {

    // This member is not documented.
    #[allow(missing_docs)]
    pub success: bool,

}



impl Default for RemoveObstacle_Response {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !robot_interfaces__srv__RemoveObstacle_Response__init(&mut msg as *mut _) {
        panic!("Call to robot_interfaces__srv__RemoveObstacle_Response__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for RemoveObstacle_Response {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { robot_interfaces__srv__RemoveObstacle_Response__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { robot_interfaces__srv__RemoveObstacle_Response__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { robot_interfaces__srv__RemoveObstacle_Response__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for RemoveObstacle_Response {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for RemoveObstacle_Response where Self: Sized {
  const TYPE_NAME: &'static str = "robot_interfaces/srv/RemoveObstacle_Response";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__robot_interfaces__srv__RemoveObstacle_Response() }
  }
}


#[link(name = "robot_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__robot_interfaces__srv__SolveDK_Request() -> *const std::ffi::c_void;
}

#[link(name = "robot_interfaces__rosidl_generator_c")]
extern "C" {
    fn robot_interfaces__srv__SolveDK_Request__init(msg: *mut SolveDK_Request) -> bool;
    fn robot_interfaces__srv__SolveDK_Request__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<SolveDK_Request>, size: usize) -> bool;
    fn robot_interfaces__srv__SolveDK_Request__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<SolveDK_Request>);
    fn robot_interfaces__srv__SolveDK_Request__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<SolveDK_Request>, out_seq: *mut rosidl_runtime_rs::Sequence<SolveDK_Request>) -> bool;
}

// Corresponds to robot_interfaces__srv__SolveDK_Request
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct SolveDK_Request {

    // This member is not documented.
    #[allow(missing_docs)]
    pub joint_angles: rosidl_runtime_rs::Sequence<f64>,


    // This member is not documented.
    #[allow(missing_docs)]
    pub tcp_offset: geometry_msgs::msg::rmw::Pose,


    // This member is not documented.
    #[allow(missing_docs)]
    pub tool_dimensions: rosidl_runtime_rs::Sequence<f64>,

}



impl Default for SolveDK_Request {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !robot_interfaces__srv__SolveDK_Request__init(&mut msg as *mut _) {
        panic!("Call to robot_interfaces__srv__SolveDK_Request__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for SolveDK_Request {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { robot_interfaces__srv__SolveDK_Request__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { robot_interfaces__srv__SolveDK_Request__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { robot_interfaces__srv__SolveDK_Request__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for SolveDK_Request {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for SolveDK_Request where Self: Sized {
  const TYPE_NAME: &'static str = "robot_interfaces/srv/SolveDK_Request";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__robot_interfaces__srv__SolveDK_Request() }
  }
}


#[link(name = "robot_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__robot_interfaces__srv__SolveDK_Response() -> *const std::ffi::c_void;
}

#[link(name = "robot_interfaces__rosidl_generator_c")]
extern "C" {
    fn robot_interfaces__srv__SolveDK_Response__init(msg: *mut SolveDK_Response) -> bool;
    fn robot_interfaces__srv__SolveDK_Response__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<SolveDK_Response>, size: usize) -> bool;
    fn robot_interfaces__srv__SolveDK_Response__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<SolveDK_Response>);
    fn robot_interfaces__srv__SolveDK_Response__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<SolveDK_Response>, out_seq: *mut rosidl_runtime_rs::Sequence<SolveDK_Response>) -> bool;
}

// Corresponds to robot_interfaces__srv__SolveDK_Response
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct SolveDK_Response {

    // This member is not documented.
    #[allow(missing_docs)]
    pub target_pose: geometry_msgs::msg::rmw::Pose,


    // This member is not documented.
    #[allow(missing_docs)]
    pub success: bool,

}



impl Default for SolveDK_Response {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !robot_interfaces__srv__SolveDK_Response__init(&mut msg as *mut _) {
        panic!("Call to robot_interfaces__srv__SolveDK_Response__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for SolveDK_Response {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { robot_interfaces__srv__SolveDK_Response__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { robot_interfaces__srv__SolveDK_Response__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { robot_interfaces__srv__SolveDK_Response__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for SolveDK_Response {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for SolveDK_Response where Self: Sized {
  const TYPE_NAME: &'static str = "robot_interfaces/srv/SolveDK_Response";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__robot_interfaces__srv__SolveDK_Response() }
  }
}


#[link(name = "robot_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__robot_interfaces__srv__ManageTool_Request() -> *const std::ffi::c_void;
}

#[link(name = "robot_interfaces__rosidl_generator_c")]
extern "C" {
    fn robot_interfaces__srv__ManageTool_Request__init(msg: *mut ManageTool_Request) -> bool;
    fn robot_interfaces__srv__ManageTool_Request__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<ManageTool_Request>, size: usize) -> bool;
    fn robot_interfaces__srv__ManageTool_Request__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<ManageTool_Request>);
    fn robot_interfaces__srv__ManageTool_Request__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<ManageTool_Request>, out_seq: *mut rosidl_runtime_rs::Sequence<ManageTool_Request>) -> bool;
}

// Corresponds to robot_interfaces__srv__ManageTool_Request
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct ManageTool_Request {

    // This member is not documented.
    #[allow(missing_docs)]
    pub name: rosidl_runtime_rs::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub type_: i32,


    // This member is not documented.
    #[allow(missing_docs)]
    pub dimensions: rosidl_runtime_rs::Sequence<f64>,


    // This member is not documented.
    #[allow(missing_docs)]
    pub offset: geometry_msgs::msg::rmw::Pose,

}



impl Default for ManageTool_Request {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !robot_interfaces__srv__ManageTool_Request__init(&mut msg as *mut _) {
        panic!("Call to robot_interfaces__srv__ManageTool_Request__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for ManageTool_Request {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { robot_interfaces__srv__ManageTool_Request__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { robot_interfaces__srv__ManageTool_Request__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { robot_interfaces__srv__ManageTool_Request__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for ManageTool_Request {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for ManageTool_Request where Self: Sized {
  const TYPE_NAME: &'static str = "robot_interfaces/srv/ManageTool_Request";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__robot_interfaces__srv__ManageTool_Request() }
  }
}


#[link(name = "robot_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__robot_interfaces__srv__ManageTool_Response() -> *const std::ffi::c_void;
}

#[link(name = "robot_interfaces__rosidl_generator_c")]
extern "C" {
    fn robot_interfaces__srv__ManageTool_Response__init(msg: *mut ManageTool_Response) -> bool;
    fn robot_interfaces__srv__ManageTool_Response__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<ManageTool_Response>, size: usize) -> bool;
    fn robot_interfaces__srv__ManageTool_Response__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<ManageTool_Response>);
    fn robot_interfaces__srv__ManageTool_Response__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<ManageTool_Response>, out_seq: *mut rosidl_runtime_rs::Sequence<ManageTool_Response>) -> bool;
}

// Corresponds to robot_interfaces__srv__ManageTool_Response
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct ManageTool_Response {

    // This member is not documented.
    #[allow(missing_docs)]
    pub success: bool,


    // This member is not documented.
    #[allow(missing_docs)]
    pub message: rosidl_runtime_rs::String,

}



impl Default for ManageTool_Response {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !robot_interfaces__srv__ManageTool_Response__init(&mut msg as *mut _) {
        panic!("Call to robot_interfaces__srv__ManageTool_Response__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for ManageTool_Response {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { robot_interfaces__srv__ManageTool_Response__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { robot_interfaces__srv__ManageTool_Response__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { robot_interfaces__srv__ManageTool_Response__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for ManageTool_Response {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for ManageTool_Response where Self: Sized {
  const TYPE_NAME: &'static str = "robot_interfaces/srv/ManageTool_Response";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__robot_interfaces__srv__ManageTool_Response() }
  }
}


#[link(name = "robot_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__robot_interfaces__srv__SpawnObject_Request() -> *const std::ffi::c_void;
}

#[link(name = "robot_interfaces__rosidl_generator_c")]
extern "C" {
    fn robot_interfaces__srv__SpawnObject_Request__init(msg: *mut SpawnObject_Request) -> bool;
    fn robot_interfaces__srv__SpawnObject_Request__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<SpawnObject_Request>, size: usize) -> bool;
    fn robot_interfaces__srv__SpawnObject_Request__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<SpawnObject_Request>);
    fn robot_interfaces__srv__SpawnObject_Request__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<SpawnObject_Request>, out_seq: *mut rosidl_runtime_rs::Sequence<SpawnObject_Request>) -> bool;
}

// Corresponds to robot_interfaces__srv__SpawnObject_Request
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct SpawnObject_Request {

    // This member is not documented.
    #[allow(missing_docs)]
    pub name: rosidl_runtime_rs::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub package_name: rosidl_runtime_rs::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub piece_type: rosidl_runtime_rs::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub x: f64,


    // This member is not documented.
    #[allow(missing_docs)]
    pub y: f64,


    // This member is not documented.
    #[allow(missing_docs)]
    pub z: f64,

}



impl Default for SpawnObject_Request {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !robot_interfaces__srv__SpawnObject_Request__init(&mut msg as *mut _) {
        panic!("Call to robot_interfaces__srv__SpawnObject_Request__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for SpawnObject_Request {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { robot_interfaces__srv__SpawnObject_Request__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { robot_interfaces__srv__SpawnObject_Request__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { robot_interfaces__srv__SpawnObject_Request__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for SpawnObject_Request {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for SpawnObject_Request where Self: Sized {
  const TYPE_NAME: &'static str = "robot_interfaces/srv/SpawnObject_Request";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__robot_interfaces__srv__SpawnObject_Request() }
  }
}


#[link(name = "robot_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__robot_interfaces__srv__SpawnObject_Response() -> *const std::ffi::c_void;
}

#[link(name = "robot_interfaces__rosidl_generator_c")]
extern "C" {
    fn robot_interfaces__srv__SpawnObject_Response__init(msg: *mut SpawnObject_Response) -> bool;
    fn robot_interfaces__srv__SpawnObject_Response__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<SpawnObject_Response>, size: usize) -> bool;
    fn robot_interfaces__srv__SpawnObject_Response__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<SpawnObject_Response>);
    fn robot_interfaces__srv__SpawnObject_Response__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<SpawnObject_Response>, out_seq: *mut rosidl_runtime_rs::Sequence<SpawnObject_Response>) -> bool;
}

// Corresponds to robot_interfaces__srv__SpawnObject_Response
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct SpawnObject_Response {

    // This member is not documented.
    #[allow(missing_docs)]
    pub success: bool,

}



impl Default for SpawnObject_Response {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !robot_interfaces__srv__SpawnObject_Response__init(&mut msg as *mut _) {
        panic!("Call to robot_interfaces__srv__SpawnObject_Response__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for SpawnObject_Response {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { robot_interfaces__srv__SpawnObject_Response__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { robot_interfaces__srv__SpawnObject_Response__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { robot_interfaces__srv__SpawnObject_Response__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for SpawnObject_Response {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for SpawnObject_Response where Self: Sized {
  const TYPE_NAME: &'static str = "robot_interfaces/srv/SpawnObject_Response";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__robot_interfaces__srv__SpawnObject_Response() }
  }
}


#[link(name = "robot_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__robot_interfaces__srv__AddCamera_Request() -> *const std::ffi::c_void;
}

#[link(name = "robot_interfaces__rosidl_generator_c")]
extern "C" {
    fn robot_interfaces__srv__AddCamera_Request__init(msg: *mut AddCamera_Request) -> bool;
    fn robot_interfaces__srv__AddCamera_Request__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<AddCamera_Request>, size: usize) -> bool;
    fn robot_interfaces__srv__AddCamera_Request__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<AddCamera_Request>);
    fn robot_interfaces__srv__AddCamera_Request__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<AddCamera_Request>, out_seq: *mut rosidl_runtime_rs::Sequence<AddCamera_Request>) -> bool;
}

// Corresponds to robot_interfaces__srv__AddCamera_Request
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct AddCamera_Request {

    // This member is not documented.
    #[allow(missing_docs)]
    pub name: rosidl_runtime_rs::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub x: f64,


    // This member is not documented.
    #[allow(missing_docs)]
    pub y: f64,


    // This member is not documented.
    #[allow(missing_docs)]
    pub z: f64,


    // This member is not documented.
    #[allow(missing_docs)]
    pub target_x: f64,


    // This member is not documented.
    #[allow(missing_docs)]
    pub target_y: f64,


    // This member is not documented.
    #[allow(missing_docs)]
    pub target_z: f64,


    // This member is not documented.
    #[allow(missing_docs)]
    pub width: i32,


    // This member is not documented.
    #[allow(missing_docs)]
    pub height: i32,

}



impl Default for AddCamera_Request {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !robot_interfaces__srv__AddCamera_Request__init(&mut msg as *mut _) {
        panic!("Call to robot_interfaces__srv__AddCamera_Request__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for AddCamera_Request {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { robot_interfaces__srv__AddCamera_Request__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { robot_interfaces__srv__AddCamera_Request__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { robot_interfaces__srv__AddCamera_Request__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for AddCamera_Request {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for AddCamera_Request where Self: Sized {
  const TYPE_NAME: &'static str = "robot_interfaces/srv/AddCamera_Request";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__robot_interfaces__srv__AddCamera_Request() }
  }
}


#[link(name = "robot_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__robot_interfaces__srv__AddCamera_Response() -> *const std::ffi::c_void;
}

#[link(name = "robot_interfaces__rosidl_generator_c")]
extern "C" {
    fn robot_interfaces__srv__AddCamera_Response__init(msg: *mut AddCamera_Response) -> bool;
    fn robot_interfaces__srv__AddCamera_Response__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<AddCamera_Response>, size: usize) -> bool;
    fn robot_interfaces__srv__AddCamera_Response__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<AddCamera_Response>);
    fn robot_interfaces__srv__AddCamera_Response__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<AddCamera_Response>, out_seq: *mut rosidl_runtime_rs::Sequence<AddCamera_Response>) -> bool;
}

// Corresponds to robot_interfaces__srv__AddCamera_Response
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct AddCamera_Response {

    // This member is not documented.
    #[allow(missing_docs)]
    pub success: bool,

}



impl Default for AddCamera_Response {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !robot_interfaces__srv__AddCamera_Response__init(&mut msg as *mut _) {
        panic!("Call to robot_interfaces__srv__AddCamera_Response__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for AddCamera_Response {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { robot_interfaces__srv__AddCamera_Response__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { robot_interfaces__srv__AddCamera_Response__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { robot_interfaces__srv__AddCamera_Response__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for AddCamera_Response {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for AddCamera_Response where Self: Sized {
  const TYPE_NAME: &'static str = "robot_interfaces/srv/AddCamera_Response";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__robot_interfaces__srv__AddCamera_Response() }
  }
}


#[link(name = "robot_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__robot_interfaces__srv__PutPiece_Request() -> *const std::ffi::c_void;
}

#[link(name = "robot_interfaces__rosidl_generator_c")]
extern "C" {
    fn robot_interfaces__srv__PutPiece_Request__init(msg: *mut PutPiece_Request) -> bool;
    fn robot_interfaces__srv__PutPiece_Request__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<PutPiece_Request>, size: usize) -> bool;
    fn robot_interfaces__srv__PutPiece_Request__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<PutPiece_Request>);
    fn robot_interfaces__srv__PutPiece_Request__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<PutPiece_Request>, out_seq: *mut rosidl_runtime_rs::Sequence<PutPiece_Request>) -> bool;
}

// Corresponds to robot_interfaces__srv__PutPiece_Request
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct PutPiece_Request {

    // This member is not documented.
    #[allow(missing_docs)]
    pub fila: i32,


    // This member is not documented.
    #[allow(missing_docs)]
    pub columna: i32,

}



impl Default for PutPiece_Request {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !robot_interfaces__srv__PutPiece_Request__init(&mut msg as *mut _) {
        panic!("Call to robot_interfaces__srv__PutPiece_Request__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for PutPiece_Request {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { robot_interfaces__srv__PutPiece_Request__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { robot_interfaces__srv__PutPiece_Request__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { robot_interfaces__srv__PutPiece_Request__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for PutPiece_Request {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for PutPiece_Request where Self: Sized {
  const TYPE_NAME: &'static str = "robot_interfaces/srv/PutPiece_Request";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__robot_interfaces__srv__PutPiece_Request() }
  }
}


#[link(name = "robot_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__robot_interfaces__srv__PutPiece_Response() -> *const std::ffi::c_void;
}

#[link(name = "robot_interfaces__rosidl_generator_c")]
extern "C" {
    fn robot_interfaces__srv__PutPiece_Response__init(msg: *mut PutPiece_Response) -> bool;
    fn robot_interfaces__srv__PutPiece_Response__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<PutPiece_Response>, size: usize) -> bool;
    fn robot_interfaces__srv__PutPiece_Response__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<PutPiece_Response>);
    fn robot_interfaces__srv__PutPiece_Response__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<PutPiece_Response>, out_seq: *mut rosidl_runtime_rs::Sequence<PutPiece_Response>) -> bool;
}

// Corresponds to robot_interfaces__srv__PutPiece_Response
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct PutPiece_Response {

    // This member is not documented.
    #[allow(missing_docs)]
    pub success: bool,


    // This member is not documented.
    #[allow(missing_docs)]
    pub message: rosidl_runtime_rs::String,

}



impl Default for PutPiece_Response {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !robot_interfaces__srv__PutPiece_Response__init(&mut msg as *mut _) {
        panic!("Call to robot_interfaces__srv__PutPiece_Response__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for PutPiece_Response {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { robot_interfaces__srv__PutPiece_Response__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { robot_interfaces__srv__PutPiece_Response__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { robot_interfaces__srv__PutPiece_Response__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for PutPiece_Response {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for PutPiece_Response where Self: Sized {
  const TYPE_NAME: &'static str = "robot_interfaces/srv/PutPiece_Response";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__robot_interfaces__srv__PutPiece_Response() }
  }
}






#[link(name = "robot_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_service_type_support_handle__robot_interfaces__srv__SolveIK() -> *const std::ffi::c_void;
}

// Corresponds to robot_interfaces__srv__SolveIK
#[allow(missing_docs, non_camel_case_types)]
pub struct SolveIK;

impl rosidl_runtime_rs::Service for SolveIK {
    type Request = SolveIK_Request;
    type Response = SolveIK_Response;

    fn get_type_support() -> *const std::ffi::c_void {
        // SAFETY: No preconditions for this function.
        unsafe { rosidl_typesupport_c__get_service_type_support_handle__robot_interfaces__srv__SolveIK() }
    }
}




#[link(name = "robot_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_service_type_support_handle__robot_interfaces__srv__AddObstacle() -> *const std::ffi::c_void;
}

// Corresponds to robot_interfaces__srv__AddObstacle
#[allow(missing_docs, non_camel_case_types)]
pub struct AddObstacle;

impl rosidl_runtime_rs::Service for AddObstacle {
    type Request = AddObstacle_Request;
    type Response = AddObstacle_Response;

    fn get_type_support() -> *const std::ffi::c_void {
        // SAFETY: No preconditions for this function.
        unsafe { rosidl_typesupport_c__get_service_type_support_handle__robot_interfaces__srv__AddObstacle() }
    }
}




#[link(name = "robot_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_service_type_support_handle__robot_interfaces__srv__RemoveObstacle() -> *const std::ffi::c_void;
}

// Corresponds to robot_interfaces__srv__RemoveObstacle
#[allow(missing_docs, non_camel_case_types)]
pub struct RemoveObstacle;

impl rosidl_runtime_rs::Service for RemoveObstacle {
    type Request = RemoveObstacle_Request;
    type Response = RemoveObstacle_Response;

    fn get_type_support() -> *const std::ffi::c_void {
        // SAFETY: No preconditions for this function.
        unsafe { rosidl_typesupport_c__get_service_type_support_handle__robot_interfaces__srv__RemoveObstacle() }
    }
}




#[link(name = "robot_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_service_type_support_handle__robot_interfaces__srv__SolveDK() -> *const std::ffi::c_void;
}

// Corresponds to robot_interfaces__srv__SolveDK
#[allow(missing_docs, non_camel_case_types)]
pub struct SolveDK;

impl rosidl_runtime_rs::Service for SolveDK {
    type Request = SolveDK_Request;
    type Response = SolveDK_Response;

    fn get_type_support() -> *const std::ffi::c_void {
        // SAFETY: No preconditions for this function.
        unsafe { rosidl_typesupport_c__get_service_type_support_handle__robot_interfaces__srv__SolveDK() }
    }
}




#[link(name = "robot_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_service_type_support_handle__robot_interfaces__srv__ManageTool() -> *const std::ffi::c_void;
}

// Corresponds to robot_interfaces__srv__ManageTool
#[allow(missing_docs, non_camel_case_types)]
pub struct ManageTool;

impl rosidl_runtime_rs::Service for ManageTool {
    type Request = ManageTool_Request;
    type Response = ManageTool_Response;

    fn get_type_support() -> *const std::ffi::c_void {
        // SAFETY: No preconditions for this function.
        unsafe { rosidl_typesupport_c__get_service_type_support_handle__robot_interfaces__srv__ManageTool() }
    }
}




#[link(name = "robot_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_service_type_support_handle__robot_interfaces__srv__SpawnObject() -> *const std::ffi::c_void;
}

// Corresponds to robot_interfaces__srv__SpawnObject
#[allow(missing_docs, non_camel_case_types)]
pub struct SpawnObject;

impl rosidl_runtime_rs::Service for SpawnObject {
    type Request = SpawnObject_Request;
    type Response = SpawnObject_Response;

    fn get_type_support() -> *const std::ffi::c_void {
        // SAFETY: No preconditions for this function.
        unsafe { rosidl_typesupport_c__get_service_type_support_handle__robot_interfaces__srv__SpawnObject() }
    }
}




#[link(name = "robot_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_service_type_support_handle__robot_interfaces__srv__AddCamera() -> *const std::ffi::c_void;
}

// Corresponds to robot_interfaces__srv__AddCamera
#[allow(missing_docs, non_camel_case_types)]
pub struct AddCamera;

impl rosidl_runtime_rs::Service for AddCamera {
    type Request = AddCamera_Request;
    type Response = AddCamera_Response;

    fn get_type_support() -> *const std::ffi::c_void {
        // SAFETY: No preconditions for this function.
        unsafe { rosidl_typesupport_c__get_service_type_support_handle__robot_interfaces__srv__AddCamera() }
    }
}




#[link(name = "robot_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_service_type_support_handle__robot_interfaces__srv__PutPiece() -> *const std::ffi::c_void;
}

// Corresponds to robot_interfaces__srv__PutPiece
#[allow(missing_docs, non_camel_case_types)]
pub struct PutPiece;

impl rosidl_runtime_rs::Service for PutPiece {
    type Request = PutPiece_Request;
    type Response = PutPiece_Response;

    fn get_type_support() -> *const std::ffi::c_void {
        // SAFETY: No preconditions for this function.
        unsafe { rosidl_typesupport_c__get_service_type_support_handle__robot_interfaces__srv__PutPiece() }
    }
}


