#pragma once

namespace SERIAL_MONITOR
{
//#######################################################################
class MONITOR_C
    {
private:
    enum SMODE {MENU = -1, CMD = 0, TUNING, ADIAG, INSSID, INPWD, ZAP };

    SMODE  InputMode;
    String InputString;

    void Mode        (SMODE m);
    void DumpStats   (void);
    bool Save        (SMODE m);
    void InputPrompt (int num, const char* text);
    bool PromptZap   (void);
    void Tuning      (int num);
    void MenuSel     (void);
    void Menu        (void);
    void TextIn      (void);

public:
    MONITOR_C        (void);
    ~MONITOR_C       (void);
    bool Detect      (void);
    void Loop        (void);
    void Begin       (void);
    };
}


extern SERIAL_MONITOR::MONITOR_C Monitor;


