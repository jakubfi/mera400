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

#include <stdlib.h>
#include <inttypes.h>

#include "io/mx/mx.h"
#include "io/mx/line.h"
#include "io/mx/irq.h"
#include "io/mx/proto_common.h"

// -----------------------------------------------------------------------
int mx_puncher_init(struct mx_line *pline, uint16_t *data)
{
	return MX_SC_E_OK;
}

// -----------------------------------------------------------------------
void mx_puncher_destroy(struct mx_line *pline)
{
}

// -----------------------------------------------------------------------
const struct mx_proto mx_drv_puncher = {
	.name = "puncher",
	.dir = MX_DIR_OUTPUT,
	.phy_types = { MX_PHY_USART_ASYNC, MX_PHY_8255, -1 },
	.init = mx_puncher_init,
	.destroy = mx_puncher_destroy,
	.cmd = {
		[MX_CMD_ATTACH] = { 1, 0, NULL, NULL, mx_dummy_attach },
		[MX_CMD_TRANSMIT] = { 4, 2, NULL, NULL, NULL },
		[MX_CMD_DETACH] = { 0, 0, NULL, NULL, mx_dummy_detach },
		[MX_CMD_ABORT] = { 0, 0, NULL, NULL, NULL },
	}
};

// vim: tabstop=4 shiftwidth=4 autoindent
