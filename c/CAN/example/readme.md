# XNET C-code example

This example starts and listens on CAN frames. It uses the shared library libnixnet.so and nixnet.h
For API help, see section NI-XNET API for C in document NI-XNET Hardware and software help.

## Example usage

1) Clone repo
2) Create build directory
3) Run cmake -D CMAKE_CXX_COMPILER=x86_64-nilrt-linux-gcc .. && make && ./CAN_Frame_Stream_Input

## Tip on signal generation

Use NI-XNET Bus Monitor to generate CAN signal.

![image](https://user-images.githubusercontent.com/86668144/146000089-50e25f33-40a4-432c-ad65-15613b875c2c.png)

