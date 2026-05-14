Note: as of November 13 2025, SolArk quietly changed their internal serial protocol, so this sketch no longer works.
However, I do have parsing working once more in the SolArk Copilot.  See: <a href=https://github.com/judasgutenberg/SolArk_Copilot target=solarc>SolArk Copilot</a>
This is what I wrote to SolArk Tech support:

<blockquote>
I had been intercepting serial data being communicated between the WiFi Dongle and the SolArk inverter for use in real-time automation on-premises (that is, not using the cloud) but a firmware update seems to have completely changed the format of this data. I have tried to write a parser for the new data but have not had success. I am asking to confirm whether such a data format change happened from a firmware dated Oct 15 2025 and whether anything can be done to restore the old firmware.
</blockquote

  Here was their response:
<blockquote>

Thank you for reaching out, and I appreciate the detailed explanation of what you are trying to accomplish.

I do want to clarify a few important points regarding the Wi-Fi dongle and the data stream you were previously accessing.

1. The firmware update did change the internal serial data format
Yes, beginning with the October 15, 2025 firmware release, the internal communication protocol between the Wi-Fi dongle and the inverter was updated. This update included packet restructuring, encapsulation changes, and data-handling improvements. Because this is an internal communication channel, the format is not guaranteed to remain fixed from one firmware version to another.

2. The internal serial stream is not a supported interface
The UART/serial data exchanged between the dongle and the inverter is considered an internal protocol. It is not documented, and it can change at any time as part of normal firmware development. For this reason, engineering does not provide or support access to that stream for third-party automation.

3. Firmware rollback is not available
We are not able to roll dongles or inverters back to older firmware versions. All units automatically move forward in order to maintain system stability, cybersecurity, and compliance.

</blockquote>


This Arduino sketch allows you to intercept data communicated between your SolArk inverter and its WiFi dongle, allowing you to get power data much more quickly and at much finer granularity locally. You can then act on this data to do things like turn EV chargers on and off.

If you open up the shell of the WiFi dongle, you will find the unpopulated pin header where the serial port can be intercepted. The pins are spaced 0.2 mm apart, which is tighter than the normal 0.254 mm (0.1 inch) spacing.  But if you bend the pins inward on a three-pin 0.1-inch-pitch header and stuff it in those holes, it will hold with no soldering required and work reliably.
The logic levels of that serial port are 3.3v, so RX and TX can connect directly to pins on a 3.3v microcontroller.  For the wires in this photo, black is ground, green is Tx and white is Rx. They are connected to a D1 Arduino-style ESP8266 board (pin D8 is TX and pin D7 is RX). None of this affects the normal behavior of the dongle, which continues communicating data to Chinese intelligence agencies or whatever (disabling and re-enabling that remotely is something I will be exploring).

![alt text](dongle_serial.jpg?raw=true)

There is a constant stream of data coming from the inverter and going to the dongle.  Looking carefully at this data with knowledge of what values the SolArk cloud was reporting at the time (particularly the battery percentage, which changes slowly) I was able to find the packet containing the most important bytes. These packets are ASCII-encoded hexadecimal strings.  If you convert certain substrings of these hex values into either bytes or two-byte words (sometimes using 2's complement, where negative is important) then you will get the important values you most care about. I've figured out what these are, though I hope to find more (such as the raw voltage values from the solar panel strings and battery -- the former of which would help me determine how sunny it is, which I could then use to control a circulator pump for a solar hydronic system, while the latter would allow me to interpolate between the clunky integer steps of the battery charge percentage).

Since the ESP8266 doesn't fully support more than one serial port, I was forced to use the web page it serves to debug. An ESP32 would've solved that problem, but I like squeezing all the utility I can out of its plucky predecessor.

For now this sketch just produces a *-delimited list of values on the web page it serves:

1st is gridPower, 2nd is batteryPercentage, 3rd batteryPower (2's complement gives us negative for charging as opposed to draining), 4th is loadPower, 5th and 6th are the power produced by the two solar strings, and all this is followed by some integers whose purpose I do not yet know.

You will probably want to do something else with the data. What I do is send it to a MySQL server to log it with a timestamp so I can see pretty (and extremely-detailed) graphs, which put those produced on the data page served by MySolArk to shame.  I do that using the SolArk Copilot

https://github.com/judasgutenberg/SolArk_Copilot

communicating with server.php in the backend of my remote control system

https://github.com/judasgutenberg/Esp8266_RemoteControl


