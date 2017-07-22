# Elgato Live Preview
The Elgato Game Capture HD application (GCHD) for macOS allows the user to get a low-latency live-preview (called *"Instant Gameview"*) of the content being captured by the Elgato HD60S.
Unfortunately (as of version 2.6.1), the framerate of this preview is quite choppy, making it barely usable for those who intend to play directly within this view.

This project aims to fix this issue by providing an external app that display the uncompressed frames captured by the device. This app offers two advantages compared to the official live-preview :
* No framedrops
* Improved image quality when the image is stretched (e.g. on a 5K iMac screen) thanks to a sharpen filter

As a side-effect, this project allows to access uncompressed video-frames from the GCHD application. It means that third-party software like OBS Studio can use this to capture HD60S video with no extra latency or compression. As a proof-of-concept, this repository hosts an OBS plugin that does exactly that.

## How it works ##
This project is based on the observation that while the preview is choppy, the recorded videos have stable 60fps.
It means that the video encoder used by the GCHD application gets uncompressed frames at a stable framerate.

This project is made of 3 parts :
* **x264_hook**: a library that intercepts calls made to the x264 library (software video encoder used by GCHD).
To achieve this, it uses the `dyld_interposing` feature from the DYLD loader on macOS.
With this, it retrieves uncompressed YUV420P frames sent to the `x264_encoder_encode()` function.
These frames are made available to external softwares through a Unix domain socket (located at `/tmp/elgato_raw_frames`)
* **live_preview**: an OpenGL application that receives uncompressed frames from the socket and displays them.
This application also applies a sharpen filter on the displayed frames to improve image quality when the image is stretched.
* **obs_plugin**: a simple OBS plugin that receives uncompressed frames from the socket and displays them in a layer of OBS studio. Note that this is video only, no audio is captured (though a similar approach is probably possible for audio). **Disclaimer:** This plugin is a working proof-of-concept that has been tested with OBS Studio 19.0.3 only. I have no plan to maintain it as I do not use OBS personally.

## How to run ##
### Live Preview ###
To run the live-preview, you first need to launch the GCHD application with the x264 hook activated. The easiest way to do that is to open a terminal and to launch the *startGameCaptureHD.sh* script:
```
cd elgato-live-preview
./startGameCaptureHD.sh
```
It assumes that the GCHD application is installed at its default path (`/Applications//Applications/Game\ Capture\ HD.app`). In order to intercept **software** encoder functions, **hardware encoding has to be disabled** in the HD60S settings from GCHD. If everything went well, you should see the following line in the terminal: `Frame-server started successfully`.

Now, you can run the live-preview application by opening a terminal and launching the *elgato_live_preview* executable:
```
cd elgato-live-preview
./elgato_live_preview
```

**Note:** To save CPU power, software encoding is actually disabled by *x264_hook* library. If you want still want software encoding in the GCHD application while using *elgato_live_preview*, you can start GHCD with the following command: `X264_HOOK_ENCODE_ENABLED=1 ./startGameCaptureHD.sh`

### OBS Plugin ###
To display HD60S frames inside OBS Studio, you first need to install the plugin. To do so, copy `obs-elgato.so` to the plugins directory of OBS Studio (`/Applications/OBS.app/Contents/Resources/obs-plugins` if OBS is installed in `/Applications`).

Then, launch the *startGameCaptureHD.sh* script (cf. previous section) and launch OBS Studio. In OBS Studio, create a new *Elgago HD60S* source and that's it.

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

### How to build obs_plugin ###
* Clone the [OBS studio repository](https://github.com/jp9000/obs-studio)
* Create a soft-link of the `obs_plugin` directory to `obs-studio/plugins/obs-elgato`
* Add an entry for `obs-elgato` in `obs-studio/plugins/CMakeLists.txt`
* Build OBS Studio again

## What's next ##
* Intercept calls made to the hardware encoder (VideoToolbox framework)
* Make the sharpen filter customizable
