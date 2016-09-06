// exclude some windows resources
// otherwise Kinect and ZMQ won't work together
// #ifndef WIN32_LEAN_AND_MEAN
// #define WIN32_LEAN_AND_MEAN
// #endif

#include <openvr.h> 
#include <string>
#include <iostream>
#include <fstream>
#include "server.hpp"
#include "message.hpp"
#include <exception>


int main(int argc, char** argv) {
  // init zmq server

    Server server("tcp://141.54.147.52:7701");

    std::cout << "Initializing..." << std::endl;

    vr::EVRInitError error = vr::VRInitError_None;
    vr::IVRSystem* vrsys = vr::VR_Init(&error, vr::VRApplication_Background);
    if (error != vr::VRInitError_None) {
        vrsys = NULL;
        std::cerr << "ERROR while initializing" << std::endl;
    }

	vr::TrackedDevicePose_t devices[vr::k_unMaxTrackedDeviceCount];
	
    while(true) {
		Message message;
		vr::VREvent_t event;
		

		//set coordinates
		vrsys->GetDeviceToAbsoluteTrackingPose(vr::TrackingUniverseStanding, 0.0, devices, vr::k_unMaxTrackedDeviceCount);
		for (int i = 0; i < vr::k_unMaxTrackedDeviceCount; ++i)
		{
			if (devices[i].bPoseIsValid && devices[i].bDeviceIsConnected) {
				if (vr::VRSystem()->GetTrackedDeviceClass(i) == vr::TrackedDeviceClass_HMD) {
					vr::HmdMatrix34_t pos = devices[i].mDeviceToAbsoluteTracking;

					// inverted matrix
					message.hmd[0] = pos.m[0][0];
					message.hmd[1] = pos.m[1][0];
					message.hmd[2] = pos.m[2][0];
					message.hmd[4] = pos.m[0][1];
					message.hmd[5] = pos.m[1][1];
					message.hmd[6] = pos.m[2][1];
					message.hmd[8] = pos.m[0][2];
					message.hmd[9] = pos.m[1][2];
					message.hmd[10] = pos.m[2][2];
					message.hmd[12] = pos.m[0][3];
					message.hmd[13] = pos.m[1][3];
					message.hmd[14] = pos.m[2][3];
				}
				if (vr::VRSystem()->GetTrackedDeviceClass(i) == vr::TrackedDeviceClass_Controller) {
					Controller ctrlr;
					ctrlr.status = true;
					
					vr::VRControllerState_t *c_state;
					vr::HmdMatrix34_t pos = devices[i].mDeviceToAbsoluteTracking;

					ctrlr.matrix[0] = pos.m[0][0];
					ctrlr.matrix[1] = pos.m[1][0];
					ctrlr.matrix[2] = pos.m[2][0];
					ctrlr.matrix[4] = pos.m[0][1];
					ctrlr.matrix[5] = pos.m[1][1];
					ctrlr.matrix[6] = pos.m[2][1];
					ctrlr.matrix[8] = pos.m[0][2];
					ctrlr.matrix[9] = pos.m[1][2];
					ctrlr.matrix[10] = pos.m[2][2];
					ctrlr.matrix[12] = pos.m[0][3];
					ctrlr.matrix[13] = pos.m[1][3];
					ctrlr.matrix[14] = pos.m[2][3];

					if (event.data.controller.button == vr::k_EButton_ApplicationMenu) {
						if (event.eventType == vr::VREvent_ButtonPress) {
							ctrlr.app_menu = true;
						}
					}
					else if (event.data.controller.button == vr::k_EButton_Grip) {
						if (event.eventType == vr::VREvent_ButtonPress) {
							ctrlr.grip = true;
						}
					}
					else if (event.data.controller.button == vr::k_EButton_SteamVR_Trigger) {
						if (event.eventType == vr::VREvent_ButtonPress) {
							ctrlr.trigger_t = true;
						}
					}
					else if (event.data.controller.button == vr::k_EButton_SteamVR_Touchpad) {
						if (event.eventType == vr::VREvent_ButtonTouch) {
							ctrlr.pad_touch = true;
							ctrlr.pad_x = c_state->rAxis[1].x;
							ctrlr.pad_y = c_state->rAxis[1].y;
						}
					}
					else if (event.data.controller.button == vr::k_EButton_SteamVR_Touchpad) {
						if (event.eventType == vr::VREvent_ButtonPress) {
							ctrlr.pad_press = true;
						}
					}

					ctrlr.trigger = c_state->rAxis[3].x;



					message.controller.push_back(ctrlr);

				}
				if (vr::VRSystem()->GetTrackedDeviceClass(i) == vr::TrackedDeviceClass_TrackingReference) {
					Tracker trckr;
					trckr.status = true;
					vr::HmdMatrix34_t pos = devices[i].mDeviceToAbsoluteTracking;

					trckr.matrix[0] = pos.m[0][0];
					trckr.matrix[1] = pos.m[1][0];
					trckr.matrix[2] = pos.m[2][0];
					trckr.matrix[4] = pos.m[0][1];
					trckr.matrix[5] = pos.m[1][1];
					trckr.matrix[6] = pos.m[2][1];
					trckr.matrix[8] = pos.m[0][2];
					trckr.matrix[9] = pos.m[1][2];
					trckr.matrix[10] = pos.m[2][2];
					trckr.matrix[12] = pos.m[0][3];
					trckr.matrix[13] = pos.m[1][3];
					trckr.matrix[14] = pos.m[2][3];


					message.tracker.push_back(trckr);
				}
			}
		}
		server.send(message);
    }

}


