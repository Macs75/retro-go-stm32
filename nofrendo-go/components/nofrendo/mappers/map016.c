#include "build/config.h"

#ifdef ENABLE_EMULATOR_NES
/*
** Nofrendo (c) 1998-2000 Matthew Conte (matt@conte.com)
**
**
** This program is free software; you can redistribute it and/or
** modify it under the terms of version 2 of the GNU Library General
** Public License as published by the Free Software Foundation.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Library General Public License for more details.  To obtain a
** copy of the GNU Library General Public License, write to the Free
** Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
**
** Any permitted reproduction of these routines, in whole or in part,
** must bear this legend.
**
**
** map16.c
**
** mapper 16 interface
** $Id: map016.c,v 1.2 2001/04/27 14:37:11 neil Exp $
*/

#include <nofrendo.h>
#include <nes_mmc.h>
#include <nes_ppu.h>
#include <nes.h>

static struct
{
   int counter;
   bool enabled;
} irq;

/* mapper 16: Bandai */

static void map_write(uint32 address, uint8 value)
{
    int reg = address & 0xF;

    if (reg < 8)
    {
        mmc_bankvrom(1, reg << 10, value);
    }
    else
    {
        switch (address & 0x000F)
        {
        case 0x8:
            mmc_bankrom(16, 0x8000, value);
            break;

        case 0x9:
            switch (value & 3)
            {
            case 0: ppu_setmirroring(PPU_MIRROR_VERT); break;
            case 1: ppu_setmirroring(PPU_MIRROR_HORI); break;
            case 2: ppu_setmirroring(PPU_MIRROR_SCR0); break;
            case 3: ppu_setmirroring(PPU_MIRROR_SCR1); break;
            }
            break;

        case 0xA:
            irq.enabled = (value & 1);
            break;

        case 0xB:
            irq.counter = (irq.counter & 0xFF00) | value;
            break;

        case 0xC:
            irq.counter = (value << 8) | (irq.counter & 0xFF);
            break;

        case 0xD:
            /* eeprom I/O port? */
            break;
        }
    }
}

static void map_hblank(int vblank)
{
   UNUSED(vblank);

    if (irq.enabled && irq.counter)
    {
        if (0 == --irq.counter)
            nes6502_irq();
    }
}

static void map_getstate(uint8 *state)
{
    state[0] = irq.counter & 0xFF;
    state[1] = irq.counter >> 8;
    state[2] = irq.enabled;
}

static void map_setstate(uint8 *state)
{
    irq.counter = (state[1] << 8) | state[0];
    irq.enabled = state[2];
}
static void map_init(void)
{
    mmc_bankrom(16, 0x8000, 0);
    mmc_bankrom(16, 0xC000, MMC_LASTBANK);
    irq.counter = 0;
    irq.enabled = false;
}
static mem_write_handler_t map_memwrite[] =
{
   { 0x6000, 0x600D, map_write },
   { 0x7FF0, 0x7FFD, map_write },
   { 0x8000, 0x800D, map_write },
   LAST_MEMORY_HANDLER
};

mapintf_t map16_intf =
{
   16, /* mapper number */
   "Bandai", /* mapper name */
   map_init, /* init routine */
   NULL, /* vblank callback */
   map_hblank, /* hblank callback */
   map_getstate, /* get state (snss) */
   map_setstate, /* set state (snss) */
   NULL, /* memory read structure */
   map_memwrite, /* memory write structure */
   NULL /* external sound device */
};

/*
** $Log: map016.c,v $
** Revision 1.2  2001/04/27 14:37:11  neil
** wheeee
**
** Revision 1.1  2001/04/27 12:54:40  neil
** blah
**
** Revision 1.1.1.1  2001/04/27 07:03:54  neil
** initial
**
** Revision 1.1  2000/10/24 12:19:33  matt
** changed directory structure
**
** Revision 1.8  2000/10/22 19:17:46  matt
** mapper cleanups galore
**
** Revision 1.7  2000/10/22 15:03:13  matt
** simplified mirroring
**
** Revision 1.6  2000/10/21 19:33:38  matt
** many more cleanups
**
** Revision 1.5  2000/10/10 13:58:16  matt
** stroustrup squeezing his way in the door
**
** Revision 1.4  2000/08/16 02:50:11  matt
** random mapper cleanups
**
** Revision 1.3  2000/07/15 23:52:20  matt
** rounded out a bunch more mapper interfaces
**
** Revision 1.2  2000/07/11 05:03:49  matt
** value masking isn't necessary for the banking routines
**
** Revision 1.1  2000/07/11 03:14:18  melanson
** Initial commit for mappers 16, 34, and 231
**
**
*/

#endif