# CV-Vision-2016

This project is a practice of openCV for the 2016 FRC Stronghold.
It's based on Team955's code.

The basic structure of the code is two different threads.  The main
thread takes images from the camera and parse them.  The second thread
takes the parsed data and sends it to the RoboRIO for usage on the
robot.

The main thread is defined in `main.cc`.  The second thread is defined
in `netcode_thread.cc`
