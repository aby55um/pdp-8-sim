# PDP 8 Simulator

Compilation:

Prerequisites: SDL3: https://github.com/libsdl-org/SDL.git

clone the pdp-sim source:

git clone https://github.com/aby55um/pdp-8-sim.git

cd pdp-8-sim

gcc hello.c -L/path/to/SDL3/build/ -lSDL3

./a.out


## Usage:

1. Write the program

2. Press Left SHIFT to compile

3. Press Right SHIFT to step

4. ESC to quit

## Currently implemented commands:

{
                            "and","tad","isz","dca","jms","jmp",
                            "cla","cll","cma","cml","iac","rar","ral","rtr","rtl","bsw",
                            "sma","sza","snl","spa","sna","szl",
                            "osr","hlt",
                            "mqa","mql"
                        };