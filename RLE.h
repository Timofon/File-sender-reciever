#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

char *RLE(const char *str) {
	char *output_text = (char *)malloc(strlen(str) * 2 * sizeof(char));
	size_t n = strlen(str);
	int j = 0;
	for (int i = 0; i < n; i++) {
		int cnt = 1;
		while (i < n - 1 && str[i] == str[i + 1]) {
			cnt++;
			i++;
		}
		output_text[j++] = str[i];
		if (cnt != 1) {
			char cnt_str[10];
			sprintf(cnt_str, "%d", cnt);
			strcat(output_text, cnt_str);
			j += strlen(cnt_str);
		}
	}
	return output_text;
}

char *DecodeRLE(const char *str) {
	char *output_text = (char *)malloc(strlen(str) * sizeof(char));
	char cnt_str[10];
	int k = 0;
	char last = str[0];
	for (int i = 0; i < strlen(str); i++) {
		if (isdigit(str[i])) {
			cnt_str[k++] = str[i];
		} else {
			cnt_str[k] = '\0';
			if (k == 0) {
				if (i == 0)
					continue;
				output_text[strlen(output_text)] = last;
				last = str[i];
			} else {
				int n = atoi(cnt_str);
				for (int j = 0; j < n; j++) {
					output_text[strlen(output_text)] = last;
				}
				k = 0;
				last = str[i];
			}
		}
	}
	int n = atoi(cnt_str);
	for (int j = 0; j < n; j++) {
		output_text[strlen(output_text)] = last;
	}
	return output_text;
}

char *pack(const char *input_file, const char *output_file) {
	printf("packing...(\"%s\", \"%s\")\n", input_file, output_file);

	FILE *fp_in = fopen(input_file, "rb");
	if (fp_in == NULL) {
		perror("Error opening input file");
		exit(1);
	}
	fseek(fp_in, 0L, SEEK_END);
	long int size = ftell(fp_in);
	rewind(fp_in);
	char *input_text = (char *)malloc(size * sizeof(char));
	if (fread(input_text, sizeof(char), size, fp_in) != size) {
		perror("Error reading input file");
		exit(1);
	}
	fclose(fp_in);
	input_text[size - 1] = '\0';
	char *output_text = RLE(input_text);
	FILE *fp_out = fopen(output_file, "wb");
	if (fp_out == NULL) {
		perror("Error opening output file");
		exit(1);
	}
	if (fwrite(output_text, sizeof(char), strlen(output_text), fp_out) !=
		strlen(output_text)) {
		perror("Error writing to output file");
		exit(1);
	}
	fclose(fp_out);
	free(input_text);
	free(output_text);
	return output_text;
}

void unpack(const char *input_file, const char *output_file) {
	printf("unpack(\"%s\", \"%s\")\n", input_file, output_file);
	// your code here
	char *input_text = NULL;
	char *output_text = NULL;
	FILE *input = fopen(input_file, "r");
	if (input == NULL) {
		perror("Error opening input file");
		exit(EXIT_FAILURE);
	}
	fseek(input, 0, SEEK_END);
	long input_size = ftell(input);
	rewind(input);
	input_text = malloc(input_size + 1);
	if (input_text == NULL) {
		perror("Error allocating memory for input file");
		exit(EXIT_FAILURE);
	}
	fread(input_text, input_size, 1, input);
	fclose(input);
	input_text[input_size] = '\0';
	output_text = DecodeRLE(input_text);
	FILE *output = fopen(output_file, "w");
	if (output == NULL) {
		perror("Error opening output file");
		exit(EXIT_FAILURE);
	}
	fprintf(output, "%s", output_text);
	fclose(output);
	free(input_text);
	free(output_text);
}


