This Arduino sketch allows you to intercept data communicated between your SolArk inverter and its WiFi dongle, allowing you to get power data much more quickly and at much finer granularity locally. You can then act on this data to do things like turn EV chargers on and off.

If you open up the shell of the WiFi dongle, you will find the unpopulated pin header where the serial port can be intercepted. The pins are spaced 0.2 mm apart, which is closer than the normal 0.254 spacing.  But if you bend the pins on a three-pin 0.1-inch-pitch header and stuff it in those holes, it will hold with no soldering required.
The logic levels of that serial port are 3.3v, so they will directly connect to a 3.3v microcontroller.  In the wires in this photo, black is ground, green is Tx and white is Rx. They are connected to a D1 Arduino-style ESP8266 board (pin D8 is TX and pin D7 is RX).

![alt text](dongle_serial.jpg?raw=true)

There is a constant stream of traffic coming from the inverter and going to the dongle.  Looking carefully at the data with knowledge of what values the SolArk cloud was reporting (particularly the battery percentage, which changes slowly) I was able to find the packet containing the most important bytes. These packets are ASCII-encoded hexadecimal strings.  If you convert certain substrings of these hex values into either bytes or longs (sometimes using 2's complement, where negative is important) then you will get the most important values you care about. I've figured out what these are, though I hope to find more (such as the raw voltage values from the solar panel strings and battery, the former of which would help me determine how sunny it is, which I could then use to control a circulator pump for a solar hydronic system, while the latter would allow me to interpolate between the clunky integer steps of the battery charge percentage).

Since the ESP8266 doesn't fully support more than one serial port, I was forced to use the web page it serves to debug.

For now this sketch just produces a *-delimited list of values on the web page it serves:

1st is gridPower, 2nd is batteryPercentage, 3rd batteryPower (2's complement gives us negative for charging as opposed to draining), 4th is loadPower, 5th and 6th are the power produced by the two solar strings, and all this is followed by some integers whose purpose I do not yet know.

You will probably want to do something else with the data. What I do is send this data to a MySQL server to log it with a timestamp so I can see pretty (and extremely-detailed) graphs, which put those produced on the data page served by MySolArk to shame.  I do that using the SolArk Copilot

https://github.com/judasgutenberg/SolArk_Copilot

communicating with the backend of my remote control system

https://github.com/judasgutenberg/Esp8266_RemoteControl


