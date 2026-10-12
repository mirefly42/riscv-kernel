#include "bits.h"
#include "byte_order.h"
#include "io.h"
#include "types.h"
#include <stdbool.h>
#include <stddef.h>

#define FOURCC_CODE(a, b, c, d) ((u32)(a) | ((u32)(b) << 8) | ((u32)(c) << 16) | ((u32)(d) << 24))

typedef struct QemuFwCfgDmaAccess {
    U32Be control;
    U32Be length;
    U64Be address;
} QemuFwCfgDmaAccess;

typedef struct QemuFwCfgFile {
    U32Be size;
    U16Be select;
    U16Be reserved;
    char name[56];
} QemuFwCfgFile;

typedef struct QemuFwCfgFiles {
    uint32_t count;
    struct QemuFwCfgFile f[];
} QemuFwCfgFiles;

typedef struct QemuRamFbCfg {
    U64Be addr;
    U32Be fourcc;
    U32Be flags;
    U32Be width;
    U32Be height;
    U32Be stride;
    // compiler puts padding at the end of this structure, which we mustn't send via DMA
    u8 _end_pad[];
} QemuRamFbCfg;

typedef struct Color {
    u8 b, g, r, x;
} Color;

extern void storeU64(u64 *ptr, u64 value);

static void qemuFwCfgDmaAccess(QemuFwCfgDmaAccess *access) {
    storeU64((uPtr *)0x10100010, u64ToBe((uPtr)access).data);
}

static bool qemuFwCfgDmaAccessEasy(const char *flags, void *address, u32 length, u16 selector) {
    QemuFwCfgDmaAccess access = {
        .control = u32ToBe(0),
        .length = u32ToBe(length),
        .address = u64ToBe((uPtr)address),
    };

    for (char c; (c = flags[0]) != '\0'; ++flags) {
        switch (c) {
            case 'r': // read
                access.control.data |= u32ToBe(HIGH_BIT(1)).data;
                break;
            case 'w': // write
                access.control.data |= u32ToBe(HIGH_BIT(4)).data;
                break;
            case 's': // select
                access.control.data |= u32ToBe(HIGH_BIT(3) | ((u32)selector << 16)).data;
                break;
        }
    }

    qemuFwCfgDmaAccess(&access);

    return access.control.data == 0;
}

static bool strEqual(const char *a, const char *b) {
    for (;;++a, ++b) {
        if (*a != *b) {
            return false;
        } else if (*a == '\0') {
            return true;
        }
    }
}

#define PROPAGATE(success) do { \
    if (!(success)) return false; \
} while (0)

static bool findRamFbSelector(U16Be *out_selector) {
    U32Be count;
    PROPAGATE(qemuFwCfgDmaAccessEasy("rs", &count, sizeof(count), 0x19));

    for (u32 i = 0; i < u32FromBe(count); ++i) {
        QemuFwCfgFile file;
        PROPAGATE(qemuFwCfgDmaAccessEasy("r", &file, sizeof(QemuFwCfgFile), 0));

        if (strEqual(file.name, "etc/ramfb")) {
            *out_selector = file.select;
            return true;
        }
    }

    return false;
}

void testRamFb(void) {
    U16Be ramfb_selector;
    if (!findRamFbSelector(&ramfb_selector)) {
        printString("ramfb unavailable\n");
        return;
    }

    enum { width = 640, height = 480 };
    static Color framebuffer[height][width];

    QemuRamFbCfg ram_fb_cfg = {
        .addr = u64ToBe((uPtr)framebuffer),
        .fourcc = u32ToBe(FOURCC_CODE('X', 'R', '2', '4')),
        .flags = u32ToBe(0),
        .width = u32ToBe(width),
        .height = u32ToBe(height),
        .stride = u32ToBe(0),
    };

    if (!qemuFwCfgDmaAccessEasy("ws", &ram_fb_cfg, offsetof(QemuRamFbCfg, _end_pad), u16FromBe(ramfb_selector))) {
        printString("error: unable to initialize ramfb\n");
        return;
    }

    printString("ramfb initialized\n");

    const u8 power = 5;
    const u8 mask = (1u << power) - 1;

    for (u32 y = 0; y < height; ++y) {
        for (u32 x = 0; x < width; ++x) {
            framebuffer[y][x] = (Color){
                .r = (x & mask) << (8 - power),
                .g = (y & mask) << (8 - power),
                .b = (x ^ y) >> power & 1 ? 0xFF : 0x00};
        }
    }

    // force the compiler to actually write to the framebuffer memory
    __asm__ volatile ("" : : : "memory");
}
