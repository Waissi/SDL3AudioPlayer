#!/bin/bash
BIN=bin/release/SdlAudioPlayer
if [ -f "$BIN" ]; then rm $BIN
fi
gcc -Wall -o $BIN src/*.c `pkg-config --cflags --libs sdl3` -lSDL3_image -lSDL3_ttf