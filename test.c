#include <stdio.h>

typedef unsigned char BYTE;

long get_file_size(FILE *fp)
{
	fseek(fp, 0, SEEK_END);
	long len = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	return len;
}

int main()
{
	FILE *fp = fopen("rom/pong.rom", "rb");
	if (!fp)
	{
		return -1;
	}

	long len = get_file_size(fp);
	long read = 0;
	BYTE opcode[2];

	while (read < len)
	{
		fread(opcode, 1, 2, fp);
		read += 2;
		//little endian
		int opc = (opcode[0] << 8) | opcode[1];
		printf("0x%04X %d %d\n", opc, opcode[0], opcode[1]);
		//printf("read: %ld\n", read);
	}

	fclose(fp);
}
