/************************************************
 * Makefont                                     *
 * Main program                                 *
 * makefont.cpp                                 *
 ************************************************/

#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[])
{
	FILE *fp0, *fp1;
	enum {
		COMP, DECOMP
	} mode;

	if (argc == 3) {
		mode = COMP;
		fp0 = fopen(argv[1], "rb");
		fp1 = fopen(argv[2], "wb");
	} else if (argc == 4) {
		if (strcmp(argv[1], "comp") == 0) {
			mode = COMP;
		} else if (strcmp(argv[1], "decomp") == 0) {
			mode = DECOMP;
		} else {
			printf("Invalid mode: \'%s\'\n", argv[1]);
			return 1;
		}
		fp0 = fopen(argv[1], "rb");
		fp1 = fopen(argv[2], "wb");
	} else {
		printf("Makefont Command Line Help\n");
		printf("usage>makefont [comp|decomp] <input file> <output file>\n\n");
		printf("option :\n");
		printf("\tCOMP   : Compile input file.\n");
		printf("\tDECOMP : Decompile input file.\n");
		printf("\nexit codes:\n");
		printf("\t0  : Successful completion.\n");
		printf("\t1  : Command line error.\n");
		printf("\t2  : Application cannot open input file.\n");
		printf("\t3  : Application cannot open output file.\n");
		printf("\t4  : Not implemented.\n");
		return 1;
	}

	if (fp0 == NULL) {
		printf("Application cannot open input file.\n");
		return 2;
	}
	if (fp1 == NULL) {
		printf("Application cannot open output file.\n");
		return 3;
	}

	if (mode == COMP) {
		while (!feof(fp0)) {
			char s[12];
			int i;
			if (fgets(s, 12, fp0) != NULL && (/*s[0] == ' ' || */s[0] == '*' || s[0] == '.')) {
				i  = (s[0] == '*') << 7;
				i |= (s[1] == '*') << 6;
				i |= (s[2] == '*') << 5;
				i |= (s[3] == '*') << 4;
				i |= (s[4] == '*') << 3;
				i |= (s[5] == '*') << 2;
				i |= (s[6] == '*') << 1;
				i |= (s[7] == '*');
				fputc(i, fp1);
			}
		}
	} else {
		printf("This program is not implemented.");
		fclose(fp0);
		fclose(fp1);
		return 4;
#if false
		while (!feof(fp0)) { // ‰½ŒÌ‚©–³ŒÀƒ‹[ƒv‚·‚é
			char i[1], o[12] = "........\r\n";
			if (fgets(i, 1, fp0) != NULL) {
				if (((i[0] >> 7) & 0x01) == 0x01) o[0] = '*';
				if (((i[0] >> 6) & 0x01) == 0x01) o[1] = '*';
				if (((i[0] >> 5) & 0x01) == 0x01) o[2] = '*';
				if (((i[0] >> 4) & 0x01) == 0x01) o[3] = '*';
				if (((i[0] >> 3) & 0x01) == 0x01) o[4] = '*';
				if (((i[0] >> 2) & 0x01) == 0x01) o[5] = '*';
				if (((i[0] >> 1) & 0x01) == 0x01) o[6] = '*';
				if (((i[0]) & 0x01) == 0x01) o[7] = '*';
				fputs(o, fp1);
			}
		}
#endif
	}


	fclose(fp0);
	fclose(fp1);
	return 0;
}
