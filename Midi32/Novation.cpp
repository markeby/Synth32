//#######################################################################
// Module:     FrontEnd.cpp
// Descrption: Synthesizer front end controller
// Creator:    markeby
// Date:       12/9/2024
//#######################################################################
#include <Arduino.h>

#include "Config.h"
#include "Novation.h"
#include "MidiConf.h"

using namespace MIDI_NAMESPACE;

//#######################################################################
    NOVATION_XL_C::NOVATION_XL_C ()
    {
    _CurrentMap   = 0;
    _FlashState   = false;
    _Counter      = NOVATION_LOOP_COUNT;
    _ButtonChange = false;
    }

//#######################################################################
void NOVATION_XL_C::Begin ()
    {
    for ( short z = 0;  z < 20;  z++ )      // read in a bunch of stuff so that we can start using this device.
        {
        Usb.Task    ();
        Midi_0.read ();
        delay (100);
        }
    }

//#######################################################################
void NOVATION_XL_C::SendTo (unsigned length, byte* buff)
    {
    // Debugging message
//  String str;
//  for ( short z = 0;  z < length;  z++ )
//      str += " " + String (buff[z], 16);
//  printf("@@ message %s\n", str.c_str ());

    // Message sent to deice
    Midi_0.sendSysEx (length, buff, false);
    }

//#####################################################################
void NOVATION_XL_C::SetColor (byte index, byte led, XL color)
    {
    static byte midi_color_sysex[9] = { 0x00, 0x20, 0x29, 0x02, 0x11, 0x78 };

    delay (10);
    midi_color_sysex[6] = index;
    midi_color_sysex[7] = led;
    midi_color_sysex[8] = (byte)color;
    SendTo (sizeof (midi_color_sysex), midi_color_sysex);
    }

//#######################################################################
void NOVATION_XL_C::ResetAllLED (byte index)
    {
    Midi_0.send ((MidiType)(0x90 | index), 0, 0, 0);
    delay (100);
    }

//#######################################################################
void NOVATION_XL_C::SetColorTri (int index, byte tri)
    {
    switch ( tri )
        {
        case 1:     // DAMPER::NORMAL
            tri = (byte)XL::AMBER;
            break;
        case 2:     // DAMPER::INVERT
            tri = (byte)XL::GREEN_LOW;
            break;
        default:    // DAMPER::OFF
            tri = (byte)XL::GREEN;
            break;
        }
    SetButtonState (index, tri);
    }

//#######################################################################
void NOVATION_XL_C::TemplateReset (byte index)
    {
    static byte midi_sysex_led[7 + ((XL_MIDI_MAP_SIZE - 8) * 2)] = { 0x00, 0x20, 0x29, 0x02, 0x11, 0x78 };

    ResetAllLED(index);
    memset(&midi_sysex_led[7], 0, sizeof (midi_sysex_led) - 7);         // clear the map space in the sysex message
    midi_sysex_led[6] = index;                                          // setup template index in message
    short zi = 7;                                                       // start loading array at this index
    for ( int z = 0;  z < (XL_MIDI_MAP_SIZE - 8);  z++ )
        {
        midi_sysex_led[zi] = z;                                         // setup index of LED
        midi_sysex_led[zi + 1] = XL_MidiMapArray[index][z].Color;       // get color value for LED
        zi += 2;                                                        // bump target array index
        }
    delay (100);
    SendTo (sizeof (midi_sysex_led), midi_sysex_led);                    // Send message to set all LEDs

    for ( int z = 0;  z < XL_BUTTON_COUNT;  z++ )                       // Initialize button state map for LEDs
        _ButtonState[z] = XL_MidiMapArray[index][z].Color;
    }

//#######################################################################
void NOVATION_XL_C::SelectTemplate (byte index)
    {
    static byte midi_msg_template[7] = { 0x00, 0x20, 0x29, 0x02, 0x11, 0x77, 0x00 };

    delay (10);
    midi_msg_template[6] = index;
    _CurrentMap           = index;
    SendTo (sizeof (midi_msg_template), midi_msg_template);
    TemplateReset (index);
//    UpdateButtons ();
    }

//#######################################################################
void NOVATION_XL_C::Loop ()
    {
    if ( --_Counter == 0 )
        {
        _Counter = NOVATION_LOOP_COUNT;
        switch ( _CurrentMap  )
            {
            case XL_MIDI_MAP_OSC:
                if ( _ButtonChange )
                    UpdateButtons ();
                break;

            case XL_MIDI_MAP_FLT:
                if ( _ButtonChange )
                    UpdateButtons ();
                break;

            case XL_MIDI_MAP_LFO:
                if ( _ButtonChange )
                    UpdateButtons ();
                break;

            case XL_MIDI_MAP_MAPPING:
                _FlashState = !_FlashState;
                SetColor (XL_MIDI_MAP_MAPPING, 40, ( _FlashState ) ? XL::AMBER : XL::OFF);
                break;

            case XL_MIDI_MAP_LOADSAVE:
                _FlashState = !_FlashState;
                SetColor (XL_MIDI_MAP_LOADSAVE, ( _SaveFlag ) ? 41 : 42, ( _FlashState ) ? XL::AMBER : XL::OFF);
                SetColor (XL_MIDI_MAP_LOADSAVE, ( _SaveFlag ) ? 42 : 41, XL::OFF);
                break;

            case XL_MIDI_MAP_TUNING:
                break;

            default:        // should never get here
                break;
            }
        _ButtonChange = false;
        }
    }

//#####################################################################
void NOVATION_XL_C::UpdateButtons ()
    {
    static byte midi_button_sysex[7 + (XL_BUTTON_COUNT * 2)] = { 0x00, 0x20, 0x29, 0x02, 0x11, 0x78, 0x00 };
    String st;

    delay(10);
    midi_button_sysex[6] = _CurrentMap;
    for ( short z = 0;  z < XL_BUTTON_COUNT;  z++ )
        {
        midi_button_sysex[(z * 2) + 7] = XL_BUTTON_START + z;
        midi_button_sysex[(z * 2) + 8] = _ButtonState[z];
        }
    SendTo (sizeof (midi_button_sysex), midi_button_sysex);
    }


