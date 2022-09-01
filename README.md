# msp430-cli

A Command Line Interface (CLI) for MSP430FR5969 LaunchPad

## Environment

* PC:             Linux Mint 20.3 Una 64-bit
* Compiler:       msp430-gcc
* Flasher:        mspdebug
* IDE:            None
* Build:          Makefile
* Serial Console: GTKTerm

## How to compile and upload the program

To compile and load program into the board, you need to install some requisite tools such as: *msp430-gcc*, *mspdebug* and *make*.
After above tools have already installed completely, you perform the guide as follows:

* Compile and upload the program on MCU

  ```shell
  make all
  ```

* Debug (if needed)

  * Start gdb server with port 2000

    ```shell
    sudo mspdebug rf2500
    ```

  * Another windows, start gdb client

    ```shell
    make debug
    ```

  * Finally, connect to gdb server and debug.

    ```shell
    (gdb) target remote localhost:2000
    (gdb) load build/cli_main.elf
    (gdb) continue
    ```

* Clean the project

  ```shell
  make clean
  ```

* Using the serial interface

  ```shell
  gtkterm -s 9600 -p /dev/ttyACM1
  ```

## Screenshot

![MSP430-CLI](https://raw.githubusercontent.com/nhivp/msp430-cli/master/docs/imgs/msp430-cli.png "MSP430-CLI")

## Changelog

See [CHANGELOG.md](CHANGELOG.md)

## Useful links

* [A list of embedded command-line-interface projects](http://www.dalescott.net/an-embedded-command-line-interface/)

## Contributing
Feel free to create pull requests and I will gladly accept them! :clap: :clap:

Good luck, and happy hacking?! :yum:
