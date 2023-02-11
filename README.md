# TFA 433 for Dostmann 30.3208.02

Use your Arduino to receive temperature and humidity data from TFA Dostmann 30.3208.02 remote sensor.

I believe it should also work for these devices, though it has not been tested on them:
- Ambient Weather F007TH Thermo-Hygrometer
- Ambient Weather F012TH Indoor/Display Thermo-Hygrometer
- SwitchDoc Labs F016TH

## Download
https://github.com/d10i/TFA433

## Info
### Hardware

There are several 433 MHz receiver available in many places. I used a cheap type and it works perfectly. You should use Arduino's interrupt enabled digital input pins (on Uno/Nano: D2 or D3) since this library built on top of that feature.

### Usage

In the examples directory you can find a simple way of usage.

### Inspiration

The library README, interface and code structure has been inspired by https://github.com/denxhun/TFA433. However, the actual implementation is completely different and is based on the receiving logic found in https://github.com/robwlakes/ArduinoWeatherOS. The TFA package structure and the verification logic has been made possible thanks to https://github.com/merbanan/rtl_433.
