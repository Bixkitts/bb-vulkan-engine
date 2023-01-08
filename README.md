# Vulkan engine
A test vulkan project where I aim to be able to experiment with
shader language for deployment in other contexts.
I also might make a game or something.

## Building

To build, make sure the vulkan runtime and sdk are installed.

- clone the repo with git clone
- cd into that new directory (bb-vulkan-engine)
- git submodule update --init --recursive
- mkdir build
- cd build
- cmake ..
- cmake --build .

## Running
Compile the shaders by running compileShaders.sh
(this will eventually be automated!!)

The "testbin" binary in the build directory is a test application that interfaces with the
vulkan library and show off some of the library's capabilities.

## Using the library
At the moment the library has no kind of practical interface
and all that can be done is looking at the source code and the example application "testbin".
As a proper interface is implemented, it will be documented.
