// filesystem definitions

//
// general DVD tables (BB2, BI2)
//

// offsets
#define DVD_ID_OFFSET       0x0000  // disk id
#define DVD_BB2_OFFSET      0x0420  // BB2
#define DVD_BI2_OFFSET      0x0440  // BI2
#define DVD_APPLDR_OFFSET   0x2440  // apploader

typedef struct
{
    uint32_t     bootFilePosition;
    uint32_t     FSTPosition;
    uint32_t     FSTLength;
    uint32_t     FSTMaxLength;
    uint32_t     userPosition;
    uint32_t     userLength;
    uint8_t      padding[8];
} DVDBB2;

// BI2 is omitted here..

//
// file string table (FST)
//

#define DVD_FST_MAX_SIZE 0x00100000 // 1 mb
#define DVD_MAXPATH      256        // path length

typedef struct
{
    uint8_t      isDir;                  // 1, if directory
    uint8_t      nameOffsetHi;
    uint16_t     nameOffsetLo;
    union
    {
        struct                      // file
        {
            uint32_t     fileOffset;
            uint32_t     fileLength;
        };
        struct                      // directory
        {
            uint32_t     parentOffset;   // previous
            uint32_t     nextOffset;     // next
        };
    };
} DVDFileEntry;

// externals
BOOL    dvd_fs_init();
int     dvd_open(char *path, DVDFileEntry *root=NULL);
