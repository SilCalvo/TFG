#[cfg(feature = "serde")]
use serde::{Deserialize, Serialize};




// Corresponds to robot_interfaces__srv__SolveIK_Request

// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct SolveIK_Request {

    // This member is not documented.
    #[allow(missing_docs)]
    pub target_pose: geometry_msgs::msg::Pose,


    // This member is not documented.
    #[allow(missing_docs)]
    pub tcp_offset: geometry_msgs::msg::Pose,


    // This member is not documented.
    #[allow(missing_docs)]
    pub tool_dimensions: Vec<f64>,


    // This member is not documented.
    #[allow(missing_docs)]
    pub tool_type: i32,

}



impl Default for SolveIK_Request {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::srv::rmw::SolveIK_Request::default())
  }
}

impl rosidl_runtime_rs::Message for SolveIK_Request {
  type RmwMsg = super::srv::rmw::SolveIK_Request;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        target_pose: geometry_msgs::msg::Pose::into_rmw_message(std::borrow::Cow::Owned(msg.target_pose)).into_owned(),
        tcp_offset: geometry_msgs::msg::Pose::into_rmw_message(std::borrow::Cow::Owned(msg.tcp_offset)).into_owned(),
        tool_dimensions: msg.tool_dimensions.into(),
        tool_type: msg.tool_type,
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        target_pose: geometry_msgs::msg::Pose::into_rmw_message(std::borrow::Cow::Borrowed(&msg.target_pose)).into_owned(),
        tcp_offset: geometry_msgs::msg::Pose::into_rmw_message(std::borrow::Cow::Borrowed(&msg.tcp_offset)).into_owned(),
        tool_dimensions: msg.tool_dimensions.as_slice().into(),
      tool_type: msg.tool_type,
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      target_pose: geometry_msgs::msg::Pose::from_rmw_message(msg.target_pose),
      tcp_offset: geometry_msgs::msg::Pose::from_rmw_message(msg.tcp_offset),
      tool_dimensions: msg.tool_dimensions
          .into_iter()
          .collect(),
      tool_type: msg.tool_type,
    }
  }
}


// Corresponds to robot_interfaces__srv__SolveIK_Response

// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct SolveIK_Response {

    // This member is not documented.
    #[allow(missing_docs)]
    pub joint_angles: Vec<f64>,


    // This member is not documented.
    #[allow(missing_docs)]
    pub success: bool,

}



impl Default for SolveIK_Response {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::srv::rmw::SolveIK_Response::default())
  }
}

impl rosidl_runtime_rs::Message for SolveIK_Response {
  type RmwMsg = super::srv::rmw::SolveIK_Response;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        joint_angles: msg.joint_angles.into(),
        success: msg.success,
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        joint_angles: msg.joint_angles.as_slice().into(),
      success: msg.success,
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      joint_angles: msg.joint_angles
          .into_iter()
          .collect(),
      success: msg.success,
    }
  }
}


// Corresponds to robot_interfaces__srv__AddObstacle_Request

// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct AddObstacle_Request {

    // This member is not documented.
    #[allow(missing_docs)]
    pub name: std::string::String,


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
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::srv::rmw::AddObstacle_Request::default())
  }
}

impl rosidl_runtime_rs::Message for AddObstacle_Request {
  type RmwMsg = super::srv::rmw::AddObstacle_Request;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        name: msg.name.as_str().into(),
        x: msg.x,
        y: msg.y,
        z: msg.z,
        width: msg.width,
        height: msg.height,
        depth: msg.depth,
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        name: msg.name.as_str().into(),
      x: msg.x,
      y: msg.y,
      z: msg.z,
      width: msg.width,
      height: msg.height,
      depth: msg.depth,
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      name: msg.name.to_string(),
      x: msg.x,
      y: msg.y,
      z: msg.z,
      width: msg.width,
      height: msg.height,
      depth: msg.depth,
    }
  }
}


// Corresponds to robot_interfaces__srv__AddObstacle_Response

// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct AddObstacle_Response {

    // This member is not documented.
    #[allow(missing_docs)]
    pub success: bool,

}



impl Default for AddObstacle_Response {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::srv::rmw::AddObstacle_Response::default())
  }
}

impl rosidl_runtime_rs::Message for AddObstacle_Response {
  type RmwMsg = super::srv::rmw::AddObstacle_Response;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        success: msg.success,
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
      success: msg.success,
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      success: msg.success,
    }
  }
}


// Corresponds to robot_interfaces__srv__RemoveObstacle_Request

// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct RemoveObstacle_Request {

    // This member is not documented.
    #[allow(missing_docs)]
    pub name: std::string::String,

}



impl Default for RemoveObstacle_Request {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::srv::rmw::RemoveObstacle_Request::default())
  }
}

impl rosidl_runtime_rs::Message for RemoveObstacle_Request {
  type RmwMsg = super::srv::rmw::RemoveObstacle_Request;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        name: msg.name.as_str().into(),
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        name: msg.name.as_str().into(),
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      name: msg.name.to_string(),
    }
  }
}


// Corresponds to robot_interfaces__srv__RemoveObstacle_Response

// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct RemoveObstacle_Response {

    // This member is not documented.
    #[allow(missing_docs)]
    pub success: bool,

}



impl Default for RemoveObstacle_Response {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::srv::rmw::RemoveObstacle_Response::default())
  }
}

impl rosidl_runtime_rs::Message for RemoveObstacle_Response {
  type RmwMsg = super::srv::rmw::RemoveObstacle_Response;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        success: msg.success,
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
      success: msg.success,
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      success: msg.success,
    }
  }
}


// Corresponds to robot_interfaces__srv__SolveDK_Request

// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct SolveDK_Request {

    // This member is not documented.
    #[allow(missing_docs)]
    pub joint_angles: Vec<f64>,


    // This member is not documented.
    #[allow(missing_docs)]
    pub tcp_offset: geometry_msgs::msg::Pose,


    // This member is not documented.
    #[allow(missing_docs)]
    pub tool_dimensions: Vec<f64>,

}



impl Default for SolveDK_Request {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::srv::rmw::SolveDK_Request::default())
  }
}

impl rosidl_runtime_rs::Message for SolveDK_Request {
  type RmwMsg = super::srv::rmw::SolveDK_Request;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        joint_angles: msg.joint_angles.into(),
        tcp_offset: geometry_msgs::msg::Pose::into_rmw_message(std::borrow::Cow::Owned(msg.tcp_offset)).into_owned(),
        tool_dimensions: msg.tool_dimensions.into(),
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        joint_angles: msg.joint_angles.as_slice().into(),
        tcp_offset: geometry_msgs::msg::Pose::into_rmw_message(std::borrow::Cow::Borrowed(&msg.tcp_offset)).into_owned(),
        tool_dimensions: msg.tool_dimensions.as_slice().into(),
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      joint_angles: msg.joint_angles
          .into_iter()
          .collect(),
      tcp_offset: geometry_msgs::msg::Pose::from_rmw_message(msg.tcp_offset),
      tool_dimensions: msg.tool_dimensions
          .into_iter()
          .collect(),
    }
  }
}


// Corresponds to robot_interfaces__srv__SolveDK_Response

// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct SolveDK_Response {

    // This member is not documented.
    #[allow(missing_docs)]
    pub target_pose: geometry_msgs::msg::Pose,


    // This member is not documented.
    #[allow(missing_docs)]
    pub success: bool,

}



impl Default for SolveDK_Response {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::srv::rmw::SolveDK_Response::default())
  }
}

impl rosidl_runtime_rs::Message for SolveDK_Response {
  type RmwMsg = super::srv::rmw::SolveDK_Response;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        target_pose: geometry_msgs::msg::Pose::into_rmw_message(std::borrow::Cow::Owned(msg.target_pose)).into_owned(),
        success: msg.success,
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        target_pose: geometry_msgs::msg::Pose::into_rmw_message(std::borrow::Cow::Borrowed(&msg.target_pose)).into_owned(),
      success: msg.success,
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      target_pose: geometry_msgs::msg::Pose::from_rmw_message(msg.target_pose),
      success: msg.success,
    }
  }
}


// Corresponds to robot_interfaces__srv__ManageTool_Request

// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct ManageTool_Request {

    // This member is not documented.
    #[allow(missing_docs)]
    pub name: std::string::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub type_: i32,


    // This member is not documented.
    #[allow(missing_docs)]
    pub dimensions: Vec<f64>,


    // This member is not documented.
    #[allow(missing_docs)]
    pub offset: geometry_msgs::msg::Pose,

}



impl Default for ManageTool_Request {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::srv::rmw::ManageTool_Request::default())
  }
}

impl rosidl_runtime_rs::Message for ManageTool_Request {
  type RmwMsg = super::srv::rmw::ManageTool_Request;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        name: msg.name.as_str().into(),
        type_: msg.type_,
        dimensions: msg.dimensions.into(),
        offset: geometry_msgs::msg::Pose::into_rmw_message(std::borrow::Cow::Owned(msg.offset)).into_owned(),
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        name: msg.name.as_str().into(),
      type_: msg.type_,
        dimensions: msg.dimensions.as_slice().into(),
        offset: geometry_msgs::msg::Pose::into_rmw_message(std::borrow::Cow::Borrowed(&msg.offset)).into_owned(),
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      name: msg.name.to_string(),
      type_: msg.type_,
      dimensions: msg.dimensions
          .into_iter()
          .collect(),
      offset: geometry_msgs::msg::Pose::from_rmw_message(msg.offset),
    }
  }
}


// Corresponds to robot_interfaces__srv__ManageTool_Response

// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct ManageTool_Response {

    // This member is not documented.
    #[allow(missing_docs)]
    pub success: bool,


    // This member is not documented.
    #[allow(missing_docs)]
    pub message: std::string::String,

}



impl Default for ManageTool_Response {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::srv::rmw::ManageTool_Response::default())
  }
}

impl rosidl_runtime_rs::Message for ManageTool_Response {
  type RmwMsg = super::srv::rmw::ManageTool_Response;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        success: msg.success,
        message: msg.message.as_str().into(),
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
      success: msg.success,
        message: msg.message.as_str().into(),
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      success: msg.success,
      message: msg.message.to_string(),
    }
  }
}


// Corresponds to robot_interfaces__srv__SpawnObject_Request

// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct SpawnObject_Request {

    // This member is not documented.
    #[allow(missing_docs)]
    pub name: std::string::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub package_name: std::string::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub piece_type: std::string::String,


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
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::srv::rmw::SpawnObject_Request::default())
  }
}

impl rosidl_runtime_rs::Message for SpawnObject_Request {
  type RmwMsg = super::srv::rmw::SpawnObject_Request;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        name: msg.name.as_str().into(),
        package_name: msg.package_name.as_str().into(),
        piece_type: msg.piece_type.as_str().into(),
        x: msg.x,
        y: msg.y,
        z: msg.z,
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        name: msg.name.as_str().into(),
        package_name: msg.package_name.as_str().into(),
        piece_type: msg.piece_type.as_str().into(),
      x: msg.x,
      y: msg.y,
      z: msg.z,
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      name: msg.name.to_string(),
      package_name: msg.package_name.to_string(),
      piece_type: msg.piece_type.to_string(),
      x: msg.x,
      y: msg.y,
      z: msg.z,
    }
  }
}


// Corresponds to robot_interfaces__srv__SpawnObject_Response

// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct SpawnObject_Response {

    // This member is not documented.
    #[allow(missing_docs)]
    pub success: bool,

}



impl Default for SpawnObject_Response {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::srv::rmw::SpawnObject_Response::default())
  }
}

impl rosidl_runtime_rs::Message for SpawnObject_Response {
  type RmwMsg = super::srv::rmw::SpawnObject_Response;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        success: msg.success,
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
      success: msg.success,
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      success: msg.success,
    }
  }
}


// Corresponds to robot_interfaces__srv__AddCamera_Request

// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct AddCamera_Request {

    // This member is not documented.
    #[allow(missing_docs)]
    pub name: std::string::String,


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
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::srv::rmw::AddCamera_Request::default())
  }
}

impl rosidl_runtime_rs::Message for AddCamera_Request {
  type RmwMsg = super::srv::rmw::AddCamera_Request;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        name: msg.name.as_str().into(),
        x: msg.x,
        y: msg.y,
        z: msg.z,
        target_x: msg.target_x,
        target_y: msg.target_y,
        target_z: msg.target_z,
        width: msg.width,
        height: msg.height,
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        name: msg.name.as_str().into(),
      x: msg.x,
      y: msg.y,
      z: msg.z,
      target_x: msg.target_x,
      target_y: msg.target_y,
      target_z: msg.target_z,
      width: msg.width,
      height: msg.height,
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      name: msg.name.to_string(),
      x: msg.x,
      y: msg.y,
      z: msg.z,
      target_x: msg.target_x,
      target_y: msg.target_y,
      target_z: msg.target_z,
      width: msg.width,
      height: msg.height,
    }
  }
}


// Corresponds to robot_interfaces__srv__AddCamera_Response

// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct AddCamera_Response {

    // This member is not documented.
    #[allow(missing_docs)]
    pub success: bool,

}



impl Default for AddCamera_Response {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::srv::rmw::AddCamera_Response::default())
  }
}

impl rosidl_runtime_rs::Message for AddCamera_Response {
  type RmwMsg = super::srv::rmw::AddCamera_Response;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        success: msg.success,
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
      success: msg.success,
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      success: msg.success,
    }
  }
}


// Corresponds to robot_interfaces__srv__PutPiece_Request

// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
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
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::srv::rmw::PutPiece_Request::default())
  }
}

impl rosidl_runtime_rs::Message for PutPiece_Request {
  type RmwMsg = super::srv::rmw::PutPiece_Request;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        fila: msg.fila,
        columna: msg.columna,
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
      fila: msg.fila,
      columna: msg.columna,
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      fila: msg.fila,
      columna: msg.columna,
    }
  }
}


// Corresponds to robot_interfaces__srv__PutPiece_Response

// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct PutPiece_Response {

    // This member is not documented.
    #[allow(missing_docs)]
    pub success: bool,


    // This member is not documented.
    #[allow(missing_docs)]
    pub message: std::string::String,

}



impl Default for PutPiece_Response {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::srv::rmw::PutPiece_Response::default())
  }
}

impl rosidl_runtime_rs::Message for PutPiece_Response {
  type RmwMsg = super::srv::rmw::PutPiece_Response;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        success: msg.success,
        message: msg.message.as_str().into(),
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
      success: msg.success,
        message: msg.message.as_str().into(),
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      success: msg.success,
      message: msg.message.to_string(),
    }
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


