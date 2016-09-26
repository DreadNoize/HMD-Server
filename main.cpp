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
#include <algorithm>


int main(int argc, char** argv) {
  // init zmq server
    std::string ip_port = "tcp://";
    if(argc != 2) {
		std::cerr << "ERROR: You have to give the servers ip and port as one argument!" << std::endl;
		std::cout << "Usage: HMDServer.exe ip:port" << std::endl;
		exit(1);
    }
    ip_port.append(argv[1]);
    Server server(ip_port);

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
		vr::TrackedDevicePose_t* pose;
		vr::VRControllerState_t c_state;
		int iter_c = 0;
		int iter_t = 0;

		//set coordinates
		vrsys->GetDeviceToAbsoluteTrackingPose(vr::TrackingUniverseStanding, 0.0, devices, vr::k_unMaxTrackedDeviceCount);
		for (int i = 0; i < vr::k_unMaxTrackedDeviceCount; ++i) {
			if (devices[i].bPoseIsValid && devices[i].bDeviceIsConnected) {
				if (vr::VRSystem()->GetTrackedDeviceClass(i) == vr::TrackedDeviceClass_HMD) {
					vr::HmdMatrix34_t pos = devices[i].mDeviceToAbsoluteTracking;
					//std::cout << "HMD found!" << std::endl;
					message.hmd_id = i;
					//std::cout << "IDH = " << message.hmd_id << std::endl;

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
				else if (vr::VRSystem()->GetTrackedDeviceClass(i) == vr::TrackedDeviceClass_Controller) {
					Controller ct;
					ct.status = true;
					ct.id = i;
					//std::cout << "IDC = " << ct.id << std::endl;

					vr::HmdMatrix34_t pos = devices[i].mDeviceToAbsoluteTracking;
					//std::cout << "Controller found!" << std::endl;

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

					
					bool state = vrsys->GetControllerState(i, &c_state);
					if (!state)	{
						continue;
					}

					//Buttons and Values
					ct.trigger = c_state.rAxis[1].x;
					//std::cout << "Trigger: " << ct.trigger << std::endl;
					
					if (c_state.ulButtonTouched == vr::ButtonMaskFromId(vr::k_EButton_SteamVR_Touchpad)) {
						//std::cout << "Touchpad touched" << std::endl;
						ct.pad_touch = true;
						ct.pad_x = c_state.rAxis[0].x;
						ct.pad_y = c_state.rAxis[0].y;
						
						//std::cout << "Pad: " << ct.pad_x << " | " << ct.pad_y << std::endl;
					}

					if (c_state.ulButtonPressed == vr::ButtonMaskFromId(vr::k_EButton_ApplicationMenu)) {
						//std::cout << "Menu button pressed" << std::endl;
						ct.app_menu = true;
					}
					else if (c_state.ulButtonPressed == vr::ButtonMaskFromId(vr::k_EButton_Grip)) {
						//std::cout << "Grip button pressed" << std::endl;
						ct.grip = true;
					}
					else if (c_state.ulButtonPressed == vr::ButtonMaskFromId(vr::k_EButton_SteamVR_Trigger)) {
						//std::cout << "Trigger pressed" << std::endl;
						ct.trigger_p = true;
					}
					else if (c_state.ulButtonPressed == vr::ButtonMaskFromId(vr::k_EButton_SteamVR_Touchpad)) {
						//std::cout << "Touchpad pressed" << std::endl;
						ct.pad_press = true;
					}

					message.controller[iter_c] = ct;
					//std::cout << iter_c << std::endl;
					iter_c = iter_c + 1;

					
				}
				else if (vr::VRSystem()->GetTrackedDeviceClass(i) == vr::TrackedDeviceClass_TrackingReference) {
					Tracker tr;
					tr.status = true;
					tr.id = i;
					//std::cout << "IDT = " << tr.id << std::endl;
					vr::HmdMatrix34_t pos = devices[i].mDeviceToAbsoluteTracking;

					//std::cout << "Tracking device found!" << std::endl;

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

					message.tracker[iter_t] = tr;
					iter_t = iter_t + 1;
				}
			}
		}
			
		server.send(&message);
		
    }

}


