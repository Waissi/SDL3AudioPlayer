#!/bin/bash
BIN=bin/debug/SdlAudioPlayer
if [ -f "$BIN" ]; then rm $BIN
fi
gcc -g -Wall -o $BIN src/*.c `pkg-config --cflags --libs sdl3` -lSDL3_image -lSDL3_ttf