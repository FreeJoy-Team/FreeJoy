I. Building binaries from source with ARM GCC compiler

The firmware consist of 2 parts: 
- bootloader 
- application

Both parts can be build separately or together. For building use following commands:

1) Building bootloader:	

> make boot

2) Building application: 

> make app

3) Building both bootloader and application:

> make 

or 

> make all

4) Cleaning build directories:

> make clean


After building binaries and hex files are place in build sub-directories:

/build/app/Bootlooader.bin
/build/app/FreeJoy.bin
/build/app/FreeJoy.hex


II. Flashing binaries to MCU

Bootlooader and aplication have different base addresses in flash memory:

bootloader base address:	0x8000000
bootloader flash size: 		0x2000

aplication base address:	0x8002000
aplication flash size:		0xC800

So as you can see FreeJoy application have 0x2000 ofsset from the start of the flash memory (0x8000000). If you flashing .bin files the offset must be applied to the start address.




