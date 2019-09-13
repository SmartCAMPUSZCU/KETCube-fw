# KETCube (Linux) Makefile Project Usage

** Note: this is poorly tested workflow and a documentation snippet - it requires further evaluation. Finally, it will be transformed to the AppNote in the future! **

KETCube-fw can be compiled and flashed by using classic Makefile approach.
This allows to use KETCube project with any Makefile-ready IDE.

It is sometimes hard to get it work, thus this short HowTo describes how to set-up your system to be able to compile and flash KETCube-fw.

A KDevelop IDE and Kate editor were used for user-level testing (Kate and KDevelop configuration files are available).

## Prerequisities
Quick step-by-step guide:
  * Install the following packages: cmake, libusb1 (Fedora28's dnf for refference: dnf install cmake libusb-devel)
  * Install arm-none-eabi (Fedora28's dnf for reference: dnf install arm-none-eabi-gcc-cs arm-none-eabi-gcc-cs-c++ arm-none-eabi-newlib arm-none-eabi-binutils-cs arm-none-eabi-gdb)
  * Install st-link (to /usr/local):

~~~bash
git clone github.com:texane/stlink.git
cd stlink
make release
su
cd build/Release && make install
~~~

  * it may be necessary to set-up LD_LIBRARY_PATH for st-link:

~~~bash
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib
~~~

  * for debugging, install openOCD and gdb (Fedrora28's dnf for reference: dnf install openocd gdb)


## KETCube Makefile
  * Makefile expects st-link in /usr/local - if you have another path, please modify "STLINK" variable.
  * If you are adding new files to KETCube project, don't forget to modify "SRCS" and "ASRCS" variable respectively.
  
  * Test KETCube connection
~~~bash
make chipinfo
st-info --chipid
0x0447
~~~
  * full flash erase: 
~~~bash
make erase
~~~
  * compile&link KETCube-fw: 
~~~bash
make
~~~
  * display KETCube-fw size: 
~~~bash
make size
~~~
  * program KETCube-fw: 
~~~bash
make flash
~~~

## Debug

  * erase MCU Flash:

~~~bash
make erase
~~~

  * run openOCD by using the following command:
~~~bash
openocd -f ketcube-openocd.cfg
~~~

  * in anther terminal run the following make target to start GDB debugger:

~~~bash
make debug
~~~

## References
  * A handy STM32 Makefile HowTo is provided by [Łukasz Podkalicki](http://blog.podkalicki.com/how-to-compile-and-burn-the-code-to-stm32-chip-on-linux-ubuntu/)
  * Details by Fedora's [Embedded Systems Development Special Interest Group](https://fedoraproject.org/wiki/Embedded)
  * ST-Link [documentation](https://github.com/texane/stlink/tree/master/doc)
  * OpenOCD [README](http://openocd.org/doc-release/README)
