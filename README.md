# ISO/C Engine
![LICENSE](https://img.shields.io/badge/LICENSE-MIT-green.svg)

ICE is a DOS 3D game engine written entirely in C99 with optional bindings for [ISO](https://github.com/0x1ED1CE/ISO) bytecode.

<img src="/screenshots/triangle.png?raw=true">

**NOTE:** This engine is still far from complete and needs a lot more work done!

## 3D Example

<img src="/screenshots/miku.gif?raw=true">

## Features:
- SoundBlaster 16 (8 bit unsigned mono audio @ 22KHz)
- 256 mixed samples
- VESA mode 10Dh (320x200x15)
- 256 32 bit texture buffer + Optional Z buffer
- 3D triangle rasterization

## TODO:
- 3dfx Glide support
- File handling
- [Covox Speech Thing](https://en.wikipedia.org/wiki/Covox_Speech_Thing) support
- Joystick support
- SDL2 implementation

## Might Do
- Lua scripting support

## How to build
You will need [DJGPP](https://github.com/andrewwutw/build-djgpp) and gmake to compile.

You can compile a demo program by running ``make MAIN=demos/<filename>.c`` in the source folder

You can find the examples under [demos](src/demos)

Before running, make sure you have [CWSDPMI.EXE](http://sandmann.dotster.com/cwsdpmi/) in the same location as the program!

## License
This software is free to use. You can modify it and redistribute it under the terms of the 
MIT license. Check [LICENSE](LICENSE) for further details.
