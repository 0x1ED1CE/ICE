# ICE
![LICENSE](https://img.shields.io/badge/LICENSE-MIT-green.svg) ![VERSION](https://img.shields.io/badge/BUILD-2-blue)

ICE (ISO/C Engine) is a DOS 3D game engine written entirely in C99 with optional bindings for [ISO](https://github.com/0x1ED1CE/ISO) language.

![Graphics Demo](https://i.imgur.com/NrV2MBO.png)

**NOTE:** This engine is still far from complete and needs a lot more work done!

## 3D Example
![3D Demo](https://i.imgur.com/J9VvXu9.gif)

## Features:
- SoundBlaster 16 (8 bit unsigned mono audio @ 32KHz)
- 256 mixed samples
- VESA mode 10Dh (320x200x15)
- 256 32 bit texture buffer + Optional Z buffer
- 3D triangle rasterization

## TODO:
- File handling
- ISO bindings
- Near clipping
- [Covox Speech Thing](https://en.wikipedia.org/wiki/Covox_Speech_Thing) support
- Joystick support
- SDL2 implementation

## Might Do
- Lua scripting support
- Perspective correct texture mapping

## How to build
You will need [DJGPP](https://github.com/andrewwutw/build-djgpp) and gmake to compile.

You can compile a demo program by running ``make MAIN=demos/<filename>.c`` in the source folder

You can find the examples under [demos](src/demos)

Before running make sure you have [CWSDPMI.EXE](http://sandmann.dotster.com/cwsdpmi/) in the same location as the demo program!

## License
This software is free to use. You can modify it and redistribute it under the terms of the 
MIT license. Check [LICENSE](LICENSE) for further details.
