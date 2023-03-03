#include <stdio.h>
#include <stdlib.h>
#include "RLE.h"
#include "zlib_compress.h"
int main(int argc, char *argv[]) {
	if (argc != 3) {
		fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
		exit(1);
	}

	char *from_file = argv[1];
	char *to_file = argv[2];

// Предложение пользователю выбрать метод кодирования
	printf("Select a compression method:\n");
	printf("1. Run-length encoding (RLE)\n");
	printf("2. Zlib library\n");
	printf("3. Other method\n"); // TODO
	int method;
	int m = scanf("%d", &method);
	if (m != 1) {
		fprintf(stderr, "Err");
		exit(1);
	}
	printf("You want to:\n");
	printf("1. Pack file\n");
	printf("2. Unpack file\n");

	int what_to_do;
	int w  = scanf("%d", &what_to_do);
	if (w != 1) {
		fprintf(stderr, "Err");
		exit(1);
	}
	if (method == 1) { // RLE
		if (what_to_do == 1) {
			pack(from_file, to_file);
		} else if (what_to_do == 2) {
			unpack(from_file, to_file);
		} else {
			fprintf(stderr, "There is no such method here");
			exit(1);
		}
	} else if (method == 2) {
		if (what_to_do == 1) {
			compress_file(from_file, to_file);
		} else if (what_to_do == 2) {
			decompress_file(from_file, to_file);
		} else {
			fprintf(stderr, "There is no such an option here");
			exit(1);
		}
	} else {
		// TODO другие алгоритмы сжатия нужно придумать
	}
	return 0;
}
