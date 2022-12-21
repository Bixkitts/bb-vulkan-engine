# Vulkan engine
A test vulkan project where I aim to be able to experiment with
shader language for deployment in other contexts.
I also might make a game or something.

## Building

To build, make sure the vulkan runtime and sdk are installed.

- clone the repo with git clone
- cd into that new directory
- mkdir build
- cd build
- git submodule update --init --recursive
- cmake ..
- cmake --build .

## Running
Compile the shaders with compileShaders.sh,
Then the engine can be run from ./build with bb-vulkan-engine as the binary
