# Build a HomeKit-compatible Bluetooth Temperature and Humidity Sensor

I don't know if you have ever been to Montreal in the winter, but the weather
here this time of year is quite special. Not only the temperature varies a
lot, but also humidity can drop dramatically. And those electric heaters
in the appartment we're renting dry out the air in the room really fast.
Basically, without a decent humidifier you can end up with 20% humidity and
even below, which is extremely bad for your health, not to mention little
kids who start getting sick all the time.

Long story short, knowing temperature and humidity turned out to be pretty
important for us, especially during the winter. And since Siri is already
doing a lot of stuff at our place, I figured it would be nice to make her
aware of the temperature and humidity in our rooms as well.

So, I went shopping. Essentially, there was only two devices on the market:

- [Elgato Eve Room](https://www.elgato.com/en/eve/eve-room) *USD 79.95*
- [iHome iSS50 SmartMonitor](https://www.ihomeaudio.com/iSS50/) *USD 79.99*

Seemed kinda pricy, especially since I wasn't planning on using their
software component (I consider having as less apps on my phone as possible
a benefit). Besides, the first one wasn't easily available in Canada.

So, I asked myself: can we go with something simple and home-made instead?
Turned out the answer was yes. And while this solution is not cheaper by
a lot and probably doesn't look as elegant, but it's certainly more fun
for someone who likes to tinker around with electronics.

## Shopping List

Here's what you gonna need:

- [Arduino 101](https://www.adafruit.com/products/3033) (aka Genuino 101
outside US) *USD 39.95*
- [DHT22](https://www.adafruit.com/products/385) or
[AM2302 (aka wired DHT22)](https://www.adafruit.com/products/393)
*USD 9.95-15*
- [Raspberry Pi 3](https://www.adafruit.com/products/3055)
(alternatively, RPI 2 with a Bluetooth / WiFi dongles works pretty well
also) *USD 39.95*
- cables to power/program Arduino (A-male to B-male), to connect your Pi
to a monitor (HDMI to HDMI), wires (if your sensor is not wired), wall
adapters to power Arduino and Pi *USD 20-ish* (but you probably have a
bunch of those somewhere already)

I know, if you sum it up, it's actually more expensive then any of the
ready-to-use devices from the previous section, but:

- there's a chance you have a lot of the stuff already (like wall
adapters, cables, etc.);
- you only need one Raspberry Pi if you want to build multiple sensors,
e.g.for different rooms (unless you have a large house with rooms that
are out of Bluetooth range from the spot you're planning to put your
Pi hub in);
- since Arduino is an open source hardware, there's a chance you can
find a nice knockoff, and those can be really cheap, and though I
haven't seen any for Arduino 101, but I'm pretty sure there'll be
plenty of them eventually, just like
[there is for UNO now](https://www.amazon.com/s/url=search-alias%3Daps&field-keywords=arduino+uno+clone&sprefix=arduino+uno+clo%2Caps%2C137);
- finally, the fun you're gonna have while building it is, obviously,
priceless. ;)

Also worth mentioning that the reason we want to use Arduino 101 is
simply because it's extremely convinient to have Bluetooth Low Energy
already built in. And since our plan is to have that Arduino connected
to an RPI-based hub, that is in its turn connected to a WiFi network,
using Pi 3 that comes with WiFi and Bluetooth is certainly a lot
easier as well.

## Arduino Wiring

Connecting DHT sensors to Arduino is pretty simple, see
[this guide](https://learn.adafruit.com/dht/connecting-to-a-dhtxx-sensor),
for example. If you go with AM2302, it's even simpler: since it already
has a resistor inside, you don't need any additional pullup resistors
and can just stick the wires into the board: the black one gets grounded,
the red one goes to 5V and the yellow is for the data, which we'll be
using pin 2 for.

And that's pretty much all the wiring that needs to be done for the
project.

## Programming Arduino

If you're just starting with Arduino,
[this guide](https://www.arduino.cc/en/Guide/Arduino101) will get you
covered, and it's 101-specific. Otherwise just make sure you have
Curie Core library installed.

Next you're gonna need libraries for your DHT sensor. We'll go with
Adafruit's *DHT Sensor Library*, which requires *Adafruit Universal Sensor
Library*. You can install both of those via Library Manager of your
Arduino IDE (see Sketch > Include Library menu).

Once it's done, you're ready to upload your firmware. Grab a sketch from
[dht_ble.ino](dht_ble.ino) and paste it in the IDE. Upload it to the
board and your BLE temperature and humidity sensor is ready. Let's connect
it to Apple HomeKit now.

## Setting Up Your Homebridge

In order for your Arduino-based sensor to communicate with your Apple
devices you should have some bridge server running on your WiFi network.
We're gonna use [Homebridge](https://github.com/nfarina/homebridge) which
has a lot of plugins for different platforms, including one for BLE: [Homebridge
Bluetooth Plugin](https://github.com/vojtamolda/homebridge-bluetooth).
This makes our task as easy as Pi (pun intended).

Basically, assuming that you have your RPI set up (you can follow
[this guide](https://www.raspberrypi.org/documentation/setup/), for example)
and running one of the latest versions of Raspbian, all you need to do is
to install Node, Homebridge (both covered
[here](https://github.com/nfarina/homebridge/wiki/Running-HomeBridge-on-a-Raspberry-Pi))
and BLE plugin (simply run `[sudo] npm install -g homebridge-bluetooth`).
I would recommend paying special attention to the
[section describing running Homebridge on the system
startup](https://github.com/nfarina/homebridge/wiki/Running-HomeBridge-on-a-Raspberry-Pi#running-homebridge-on-bootup),
since it would let you have a box you can unplug at any moment, then
plug back, and it'll be up and running automatically in seconds.
Configuration basics are covered there as well, but let's look at our
specific case closer.

## Configuration

In order to add your accessories to the Home app, your Homebridge server
needs to be aware of them, which gets done via `config.json` (the
specific location depends on your Homebridge setup, as described in the
setup guide mentioned above). In our case, we should add a Bluetooth
platform with our Arduino as accessory and two services (for temperature
and humidity). See [config.example.json](config.example.json) for details.
Note however, that if you're setting up several devices, they should have
different UUIDs, though corresponding to those you got set in the Arduino
sketch.

## Running

At this point all should be set up and ready to get going. Power up your
Arduino using a wall adapter. It's tempting to try making your sensor
completely wireless and try running it on a battery, but turns out
Arduinos are actually consuming a lot of energy even when they are not
doing anything, so using, for example, a 9V battery will run it dry in
several days, unless you [optimize it on both software and hardware
side](https://hwstartup.wordpress.com/2013/03/11/how-to-run-an-arduino-on-a-9v-battery-for-weeks-or-months/).
But I'd say it's a different topic, right now we just wanna see the device
working.

Next, power up your RPI with an adapter as well and start the Homebridge
server as described in the
[setup guide](https://github.com/nfarina/homebridge/wiki/Running-HomeBridge-on-a-Raspberry-Pi).

Finally, open the Home app on your iPhone and add Homebridge as a new
accessory using the *pin* from your configuration file. If all is done
correctly, you should see your new sensors in the HomeKit accessory list.

## Credits

* [Homebridge](https://github.com/nfarina/homebridge)
* [Homebridge Bluetooth Plugin](https://github.com/vojtamolda/homebridge-bluetooth)
* [Adafruit DHT Sensor Library](https://github.com/adafruit/DHT-sensor-library)

## License

[The MIT License](http://opensource.org/licenses/MIT)

Copyright (c) 2017 [Ruslan Bredikhin](http://ruslanbredikhin.com/)
