# ISO/C Engine
![LICENSE](https://img.shields.io/badge/LICENSE-MIT-green.svg)

ICE is a 3D game engine for DOS written entirely in C99 with optional bindings for [ISO](https://github.com/0x1ED1CE/ISO).

**NOTE:** This engine is still far from complete and needs a lot more work done!

## Features:
- 3D software rendering (mode 110h / 10Dh)
- 3D hardware acceleration via 3dfx Voodoo
- SoundBlaster 16 (8 bit unsigned mono audio @ 22KHz)
- Keyboard input

## TODO:
- Mouse support
- [Covox Speech Thing](https://en.wikipedia.org/wiki/Covox_Speech_Thing) support
- Joystick support
- SDL2 backend
- Documentation

## Might Do
- Lua scripting support

## Examples
**Software rendered triangle in 320x200**

<img src="/screenshots/triangle.png?raw=true">

**Hatsune Miku rendered using 3dfx Voodoo**

<img src="/screenshots/snwiaXw.gif?raw=true">

## How to build
You will need [DJGPP](https://github.com/andrewwutw/build-djgpp) and gmake to compile.

You can compile a demo program by running ``make MAIN=demos/<filename>.c`` in the source folder

You can find the examples under [demos](src/demos)

Before running, make sure you have [CWSDPMI.EXE](http://sandmann.dotster.com/cwsdpmi/) in the same location as the program!

## License
This software is free to use. You can modify it and redistribute it under the terms of the 
MIT license. Check [LICENSE](LICENSE) for further details.
