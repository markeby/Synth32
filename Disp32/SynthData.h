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
        bool    NoSustainTime;      //
        float   LimitLevel;         // Maximum amplitude in percent 0.0 = 1.0
        float   AttackTime;         // Attack time 0 - 127
        float   DecayTime;          // Decay time 0 - 127
        float   SustainLevel;       // Sustain aplitude in percent 0.0 = 1.0
        float   SustainTime;        // How long to hold the sustain level (0 = hold with key down)
        float   ReleaseTime;        // Release time 0 - 127
        int     Color;              // Color of oscillator display
        String  Name;               // Name of oscillator
        LAST_T  Last[12];           // vector storage of current output lines for ADSR plus marker lines
        int     VectorCount;        // Number of vectors stored
        } MIXER_T;
    MIXER_T     Mix[OSC_MIXER_COUNT];

public:
                    SYNTH_DATA_C        (void);
    inline bool     GetSelected         (byte sel)              { return (Mix[sel].Selected);       }
    inline bool     GetActive           (byte sel)              { return (Mix[sel].Active);         }
    inline bool     GetNoSustainTime    (byte sel)              { return (Mix[sel].NoSustainTime);  }
    inline float    GetMaxLevel         (byte sel)              { return (Mix[sel].LimitLevel);     }
    inline float    GetSustainLevel     (byte sel)              { return (Mix[sel].SustainLevel);   }
    inline float    GetAttackTime       (byte sel)              { return (Mix[sel].AttackTime);     }
    inline float    GetDecayTime        (byte sel)              { return (Mix[sel].DecayTime);      }
    inline float    GetSustainTime      (byte sel)              { return (Mix[sel].SustainTime);    }
    inline float    GetReleaseTime      (byte sel)              { return (Mix[sel].ReleaseTime);    }
    inline String   GetName             (byte sel)              { return (Mix[sel].Name);           }
    inline int      GetColor            (byte sel)              { return (Mix[sel].Color);          }

    inline void     SetSelected         (byte sel, bool set)    { Mix[sel].Selected     = (float)set; }
    inline void     SetAttackTime       (byte sel, byte set)    { Mix[sel].AttackTime   = (float)set; }
    inline void     SetDecayTime        (byte sel, byte set)    { Mix[sel].DecayTime    = (float)set; }
    inline void     SetSustainLevel     (byte sel, byte set)    { Mix[sel].SustainLevel = (float)set * PRS_SCALER; }
    inline void     SetReleaseTime      (byte sel, byte set)    { Mix[sel].ReleaseTime  = (float)set; }

    inline void     InitSaveVectors     (byte sel)              { Mix[sel].VectorCount = 0; }

    inline void     SetSustainTime      (byte sel, byte set)
        {
        Mix[sel].SustainTime = (float)set;
        if ( set == 0 )
             Mix[sel].NoSustainTime = true;
        else
             Mix[sel].NoSustainTime = false;
        }

    inline void SetMaxLevel (byte sel, byte set)
        {
        Mix[sel].LimitLevel = (float)set * PRS_SCALER;
        if ( set == 0 )
            Mix[sel].Active = false;
        else
            Mix[sel].Active = true;
        }

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

    };

extern SYNTH_DATA_C  SynthD;
