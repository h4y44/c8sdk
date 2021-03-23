#include "c8emu.h"

/* 
 * chip 8 initialization, set up memory, stack and registers
 */
int c8_init(Chip8_t *c8) {
	memset(c8->mem, 0x00, 4096);
	//load sprites into memory
	memcpy(c8->mem, sprites, sizeof(sprites));
	// stack pointer on top of preserved memory
	c8->SP = 0x200;
	// program counter
	c8->PC = 0x200;
	// timers
	c8->DT = 0;
	c8->ST = 0;
	c8->I = 0x0;
	c8->VF = 0x0;
	/*
	 * set all general registers to 0
	 */
	memset(c8->V, 0, 16);

	c8->state = C8_STATE_STOPPED;

#ifdef DEBUG_ENABLED
	puts("Done initializing chip8");
#endif
	return 0;
}

long get_size(FILE *fp)
{
	fseek(fp, 0, SEEK_END);
	long flen = ftell(fp);
	fseek(fp, 0, SEEK_SET);
#ifdef DEBUG_ENABLED
	C8_DEBUG("Size: %ld (bytes)\n", flen);
#endif
	return flen;
}

/*
 * load rom from buffer to memory
 * return the size of the rom or -1 if any error
 */
ssize_t c8_load(Chip8_t *c8, FILE *romfp) {
	// get rom size first
	long rom_size = get_size(romfp);

	if (rom_size > (0x1000 - 0x200)) {
		C8_DEBUG("%s", "Size of rom exceeded the memory of chip8!\n");
		return -1;
	}
	
	//memcpy(c8->mem + 0x200, rom, rom_size);
	fread(c8->mem + 0x200, 1, 1, romfp);

	c8->rom_size = rom_size;
#ifdef DEBUG_ENABLED
	C8_DEBUG("Loaded %ld bytes of rom into memory successfully!", rom_size);
#endif
	c8->state = C8_STATE_LOADED;
	return 0;
}

int c8_interpret(Chip8_t *c8) {
	BYTE *ptr = c8->mem + 0x200;
	OPCODE opc;
	size_t read = 0;

	for (; read < c8->rom_size ; ptr += 2, read += 2)
	{
		/* fetch 2 bytes opcode from memory */
		opc = (*ptr << 8) | (*(ptr + 1));

		C8_DEBUG("Opcode: 0x%4x\n", opc);

		uint8_t x = (opc & 0x0F00) >> 8;
		uint8_t y = (opc & 0x00F0) >> 4;
		uint8_t val = (opc & 0x000F);
		uint8_t o = opc >> 12;
		uint8_t kk = opc & 0x00FF;
		uint16_t xxx = opc & 0x0FFF;
		
		if (opc == 0x00E0) {
			//do somthing with SDL to clear screen
#ifdef DEBUG_ENABLED
			C8_DEBUG("%s\n", "SDL clear screen");
#endif
			break;
		}
		
		if (opc == 0x00EE) {
			/*
			 * returns from a routine
			 * jumps to previous address in stack
			 * then pop it out
			 */
			c8->PC = c8->SP;
			c8->SP += sizeof(OPCODE);

#ifdef DEBUG_ENABLED
			C8_DEBUG("Returned from a subroutine to %#3x\n", c8->SP);
#endif
			break;

		}
		
		switch (o) {
			case 0:
				/*
				 * modern interpreter ignores this
				 * jump to address xxx
				 */
#ifdef DEBUG_ENABLED
				C8_DEBUG("Deprecated opcode %dxxx\n", o);
#endif
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
				 * call new routine at xxx
				 * sub stack by 2 bytes, push current PC to it
				 * then set PC to xxx
				 */
				c8->SP += sizeof(OPCODE);
				c8->SP = c8->PC;
				c8->PC = xxx; // set program counter to the new address
#ifdef DEBUG_ENABLED
				C8_DEBUG("Jumped to new routine at %03x, stack value: %03x\n", xxx, c8->SP);
#endif
				break;
			case 3:
				/*
				 * next instruction if Vx = kk
				 */
				if (c8->V[x] == kk) {
					c8->PC += sizeof(OPCODE);
				}
				break;
			case 4:
				/*
				 * next instruction if Vx != kk
				 */
				if (c8->V[x] != kk) {
					c8->PC += sizeof(OPCODE);
				}
				break;
			case 5:
				/*
				 * next instruction if Vx = Vy
				 */
				if (c8->V[x] == c8->V[y]) {
					c8->PC += sizeof(OPCODE);
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
				 * what if kk overflows V[x]??
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
						 * add Vy to Vx, if sum of it exceeds 16 bit
						 * set VF to 1
						 */
						{
							uint16_t temp = c8->V[x] + c8->V[y];
							if (temp > 0xff) {
#ifdef DEBUG_ENABLED
								C8_DEBUG("Integer overflow triggered (%d)\n", temp);
#endif
								c8->V[x] = temp << 1;
								c8->VF = 1;
								break;
							}
							c8->V[x] = temp;
						}
						break;
					case 5:
						/*
						 * subtract Vx by Vy
						 */
						c8->V[x] -= c8->V[y];
						break;
					case 6:
						/*
						 * right shift Vx by 1
						 */
						c8->V[x] >>= 1;
						c8->VF = c8->V[x] % 2;
						break;
					case 7:
						/*
						 * SUBN Vx Vy
						 */
						c8->V[x] = c8->V[y] - c8->V[x];
						break;
					case 0xe:
						/*
						 * left shift Vx by 1
						 */
						c8->V[x] <<= 1;
						c8->VF = c8->V[x] >> 7;
						break;
				} //switch val
				break;
			case 9:
				if (c8->V[x] != c8->V[y]) {
					c8->PC += sizeof(OPCODE);
				}
				break;
				
			case 0xa:
				c8->I = xxx;
				break;
				
			case 0xb:
				c8->PC = xxx + c8->V[0];
				break;
				
			case 0xc:
				
				break;
				
			case 0xd:
				/*
				 * calls SDL to draw
				 */
#ifdef DEBUG_ENABLED
				C8_DEBUG("%s\n", "Calling SDL to draw");
#endif
				break;
				
			case 0xe:
				if (kk == 0x9e) {
					
					break;
				}
				else if (kk == 0xa1) {
					
					break;
				}
				break;
				
			case 0xf:
				switch (kk) {
					case 0x07:
#ifdef DEBUG_ENABLED
						C8_DEBUG("%s", "Loading DT into Vx\n");
#endif
						c8->V[x] = c8->DT;
						break;
					
					case 0x0a:
						/*
						 * interrupted, waiting for user input
						 */
#ifdef DEBUG_ENABLED
						C8_DEBUG("%s", "Waiting for user input\n");
						scanf("%d\n", &c8->K);
#endif
						c8->V[x] = c8->K;
						break;
						
					case 0x15:
						/*
						 * Set delay time to Vx
						 */
						c8->DT = c8->V[x];
						break;
						
					case 0x18:
						c8->ST = c8->V[x];
						break;
						
					case 0x1e:
						c8->I += c8->V[x];
						break;
						
					case 0x29:
#ifdef DEBUG_ENABLED
						C8_DEBUG("Loading texture %c into register I\n", c8->V[x]);
#endif
						c8->I = c8->V[x] * 5;
						break;
						
					case 0x33:
						
						break;
						
					case 0x55:
					
						break;
						
					case 0x65:
						
						
						break;
						
					default:
						C8_DEBUG("Unimplemented opcode: %#3X\n", opc);
				}

				
			default:
				C8_DEBUG("Unimplemented opcode: %#3X\n", opc);
		} //switch (o)
		
	} //for each opcode in rom
}

int c8_print_vm(Chip8_t *c8) {
	puts("=== C8 VM INFO ===");
	
	printf("V[0 -> 15]: ");
	for (int i = 0; i < 16; ++i) {
		printf("0x%02x ", c8->V[i]);
	}

	printf("\nVF: 0x%2x | I : 0x%2x | DT: 0x%2x | ST: 0x%2x\n", c8->VF, c8->I, c8->DT, c8->ST);
	printf("K: 0x%2x | PC: 0x%2x | SP: 0x%2x\n", c8->K, c8->PC, c8->SP);
	printf("State: %d | ROM size: %ld\n", c8->state, c8->room_size);

	return 0;
}

int main(int argc, char**argv) {
	char *fname = argv[1]; // fail if doesn't exist

	FILE *fp = fopen(fname, "rb");
	if (!fp) {
		C8_DEBUG("%s", "Failed to open file");
		exit(-1);
	}

	puts(":: Creating c8 vm");
	Chip8_t c8vm;
	c8_init(&c8vm);

	int err = c8_load(&c8vm, fp);
	c8_print_vm(&c8vm);

	c8_interpret(&c8vm);

	return 0;
}


