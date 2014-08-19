%{
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

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include "cfg.h"

struct cfg_chan_t *this_chan;

void cyyerror(char *s, ...);
int cyylex(void);
%}

%locations
%union {
	struct value_t {
		int v;
		char *s;
	} value;
	struct cfg_arg_t *arg;
};

%token COMPUTER CHANNEL UNIT
%token SPEED_REAL TIMER_STEP TIMER_START CPU_MOD CPU_USER_IO_ILLEGAL CPU_AWP
<<<<<<< HEAD
%token ELWRO MEGA MEGA_PROM MEGA_BOOT OS_SEG CPU_STOP_ON_NOMEM
%token <value> TEXT
=======
%token ELWRO MEGA MEGA_PROM MEGA_BOOT OS_SEG CPU_NOMEM_STOP
%token EMULOG ENABLED PAUSED LFILE FORMAT
%token <value> TEXT STRING
>>>>>>> emulog conf., cleanups, cfg() cleanups, emulog dbg
%token <value> VALUE
%token <value> BOOL
%type <arg> arg arglist

%token '{' '}' '=' ':' ','

%%

objects:
	objects object
	|
	;

object:
	COMPUTER '{' computer_opts '}'
	| EMULOG '{' emulog_opts '}'
	| CHANNEL VALUE '=' TEXT { cfg_make_chan($2.v, $4.s); free($2.s); } '{' units '}'
	;

units:
	unit units
	|
	;

unit:
	UNIT VALUE '=' TEXT ':' arglist	{ cfg_make_unit($2.v, $4.s, $6); free($2.s); }
	| UNIT VALUE '=' TEXT			{ cfg_make_unit($2.v, $4.s, NULL); free($2.s); }
	;

arglist:
	arg ',' arglist	{ $1->next = $3; $$ = $1; }
	| arg			{ $$ = $1; }
	;

arg:
	VALUE	{ $$ = cfg_make_arg($1.s); }
	| TEXT	{ $$ = cfg_make_arg($1.s); }
	;

computer_opts:
	computer_opts computer_opt
	|
	;

computer_opt:
	SPEED_REAL '=' BOOL		{ em400_cfg.speed_real = $3.v; free($3.s); }
	| TIMER_STEP '=' VALUE	{ em400_cfg.timer_step = $3.v; free($3.s); }
	| TIMER_START '=' BOOL	{ em400_cfg.timer_start = $3.v; free($3.s); }
	| CPU_MOD '=' BOOL		{ em400_cfg.cpu_mod = $3.v; free($3.s); }
	| CPU_USER_IO_ILLEGAL '=' BOOL	{ em400_cfg.cpu_user_io_illegal = $3.v; free($3.s); }
	| CPU_AWP '=' BOOL		{ em400_cfg.cpu_awp = $3.v; free($3.s); }
	| ELWRO '=' VALUE		{ em400_cfg.mem_elwro = $3.v; free($3.s); }
	| MEGA '=' VALUE		{ em400_cfg.mem_mega = $3.v; free($3.s); }
	| MEGA_BOOT '=' BOOL	{ em400_cfg.mem_mega_boot = $3.v; em400_cfg.cpu_stop_on_nomem = 0; free($3.s); }
	| CPU_STOP_ON_NOMEM '=' BOOL { if (!em400_cfg.mem_mega_boot) em400_cfg.cpu_stop_on_nomem = $3.v ; free($3.s)}
	| MEGA_PROM '=' STRING	{ em400_cfg.mem_mega_prom = $3.s; }
	| OS_SEG '=' VALUE		{ em400_cfg.mem_os = $3.v; free($3.s); }
	;

emulog_opts:
	emulog_opts emulog_opt
	|
	;

emulog_opt:
	ENABLED '=' BOOL { em400_cfg.emulog_enabled = $3.v; free($3.s); }
	| PAUSED '=' BOOL { em400_cfg.emulog_paused = $3.v; free($3.s); }
	| LFILE '=' STRING { free(em400_cfg.emulog_file); em400_cfg.emulog_file = $3.s; }
	| FORMAT '=' STRING { free(em400_cfg.emulog_format); em400_cfg.emulog_format = $3.s; }
	;
%%

// -----------------------------------------------------------------------
void cyyerror(char *s, ...)
{
	va_list ap;
	va_start(ap, s);
	printf("Error parsing config, line %d: ", yylloc.first_line);
	vprintf(s, ap);
	printf("\n");
	cfg_error = 1;
}

// vim: tabstop=4 shiftwidth=4 autoindent
