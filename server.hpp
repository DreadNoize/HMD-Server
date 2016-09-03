#pragma once

#ifndef VRSYS_OCULUS_SERVER
#define VRSYS_OCULUS_SERVER

#include <zmq.hpp>
#include <array>

std::array<float, 16> fillMatrix(vr::IVRSystem* hmd) {
	std::array<float,16> matrix;
	matrix.fill(0);
	// initialize identity matrix
	matrix[0] = 1;
	matrix[5] = 1;
	matrix[10] = 1;
	matrix[15] = 1;

	vr::TrackedDevicePose_t devices[vr::k_unMaxTrackedDeviceCount];
	
	//set coordinates
	hmd->GetDeviceToAbsoluteTrackingPose(vr::TrackingUniverseStanding, 0.0, devices, vr::k_unMaxTrackedDeviceCount);
	        for (int i = 0; i < vr::k_unMaxTrackedDeviceCount; ++i)
	        {
	            if (devices[i].bPoseIsValid && devices[i].bDeviceIsConnected) {
	                if (vr::VRSystem()->GetTrackedDeviceClass(i) == vr::TrackedDeviceClass_HMD) {
	                        vr::HmdMatrix34_t pos = devices[i].mDeviceToAbsoluteTracking;
	                        /* old matrix
	                        matrix[0] = pos.m[0][0]; 
	                        matrix[1] = pos.m[0][1]; 
	                        matrix[2] = pos.m[0][2]; 
	                        matrix[3] = pos.m[0][3];
	                        matrix[4] = pos.m[1][0]; 
	                        matrix[5] = pos.m[1][1]; 
	                        matrix[6] = pos.m[1][2]; 
	                        matrix[7] = pos.m[1][3];       
	                        matrix[8] = pos.m[2][0]; 
	                        matrix[9] = pos.m[2][1]; 
	                        matrix[10] = pos.m[2][2]; 
	                        matrix[11] = pos.m[2][3];       
	                        matrix[12] = 1.0; 
	                        matrix[13] = 1.0; 
	                        matrix[14] = 1.0; 
	                        matrix[15] = 1.0; 
	                		*/
	                        // inverted
	                        matrix[0] = pos.m[0][0]; 
	                        matrix[1] = pos.m[1][0]; 
	                        matrix[2] = pos.m[2][0]; 
	                        matrix[4] = pos.m[0][1]; 
	                        matrix[5] = pos.m[1][1]; 
	                        matrix[6] = pos.m[2][1]; 
	                        matrix[8] = pos.m[0][2]; 
	                        matrix[9] = pos.m[1][2]; 
	                        matrix[10] = pos.m[2][2]; 
	                        matrix[12] = pos.m[0][3];
	                        matrix[13] = pos.m[1][3];       
	                        matrix[14] = pos.m[2][3];       
	                }
	            }
	        }

	return matrix;
}

struct Server {
	Server(std::string address)
	  : m_ctx(1)
	  , m_socket(m_ctx, ZMQ_PUB)
	{
		uint32_t hwm = 1;
		m_socket.setsockopt(ZMQ_SNDHWM, &hwm, sizeof(hwm));

		m_socket.bind(address.c_str());
	}
	bool send(vr::IVRSystem* hmd) {
		std::array<float, 16> matrix = fillMatrix(hmd);

		// DEBUG
		/*std::cout << matrix[0] << " " << matrix[4] << " " << matrix[8] << " " << matrix[12] << std::endl;
		std::cout << matrix[1] << " " << matrix[5] << " " << matrix[9] << " " << matrix[13] << std::endl;
		std::cout << matrix[2] << " " << matrix[6] << " " << matrix[10] << " " << matrix[14] << std::endl;
		std::cout << matrix[3] << " " << matrix[7] << " " << matrix[11] << " " << matrix[15] << std::endl;
		*/
		size_t len = sizeof(std::array<float, 16>);

		zmq::message_t message(len);
		memcpy(message.data(), matrix.data(), len);
		m_socket.send(message);
		std::cout << ".";
		return 1;
	}
	zmq::context_t m_ctx;
	zmq::socket_t m_socket;
};

#endif
