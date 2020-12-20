CoolerMaster has a nice mouse for the price range, the CM Storm Alcor. However, unlike its brother, CM Storm Mizar, it does not have a software nor any means of changing the LED color.

Fortunately, it's not a device limitation: The firmware for the Alcor and Mizar are the same, therefore much like the Mizar, it's possible to set Alcor's LED color to any RGB color at runtime, by sending the correct commands through the USB device.

This application does just that. It supports Windows (GUI and CLI) and Linux (CLI only).

*This also works on the Mizar with the mizar_patch!*

## Installation

### Option 1: Binary

Pre-compiled binaries for Windows are available in the [Releases page](https://github.com/thelink2012/cmalcor/releases).

You need [.NET Framework 4.5](https://www.microsoft.com/pt-br/download/details.aspx?id=30653) to run the GUI.

### Option 2: Building

You may wish to build the project manually.

For Windows, execute the `build-win32.bat` script in the root of this repository. Run it from the Visual Studio Command Prompt, You also need [ILMerge](https://www.microsoft.com/en-us/download/details.aspx?id=17630) on PATH.

For Linux, `cmalcor-gui` is not supported, and interacting with the USB device is done through the `hidapi` package. Please make sure you have that installed on your system. Then `cd cmalcor` and use `premake5 gmake` to generate a Makefile which you can use to build by using `make`.

## Usage

![CmAlcorGUI](http://i.imgur.com/ZpXAZWM.png)

 + Select the color, brightness and LED mode you want, then click **Apply**, the changes will take effect immediately.
 + By clicking in **Default** you'll get the default behaviour of the Alcor _(fixed colors on certain DPI)_.
 + Due to firmware limitations, the color is not restored when the mouse is turned on again, thus by using the **Restore LED Color On Windows Startup** you can install a task that applies the color on Windows startup.
 + You may store your own colors for future use at the secondary pallete on the right. Do note those are stored on the filesystem, not on mouse memory. The primary pallete are the selectable colors for the Mizar.

Notice that some colors aren't exactly the same on the physical LED. For example, the last color on the primary pallete is the color used for *white* in the Mizar Software and on some Alcor Firmwares, even though it's not really RGB white.

## Troubleshooting

Notice this software comes with no warranty (see LICENSE), but if you have any trouble don't hesitate to open a issue.

Possible problems you may encounter (directly or indirectly) are listed down here.

### 1. CoolerMaster Warranty

There's no problem in leaving the LED color settings on memory. However, it may _(or may not)_ break your warranty with CoolerMaster to have custom settings on the Alcor memory if you send the mice back to them.

So, to erase the setting flash memory, run the following commands on the provided CLI program, do note it may take a while, **DON'T DISCONNECT THE MICE NOR CLOSE THE CLI** during the process.

    cmalcor disable-custom-led
    cmalcor flash-erase 0xD800 0x1F7FF --force
    cmalcor memory-read 0xD800 0x1F7FF >flash.txt
    
After that, reconnect your mouse to re-enable the scroll wheel and the LED.

It's highly recommended that **after** running `disable-custom-led` and **before** running `flash-erase`, you make physically sure (by looking at the mouse) that you are not getting any customized color / behaviour.

You can make sure the contents were really erased by checking if the output `flash.txt` is purely `0xFF`.

### 2. Flash Endurance

The settings are stored in a flash memory embedded on the microcontroller, and, as you may know, the amount of program/erase cycles on Flash memory is finite.

According to the documentation of the MCU used both by the Mizar and the Alcor, its flash memory supports at least 20000 P/E cycles before failure, that seems quite big, but I'd still recommend being careful on how many times you reprogram the LED.

But well, this is true for the Mizar as well... Maybe I'm just being paranoid...

### 3. Scroll wheel (and LED) stopped working

Just disconnect and connect the mouse again and it'll be back.

This shouldn't happen to anyone, but it did during my test sessions because, well, tests.

Don't worry, nothing is breaking, the reason for this is pretty simple: During reprogramming, the timer used to update both the LED and the scroll wheel is disabled, if reprogramming isn't completed successfully, the timer won't be restarted [until you reset the device].
