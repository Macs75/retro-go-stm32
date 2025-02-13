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
** map40.c
**
** mapper 40 interface
** $Id: map040.c,v 1.2 2001/04/27 14:37:11 neil Exp $
*/

#include <nofrendo.h>
#include <nes_mmc.h>
#include <nes.h>

#define  MAP40_IRQ_PERIOD  (4096 / 113.666666)

static struct
{
    bool  enabled;
    uint8 counter;
} irq;


static void map_hblank(int vblank)
{
   UNUSED(vblank);
    if (irq.enabled && irq.counter)
    {
        if (0 == --irq.counter)
        {
            nes6502_irq();
            irq.enabled = false;
        }
    }
}

static void map_write(uint32 address, uint8 value)
{
    int range = (address >> 13) - 4;

    switch (range)
    {
    case 0: /* 0x8000-0x9FFF */
        irq.enabled = false;
        irq.counter = (int) MAP40_IRQ_PERIOD;
        break;

    case 1: /* 0xA000-0xBFFF */
        irq.enabled = true;
        break;

    case 3: /* 0xE000-0xFFFF */
        mmc_bankrom(8, 0xC000, value & 7);
        break;

    default:
        break;
    }
}

static void map_getstate(uint8 *state)
{
    state[0] = irq.counter;
    state[1] = irq.enabled;
}

static void map_setstate(uint8 *state)
{
    irq.counter = state[0];
    irq.enabled = state[1];
}
/* mapper 40: SMB 2j (hack) */
static void map_init(void)
{
    mmc_bankrom(8, 0x6000, 6);
    mmc_bankrom(8, 0x8000, 4);
    mmc_bankrom(8, 0xA000, 5);
    mmc_bankrom(8, 0xE000, 7);

    irq.enabled = false;
    irq.counter = (int)MAP40_IRQ_PERIOD;
}

static mem_write_handler_t map_memwrite[] =
{
   { 0x8000, 0xFFFF, map_write },
   LAST_MEMORY_HANDLER
};

mapintf_t map40_intf =
{
   40, /* mapper number */
   "SMB 2j (pirate)", /* mapper name */
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
** $Log: map040.c,v $
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
** Revision 1.9  2000/10/23 15:53:27  matt
** suppressed warnings
**
** Revision 1.8  2000/10/22 19:17:46  matt
** mapper cleanups galore
**
** Revision 1.7  2000/10/21 19:33:38  matt
** many more cleanups
**
** Revision 1.6  2000/10/10 13:58:17  matt
** stroustrup squeezing his way in the door
**
** Revision 1.5  2000/08/16 02:50:11  matt
** random mapper cleanups
**
** Revision 1.4  2000/07/15 23:52:19  matt
** rounded out a bunch more mapper interfaces
**
** Revision 1.3  2000/07/10 13:51:25  matt
** using generic nes6502_irq() routine now
**
** Revision 1.2  2000/07/06 02:48:43  matt
** clearly labelled structure members
**
** Revision 1.1  2000/07/05 05:05:18  matt
** initial revision
**
*/

#endif