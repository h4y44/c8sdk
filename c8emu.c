#include "c8emu.h"

/* 
 * chip 8 initialization, set up memory, stack and registers
 */
int c8_init(Chip8_t *c8) {
	memset(c8->mem, 0, 4096);
	// stack pointer on top of preserved memory
	c8->SP = 0x200;
	// program counter
	c8->PC = 0;
	// timers
	c8->DT = 0;
	c8->ST = 0;
	c8->I = 0x0;
#ifdef C8_DEBUG
	puts("Done initializing chip8");
#endif
	/*
	 * set all registers to 0
	 */
	memset()
	return 0;
}

/*
 * load rom from buffer to memory
 * return the size of the rom or -1 if any error
 */
ssize_t c8_load(Chip8_t *c8, BYTE *rom, size_t rom_size) {
	if (rom_size > (0x1000 - 0x200)) {
		P_DEBUG("Size of rom exceeded the memory of chip8!\n");
		return -1;
	}
	
	memcpy(c8->mem + 0x200, rom, rom_size);
	c8->room_size = room_size
#ifdef C8_DEBUG
	P_DEBUG("Loaded %ld bytes of rom into memory successfully!", rom_size);
#endif
	return 0;
}

int c8_interpret(Chip8_t *c8) {
	BYTE *ptr = c8->mem + 0x200;
	OPCODE opc; //2 bytes for each opcode
	size_t read = 0;

	for (; read < c8->room_size ; ptr += 2, read += 2)
	{
		/*fetch opcode from rom*/
		opc = (*ptr << 8) | (*(ptr + 1));
#ifdef C8_DEBUG
		P_DEBUG("Fetched opcode %#4x at %#3x", opc, 0x200 + read);
#endif

		uint8_t x = (opc & 0x0F00) >> 8;
		uint8_t y = (opc & 0x00F0) >> 4;
		uint8_t val = (opc & 0x000F);
		uint8_t o = opc >> 12;
		uint8_t kk = opc & 0x00FF;
		uint16_t xxx = opc & 0x0FFF;
		
		if (opc == 0x00E0) {
			//do somthing with SDL to clear screen
		}
		if (opc == 0x00EE) {
			/*
			 * return from a routine
			 * jump to previous address in stack
			 * the pop it out
			 */

		}
		
		switch (o) {
			case 0:
				/*
				 * modern interpreter ignores this
				 * jump to address xxx
				 */
				c8->PC = xxx;
				break;
			case 1:
				/*
				 * jump to xxx
				 */
				c8->PC = xxx;
				break;
			case 2:
				/*
				 * call new routine from xxx
				 * sub stack to 2, push current PC to it
				 * then set PC to xxx
				 */
				c8->SP -= 0x02;
				c8->PC = xxx;
				break;
			case 3:
				/*
				 * next instruction if Vx = kk
				 */
				if (V[x] == kk) {
					c8->PC += 0x2;
				}
				break;
			case 4:
				/*
				 * next instruction if Vx != kk
				 */
				if (V[x] != kk) {
					c8->PC += 0x2;
				}
				break;
			case 5:
				/*
				 * next instruction if Vx = Vy
				 */
				if (c8->V[x] == c8->V[y]) {
					c8->PC += 0x2;
				}
				break;
			case 6:
				/*
				 * move kk into Vx
				 */
				c8->V[x] = kk;
				break;
			case 7:
				/*
				 * add kk to Vx
				 */
				c8->V[x] += kk;
				break;
			case 8:
				switch (val) {
					case 0:
						/*
						 * mov Vy to Vx
						 */
						c8->V[x] = c8->V[y];
						break;
					case 1:
						/*
						 * or Vx and Vy, then save in Vx
						 */
						c8->V[x] |= c8->V[y];
						break;
					case 2:
						/*
						 * and Vx and Vy, then save in Vx
						 */
						c8->V[x] &= c8->V[y];
						break;
					case 3:
						/*
						 * xor Vx and Vy, then save in Vx
						 */
						c8->V[x] ^= c8->V[y];
						break;
					case 4:
						/*
						 * add Vy to Vx
						 */
						c8->V[x] += c8->V[y];
						break;
					case 5:
						/*
						 * subtract Vx by Vy
						 */
						c8->V[x] -= c8->V[y];
						break;
					case 6:
						/*
						 * shift right Vx by 1
						 */
						c8->V[x] >>= 1;
						break;
					case 7:
						/*
						 *
						 */
				}
				break;
		} //switch (o)
	} //for each opcode in rom
}
