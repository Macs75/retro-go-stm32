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
** map042.c
**
** Mapper #42 (Baby Mario bootleg)
** Implementation by Firebug
** Mapper information courtesy of Kevin Horton
** $Id: map042.c,v 1.2 2001/04/27 14:37:11 neil Exp $
**
*/

#include <nofrendo.h>
#include <nes_mmc.h>
#include <nes.h>

static struct
{
    uint16 enabled;
    uint16 counter;
} irq;


static void map_irq_reset(void)
{
    /* Turn off IRQs */
    irq.enabled = false;
    irq.counter = 0x0000;
}


/****************************************/
/* Mapper #42 callback for IRQ handling */
/****************************************/
static void map_hblank (int vblank)
{
   /* Counter is M2 based so it doesn't matter whether */
   /* the PPU is in its VBlank period or not           */
   UNUSED(vblank);

    /* Increment the counter if it is enabled and check for strike */
    if (irq.enabled)
    {
        irq.counter += nes_getptr()->cycles_per_line;

        /* IRQ is triggered after 24576 M2 cycles */
        if (irq.counter >= 0x6000)
        {
            nes6502_irq();
            map_irq_reset();
        }
    }
}

static void map_write(uint32 address, uint8 value)
{
    switch (address & 0x03)
    {
        /* Register 0: Select ROM page at $6000-$7FFF */
        case 0x00:
            mmc_bankrom(8, 0x6000, value & 0x0F);
            break;

        /* Register 1: mirroring */
        case 0x01:
            if (value & 0x08) ppu_setmirroring(PPU_MIRROR_HORI);
            else              ppu_setmirroring(PPU_MIRROR_VERT);
            break;

        /* Register 2: IRQ */
        case 0x02:
            if (value & 0x02) irq.enabled = true;
            else              map_irq_reset();
            break;

        /* Register 3: unused */
        default:
            break;
    }
}


/********************************************/
/* Mapper #42: Baby Mario bootleg cartridge */
/********************************************/
static void map_init (void)
{
    mmc_bankrom(8, 0x8000, 0x0C);
    mmc_bankrom(8, 0xA000, 0x0D);
    mmc_bankrom(8, 0xC000, 0x0E);
    mmc_bankrom(8, 0xE000, 0x0F);
    map_irq_reset();
}
static mem_write_handler_t map_memwrite [] =
{
   { 0xE000, 0xFFFF, map_write },
   LAST_MEMORY_HANDLER
};

mapintf_t map42_intf =
{
   42,                               /* Mapper number */
   "Baby Mario (bootleg)",           /* Mapper name */
   map_init,                       /* Initialization routine */
   NULL,                             /* VBlank callback */
   map_hblank,                     /* HBlank callback */
   NULL,                             /* Get state (SNSS) */
   NULL,                             /* Set state (SNSS) */
   NULL,                             /* Memory read structure */
   map_memwrite,                   /* Memory write structure */
   NULL                              /* External sound device */
};

/*
** $Log: map042.c,v $
** Revision 1.2  2001/04/27 14:37:11  neil
** wheeee
**
** Revision 1.1  2001/04/27 12:54:40  neil
** blah
**
** Revision 1.1  2001/04/27 10:57:41  neil
** wheee
**
** Revision 1.1  2000/12/27 19:23:30  firebug
** initial revision
**
**
*/

#endif