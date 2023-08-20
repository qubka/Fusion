# Fusion

Fusion is an open-source, cross-platform game engine written in modern C++17 and structured to be fast, simple, and extremely modular.
Vulkan is the sole graphics API, Vulkan can be accessed in apps with the provided rendering pipeline.
This engine was developed for my Master's final year project at City, Univercity of London.

[![Watch the video](https://img.youtube.com/vi/xCEj8s233eM/0.jpg)](https://www.youtube.com/watch?v=xCEj8s233eM)

## Implemented features:
- Multiplatform (Windows, Linux, Android, 32bit and 64bit)
- Multithreaded command buffers and thread safety
- Event delegate callbacks with scoped functions
- Built-in editor application
- Project's management using serialization
- Object serialization (JSON and XML using cereal)
- Data-oriented Entity component system (using entt)
- Powerful scene manager
- PhysX physics
- Vulkan renderer pipelines
- Abstract input system
- GLSL to SPIR-V compilation and reflection
- Model file loading (using Assimp)
- Image file loading (ktx, kmg, dds, jpg, png, bmp, hdr, psd, tga, gif, pic, pgm, ppm)
- UI components (using ImGUI)

## TODO features:
- Audio system (flac, mp3, ogg, opus, wave)
- Embedded (script) system (using Lua)
- Particle effect systems
- Post effects pipeline (blur, SSAO, ...)

## Screenshots:
- ![alt text](https://i.ibb.co/JtdJhJx/image-028.png)
- ![alt text](https://i.ibb.co/hsM0Tx8/image-029.png)
- ![alt text](https://i.ibb.co/NCRLY34/image-030.jpg)
- ![alt text](https://i.ibb.co/fdN2KsH/image-031.jpg)
- ![alt text](https://i.ibb.co/6mH2ZxM/image-032.jpg)
- ![alt text](https://i.ibb.co/g65qp5t/image-033.png)
- ![alt text](https://i.ibb.co/xMLGrKz/image-034.png)
- ![alt text](https://i.ibb.co/gvXbV1P/image-035.png)
- ![alt text](https://i.ibb.co/19STRkk/image-036.png)
- ![alt text](https://i.ibb.co/5sCpWXN/image-037.png)
- ![alt text](https://i.ibb.co/b77z7sp/image-038.png)
- ![alt text](https://i.ibb.co/SnDQB4b/image-039.png)
- ![alt text](https://i.ibb.co/0fZp6TL/image-040.png)
- ![alt text](https://i.ibb.co/CwhtBcr/image-041.png)
- ![alt text](https://i.ibb.co/XJCYVFB/image-042.png)
- ![alt text](https://i.ibb.co/FXq16cf/image-043.png)
- ![alt text](https://i.ibb.co/8BY76jY/image-044.jpg)
- ![alt text](https://i.ibb.co/hHX8PP6/image-045.png)
- ![alt text](https://i.ibb.co/80pNpWd/image-046.jpg)
- ![alt text](https://i.ibb.co/r0bMkvv/image-047.png)
- ![alt text](https://i.ibb.co/HNTdV51/image-048.png)
- ![alt text](https://i.ibb.co/WD7MkfG/image-049.jpg)
- ![alt text](https://i.ibb.co/GpX3zBq/image-050.png)
- ![alt text](https://i.ibb.co/HxYLznn/image-051.png)
- ![alt text](https://i.ibb.co/sPRzPGT/image-052.png)
- ![alt text](https://i.ibb.co/23N9wbp/image-053.png)
- ![alt text](https://i.ibb.co/NyqYfxx/image-054.png)
- ![alt text](https://i.ibb.co/x8k7MCr/image-055.png)
- ![alt text](https://i.ibb.co/PhpkZr2/image-056.png)
- ![alt text](https://i.ibb.co/vxn2Y5Y/image-057.png)
- ![alt text](https://i.ibb.co/XJNQqvV/image-058.jpg)
- ![alt text](https://i.ibb.co/n0BByWs/image-059.jpg)
- ![alt text](https://i.ibb.co/fGT0P2x/image-060.jpg)

## Architecture:
![alt text](https://i.ibb.co/QH8JKMD/image-016.png)
![alt text](https://i.ibb.co/Q9JkrmM/image-017.png)
![alt text](https://i.ibb.co/JqqGf24/image-018.png)
![alt text](https://i.ibb.co/NpdPWPP/image-019.png)
![alt text](https://i.ibb.co/HV6MMsP/image-020.png)
![alt text](https://i.ibb.co/6416gsr/image-021.png)
![alt text](https://i.ibb.co/LQRZK9x/image-022.png)
![alt text](https://i.ibb.co/GHJGPKF/image-023.png)
![alt text](https://i.ibb.co/SymR7bq/image-024.png)
![alt text](https://i.ibb.co/b1GhxGm/image-025.png)
![alt text](https://i.ibb.co/5s3r7ZG/image-026.png)
![alt text](https://i.ibb.co/QYqd0D0/image-027.png)

## Installation instructions:
The final build of the game engine, the editor application and the source code are submitted alongside this 
report. To run the editor application, simply execute the "editor.exe" file located in the 
"EditorBuild/editor" directory. To run the game engine sample app, simply execute the "sandbox.exe" file 
located in the "SandboxBuild/game" directory. To upload all submodules for the sourcecode to compile 
the project. Execute command in the command prompt: "git submodule update --init --recursive"
