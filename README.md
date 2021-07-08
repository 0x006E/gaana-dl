# Download music from [Gaana](https://gaana.com)
This repo probably doesn't work now!. 
## Compiling
  1. Clone repo:
    `git clone https://github.com/0x006E/gaana-dl`

  2. Pull down all submodules:
    `git submodule update --init --recursive`

  3. Install CMake:
    `sudo apt install cmake`

  4. Start build:
    `cmake . && make`
  
## Running
To download tracks

  `$ ./gaana-dl -u <URL for the gaana song>`
