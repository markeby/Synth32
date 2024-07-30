//#######################################################################
// Module:     ClientI2C.h
// Descrption: Display command receiver
// Creator:    markeby
// Date:       6/23/2024
//#######################################################################
#pragma once
#include "../Common/DispMessages.h"

#define MIDI_TRIGGER_PORT   6

void  StartI2C  (uint8_t device_addr);     // Set listening addresss and start listening

inline void SendTriggerToMidi ()            // Trigger Midi side to send Current settings
    {
    digitalWrite (MIDI_TRIGGER_PORT, LOW);
    delay (RESET_TRIGGER_TIME / 1000);
    digitalWrite (MIDI_TRIGGER_PORT, HIGH);
    delay (50);
    digitalWrite (MIDI_TRIGGER_PORT, LOW);
    }



