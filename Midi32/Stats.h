//#######################################################################
// Module:     Stats.h
// Descrption: Statistical data collection
// Creator:    markeby
// Date:       12/30/2025
//#######################################################################
#pragma once

class STATS_C
    {
private:
    int _VoiceCount[VOICE_COUNT];

public:
         STATS_C (void)             { this->Clear (); }
    void Bump    (int index)        { _VoiceCount[index]++; }
    void Clear   (void)             { memset (_VoiceCount, 0, sizeof (_VoiceCount)); }
    int  Data    (int index)        { return (_VoiceCount[index]); }
    };

extern STATS_C SynthStats;

