//#######################################################################
// Module:     Channel.h
// Descrption: Synth channel instance class
// Creator:    markeby
// Date:       8/1/2022
//#######################################################################
#pragma once
#include "Osc.h"

//#################################################
//    Synthesizer channel complete voice instance
//#################################################
class CHANNEL_C
    {
private:
    OSC_C*  OscP;                   // oscillator class
    byte    Key;
    int32_t ActiveTimer;
    int     Number;
    byte    UseCount;
    bool    SawToothDirectionSet;
    float   PulseWidthSet;
public:
    bool    SelectedEnvelope[ENVELOPE_COUNT];

             CHANNEL_C            (int num, int osc_d_a, ENVELOPE_GENERATOR_C& envgen);
    void     Begin                (void);
    void     Loop                 (void);
    void     NoteSet              (byte key, byte velocity);
    bool     NoteClear            (byte key);

    void     Clear                (void)                  { this->OscP->Clear (); }
    void     SawToothDirection    (bool data)             { this->OscP->SawtoothDirection (data);  this->SawToothDirectionSet = data; }
    bool     GetSawToothDirection (void)                  { return (this->SawToothDirectionSet); }
    void     PulseWidth           (float percent)         { this->OscP->PulseWidth (percent);  this->PulseWidthSet = percent; }
    float    GetPulseWidth        (void)                  { return (this->PulseWidthSet); }
    void     SetMaxLevel          (byte ch, float level)  { this->OscP->SetMaxLevel (ch, level); }
    uint32_t IsActive             (void)                  { return (this->ActiveTimer); }
    OSC_C*   pOsc                 (void)                  { return (this->OscP); }
    };

