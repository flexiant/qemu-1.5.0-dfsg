/*
 * TI OMAP TEST-Chip-level TAP emulation.
 *
 * Copyright (C) 2007-2008 Nokia Corporation
 * Written by Andrzej Zaborowski <andrew@openedhand.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 or
 * (at your option) any later version of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, see <http://www.gnu.org/licenses/>.
 */

#include "hw/hw.h"
#include "hw/arm/omap.h"

/* TEST-Chip-level TAP */
static uint64_t omap_tap_read(void *opaque, hwaddr addr,
                              unsigned size)
{
    struct omap_mpu_state_s *s = (struct omap_mpu_state_s *) opaque;

    if (size != 4) {
        return omap_badwidth_read32(opaque, addr);
    }

    switch (addr) {
    case 0x204:	/* IDCODE_reg */
        switch (s->mpu_model) {
        case omap2420:
        case omap2422:
        case omap2423:
            return 0x5b5d902f;	/* ES 2.2 */
        case omap2430:
            return 0x5b68a02f;	/* ES 2.2 */
        case omap3430:
            return 0x4b7ae02f;  /* ES 3.1 */
        case omap3630:
            return 0x1b89102f;  /* ES 1.1 */
        default:
            hw_error("%s: Bad mpu model\n", __func__);
        }

        /* For OMAP2 there is a two word PRODUCTION_ID register at
         * 0x208..0x210.
         * For OMAP3 there is a four word PRODUCTION_ID register at
         * 0x208..0x214.
         */

    case 0x208: /* PRODUCTION_ID_reg bits 0-31 */
        switch (s->mpu_model) {
        case omap2420:
            return 0x000254f0;	/* POP ESHS2.1.1 in N91/93/95, ES2 in N800 */
        case omap2422:
            return 0x000400f0;
        case omap2423:
            return 0x000800f0;
        case omap2430:
            return 0x000000f0;
        case omap3430:
        case omap3630:
            return 0x0;
        default:
            hw_error("%s: Bad mpu model\n", __func__);
        }
        break;
    case 0x20c: /* PRODUCTION_ID bits 32-63 */
        switch (s->mpu_model) {
        case omap2420:
        case omap2422:
        case omap2423:
            return 0xcafeb5d9;	/* ES 2.2 */
        case omap2430:
            return 0xcafeb68a;	/* ES 2.2 */
        case omap3430:
        case omap3630:
            return 0x0;
        default:
            hw_error("%s: Bad mpu model\n", __func__);
        }
        break;
    case 0x210: /* PRODUCTION_ID reg bits 64-95 (OMAP3 only) */
        switch (s->mpu_model) {
        case omap2420:
        case omap2422:
        case omap2423:
        case omap2430:
            /* OMAP3 only, break out to bad-register path */
            break;
        case omap3430:
        case omap3630:
            return 0x000f00f0;
        default:
            hw_error("%s: Bad mpu model\n", __func__);
        }
        break;
    case 0x214: /* PRODUCTION_ID bits 96-127 (OMAP3 only) */
        switch (s->mpu_model) {
        case omap2420:
        case omap2422:
        case omap2423:
        case omap2430:
            /* OMAP3 only, break out to bad-register path */
            break;
        case omap3430:
            return 0xcafeb7ae;  /* ES 2.x/3.0 */
        case omap3630:
            return 0xcafeb891;  /* ES 1.0/1.1 */
        default:
            hw_error("%s: Bad mpu model\n", __func__);
        }
        break;

    case 0x218:	/* DIE_ID_reg */
        return ('Q' << 24) | ('E' << 16) | ('M' << 8) | ('U' << 0);
    case 0x21c:	/* DIE_ID_reg */
        return 0x54 << 24;
    case 0x220:	/* DIE_ID_reg */
        return ('Q' << 24) | ('E' << 16) | ('M' << 8) | ('U' << 0);
    case 0x224:	/* DIE_ID_reg */
        return ('Q' << 24) | ('E' << 16) | ('M' << 8) | ('U' << 0);
    }

    OMAP_BAD_REG(addr);
    return 0;
}

static void omap_tap_write(void *opaque, hwaddr addr,
                           uint64_t value, unsigned size)
{
    if (size != 4) {
        return omap_badwidth_write32(opaque, addr, value);
    }

    OMAP_BAD_REG(addr);
}

static const MemoryRegionOps omap_tap_ops = {
    .read = omap_tap_read,
    .write = omap_tap_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
};

void omap_tap_init(struct omap_target_agent_s *ta,
                struct omap_mpu_state_s *mpu)
{
    memory_region_init_io(&mpu->tap_iomem, &omap_tap_ops, mpu, "omap.tap",
                          omap_l4_region_size(ta, 0));
    omap_l4_attach(ta, 0, &mpu->tap_iomem);
}
