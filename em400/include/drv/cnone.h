//  Copyright (c) 2012-2013 Jakub Filipowicz <jakubf@gmail.com>
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

#ifndef DRV_CNONE_H
#define DRV_CNONE_H

#include <inttypes.h>

#include "cfg.h"
#include "io.h"

int drv_cnone_init(void *self, struct cfg_arg_t *arg);
void drv_cnone_shutdown(void *self);
void drv_cnone_reset(void *self);
int drv_cnone_cmd(void *self, int dir, uint16_t n, uint16_t *r);

#endif

// vim: tabstop=4
