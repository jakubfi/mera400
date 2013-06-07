//  Copyright (c) 2013 Jakub Filipowicz <jakubf@gmail.com>
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc.,
//  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

#ifndef DRV_PLIX_H
#define DRV_PLIX_H

#include <inttypes.h>

#include "cfg.h"
#include "io.h"

int plix_init(struct chan_t *chan, struct cfg_unit_t *units);
void plix_shutdown(struct chan_t *chan);
void plix_reset(struct chan_t *chan);
int plix_cmd(struct chan_t *chan, int dir, uint16_t n_arg, uint16_t *r_arg);

#endif

// vim: tabstop=4 shiftwidth=4 autoindent