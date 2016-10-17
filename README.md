# HMD-Server
server for sending VR-Device (Oculus Rift and HTC Vive incl. Controllers and Lighthouses) tracking data to avango daemon

Working with HTC Vive and Oculus Rift CV, tested with Visual Studio 14 2015.

:rocket:

Developed as part of the SocialVR Project at Bauhaus-Universitaet Weimar.

## What you will need
- OpenVR SDK and SteamVR
- libzmq: https://github.com/zeromq/libzmq
- CMake

## Instructions
After cloning you have to use cmake for the Visual Studion solution file. You can do it by using

`cmake -G "Visual Studio 14 2015 Win64" ..`

After building the server with VS, you have to copy the the libzmq.dll and the openvr.dll in to the runtime folder (not sure why, though. it just won’t work otherwise. **If you find a fix for this, a hint will be much appreciated** :kissing_heart:).
Before starting the Server, make sure SteamVR is running.

You need to give the ip addres and the port of your server as command line arguments, like this:

`HMDServer ip:port`

**EXAMPLE:** `C:\Users\You\git\HMDServer\build\Release>HMDServer 127.0.0.1:666`

It should work, but there is _**no guarantee**_, it actually will. 

:rocket: **Have Fun** :rocket:
