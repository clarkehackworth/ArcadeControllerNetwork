

# Arcade Controller Network
<img style="float: right;" src=".\docs\images\logos\ACN_logo.png" width="100px">
The Arcade Controller Network takes arcade controller joystick/button hardware and presents it to a computer or some game consoles (such as the Steam Deck), as standard modern game controllers. Allowing for versitle configurations of the arcade hardware to accomidate multiple types of games and controller scenerios. Even adding elements of modern game play into the arcade experience, such as modern rumble, analog joysticks and mouse/keyboard inputs.

Standard arcade controls, with the 4 way stick and 4 to 6 action buttons, can be limiting when playing modern games. Modern consoles, such as the Steam Deck, have more advanced controls and mapping available. This project is meant to complement these advancements. Configuration profiles for the Arcade Controller Network are used to faciliate different modes of play. Like 4 player standard arcade setups (for arcade cabnets/boxes with that hardware), 2 player extended controls with multiple joysticks per player, or a super setup for one user with all of the modern game controls mapped to an arcade control. 

As the name suggests this isn't just about the controls. Each controller is networked together through the i2c bus, which runs between each controller device. The network aspect allows a configuration profile to shift a joystick/button/etc to another controller. This means that controls are not stuck to the controller that they are wired to. This provides a large amount of versatility in configuration and an easy way to shift play modes without rewiring your arcade cabnet. 
<p><br>

## Overview
This project consists of three separate components. Firstly, the hardware which wires up the inputs and outputs of the arcade controls with a Teensy 4.1 microcontroller in the middle. Secondly, the software which runs on the Teensy 4.1 microcontroller and presents itself as an Xbox controller or mouse/keyboard to a computer or game console, like the Steam Deck. (note: this can *not* be used with an Xbox or other game system which requires the controller security hardware)

Lastly, the profile configuration of the controls is loaded as a json file on the SD card. This allows for easy remapping of controls within minutes, customization to your cabnet / play style, and sharing of your favorite control schemes. When combined with per game control setting from something like the Steam Deck, this creates an exceptional arcade experence. 
<p><br>

## Arcade Controller Network Hardware

The hardware consists of a breakoutboard, with a few componenets to deal with rumble motors, inter-device network communication and the like. 
<p>
The following is the base schematic for V4 of the controller. It may look complex, but it is mainly routing pins to sockets. Note: the layout of the Teensy in this diagram is in logical form not physical form. 
<img src=".\docs\images\diagrams\Schematic_ArcadeControllerNetworkV4-wo-aux.svg">
There is also a <a href=".\docs\images\diagrams\Schematic_ArcadeControllerNetworkV4.svg">schematic with an aux header</a> for all unused pins.
<p><br>

### Bill of Materials (per controller)
quantity 1 - Teensy 4.1 device and micro sdcard

quantity 1 - circuit board, following diagram above. Or perf board with magnet wires, wired by hand.

quantity 2 - Diode 1N4001 1A 50v

quantity 2 - resistor 680Ω

quantity 2 - Ceramic Capacitor 5.08mm 0.1u

quantity 2 - Transistor PN2222

quantity 2 - resistors 100kΩ

You can make the circuit board yourself with perf board and magnet wires. It is a lot of work, but is doable. Here is one I had previous made before getting PCBs.
TODO: add pic
For more information on precedures for wiring boards by hand see https://hackaday.com/2020/03/09/dont-scrape-magnet-wire-do-this-instead/


<p><br>

## Arcade Controller Network Software Setup

1. Download and install Arduino IDE. Link to software: https://www.arduino.cc/en/software
2. Install Teensyduino version which is compatible with ArduinoXInput_Teensy 
   a. in the Arduino IDE, navigate to "File" -> "Preferences"
   b. add the URL https://www.pjrc.com/teensy/package_teensy_index.json in the "Additional boards manager URLs" box, then click OK.
   c. open the Boards Manager by clicking the left-side board icon, search for "teensy", select a version which is compatible with Arduino Xinput. At the time of this writing the compatible version is 1.58. To find the latest compatible version check latest release notes here: https://github.com/dmadison/ArduinoXInput_Teensy/releases
   d. click "Install" for the compatible version
   These instructions adapted from https://www.pjrc.com/teensy/td_download.html
3. Install ArduinoXInputTeensy
   a. download lastest release from https://github.com/dmadison/ArduinoXInput_Teensy/releases
   b. unzip and copy files to correct location where {version} is compatible version number previously mentioned

      Windows: copy files to %AppData%\..\Local\Arduino15\packages\teensy\hardware\avr\{version} (TODO: check accuracy)
      
      Linux: TODO:
      
      Mac: $HOME/Library/Arduino15/packages/teensy/hardware/avr/{version}
4. Install dependancies
   a. Lauch Arduino IDE
   b. From the library manager search for "RotaryEncoder." Select and install the library by Mathias Hertel. (For additional information on the library see: http://www.mathertel.de/Arduino/RotaryEncoderLibrary.aspx)
5. Compile and flash Arcade Controller Network
   a. download zip of the Arcade Controller Network code from https://github.com/clarkehackworth/ArcadeControllerNetwork/archive/refs/heads/main.zip
   b. unzip and open src/src.ino with the Arduino IDE. The IDE may try to get you to install updates for some of the boards (i.e. the teensy board as the ArduinoXInput library may be a version behind). If this happens tell it "later." 
   c. if step 2 was successful you should be able to select the Teensy 4.1 by going to "Tools" -> "Board" -> "Teensy" -> "Teensy 4.1"
   d. if step 3 was successful you should be able to select XInput via "Tools" -> "USB type" -> "XInput"
   e. plug in your Teensy device into the computer via USB and click the upload (right arrow button) at the top of the IDE (or menu "Sketch" -> "Upload")
   f. The console at the bottom should scroll through some things. If all is successful the device should get flashed and you will see "TODO: ..." at the bottom on the console. 
6. Now that you have the device programmed successfully, it needs a configuration file. See the section on configuration.
<p><br>

## Arcade Controller Network Configuration

One of the benefits of the Arcade Controller Network is the flexibility of the configuration. Not only can it support multiple types of arcades, but can also support multiple profiles at runtime for different game/play styles. 

Default configuration profiles are setup in the following ways: 
1. Standard arcade controls with DPADs, sholder buttons default
2. Arcade presenting analog joysticks, trigger buttons default
3. Merged controls with two sticks (DPAD and analog) and extended buttons for sholders and triggers. On 2 player cabnets this accomindates one player. While on 4 player cabnets this accomidates 2 players. 
4. Super merged controlls, for 4 player cabnets only. TODO: ???

TODO: double taps?

Download of configruations per arcade cabnet setups:
* 4 player arcade cabnets
    * digital sticks and 8 buttons (6 + start & select) TODO: download link
    * digital sticks and 6 buttons (4 + start & select) TODO: download link
    * digital sticks and mixed buttons - (dual 8 buttons + dual 6 buttons) TODO: download link
* 2 player arcade cabnets
    * digital sticks and 8 buttons (6 + start & select) TODO: download link
    * digital sticks and 6 buttons (4 + start & select) TODO: download link
 
<p><br>

## Debugging
When flashing the Teensy device the USB type can be set to Serial instead of XInput. This will allow you see the log in real time. The device will no longer work as a game controller, however it will allow for some real time debugging. Alternatively, in XInput mode the teensy device will place a log file ArcadeControllerNetwork.log on the SD card, assuming the SD card is working properly.  
<p><br>

## License Special Notes
The XInput library has special notes and considerations with it's license. The Arcade Controller Network project uses the XInput library as part of it's communication, though it's code is not included with this code repository. It is merely used as an external library added by users at compile time. However, the XInput restrictions should also likely apply to distributed binaries of this project, if the XInput library is in use. For more information see the "No Commercial Use" section at the following link: https://www.partsnotincluded.com/arduino-xinput-library/ 
<br>Note: I am not a lawyer and if I was I am not your lawyer, so YMMV.

