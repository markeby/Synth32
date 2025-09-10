//#######################################################################
// Module:     FrontEnd.cpp
// Descrption: Synthesizer front end controller
// Creator:    markeby
// Date:       12/9/2024
//#######################################################################
#include <UHS2-MIDI.h>
#include <MIDI.h>

#include "Config.h"
#include "../Common/DispMessages.h"
#include "Novation.h"

byte dummyButtons[XL_BUTTON_COUNT];

//#######################################################################
    NOVATION_XL_C::NOVATION_XL_C ()
    {
    this->CurrentMap   = 0;
    this->FlashState   = false;
    this->pButtonState = nullptr;
    this->Counter      = NOVATION_LOOP_COUNT;
    this->ButtonChange = false;

    memset (dummyButtons, 0x0C, sizeof (dummyButtons));
    this->pButtonState = dummyButtons;
    }

//#######################################################################
void NOVATION_XL_C::TemplateReset (byte index)
    {
    static byte midi_sysex_led[7 + ((XL_MIDI_MAP_SIZE - 8) * 2)] = { 0x00, 0x20, 0x29, 0x02, 0x11, 0x78 };

    this->ResetAllLED(index);
    memset(&midi_sysex_led[7], 0, sizeof (midi_sysex_led) - 7);         // clear the map space in the sysex message
    midi_sysex_led[6] = index;                                          // setup template index in message
    short zi = 7;                                                       // start loading array at this index
    for ( int z = 0;  z < (XL_MIDI_MAP_SIZE - 8);  z++ )
        {
        midi_sysex_led[zi] = z;                                         // setup index of LED
        midi_sysex_led[zi + 1] = this->pMidiMap[index][z].Color;        // get color value for LED
        zi += 2;                                                        // bump target array index
        }
    delay (100);
    this->SendTo (sizeof (midi_sysex_led), midi_sysex_led);             // Send message to set all LEDs
    }

//#######################################################################
void NOVATION_XL_C::Loop ()
    {
    if ( --this->Counter == 0 )
        {
        this->Counter = NOVATION_LOOP_COUNT;
        switch ( this->CurrentMap  )
            {
            case XL_MIDI_MAP_OSC:
                if ( this->ButtonChange )
                    this->UpdateButtons ();
                break;
            case XL_MIDI_MAP_FLT:
                if ( this->ButtonChange )
                    this->UpdateButtons ();
                break;
            case XL_MIDI_MAP_LFO:
                if ( this->ButtonChange )
                    this->UpdateButtons ();
                break;
            case XL_MIDI_MAP_MAPPING:
                this->FlashState = !this->FlashState;
                this->SetColor(XL_MIDI_MAP_MAPPING, 40, ( this->FlashState ) ? XL_LED::AMBER : XL_LED::OFF);
                break;
            case XL_MIDI_MAP_TUNING:
                break;
            default:        // should never get here
                break;
            }
        this->ButtonChange = false;
        }
    }

//#######################################################################
void NOVATION_XL_C::SelectTemplate (byte index, byte* pbuttons)
    {
    static byte midi_msg_template[7] = { 0x00, 0x20, 0x29, 0x02, 0x11, 0x77, 0x00 };
    if ( pbuttons == nullptr )
        this->pButtonState = dummyButtons;
    else
        this->pButtonState = pbuttons;
    delay (10);
    midi_msg_template[6] = index;
    this->CurrentMap     = index;
    this->SendTo (sizeof (midi_msg_template), midi_msg_template);
    this->TemplateReset (index);
    this->UpdateButtons ();
    }

//#####################################################################
void NOVATION_XL_C::UpdateButtons ()
    {
    static byte midi_button_sysex[7 + (XL_BUTTON_COUNT * 2)] = { 0x00, 0x20, 0x29, 0x02, 0x11, 0x78, 0x00 };
    String st;

    if ( this->pButtonState != nullptr )
        {
        delay(10);
        midi_button_sysex[6] = this->CurrentMap;
        for ( short z = 0;  z < XL_BUTTON_COUNT;  z++ )
            {
            midi_button_sysex[(z * 2) + 7] = XL_BUTTON_START + z;
            midi_button_sysex[(z * 2) + 8] = this->pButtonState[z];
            }
        this->SendTo (sizeof (midi_button_sysex), midi_button_sysex);
        }
    }

//#####################################################################
void NOVATION_XL_C::SetColor (byte index, byte led, XL_LED color)
    {
    static byte midi_color_sysex[9] = { 0x00, 0x20, 0x29, 0x02, 0x11, 0x78 };

    delay (10);
    midi_color_sysex[6] = index;
    midi_color_sysex[7] = led;
    midi_color_sysex[8] = (byte)color;
    this->SendTo (sizeof (midi_color_sysex), midi_color_sysex);
    }


