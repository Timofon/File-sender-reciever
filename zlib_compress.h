#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>

void compress_file(const char *source_path, const char *dest_path) {
	FILE *source_file = fopen(source_path, "rb");
	if (!source_file) {
		printf("Error: could not open file '%s'\n", source_path);
		return;
	}

	FILE *dest_file = fopen(dest_path, "wb");
	if (!dest_file) {
		printf("Error: could not create file '%s'\n", dest_path);
		fclose(source_file);
		return;
	}

	const int CHUNK_SIZE = 1024;
	unsigned char input_buffer[CHUNK_SIZE];
	unsigned char output_buffer[CHUNK_SIZE];

	z_stream stream;
	memset(&stream, 0, sizeof(stream));
	deflateInit(&stream, Z_DEFAULT_COMPRESSION);

	int bytes_read = 0;
	int bytes_written = 0;
	do {
		bytes_read = fread(input_buffer, 1, CHUNK_SIZE, source_file);
		stream.avail_in = bytes_read;
		stream.next_in = input_buffer;

		do {
			stream.avail_out = CHUNK_SIZE;
			stream.next_out = output_buffer;
			deflate(&stream, Z_FINISH);
			bytes_written = CHUNK_SIZE - stream.avail_out;
			fwrite(output_buffer, 1, bytes_written, dest_file);
		} while (stream.avail_out == 0);
	} while (bytes_read == CHUNK_SIZE);

	deflateEnd(&stream);
	fclose(source_file);
	fclose(dest_file);
}

void decompress_file(const char *source_path, const char *dest_path) {
	FILE *source_file = fopen(source_path, "rb");
	if (!source_file) {
		printf("Error: could not open file '%s'\n", source_path);
		return;
	}

	FILE *dest_file = fopen(dest_path, "wb");
	if (!dest_file) {
		printf("Error: could not create file '%s'\n", dest_path);
		fclose(source_file);
		return;
	}

	const int CHUNK_SIZE = 1024;
	unsigned char input_buffer[CHUNK_SIZE];
	unsigned char output_buffer[CHUNK_SIZE];

	z_stream stream;
	memset(&stream, 0, sizeof(stream));
	inflateInit(&stream);

	int bytes_read = 0;
	int bytes_written = 0;
	int flush = Z_SYNC_FLUSH;

	do {
		bytes_read = fread(input_buffer, 1, CHUNK_SIZE, source_file);
		stream.avail_in = bytes_read;
		stream.next_in = input_buffer;

		do {
			stream.avail_out = CHUNK_SIZE;
			stream.next_out = output_buffer;
			int err = inflate(&stream, flush);
			if (err == Z_STREAM_ERROR) {
				printf("Error: inflate() failed with code %d\n", err);
				inflateEnd(&stream);
				fclose(source_file);
				fclose(dest_file);
				return;
			}
			bytes_written = CHUNK_SIZE - stream.avail_out;
			fwrite(output_buffer, 1, bytes_written, dest_file);
		} while (stream.avail_out == 0);

	} while (bytes_read == CHUNK_SIZE);

	inflateEnd(&stream);
	fclose(source_file);
	fclose(dest_file);
}