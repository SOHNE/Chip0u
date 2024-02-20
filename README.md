
# SOHNE | Chip0u


This project, Chip0u, is a learning-oriented endeavor into the world of CHIP-8 emulation. It has been developed in parallel with some comprehensive resources:

1. [CHIP-8 Research Facility]: A detailed guide that provides an overview of the CHIP-8 and its operation.
2. [Guide to making a CHIP-8 emulator]: A step-by-step tutorial on creating a CHIP-8 emulator.
3. [Awesome CHIP-8]: A fantastic repository that links to countless wonderful resources related to CHIP-8 and derivates.

This project is a fun and complex learning experience for me. It's all about getting to know more about how interpreters and emulators work.

Taking inspiration from the design and logic of existing CHIP-8 (and others) emulators, _Chip0u_ aims only the simplicity.

[![Live demo](https://img.shields.io/badge/-Live%20Demo-8f60fc?style=for-the-badge&logo=glitch&logoColor=white&colorA=392467&colorB=7E30E1)](https://chip0u.glitch.me/)

## Building

To build it, follow these steps:

1. Clone the repository: `git clone https://github.com/sohne/chip0u.git`
2. Navigate to the project directory: `cd chip0u`
3. Create a new directory for the build: `mkdir build && cd build`
4. Run CMake to configure the project: `cmake ..`
5. Build the project: `make`

## Usage

Chip0u can be used to run some assembled CHIP-8 programs. To run a program, use the GUI interface to load a "_ROM_" file.

## Caution

While every effort has been made to ensure the quality of the emulator, there may be aspects of the CHIP-8 system that are not fully understood or correctly implemented.

## Contributing

Contributions to Chip0u are welcome. Please feel free to fork the repository and submit pull requests.

## License

MIT License

Copyright (c) 2024 SOHNE
Copyright (c) 2024 Leandro Peres, aka "zschzen"

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

[//]:  (Externals)
[CHIP-8 Research Facility]: https://chip-8.github.io/
[Guide to making a CHIP-8 emulator]: https://tobiasvl.github.io/blog/write-a-chip-8-emulator/
[Awesome CHIP-8]: https://github.com/tobiasvl/awesome-chip-8
[//]:  (EOF)
