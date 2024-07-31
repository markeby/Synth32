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
Any midi controller will work.  I am using an inexpensive Smason Graphite 49.  I intend to add bluetooth and serial midi inputs as well.

The design is for 6 simultanous notes whether they be all of the same effect or split the keyboard to have dual effects but still be able to play cords.

This project is been on going for over a year now and will likely be a few more years of effort.

I have full schematics included as well as some of the experimental work I have done.  This is an unconventional modular design so as to maintain complete digital control. 

