# Arcade Controller Network
The Arcade Controller Network takes arcade controller joystick/button hardware and presents it to a computer or some game consoles (such as the Steam Deck), as standard modern game controllers. Allowing for versitle configurations of the arcade hardware to accomidate multiple types of games and controller scenerios. Even adding elements of modern game play into the arcade experience, such as modern rumble, analog joysticks and mouse/keyboard inputs.

Standard arcade controls, with the 4 way stick and 4 to 6 buttons, can be limiting when playing modern games. Modern consoles, such as the Steam Deck, have more advanced controls and mapping available. This project is meant to complement these advancements. Configuration profiles for the Arcade Controller Network are used to faciliate different modes of play. Like 4 player standard arcade setups (for arcade cabnets/boxes with that hardware), 2 player extended controls with multiple joysticks per player, or a super setup for one user with all of the modern game controls mapped to an arcade control. 

As the name suggests this isn't just about the controls. Each controller is networked together through the i2c bus, which runs between each controller device. The network aspect allows a configuration profile to shift a joystick/button/etc to another controller. This means that controls are not stuck to the controller that they are wired to. This provides a large amount of versatility in configuration and an easy way to shift play modes without rewiring your arcade cabnet. 
<p><br>

## Overview
This project consists of three separate components. Firstly, the hardware which wires up the inputs and outputs of the arcade controls with a Teensy 4.1 microcontroller in the middle. Secondly, the software which runs on the Teensy 4.1 microcontroller and presents itself as an Xbox controller or mouse/keyboard to a computer or game console, like the Steam Deck. (note: this can *not* be used with an Xbox or other game system which requires the controller security hardware)

Lastly, the profile configuration of the controls is loaded as a json file on the SD card. This allows for easy remapping of controls within minutes, customization to your cabnet / play style, and sharing of your favorite control schemes. When combined with per game control setting from something like the Steam Deck, this creates an exceptional arcade experence. 
<p><br>

### Arcade Controller Network Hardware

The hardware consists of a breakoutboard, with a few componenets to deal with rumble motors, inter-device network communication and the like. 
<p>
The following is the base schematic for V4 of the controller. It may look complex, but it is mainly routing pins to sockets. Note: the layout of the Teensy in this diagram is in logical form not physical form. 
<img src=".\docs\images\diagrams\Schematic_ArcadeControllerNetworkV4-wo-aux.svg">
There is also a <a href=".\docs\images\diagrams\Schematic_ArcadeControllerNetworkV4.svg">schematic with an aux header</a> for all unused pins.
<p><br>
TODO: BOM
<p><br>
### Arcade Controller Network Software
TODO:
install libraries:
RotaryEncoder by Mathias Hertel (reference link: http://www.mathertel.de/Arduino/RotaryEncoderLibrary.aspx)
## Arcade Controller Network Configuration
TODO:

* 4 player standard arcade setups (for arcade cabnets/boxes with that hardware)
* 2 player extended controls with multiple joysticks per player
* super setup for one user

## License Special Notes
The XInput library has special notes and considerations with it's license. The Arcade Controller Network project uses the XInput library as part of it's communication, though it's code is not included with this code repository. It is merely used as an external library added by users at compile time. However, the XInput restrictions should also likely apply to distributed binaries of this project, if the XInput library is in use. For more information see the "No Commercial Use" section at the following link: https://www.partsnotincluded.com/arduino-xinput-library/ 
<br>Note: I am not a lawyer and if I was I am not your lawyer, so YMMV.

