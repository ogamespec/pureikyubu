// Emulator includes

#pragma once

void    EMUGetHwConfig(HWConfig* config);

// emulator controls API
void    EMUInit();          // called once
void    EMUDie();           // called once
void    EMUOpen();          // [START]
void    EMUClose();         // [STOP]

// EMUReset = [STOP], [START]

// all important data is placed here
typedef struct Emulator
{
    bool    initok;         // sub-systems are ready
    bool    running;        // running game (not Idle)
    bool    doldebug;       // debugger active
} Emulator;

extern  Emulator emu;
