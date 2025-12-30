# Synth32

This project was begun as a darivative of much simpler projects.

The goal is a psudo polyphonic analog and/or multi-timbral synthesizer that is digitally controlled.  

Utilizing components from the following manufacturers:

- Espressif 
  - ESP32 processors
- Sound Semiconductor, FatKeys™
  - VCO, VCF, VCA
- Adafruit
  - D/A Converter modules, I/O Expanders
- HiLetgo
  - 2.0 ADK Mini USB Host Shield
- Waveshare
  - Display module

The core of the system is an ESP-WROOM-32 ESP32 ESP-32S development board with a  USB development shield to interface a midi keyboard.
Any midi controller will work.  I am using an inexpensive Samson Graphite 49 on the serial MIDI and a Novation LaunchControl XL on the USB MIDI.
I intend to add bluetooth MIDI input as well.

The design is for 8 simultanous notes whether they be all of the same effect or split into as many as four voice pairs with unique sound each.  

There is a second serial MIDI port for sequencer input.  I am currently usinc Logic Pro on a Macbook Air M1. 

This project has been on going for over two years now and will likely be a few more years of effort.

I have full schematics included as well as some of the experimental work I have done.  This is an unconventional modular design so as to maintain complete digital control. 

The tools used are:

- KiCad
- Visual SlickEdit IDE - I have used this for over 20 years and have tried others but found them either missing features or a colossal amount of plugins to figure out.
- Arduino-cli 
- ESP32 version 3.1.1 "arduino-cli core install esp32:esp32@3.1.1"
- Gnu make for Windows (could be used in Linux as well)
- Espressif frameworks tools to upload utilizing OTA libraries. 
- [SimplySerial](https://github.com/fasteddy516/SimplySerial) - The best serial terminal software I have seen for this application debugging.  
  Thanks [Edward Wright (fasteddy516)](https://github.com/fasteddy516).

