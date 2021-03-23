#ifndef C8EMU_H
#define C8EMU_H

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "common.h"

typedef unsigned char		BYTE;
typedef uint8_t				REGISTER;
typedef uint16_t			OPCODE;

/*
 * sprites from 0 to F
 */
BYTE sprites[][5] =
{
	{
		0xF0,
		0x90,
		0x90,
		0x90,
		0xF0
	},
	{
		0x20,
		0x60,
		0x20,
		0x20,
		0x70
	},
	{
		0xF0,
		0x10,
		0xF0,
		0x80,
		0xF0
	},
	{
		0xF0,
		0x10,
		0xF0,
		0x10,
		0xF0
	},
	{
		0x90,
		0x90,
		0xF0,
		0x10,
		0x10
	},
	{
		0xF0,
		0x80,
		0xF0,
		0x10,
		0xF0
	},
	{
		0xF0,
		0x80,
		0xF0,
		0x90,
		0xF0
	},
	{
		0xF0,
		0x10,
		0x20,
		0x40,
		0x40
	},
	{
		0xF0,
		0x90,
		0xF0,
		0x90,
		0xF0
	},
	{
		0xF0,
		0x90,
		0xF0,
		0x10,
		0xF0
	},
	{
		0xF0,
		0x90,
		0xF0,
		0x90,
		0x90
	},
	{
		0xE0,
		0x90,
		0xE0,
		0x90,
		0xE0
	},
	{
		0xF0,
		0x80,
		0x80,
		0x80,
		0xF0
	},
	{
		0xE0,
		0x90,
		0x90,
		0x90,
		0xE0
	},
	{
		0xF0,
		0x80,
		0xF0,
		0x80,
		0xF0
	},
	{
		0xF0,
		0x80,
		0xF0,
		0x80,
		0x80
	}
};

enum c8_state {
	C8_STATE_STOPPED, // uninitialized, no program loaded
	C8_STATE_LOADED, // program is loaded and ready to run
	C8_STATE_RUNNING, // it is running
	C8_STATE_DEBUGGING // in debug mode
};

enum c8_err {
	C8_ERR_WRONG_OPCODE 
};

typedef struct
{
	BYTE mem[4096];
	size_t rom_size;
	REGISTER V[16]; //16 registers from 0x0 to 0xf
	REGISTER VF;
	REGISTER I;

	REGISTER DT, ST; //delay timer and sound timer
	REGISTER K; //key

	OPCODE PC; //program counter
	OPCODE SP;  //stack pointer 16 levels of stack calls
	int state;
} Chip8_t;

int c8_init(Chip8_t *);
ssize_t c8_load(Chip8_t *, FILE *);
int c8_interpret(Chip8_t *);

int c8_print_vm(Chip8_t *);

#endif
