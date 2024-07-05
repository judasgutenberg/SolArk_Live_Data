This Arduino sketch allows you to intercept data communicated between your SolArk inverter and its WiFi dongle, allowing you to get power data much more quickly and at much finer granularity locally. You can then act on this data to do things like turn EV chargers on and off.

If you open up the shell of the WiFi dongle, you will find the unpopulated pin header where the serial port can be intercepted. The pins are spaced 0.2 mm apart, which is closer than the normal 0.254 spacing.  But if you bend the pins on a three-pin 0.1-inch-pitch header and stuff it in those holes, it will hold with no soldering required.
The logic levels of that serial port are 3.3v, so they will directly connect to a 3.3v microcontroller.  In the wires in this photo, black is ground, green is Tx and white is Rx. They are connected to a D1 Arduino-style ESP8266 board (pin D8 is TX and pin D7 is RX).

![alt text](dongle_serial.jpg?raw=true)

Since the ESP8266 doesn't fully support more than one serial port, I was forced to use the web page it serves to debug.

For now this sketch just produces a *-delimited list of values on the web page it serves, though you probably will want to do something else. What I do is send this data to a MySQL server to log it with a timestamp so I can see pretty (and extremely-detailed) graphs, which kick the ass of those produced by SolArk.


