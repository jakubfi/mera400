//  Copyright (c) 2013-2015 Jakub Filipowicz <jakubf@gmail.com>
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

#ifndef MX_PROTO_COMMON_H
#define MX_PROTO_COMMON_H

#include "io/mx_line.h"

uint8_t mx_proto_status_start(struct mx_line *line, int *irq, uint16_t *data);
uint8_t mx_proto_detach_start(struct mx_line *line, int *irq, uint16_t *data);
uint8_t mx_proto_oprq_start(struct mx_line *line, int *irq, uint16_t *data);
uint8_t mx_proto_attach_start(struct mx_line *line, int *irq, uint16_t *data);

#endif

// vim: tabstop=4 shiftwidth=4 autoindent