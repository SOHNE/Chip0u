
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

Chip0u is licensed under the MIT License. See `LICENSE` for more information.

[//]:  (Externals)
[CHIP-8 Research Facility]: https://chip-8.github.io/
[Guide to making a CHIP-8 emulator]: https://tobiasvl.github.io/blog/write-a-chip-8-emulator/
[Awesome CHIP-8]: https://github.com/tobiasvl/awesome-chip-8
[//]:  (EOF)
