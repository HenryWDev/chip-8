# chip-8

## Building

### Install build tools

`sudo apt-get update && sudo apt-get install build-essential`

### install required packages

`sudo apt-get install libglfw3-dev libgl1-mesa-dev libglu1-mesa-dev`

### Build commands
From the projects root directory:

1. Create build information:

`cmake -S . -B ./build`

2. Build the project:

`cmake --build ./build`


