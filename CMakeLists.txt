cmake_minimum_required (VERSION 2.6)
project(ZMQOculusServer)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")

include_directories("C:/Data/Repositories/libzmq/include")
include_directories("C:/Data/SDKs/OculusSDKs/OculusSDK/LibOVR/Include")
include_directories("C:/Data/SDKs/openvr/headers")

link_directories("C:/Data/SDKs/OculusSDKs/OculusSDK/LibOVR/Lib/Windows/x64/Release/VS2015")
link_directories("C:/Data/Repositories/libzmq/bin/x64/Release/v140/dynamic")
link_directories("C:/Data/SDKs/openvr/lib/win64")

add_executable(OculServer
  main.cpp
  server.hpp
)

target_link_libraries(OculServer "LibOVR.lib")
target_link_libraries(OculServer "libzmq.lib")
target_link_libraries(OculServer "openvr_api.lib")

