# elgato-live-preview
The Elgato Game Capture HD software (GCHD) for macOS allows the user to get a low-latency live-preview (called *"Instant Gameview"*) of the content being captured by the Elgato HD60S.
Unfortunately (as of version 2.6.1), the framerate of this preview is quite choppy, making it barely usable for those who intend to play directly within this view.

This project aims to fix this issue by providing an external app that display the frames captured by the device. This app offers two advantages compared to the official live-preview :
* No framedrops
* Improved image quality when the image is stretched (e.g. on a 5K iMac screen) thanks to a sharpen filter

## How it works ##
This project is based on the observation that while the preview is choppy, the recorded videos have stable 60fps.
It means that the video encoder used by the GCHD software gets the uncompressed frames at a stable framerate.

This project is made of two parts :
* The **x264_hook** library that intercepts calls to the x264 library (software video encoder used by GCHD).
To achieve that, it uses the `dyld_interposing` feature from the DYLD loader on macOS.
With this mecanism, it retrieves uncompressed YUV420P frames send to the `x264_encoder_encode()` function.
These frames are made available to external softwares through Unix domain socket (located at `/tmp/elgato_raw_frames`)
* The **live_preview** OpenGL application that receives uncompressed frames from the socket and display them.
This application also applies a sharpen filter on the displayed frames to improve image quality when the image is stretched.

## How to build ##
### Dependencies ###
To build this project, you'll need:
* *XCode*: can be installed from the AppStore
* *CMake >=3.0*: can be installed with `brew install cmake` (cf. [Brew](https://www.brew.sh) website)
* *GLFW >=3.2*: can be installed with `brew install glfw` (cf. [Brew](https://www.brew.sh) website)

### How to build x264_hook ###
From a terminal:
```shell
cd elgato-live-preview/x264_hook
mkdir build
cd build
cmake ..
make
```
The *libelgato_x264_hook.dylib* and *startGameCaptureHD.sh* will be generated.

### How to build live_preview ###
From a terminal:
```shell
cd elgato-live-preview/live_preview
mkdir build
cd build
cmake ..
make
```
The *elgato_live_preview* executable will be generated.
