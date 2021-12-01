# Teensy 3.1

## Download LiteOS

```bash
https://github.com/nabilbendafi/LiteOS
cd LiteOS
```

## Compilation

  - Download `GCC cross compiler for ARM Cortex-R/M processors`
```bash
sudo apt install gcc-arm-none-eabi
```
  - Configure eventually and build
```bash
make menuconfig
make
```
  - Convert `ELF` binary to `Intel HEX` file format
```bash
arm-none-eabi-objcopy -O ihex out/MK20DX256_Teensy31/Huawei_LiteOS.{elf,hex}
```

## Burn 

After compilation, use [Teensy Loader](https://www.pjrc.com/teensy/loader.html) to upload/flash `.hex` file to `Teensy` 3.1

## Run

  - Attach `Teensy` 3.1 UART0 RX/TX (3V and GND) to PC, using for ex. RS232-USB adaptor.
  - Open terminal and enjoy !
```bash
$> screen /dev/ttyUSB0 115200
********Hello Huawei LiteOS********

LiteOS Kernel Version : 5.1.0
Processor : Cortex-M4 (@ 41 Mhz)
Build date : Nov 26 2021 13:54:56

**********************************
OsAppInit
cpu 0 entering scheduler
Hello, welcome to liteos demo!

Huawei LiteOS # task

Name                   TaskEntryAddr       TID    Priority   Status       StackSize    WaterLine      StackPoint   TopOfStack   SemID        EventMask   MEMUSE
----                   -------------       ---    --------   --------     ---------    ----------     ----------   ----------   ----------   ---------   ------
IdleCore000            0x00001f99          0x0    31         Ready        0x800        0x80           0x1fffd844   0x1fffd0b0   0xffffffff   0x0         0
LED_Blink              0x0000b275          0x2    10         Delay        0x600        0xc4           0x1fffe44c   0x1fffded0   0xffffffff   0x0         0
FileSystemTask         0x0000cb61          0x3    2          Invalid      0x800        0xbc           0x1fffec9c   0x1fffe4e0   0xffffffff   0x0         2168
SerialShellTask        0x0000a185          0x4    9          Running      0x1000       0x56c          0x20000914   0x1ffffa00   0xffffffff   0xfff       2356
SerialEntryTask        0x00009d6d          0x5    9          Delay        0x600        0x494          0x20000f64   0x20000a10   0xffffffff   0x0         32

Huawei LiteOS #
Huawei LiteOS # uname -a
Huawei LiteOS V200R005C20B053-SMP Huawei LiteOS 5.1.0 Nov 26 2021 13:54:58

Huawei LiteOS #
Huawei LiteOS # hwi
InterruptNo     Share     ResponseCount     Name             DevId
-----------     -----     -------------     ---------        --------
15                N       985461    
61                N       18        
Huawei LiteOS # systeminfo

   Module    Used      Total
--------------------------------
   Task      5         16
   Sem       0         20
   Mutex     5         20
   Queue     1         10
   SwTmr     0         16
```
