//
//   SynthD.h
//
//   author: markeby
//
#pragma once

#include <SynthCommon.h>

class SYNTH_DATA_C
    {
private:
    typedef struct
        {
        int     X1;
        int     Y1;
        int     X2;
        int     Y2;
        } LAST_T;

    typedef struct
        {
        bool    Active;             // Status of in use
        bool    Selected;           // Currently in the selected state
        byte    LimitLevel;         // Maximum amplitude in percent 0.0 = 1.0
        byte    AttackTime;         // Attack time 0 - 127
        byte    DecayTime;          // Decay time 0 - 127
        byte    SustainLevel;       // Sustain aplitude in percent 0.0 = 1.0
        byte    SustainTime;        // How long to hold the sustain level (-1 = hold with key down)
        byte    ReleaseTime;        // Release time 0 - 127
        int     Color;              // Color of oscillator display
        String  Name;               // Name of oscillator
        LAST_T  Last[5];            // vector storage of current output
        int     VectorCount;        // Number of vectors stored
        } MIXER_T;
    MIXER_T     Mix[OSC_MIXER_COUNT];

public:
                    SYNTH_DATA_C    (void);
    inline bool     GetSelected     (byte sel)             { return (Mix[sel].Selected);     }
    inline float    GetLimitLevel   (byte sel)             { return (Mix[sel].LimitLevel);   }
    inline byte     GetAttackTime   (byte sel)             { return (Mix[sel].AttackTime);   }
    inline byte     GetDecayTime    (byte sel)             { return (Mix[sel].DecayTime);    }
    inline float    GetSustainLevel (byte sel)             { return (Mix[sel].SustainLevel); }
    inline byte     GetSustainTime  (byte sel)             { return (Mix[sel].SustainTime);  }
    inline byte     GetReleaseTime  (byte sel)             { return (Mix[sel].ReleaseTime);  }
    inline String   GetName         (byte sel)             { return (Mix[sel].Name);         }
    inline int      GetColor        (byte sel)             { return (Mix[sel].Color);        }
    inline bool     GetActive       (byte sel)             { return (Mix[sel].Active);       }

    inline void     SetSelected     (byte sel, bool set)  { Mix[sel].Selected     = set; }
    inline void     SetAttackTime   (byte sel, byte set)  { Mix[sel].AttackTime   = set; }
    inline void     SetDecayTime    (byte sel, byte set)  { Mix[sel].DecayTime    = set; }
    inline void     SetSustainLevel (byte sel, byte set)  { Mix[sel].SustainLevel = set; }
    inline void     SetSustainTime  (byte sel, byte set)  { Mix[sel].SustainTime  = set; }
    inline void     SetReleaseTime  (byte sel, byte set)  { Mix[sel].ReleaseTime  = set; }

    inline void     InitSaveVectors (byte sel)             { Mix[sel].VectorCount = 0; }

    inline void SaveVector (byte sel, int x, int y, int x1, int y1)
        {
        MIXER_T& m = Mix[sel];
        LAST_T& v = m.Last[m.VectorCount];
        m.VectorCount++;
        v.X1 = x;
        v.Y1 = y;
        v.X2 = x1;
        v.Y2 = y1;
        }

    inline bool GetVector (byte sel, int& x, int& y, int& x1, int& y1)
        {
        MIXER_T& m = Mix[sel];
        if ( m.VectorCount )
            {
            m.VectorCount--;
            LAST_T& v = m.Last[m.VectorCount];

            x  = v.X1;
            y  = v.Y1;
            x1 = v.X2;
            y1 = v.Y2;
            return (true);
            }
        return (false);
        }

    inline void SetLimitLevel (byte sel, byte set)
        {
        Mix[sel].LimitLevel = set;
        if ( set == 0 )
            Mix[sel].Active = false;
        else
            Mix[sel].Active = true;
        }
    };

extern SYNTH_DATA_C  SynthD;
