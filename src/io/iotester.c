//  Copyright (c) 2018 Jakub Filipowicz <jakubf@gmail.com>
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

#include <inttypes.h>
#include <stdlib.h>
#include <pthread.h>

#include "log.h"
#include "atomic.h"
#include "io/io.h"
#include "io/chan.h"
#include "utils/elst.h"

enum it_event_types { EV_CMD, EV_RESET, EV_QUIT, };

struct it_event {
	int type;
	int cmd;
	uint16_t r;
};

enum it_commands {
	CMD_ISP	= 0b0001, // FETCH
	CMD_RND	= 0b0010, // FETCH
	CMD_ANS	= 0b0011, // FETCH
	CMD_WNB	= 0b0001, // SEND
	CMD_WAM	= 0b0010, // SEND
	CMD_WAB	= 0b0011, // SEND
	CMD_WM	= 0b0100, // SEND
	CMD_RM	= 0b0101, // SEND
	CMD_WMM	= 0b0110, // SEND
	CMD_RMM	= 0b0111, // SEND
	CMD_IRQ	= 0b1000, // SEND
	CMD_PA	= 0b1001, // SEND
};

struct iotester {
	pthread_t thread;
	ELST evq;

	int chnum;
	uint16_t intspec;
};

static void * it_cmdproc(void *ptr);

// -----------------------------------------------------------------------
void * it_create(int num, struct cfg_unit *units)
{
	LOG(L_IO, "Creating new I/O tester");

	struct iotester *it = calloc(1, sizeof(struct iotester));
	if (!it) {
		LOGERR("Memory allocation error.");
		return NULL;
	}

	it->chnum = num;
	srand(time(NULL));

	struct cfg_unit *dev_cfg = units;
	while (dev_cfg) {
		LOG(L_IO, "IOtester can't connect devices. Ignored device: %s", dev_cfg->name);
		dev_cfg = dev_cfg->next;
	}

	it->evq = elst_create(1024);
	if (!it->evq) {
		LOGERR("Failed to create event queue.");
		free(it);
		return NULL;
	}

	if (pthread_create(&it->thread, NULL, it_cmdproc, it)) {
		LOGERR("Failed to spawn main I/O tester thread.");
		elst_destroy(it->evq);
		free(it);
		return NULL;
	}

	LOG(L_IO, "I/O tester created");

	return it;
}

// -----------------------------------------------------------------------
struct it_event *it_event_new(int type, int cmd, uint16_t r)
{
	struct it_event *ev = calloc(1, sizeof(struct it_event));
	ev->type = type;
	ev->cmd = cmd;
	ev->r = r;

	return ev;
}

// -----------------------------------------------------------------------
void it_shutdown(void *ch)
{
	if (!ch) return;

	struct iotester *it = (struct iotester *) ch;

	LOG(L_IO, "I/O tester shutting down");

	elst_insert(it->evq, it_event_new(EV_QUIT, 0, 0), 0);
	pthread_join(it->thread, NULL);
	elst_destroy(it->evq);
	free(ch);

	LOG(L_IO, "Shutdown complete");
}

// -----------------------------------------------------------------------
void it_reset(void *ch)
{
	struct iotester *it = (struct iotester *) ch;
	LOG(L_IO, "Received reset request");
	elst_insert(it->evq, it_event_new(EV_RESET, 0, 0), 0);
}

// -----------------------------------------------------------------------
static void * it_cmdproc(void *ptr)
{
	int quit = 0;
	struct iotester *it = ptr;

	LOG(L_IO, "Entering main I/O tester loop");

	uint16_t r;
	int res = 0;
	int nb = 0;
	uint16_t am = 0;
	uint16_t ab = 0;
	uint16_t buf[0x10000];

	while (!quit) {
		struct it_event *ev = elst_wait_pop(it->evq, 0);
		switch (ev->type) {
			case EV_QUIT:
				LOG(L_IO, "Quit");
				quit = 1;
				break;
			case EV_RESET:
				LOG(L_IO, "Reset");
				break;
			case EV_CMD:
				r = ev->r;
				switch (ev->cmd) {
					case CMD_WNB:
						LOG(L_IO, "NB = %i", r & 0b1111);
						nb = r & 0b1111;
						atom_store_release(&it->intspec, 0);
						io_int_set(it->chnum);
						break;
					case CMD_WAM:
						LOG(L_IO, "AM = 0x%04x", r);
						am = r;
						atom_store_release(&it->intspec, 0);
						io_int_set(it->chnum);
						break;
					case CMD_WAB:
						LOG(L_IO, "AB = 0x%04x", r);
						ab = r;
						atom_store_release(&it->intspec, 0);
						io_int_set(it->chnum);
						break;
					case CMD_WM:
						LOG(L_IO, "single: buf[0x%04x] -> [%i:0x%04x], %i words", ab, nb, am, r);
						for (int i=0 ; i<r ; i++) {
							res = io_mem_put(nb, am+i, buf[ab+i]);
							if (!res) break;
						}
						atom_store_release(&it->intspec, res);
						io_int_set(it->chnum);
						break;
					case CMD_RM:
						LOG(L_IO, "single: [%i:0x%04x] -> buf[0x%04x], %i words", nb, am, ab, r);
						for (int i=0 ; i<r ; i++) {
							res = io_mem_get(nb, am+i, buf+ab+i);
							if (!res) break;
						}
						atom_store_release(&it->intspec, res);
						io_int_set(it->chnum);
						break;
					case CMD_WMM:
						LOG(L_IO, "multi: buf[0x%04x] -> [%i:0x%04x], %i words", ab, nb, am, r);
						res = io_mem_mput(nb, am, buf+ab, r);
						atom_store_release(&it->intspec, res);
						io_int_set(it->chnum);
						break;
					case CMD_RMM:
						LOG(L_IO, "multi: [%i:0x%04x] -> buf[0x%04x], %i words", nb, am, ab, r);
						res = io_mem_mget(nb, am, buf+ab, r);
						atom_store_release(&it->intspec, res);
						io_int_set(it->chnum);
						break;
					case CMD_IRQ:
						LOG(L_IO, "Sending interrupt (intspec set to 0x%04x)", r);
						atom_store_release(&it->intspec, r);
						io_int_set(it->chnum);
						break;
					case CMD_PA:
						LOG(L_IO, "Sending Power Alarm interrupt");
						io_int_set_pa();
						break;
					default:
						LOG(L_IO, "Unknown 'SEND' command: %i", ev->cmd);
						atom_store_release(&it->intspec, 0);
						io_int_set(it->chnum);
						break;
				}
				break;
			default:
				LOG(L_IO, "Unknown event: %i", ev->type);
				break;
		}
		free(ev);
	}

	pthread_exit(NULL);
}

// -----------------------------------------------------------------------
int it_cmd(void *ch, int dir, uint16_t n_arg, uint16_t *r_arg)
{
	struct iotester *it = (struct iotester *) ch;

	int echo = (n_arg & 0b1000000000000000) >> 15;
	int cmd =  (n_arg & 0b0111100000000000) >> 11;
	int rmd =  (n_arg & 0b0000011111100000) >> 5;

	// 'FETCH' requests are served on the spot
	if (dir == IO_IN) {
		if (echo) {
			LOG(L_IO, "Echo: 0x%04x", n_arg);
			*r_arg = n_arg;
		} else {
			switch (cmd) {
				case CMD_ANS:
					LOG(L_IO, "CPU wants response: %i", rmd & 0b11);
					return (rmd & 0b11);
				case CMD_ISP:
					*r_arg = atom_load_acquire(&it->intspec);
					LOG(L_IO, "Sending intspec to CPU: 0x%04x", *r_arg);
					break;
				case CMD_RND:
					*r_arg = random();
					LOG(L_IO, "Generated random: 0x%04x", *r_arg);
					break;
				default:
					LOG(L_IO, "Unknown FETCH request: %i", cmd);
					return IO_NO;
			}
		}
	// 'SEND' requests are handled in the event thread except CMD_ANS
	} else {
		LOG(L_IO, "Enqueue command: %i, r_arg: 0x%04x", cmd, *r_arg);
		elst_append(it->evq, it_event_new(EV_CMD, cmd, *r_arg));
	}

	return IO_OK;
}

// -----------------------------------------------------------------------
const struct chan_drv it_chan_driver = {
	.name = "iotester",
	.create = it_create,
	.shutdown = it_shutdown,
	.reset = it_reset,
	.cmd = it_cmd
};

// vim: tabstop=4 shiftwidth=4 autoindent
