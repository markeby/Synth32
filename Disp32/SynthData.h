//
//   SynthD.h
//
//   author: markeby
//
#pragma once

#include <SynthCommon.h>

class DISP_CTRL_C;

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
        bool          Active;             // Status of in use
        bool          Selected;           // Currently in the selected state
        bool          NoSustainTime;      // set when SustainTime == 0.0
        float         LimitLevel;         // Maximum amplitude in percent 0.0 - 1.0
        float         AttackTime;         // Attack time 0.0 - 1.0
        float         DecayTime;          // Decay time 0.0 - 1.0
        float         SustainLevel;       // Sustain aplitude in percent 0.0 - 1.0
        float         SustainTime;        // How long to hold the sustain level 0.0 - 1.0 (0.0 = hold with key down)
        float         ReleaseTime;        // Release time 0.0 - 1.0
        String        Name;               // Name of oscillator
        LAST_T        Last[12];           // vector storage of current output lines for ADSR plus marker lines
        int           VectorCount;        // Number of vectors stored
        DISP_CTRL_C*  pDispAttackTime;    // Pointer to display for attack time
        DISP_CTRL_C*  pDispADecayTime;    // Pointer to display for decay time
        DISP_CTRL_C*  pDispSustainLevel;  // Pointer to display for sustain level
        DISP_CTRL_C*  pDispSustainTime;   // Pointer to display for sustain time
        DISP_CTRL_C*  pDispReleaseTime;   // Pointer to display for release time
        DISP_CTRL_C*  pDispLimitLevel;    // Pointer to display for level limit
        } MIXER_T;
    MIXER_T     Mix[OSC_MIXER_COUNT];

public:
                        SYNTH_DATA_C                (void);
    inline bool         GetSelected                 (byte sel)                  { return (Mix[sel].Selected);          }
    inline bool         GetActive                   (byte sel)                  { return (Mix[sel].Active);            }
    inline bool         GetNoSustainTime            (byte sel)                  { return (Mix[sel].NoSustainTime);     }
    inline float        GetMaxLevel                 (byte sel)                  { return (Mix[sel].LimitLevel);        }
    inline float        GetSustainLevel             (byte sel)                  { return (Mix[sel].SustainLevel);      }
    inline float        GetAttackTime               (byte sel)                  { return (Mix[sel].AttackTime);        }
    inline float        GetDecayTime                (byte sel)                  { return (Mix[sel].DecayTime);         }
    inline float        GetSustainTime              (byte sel)                  { return (Mix[sel].SustainTime);       }
    inline float        GetReleaseTime              (byte sel)                  { return (Mix[sel].ReleaseTime);       }
    inline String       GetName                     (byte sel)                  { return (Mix[sel].Name);              }
    inline DISP_CTRL_C* GetDispPAttackTime          (byte sel)                  { return (Mix[sel].pDispAttackTime);   }
    inline DISP_CTRL_C* GetDispPADecayTime          (byte sel)                  { return (Mix[sel].pDispADecayTime);   }
    inline DISP_CTRL_C* GetDispPSustainLevel        (byte sel)                  { return (Mix[sel].pDispSustainLevel); }
    inline DISP_CTRL_C* GetDispPSustainTime         (byte sel)                  { return (Mix[sel].pDispSustainTime);  }
    inline DISP_CTRL_C* GetDispPReleaseTime         (byte sel)                  { return (Mix[sel].pDispReleaseTime);  }
    inline DISP_CTRL_C* GetDispPLimitLevel          (byte sel)                  { return (Mix[sel].pDispLimitLevel);   }

    inline void         SetAttackTime               (byte sel, byte set)        { Mix[sel].AttackTime        = (float)set * PRS_SCALER; }
    inline void         SetDecayTime                (byte sel, byte set)        { Mix[sel].DecayTime         = (float)set * PRS_SCALER; }
    inline void         SetSustainLevel             (byte sel, byte set)        { Mix[sel].SustainLevel      = (float)set * PRS_SCALER; }
    inline void         SetReleaseTime              (byte sel, byte set)        { Mix[sel].ReleaseTime       = (float)set * PRS_SCALER; }
    inline void         SetSelected                 (byte sel, bool set)        { Mix[sel].Selected          = set;    }
    inline void         SetDispPDispAttackTime      (byte sel, DISP_CTRL_C* p)  { Mix[sel].pDispAttackTime   = p;      }
    inline void         SetDispPDispADecayTime      (byte sel, DISP_CTRL_C* p)  { Mix[sel].pDispADecayTime   = p;      }
    inline void         SetDispPDispSustainLevel    (byte sel, DISP_CTRL_C* p)  { Mix[sel].pDispSustainLevel = p;      }
    inline void         SetDispPDispSustainTime     (byte sel, DISP_CTRL_C* p)  { Mix[sel].pDispSustainTime  = p;      }
    inline void         SetDispPDispReleaseTime     (byte sel, DISP_CTRL_C* p)  { Mix[sel].pDispReleaseTime  = p;      }
    inline void         SetDispPDispLimitLevel      (byte sel, DISP_CTRL_C* p)  { Mix[sel].pDispLimitLevel   = p;      }
    inline void         InitSaveVectors             (byte sel)                  { Mix[sel].VectorCount = 0;            }

    inline void SetSustainTime (byte sel, byte set)
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
