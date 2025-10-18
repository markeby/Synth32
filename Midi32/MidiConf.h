//#######################################################################
// Module:     MidiConf.h
// Descrption: MIDI Interfaces
// Creator:    markeby
// Date:       8/25/2023
//#######################################################################
#pragma once
#include <UHS2-MIDI.h>
#include <MIDI.h>

typedef MIDI_NAMESPACE::Message<MIDI_NAMESPACE::DefaultSettings::SysExMaxSize> MidiMessage;

using MidiUsb_u    = midi::MidiInterface<uhs2Midi::uhs2MidiTransport>;
using MidiSerial_u = midi::MidiInterface<midi::SerialMIDI<HardwareSerial>>;

extern MidiUsb_u      Midi_0;
extern MidiSerial_u   Midi_1;
extern MidiSerial_u   Midi_2;
extern USB Usb;




