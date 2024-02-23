# KinectAvatarAnimation

Animate an OpenGL avatar using XBOX Kinect v2 depth sensor.

## Introduction

XBOX Kinect v2 is a depth and rgb camera with body tracking informations. This project aims to use datas from Kinect in order to animate an OpenGL avatar. Animation is done calculating rotation angles in real world and applying them to the avatar.<br>

OpenGL is a programming API for 3D computer graphic. Avatar could be anything write in OpenGL: in this project it is a simple humanoid robot. Robot has only the main parts of human body: body, head, arms and forearms, thighs and legs.

## Before you Start

This project was built in Windows OS with Visual Studio 2019 Enviroments.<br>

Kinect hardware used is the second version for XBOX and Microsoft Systems, too. You have to connect your Kinect to USB 3.0 port of your PC using Kinect Adapter (sold separately).<br>

Kinect datas are managed using KinectSDK2 API, distribuited by Microsoft. In Visual Studio 2019 you simply install them via package manager.<br>

OpenGL needs some other tools for setting OpenGL enviroment (here GLEW is used) and managing windows creation (here GLUT is used). In order to use them, you need to download them and define project properties. I advise this YouTube video:
```
https://www.youtube.com/watch?v=A1LqGsyl3C4
```
GLEW settings are equivalent. Just looking at directory locations.<br>

For OpenGL first steps I advise this Web Site:
```
https://ed.ilogues.com/Tutorials/kinect2/kinect0.html
```    
It's very complete in every explanation and code is shared.
