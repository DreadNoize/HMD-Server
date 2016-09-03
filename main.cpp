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
#include <exception>


int main(int argc, char** argv) {
  // init zmq server

    Server server("tcp://141.54.147.52:7701");

    std::cout << "Initializing..." << std::endl;

    vr::EVRInitError error = vr::VRInitError_None;
    vr::IVRSystem* hmd = vr::VR_Init(&error, vr::VRApplication_Background);
    if (error != vr::VRInitError_None) {
        hmd = NULL;
        std::cerr << "ERROR while initializing" << std::endl;
    }

     

    while(true) {
		server.send(hmd);
    }

}


