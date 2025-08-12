#pragma once

//#######################################################################
class MONITOR_C
    {
private:
    enum SMODE {MENU = -1, CMD = 0, INSSID, INPWD, CLR_TUNING, CLR_CONFIG, VARIABLE };

    SMODE  InputMode;
    String InputString;

    //#######################################################################
    void Mode (SMODE m)         { this->InputMode = m; }

    void DumpStats      (void);
    bool Save           (SMODE m);
    void InputPrompt    (const char* text);
    bool PromptZap      (void);
    void MenuSel        (void);
    void Menu           (void);
    void TextIn         (void);

public:
         MONITOR_C      (void);
    bool Detect         (void);
    void Loop           (void);
    void Begin          (void);
    void Reset          (void);
    };



extern MONITOR_C Monitor;


