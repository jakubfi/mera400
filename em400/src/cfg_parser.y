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

void cyyerror(char *s, ...);
int cyylex(void);
%}

%locations
%union {
	int value;
	char *text;
};

%token CPU MEMORY
%token SPEED MAX REAL TIMER MOD_17 MOD_SINT
%token MODULE ELWRO MEGA
%token <text> TEXT
%token <value> VALUE

%token '{' '}' '=' ':'

%%

objects:
	objects object
	|
	;

object:
	CPU '{' cpu_opts '}'
	| MEMORY '{' modules '}'
	;

cpu_opts:
	cpu_opts cpu_opt
	|
	;

cpu_opt:
	SPEED '=' MAX { em400_cfg.cpu.speed_real = 0; }
	| SPEED '=' REAL { em400_cfg.cpu.speed_real = 1; }
	| TIMER '=' VALUE { em400_cfg.cpu.timer_step = $3; }
	| MOD_17 '=' VALUE { em400_cfg.cpu.mod_17bit = $3; }
	| MOD_SINT '=' VALUE { em400_cfg.cpu.mod_sint = $3; }
	;

modules:
	modules module
	|
	;

module:
	MODULE VALUE '=' ELWRO ':' VALUE {
		cfg_set_mem($2, 0, $6);
	}
	| MODULE VALUE '=' MEGA ':' VALUE {
		cfg_set_mem($2, 1, $6);
	}
	;
%%

void cyyerror(char *s, ...)
{
	va_list ap;
	va_start(ap, s);
	printf("Error parsing config, line %d: ", yylloc.first_line);
	vprintf(s, ap);
	printf("\n");
	cfg_error = 1;
}

// vim: tabstop=4