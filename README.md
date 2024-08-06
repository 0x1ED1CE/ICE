# ISO/C Engine
![LICENSE](https://img.shields.io/badge/LICENSE-MIT-green.svg)

ICE is a lite 3D game engine for DOS and other platforms.

**This project is still in early development and is not suitable for making games yet.**

<img src="/screenshots/world.png?raw=true">

## Features
- Supports 3dfx Voodoo 3D hardware acceleration and 3D software rendering
- Supports Creative Sound Blaster and Covox Speech Thing
- Supports full motion video and audio streaming MPEG1/Ogg
- Supports mouse and keyboard

## Architecture
<img src="/screenshots/architecture.png?raw=true">

ICE will eventually have an ECS middle layer to handle scene compositing, physics and animations. This is of course will be optional and can be entirely bypassed.

## TODO
- Add asynchronous loading
- Add event handling
- Redo mouse handling
- Add scripting support
- Add ECS middle layer
- Add physics and animation system
- Rename
- Port to SDL2
- Documentation

## How to build
Refer to [src/Makefile](src/Makefile) for build instructions

## How to use
Refer to [ice.h](src/ice.h) for the API

You can find examples in [demos](src/demos)

## License
This software is free to use. You can modify it and redistribute it under the terms of the 
MIT license. Check [LICENSE](LICENSE) for further details.
