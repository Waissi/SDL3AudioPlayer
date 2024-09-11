# SDL 3 Audio Player

This is a low-level, experimental audio player written in C exploring the [new audio functionalities](https://www.youtube.com/watch?v=MLau3hWJBeE) of SDL 3.

## Features:
- Grouping audio clips by buses
- Transport: play, stop, pause, resume
- Volume adjustement
- Panning

## Dependencies
- [SDL 3](https://github.com/libsdl-org/SDL.git)
- [SDL image](https://github.com/libsdl-org/SDL_image.git)
- [SDL ttf](https://github.com/libsdl-org/SDL_ttf.git)

## Building (MacOS and Linux)
First, init the git submodules
````
git submodule update --init --recursive
````
Then you need to build and install the dependencies (here with cmake)
```
cd deps/SDL
mkdir build
cd build
cmake ..
make
sudo make install
```
Repeat the same for SDL_image and SDL_ttf (modify the first command line accordingly). 

The .vscode folder is configured for building, running and deguging the audio player using the [Zig build system](https://ziglang.org/learn/build-system/).  

You can also manually build it with
````
zig build
````
And run it with
````
zig-out/bin/SdlAudioPlayer
````

![screenshot](https://github.com/Waissi/SDL3AudioPlayer/assets/58945864/df5d9286-50ae-45a3-ae0f-9bd1e3c0cfa4)