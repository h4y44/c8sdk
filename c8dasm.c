#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "common.h"

typedef unsigned char		BYTE;
typedef uint16_t			OPCODE;

#define PUT_OPCODE(fmt, ...) \
	do { printf("0x%04x: "fmt, opc, __VA_ARGS__); } while(0)

void disassemble(BYTE *rom, long size)
{
	BYTE *ptr = rom;
	long read = 0;

	for (; read < size; ptr += 2, read += 2)
	{
		/*fetch opcode from rom*/
		OPCODE opc = (*ptr << 8) | (*(ptr + 1));
		//printf("0x%04x: ", opc);

		uint8_t x = (opc & 0x0F00) >> 8;
		uint8_t y = (opc & 0x00F0) >> 4;
		uint8_t val = (opc & 0x000F);
		uint8_t o = opc >> 12;
		uint8_t kk = opc & 0x00FF;
		uint16_t xxx = opc & 0x0FFF;
		if (opc == 0x00E0)
		{
			PUT_OPCODE("%s		;;clear screen\n", "CLS");
		}
		else if (opc == 0x00EE)
		{
			PUT_OPCODE("%s		;;return\n", "RET");
		}
		switch (o)
		{
			case 0:
				PUT_OPCODE("SYS  #0x%03X\n", opc);
				break;
			case 1:
				PUT_OPCODE("JMP  #0x%03X\n", xxx);
				break;
			case 2:
				PUT_OPCODE("CALL #0x%03X\n", xxx);
				break;
			case 3:
				PUT_OPCODE("SE   V%d #0x%02X\n", x, kk);
				break;
			case 4:
				PUT_OPCODE("SNE  V%d #0x%02X\n", x, kk);
				break;
			case 5:
				if ((opc & 0x000F) == 0x0)
				{
					PUT_OPCODE("SE   V%d V%d\n", x, y);
				}
				break;
			case 6:
				PUT_OPCODE("MOV  V%d #0x%02X\n", x, kk);
				break;
			case 7:
				PUT_OPCODE("ADD  V%d #0x%02X\n", x, kk);
				break;
			case 8:
				switch (val)
				{
					case 0:
						PUT_OPCODE("MOV  V%d V%d\n", x, y);
						break;
					case 1:
						PUT_OPCODE("OR   V%d V%d\n", x, y);
						break;
					case 2:
						PUT_OPCODE("AND  V%d V%d\n", x, y);
						break;
					case 3:
						PUT_OPCODE("XOR  V%d V%d\n", x, y);
						break;
					case 4:
						PUT_OPCODE("ADD  V%d V%d\n", x, y);
						break;
					case 5:
						PUT_OPCODE("SUB  V%d V%d\n", x, y);
						break;
					case 6:
						PUT_OPCODE("SHR  V%d 1\n", x);
						break;
					case 7:
						PUT_OPCODE("SUBN V%d V%d\n", x, y);
						break;
					case 0xE:
						PUT_OPCODE("SHL  V%d 1\n", x);
						break;
					default:
						printf("0x%04x: unimplemented\n", opc);
						break;
				}
				break;
			case 0x9:
				if ((opc & 0x000F) == 0)
				{
					PUT_OPCODE("SNE  V%d V%d\n", x, y);
				}
				break;
			case 0xA:
				PUT_OPCODE("MOV  I #0x%03X\n", xxx);
				break;
			case 0xB:
				PUT_OPCODE("JMP  V0 #0x%03X\n", xxx);
				break;
			case 0xC:
				PUT_OPCODE("RND  V%d #0x%02X\n", x, kk);
				break;
			case 0xD: //draw
				PUT_OPCODE("DRW  V%d V%d 0x%01X\n", x, y, o);
				break;
			case 0xE:
				if (kk == 0x9E)   //skip press
				{
					PUT_OPCODE("SKP  V%d\n", x);
				}
				else if (kk == 0xA1)   //skip not press
				{
					PUT_OPCODE("SKNP V%d\n", x);
				}
				break;
			case 0xF:
				switch (kk)
				{
					case 0x07:
						PUT_OPCODE("MOV  V%d DT\n", x);
						break;
					case 0x0A: //wait for a key press, save to Vx
						PUT_OPCODE("MOV  V%d K\n", x);
						break;
					case 0x15:
						PUT_OPCODE("MOV  DT V%d\n", x);
						break;
					case 0x18: //set sound timer to Vx
						PUT_OPCODE("MOV  ST V%d\n", x);
						break;
					case 0x1E:
						PUT_OPCODE("ADD  I V%d\n", x);
						break;
					case 0x29: //set I to location of sprite Vx
						PUT_OPCODE("MOV  F V%d\n", x);
						break;
					case 0x33: //store BCD presentation of Vx in I, I+1 and I+2
						PUT_OPCODE("MOV  B V%d\n", x);
						break;
					case 0x55: //???
						PUT_OPCODE("MOV  [I] V%d\n", x);
						break;
					case 0x65:
						PUT_OPCODE("MOV  V%d [I]\n", x);
						break;
					default:
						printf("0x%04x: unimplemented\n", opc);
						break;
				}
				break;
			default:
				printf("0x%04x: unimplemented\n", opc);
				break;
		}
	}
}

long get_size(FILE *fp)
{
	fseek(fp, 0, SEEK_END);
	long flen = ftell(fp);
	fseek(fp, 0, SEEK_SET);
#ifdef C8_DEBUG
	P_DEBUG("Size: %ld (bytes)\n", flen);
#endif
	return flen;
}

int main(int argc, char *argv[])
{
	if (argc != 2 || !strcmp(argv[1], "--help") || !strcmp(argv[1], "-h"))
	{

		fprintf(stderr, "CHIP8 disassembler v0.1\n\
Usage: %s [rom file]\n", argv[0]);
		return 0;
	}

	FILE *fp = fopen(argv[1], "rb");

	if (!fp)
	{
		P_DEBUG("Can not read \'%s\'\n\
Check file existence or your permission\n", argv[1]);
		return -1;
	}

	long flen = get_size(fp);

	BYTE *rom = malloc(flen);
	if (!rom)
	{
		fclose(fp);
		fputs("Can not allocate memory!", stderr);
		return -1;
	}
	printf("Disassembly of: \"%s\"\nSize: %ld (bytes)\n----\n", argv[1], flen);
	fread(rom, 1, flen, fp);
	fclose(fp);

	//invoke the call to disassemble rom file
	disassemble(rom, flen);
	free(rom);

	return 0;
}
