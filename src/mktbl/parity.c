#include <stdio.h>

const int n[] = {
	0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4
};

int main(void)
{
	int i, j;

	for(i = 0; i != 256; i += 16) {
		printf("\t");
		for(j = i; j != i + 16; j++)
			printf("%s, ", (n[j & 0x0f] + n[(j & 0xf0) >> 4]) % 2 ? "0": "MASK_PV");
		printf("\n");
	}

	return 0;
}

/* eof */
