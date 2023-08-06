// This module deals with everything related to textures: from Flipper's point of view (loading into TMEM, conversion),
// and from the point of view of graphics backend (texture upload to the real graphics device, bindings).
#pragma once

namespace GX
{

    // Texture offset

    // Texture Culling mode

    // Texture Clip mode

    // Texture Wrap mode

    // Texture filter

    // Texture format

    // Tlut format

    // Tlut size

    // Indirect texture format

    // Indirect texture bias select

    // Indirect texture alpha select

    // Indirect texture wrap

    // Indirect texture scale


    // texture params
    union TEXIMAGE0
    {
        struct
        {
            unsigned    width : 10;
            unsigned    height : 10;
            unsigned    fmt : 4;
            unsigned    op : 8;
        };
        uint32_t     bits;
    };

    // texture location
    union TEXIMAGE3
    {
        struct
        {
            unsigned    base : 24;
            unsigned    op : 8;
        };
        uint32_t     bits;
    };

    // texture mode 0
    union TexMode0
    {
        struct
        {
            unsigned    wrap_s : 2;
            unsigned    wrap_t : 2;
            unsigned    mag : 1;
            unsigned    min : 3;
            unsigned    diaglod : 1;
            unsigned    lodbias : 10;
            unsigned    maxaniso : 2;
            unsigned    lodclamp : 3;
            unsigned    rid : 8;
        };
        uint32_t     bits;
    };

    // 0x64
    union LoadTlut0
    {
        struct
        {
            unsigned    base : 20;
        };
        uint32_t     bits;
    };

    // 0x65
    union LoadTlut1
    {
        struct
        {
            unsigned    tmem : 10;
            unsigned    count : 10;
        };
        uint32_t     bits;
    };

    union SetTlut
    {
        struct
        {
            unsigned    tmem : 10;
            unsigned    fmt : 2;
        };
        uint32_t     bits;
    };

}



// TODO: Old implementation, will be redone nicely.

// texture entry
typedef struct
{
    uint32_t  ramAddr;
    uint8_t* rawData;
    Color* rgbaData;      // allocated
    int fmt, tfmt;
    int w, h, dw, dh;
    float ds, dt;
    uint32_t bind;
} TexEntry;

// texture formats
enum
{
    TF_I4 = 0,
    TF_I8,
    TF_IA4,
    TF_IA8,
    TF_RGB565,
    TF_RGB5A3,
    TF_RGBA8,
    TF_C4 = 8,
    TF_C8,
    TF_C14,
    TF_CMPR = 14    // s3tc
};

typedef struct
{
    unsigned    t : 2;
} S3TC_TEX;

typedef struct
{
    uint16_t     rgb0;       // color 2
    uint16_t     rgb1;       // color 1
    uint8_t      row[4];
} S3TC_BLK;


// interface to application

extern  TexEntry* tID[8];
