# ISO/C Engine
![LICENSE](https://img.shields.io/badge/LICENSE-MIT-green.svg)

ICE is a 3D game engine for DOS written entirely in C99 with optional bindings for [ISO](https://github.com/0x1ED1CE/ISO).

**NOTE:** This engine is still far from complete and needs a lot more work done!

## Screenshot

<img src="/screenshots/world.png?raw=true">

## Features:
- 3D software rendering (mode 110h / 10Dh)
- 3D hardware acceleration via 3dfx Voodoo
- Full motion video playback (MPEG1)
- SoundBlaster 16 (8 bit unsigned mono audio @ 22KHz)
- Keyboard input

## TODO:
- Audio streaming
- Mouse support
- Joystick support
- SDL2 port
- Documentation

## How to build
You will need [DJGPP](https://github.com/andrewwutw/build-djgpp) and make to compile.

You can compile a demo program by running ``make MAIN=demos/<filename>.c`` in the source folder

You can find the examples under [demos](src/demos)

Before running, make sure you have [CWSDPMI.EXE](http://sandmann.dotster.com/cwsdpmi/) in the same location as the program!

## License
This software is free to use. You can modify it and redistribute it under the terms of the 
MIT license. Check [LICENSE](LICENSE) for further details.
