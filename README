# Interactive Space

Interactive Space is a framework for prototyping multitouch physical desktop interaction. It is highly portable, easy to learn and to use and supports multitouch and gestural interaction on and above the desktop.

Interactive Space uses a Kinect and a projector to make any physical tabletop into an interactive surface. The framework provides an SDK to create interactive prototypes using C# and WPF. It also allows developers to exploit any GUI framework that supports the TUIO protocol. 
## Screenshots

## Getting Started

### Hardware Configuration

To use the system you'll need the following hardwares:

* A [Kinect for Windows](http://www.microsoft.com/en-us/kinectforwindows/)
* A projector (short-throw preferred)
* Equipments for mounting the Kinect and the projector together. Try DIY!
* A computer running Windows (x64 preferred)

### Software Prerequisites
You need to have the following installed:

* Visual Studio 2010 with .Net Framework 4.0
* [Kinect for Windows SDK](http://www.microsoft.com/en-us/kinectforwindows/develop/developer-downloads.aspx)
* [Kinect for Windows Developer Toolkit](http://www.microsoft.com/en-us/kinectforwindows/develop/developer-downloads.aspx) (Optional) 

### Your First Interactive Space App
1. Install the project template.
    1. Download the latest InteractiveSpaceApp template for Visual Studio. [Download Link](http://ucsd-hci.github.com/ise/#download)
    2. Move the InteractiveSpaceTemplate_(ver).zip to <br/>`%HOMEPATH%\Documents\Visual Studio 2010\Templates`. <br/>
    (Change to your own case if you changed the default folder settings in VS) <br/>
    DO NOT extract the zip file! 
2. Create and build your first app.
	1. In Visual Studio, create a new "InteractiveSpaceApp" project. 
	2. Build. If compiling errors happen, go to Trouble Shooting.
3. Adjust the projector and Kinect.
	1. Connect the projector and Kinect. 
	2. Use the projector as a **secondary** screen. Do not use it as your primary screen, nor mirror mode. Set the resolution of the projector to **1600x1200**. (Sorry, it's hard-coded right now…)
	3. Run the app you just built. Wait until the control panel window appears. (Runtime error? Go to Trouble Shooting. )
	4. Click "RGB" button to see the Kinect image. Adjust the position of your projector and Kinect to make sure that the whole projected screen falls within the Kinect view. 
4. Calibration 
	1. Click "Calibrate". Wait until chessboard is detected in the left video. 
	2. Drag yellow marks to the chessboard corners in the right video. Use your fingers to indicate the corner. 
	3. Click "Calibrate Depth Camera". The result will be saved so you don't need to do this every time. 
5. Crop noise area
	1. Open either "Depth" or "OmniTouch" video.
	2. Click "OmniTouch Crop".
	3. Drag your mouse to select noise-free area. 
6. Enjoy your first app by touching the sample buttons, you should see color changes. (Nothing happened? Go to Trouble Shooting )

Still confusing? These slides used in a class might help: [Introduction to Interactive Space SDK](http://ucsd-hci.github.com/ise/files/CSE118.pdf)
    

### Trouble Shooting
* No "InteractiveSpaceApp" in "New Project" of VS
  * In VS, check Tools -> Options -> Projects and Solutions -> General -> User project templates location. Put InteractiveSpaceTemplate_(ver).zip there.
  * Do not extract the zip file.
* Cannot find/load InteractiveSpaceEngine.dll
  * If you're running 32-bit Windows, change the solution platform to "Win32".
  * Make sure InteractiveSpaceEngine.dll, opencv_*.dll and tbb.dll exist in the lib folder of your selected platform, and they're copied into the build target folder (e.g. bin/x64/Debug). (If files are missing, go to Contact…)
* Assertion failed in KinectSensor.cpp on start up
  * Make sure your Kinect is working. You can run a demo app from the Kinect for Windows Developer Toolkit to test your Kinect connection.
* Cannot detect chessboard when calibrating
  * Remove staff on your table before the calibration; adjust your room light to have higher contract for the projected screen; make sure the whole chessboard is within the Kinect view.
* False positive fingers in the center
  * Your Kinect is too closed to the table surface. 
* Nothing happened when touching the sample buttons
  * Make sure the Multi-touch input service is running in a separate console window (should be automatically start) and it outputs `TUIO Contacts: Secondary screen found at {X=…,Y=…,Width=1600,Height=1200}`
  * Uncomment two lines in MainWindow.xaml.cs to draw fingers on the projected screen (see comments in that cs file).
## Build the Engine

Please refer to this wiki page: [Build the Interactive Space Engine](https://github.com/UCSD-HCI/ise/wiki/Build-the-Interactive-Space-Engine).

## Contact
You can contact the developers via Google Group [Interactive Space Engine Discussion](https://groups.google.com/forum/?fromgroups#!forum/ise-discuss). 

## Contributing
Feel free to fork this project and send a pull request. We're more than happy to accept contributions.

## License
(TBD)

