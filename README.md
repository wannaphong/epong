

EPONG
--------------------------------------------------------------------------------

Nokia Developer 2011


Introduction
--------------------------------------------------------------------------------
This a Symbian Pong game, which is made as an easy to understand game example 
for Symbian^3 platform.  It builds on the Symbian platform, but attempts to
separate platform dependant code from the actual game code.  The actual game
code is thus portable and should be familiar to developers with experience 
on other mobile platforms, e.g. iPhone, Android and Meego.



Compatibility
--------------------------------------------------------------------------------
This example is targeted towards Symbian^3 devices, e.g. Nokia N8 and Nokia E7.
Emulator build is not supported, the game relies on OpenGL ES 2.0, which is 
not supported in WINSCW emulator.


Highlights
--------------------------------------------------------------------------------
This example demonstrates on the following aspects:

- Entering Symbian game programming without previous Symbian experience.
- Writing platform independent game engine 
- Writing C++ game with multiple platforms in mind
- Using existing bitmap asset as application icon
- Handling Symbian multitasking (game pausing, releasing graphics resources)
- How to incorporate sound engine (public domain OGG decoder)
- How to mix sounds on top of sound engine
- Loading PNG graphics to OpenGL texture (stb_image.c, public domain) 
- Handling keyboard events
- Handling touch events (multitouch on E7)
- Implementing hardware volume keys
- Honouring silent profile (warning tones on/off)
- Reading accellerometer sensor
- Using floating point math hardware
- Using haptic feedback
- Launching browser (using no Symbian capabilities)
- Implementing HDMI output with check for cable connection
 

Required software
--------------------------------------------------------------------------------
Carbide C++ 3.2  OR  Nokia Qt SDK 1.3
Nokia Symbian^3 SDK v1.0
 * NOTE! The Symbian^1 target is NOT supported.

Building
--------------------------------------------------------------------------------
Note about GOOM monitor API:  This API is not supplied with SDK 1.0.  You can
enable it by copying goomapi directory contents to your SDK's epoc32 directory.
Enabling USE_GOOM_MONITOR flag from inc/epong.hrh then set project to use GOOM
to signal memory allocation.

Import project folder into Carbide by selecting File/Import... and selecting
"Symbian OS bld.inf file".  Browse to this example's EpocPong\group directory.
Enable GCCE phone configuration and SBSv1 project builder.

To create a SIS package when building, selct Project/Properties, select Carbide 
C++ and Build Settings.  Add sis/epong.pkg file to SIS builder.  Use self-signed
for trial version.  

For Ovi Store, you need to use trusted range UID (starts with 0x2), these need to 
be signed with developer certificate.  This package also contains full Ovi Store
game, which can be compiled by commenting out TRIAL_VERSION macro in 
group/epong.mmp file


Directories
--------------------------------------------------------------------------------
This example is structured as follows

game_engine
  Header definitions for platform independent game engine
  
epong_game_logic
  Platform independent game code

epong_graphics
  Graphics assets
  
epong_sounds
  Sound assets
  
epong_s3_build
  group
    Symbian specific project files
  src
    Symbian source code, which implements game engine using Symbian
    application framework
  inc
    Symbian source headers
  gfx
    Symbian icon
  data
    Symbian resource files and includes
  sis
    Symbian packaging files
  tools
    Simple tool for extracting 32bit png into two separate bmp files

epong_qt_build
   Qt source code, project file and assets.


Known Issues
--------------------------------------------------------------------------------
There are problems in multitouch with Nokia N8. This causes the axis to get
mixed from time to time. This makes multiplayer-mode problematic with N8 device.

HDMI output displays game only using HDMI, not on the internal display.  
