// common include project header

// plugin version
#define PAD_VER         "0.7"

// compiler and Windows API includes
#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "resource.h"

// Dolwin plugin specifications. we need only PAD.
#include "DolwinPluginSpecs.h"

// other include files
#include "Configure.h"      // PAD configure dialog

// all important data is placed here
typedef struct
{
    HINSTANCE   inst;       // plugin dll handler
    HWND*       hwndParent; // main window

    uint32_t    rumbleFlag[4];
    int         padToConfigure;
    PADCONF     config[4];
} PAD;

extern PAD pad;             // share with other modules
