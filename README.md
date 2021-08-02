# PNG to grayscale.
A command line application based on Gsteamer library which accepts a colored PNG image file as input, and outputs its grayscale equivalent into a PNG format file.

# Platform
Linux - application is built and tested on Linux.

# Requirements
**Gstreamer**  
Install GStreamer on Ubuntu  
Run the following command:

`sudo apt-get install libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev libgstreamer-plugins-bad1.0-dev gstreamer1.0-plugins-base gstreamer1.0-plugins-good gstreamer1.0-plugins-bad gstreamer1.0-plugins-ugly gstreamer1.0-libav gstreamer1.0-doc gstreamer1.0-tools gstreamer1.0-x gstreamer1.0-alsa gstreamer1.0-gl gstreamer1.0-gtk3 gstreamer1.0-qt5 gstreamer1.0-pulseaudio`

**g++**  
Install G++ the C++ compiler:  
sudo apt install g++

# Build

```bash
g++ main.cpp png_reader.cpp png_writer.cpp `pkg-config --cflags --libs gstreamer-1.0 gstreamer-app-1.0 gstreamer-video-1.0` -o png_app
```

# Run
` ./pngdecoder mario_PNG125.png out.png`
