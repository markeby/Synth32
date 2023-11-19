//
//   SynthData.h
//
//   author: markeby
//
#pragma once

class SYNTH_DATA_C
    {
private:
    typedef struct
        {
        bool        Active;             // LimitLevel > 0
        bool        Selected;           // Currently in the selected state
        uint16_t    LimitLevel;         // Maximum value for channel as 12 bit D/A
        float       AttackTime;         // Attack time in mSec.
        float       DecayTime;          // Decay time to sustatin level in mSec.
        uint16_t    SustainLevel;       // Sustain level in channel as 12 bit D/A
        float       SustainTime;        // How long to hold the sustain level (-1 = hold with key down)
        float       ReleaseTime;        // How long to devel 0 in mSec.
        int         Color;
        String      Name;
        } MIXER_T;
    MIXER_T     Mix[OSC_MIXER_COUNT];

public:
             SYNTH_DATA_C    (void);
    bool     GetSelected     (byte sel)               { return (Mix[sel].Selected);     }
    uint16_t GetLimitLevel   (byte sel)               { return (Mix[sel].LimitLevel);   }
    float    GetAttackTime   (byte sel)               { return (Mix[sel].AttackTime);   }
    float    GetDecayTime    (byte sel)               { return (Mix[sel].DecayTime);    }
    uint16_t GetSustainLevel (byte sel)               { return (Mix[sel].SustainLevel); }
    float    GetSustainTime  (byte sel)               { return (Mix[sel].SustainTime);  }
    float    GetReleaseTime  (byte sel)               { return (Mix[sel].ReleaseTime);  }

    bool     GetActive       (byte sel)               { return (Mix[sel].Active);       }
    void     SetSelected     (byte sel, bool set)     { Mix[sel].Selected     = set; }
    void     SetAttackTime   (byte sel, float set)    { Mix[sel].AttackTime   = set; }
    void     SetDecayTime    (byte sel, float set)    { Mix[sel].DecayTime    = set; }
    void     SetSustainLevel (byte sel, uint16_t set) { Mix[sel].SustainLevel = set; }
    void     SetSustainTime  (byte sel, float set)    { Mix[sel].SustainTime  = set; }
    void     SetReleaseTime  (byte sel, float set)    { Mix[sel].ReleaseTime  = set; }

    void SetLimitLevel   (byte sel, uint16_t set)
        {
        Mix[sel].LimitLevel = set;
        if ( set == 0 )
            Mix[sel].Active=false;
        else
            Mix[sel].Active=true;
        }
    };

extern SYNTH_DATA_C  SynthData;
