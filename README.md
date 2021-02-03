# WYOOS

## Introduction
Writing my own operating system based on the [Write Your Own Operating System](http://wyoos.org/) tutorial till memory management part.
## Reqirements
 - g++
 - binutils
 - libc6-dev-i386
## Installation
If you want to make an export of .ISO file you can use this command in make file:
```bash
make kernel.iso
```
You can also build and add binary file to the grub with command
```bash
make install
```
Then if you want to boot it on hardware you should add menu item to the grub.cfg file.

## Implemented topics

 - Global Descriptor Table (GDT)
 - Interrupt Descriptor Table (IDT)
 - Handling Interrupts
 - Peripheral Component Interconnection (PCI)
 - Video Graphics Array Communication(VGA)
 - Dynamic Memory Allocation (malloc)
