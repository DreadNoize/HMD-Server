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

    Server server("tcp://141.54.147.52:7770"); //should be adjusted, maybe via user input?
    bool event_flag;

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
		
		event_flag = vrsys->PollNextEvent(&event, sizeof(event)));
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
					Controller ct;
					ct.status = true;
					
					vr::VRControllerState_t *c_state;
					vr::HmdMatrix34_t pos = devices[i].mDeviceToAbsoluteTracking;

					ct.matrix[0] = pos.m[0][0];
					ct.matrix[1] = pos.m[1][0];
					ct.matrix[2] = pos.m[2][0];
					ct.matrix[4] = pos.m[0][1];
					ct.matrix[5] = pos.m[1][1];
					ct.matrix[6] = pos.m[2][1];
					ct.matrix[8] = pos.m[0][2];
					ct.matrix[9] = pos.m[1][2];
					ct.matrix[10] = pos.m[2][2];
					ct.matrix[12] = pos.m[0][3];
					ct.matrix[13] = pos.m[1][3];
					ct.matrix[14] = pos.m[2][3];

					if(event_flag) {
						if (event.data.controller.button == vr::k_EButton_ApplicationMenu) {
							if (event.eventType == vr::VREvent_ButtonPress) {
								ct.app_menu = true;
							}
						}
						else if (event.data.controller.button == vr::k_EButton_Grip) {
							if (event.eventType == vr::VREvent_ButtonPress) {
								ct.grip = true;
							}
						}
						else if (event.data.controller.button == vr::k_EButton_SteamVR_Trigger) {
							if (event.eventType == vr::VREvent_ButtonPress) {
								ct.trigger_t = true;
							}
						}
						else if (event.data.controller.button == vr::k_EButton_SteamVR_Touchpad) {
							if (event.eventType == vr::VREvent_ButtonTouch) {
								ct.pad_touch = true;
								ct.pad_x = c_state->rAxis[1].x;
								ct.pad_y = c_state->rAxis[1].y;
							}
						}
						else if (event.data.controller.button == vr::k_EButton_SteamVR_Touchpad) {
							if (event.eventType == vr::VREvent_ButtonPress) {
								ct.pad_press = true;
							}
						}
					}

					ct.trigger = c_state->rAxis[3].x;



					message.controller.push_back(ct);

				}
				if (vr::VRSystem()->GetTrackedDeviceClass(i) == vr::TrackedDeviceClass_TrackingReference) {
					Tracker tr;
					tr.status = true;
					vr::HmdMatrix34_t pos = devices[i].mDeviceToAbsoluteTracking;

					tr.matrix[0] = pos.m[0][0];
					tr.matrix[1] = pos.m[1][0];
					tr.matrix[2] = pos.m[2][0];
					tr.matrix[4] = pos.m[0][1];
					tr.matrix[5] = pos.m[1][1];
					tr.matrix[6] = pos.m[2][1];
					tr.matrix[8] = pos.m[0][2];
					tr.matrix[9] = pos.m[1][2];
					tr.matrix[10] = pos.m[2][2];
					tr.matrix[12] = pos.m[0][3];
					tr.matrix[13] = pos.m[1][3];
					tr.matrix[14] = pos.m[2][3];

					message.tracker.push_back(tr);
				}
			}
		}
		server.send(message);
    }

}


