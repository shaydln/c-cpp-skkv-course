#include "return_codes.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#if defined LIBDEFLATE
#include <libdeflate.h>
#elif defined ZLIB
#include <zlib.h>
#else
#error "Unsupported library"
#endif

struct Image
{
	unsigned int width;
	unsigned int height;
	unsigned int color_type;
	unsigned int filtration_type;
	unsigned int max_length;
	unsigned int current_length;
	unsigned int plte_length;
	unsigned char *data;
	unsigned char *plte;
};

void filtration_paeth(const struct Image *image, int bytes_size, unsigned char *buffer, int i);
void filtration_average(const struct Image *image, int bytes_size, unsigned char *buffer, int i);

bool compare_two_arrays(const unsigned char *a1, const unsigned char *a2, int n)
{
	for (int i = 0; i < n; i++)
	{
		if (a1[i] != a2[i])
		{
			return false;
		}
	}
	return true;
}

int calculate_char(const unsigned char *array, int i)
{
	return ((array[i] * 256 + array[i + 1]) * 256 + array[i + 2]) * 256 + array[i + 3];
}

int parse_signature(FILE *in)
{
	const unsigned char png_signature[8] = { 137, 80, 78, 71, 13, 10, 26, 10 };
	unsigned char current_signature[8];
	if (fread(current_signature, sizeof(unsigned char), 8, in) != 8)
	{
		fprintf(stderr, "There is not full signature\n");
		return ERROR_OUT_OF_MEMORY;
	}
	if (!compare_two_arrays(current_signature, png_signature, 8))
	{
		fprintf(stderr, "The file is not png, due to different signature\n");
		return ERROR_DATA_INVALID;
	}
	return SUCCESS;
}

void print_chars(const unsigned char *a, unsigned int n)
{
	for (int i = 0; i < n; i++)
	{
		printf("%u ", a[i]);
	}
	printf("\n");
}

int parse_IHDR(FILE *in, struct Image *image)
{
	unsigned const char IHDR[8] = { 0x00, 0x00, 0x00, 0x0D, 0x49, 0x48, 0x44, 0x52 };
	unsigned char current_bytes[17];
	if (fread(current_bytes, sizeof(unsigned char), 8, in) != 8)
	{
		fprintf(stderr, "It is not IHDR\n");
		return ERROR_UNKNOWN;
	}
	if (!compare_two_arrays(current_bytes, IHDR, 8))
	{
		fprintf(stderr, "The file is not png, due to different IHDR\n");
		return ERROR_DATA_INVALID;
	}
	if (fread(current_bytes, sizeof(unsigned char), 17, in) != 17)
	{
		fprintf(stderr, "There is invalid IHDR\n");
		return ERROR_UNKNOWN;
	}
	image->width = calculate_char(current_bytes, 0);
	image->height = calculate_char(current_bytes, 4);
	if (image->width == 0 || image->height == 0)
	{
		fprintf(stderr, "height or width is zero\n");
		return ERROR_UNKNOWN;
	}
	if (current_bytes[8] != 8)
	{
		fprintf(stderr, "depth of color is not 8\n");
		return ERROR_UNKNOWN;
	}
	image->color_type = current_bytes[9];
	if (image->color_type != 0 && image->color_type != 2 && image->color_type != 3)
	{
		fprintf(stderr, "unsupported color type\n");
		return ERROR_UNSUPPORTED;
	}
	image->filtration_type = current_bytes[10];
	if (image->filtration_type)
	{
		fprintf(stderr, "incorrect filtration type\n");
		return ERROR_UNSUPPORTED;
	}
	return SUCCESS;
}

int parse_chucks(FILE *in, struct Image *image)
{
	unsigned char const PLTE[] = { 0x50, 0x4C, 0x54, 0x45 };
	unsigned const char IDAT[] = { 0x49, 0x44, 0x41, 0x54 };
	unsigned const char IEND[] = { 0x49, 0x45, 0x4E, 0x44 };
	unsigned char chunk_length[4];
	unsigned char chunk_type[4];
	unsigned char crc[4];
	unsigned char unsupported_data[1];
	int chunk_int_length;
	bool there_is_IDAT = false;
	while (true)
	{
		if (fread(chunk_length, sizeof(unsigned char), 4, in) != 4)
		{
			fprintf(stderr, "There is not full chunk length\n");
			return ERROR_DATA_INVALID;
		}
		chunk_int_length = calculate_char(chunk_length, 0);
		if (fread(chunk_type, sizeof(unsigned char), 4, in) != 4)
		{
			fprintf(stderr, "There is not full chunk name\n");
			return ERROR_DATA_INVALID;
		}
		if (compare_two_arrays(chunk_type, IDAT, 4))
		{
			unsigned char *buffer = malloc(sizeof(unsigned char) * chunk_int_length + 4);
			if (fread(buffer, sizeof(unsigned char), chunk_int_length + 4, in) != chunk_int_length + 4)
			{
				fprintf(stderr, "There is not enough memory for data of IDAT\n");
				return ERROR_OUT_OF_MEMORY;
			}
			if (image->current_length + chunk_int_length > image->max_length - 10)
			{
				image->data = realloc(image->data, sizeof(unsigned char) * (image->max_length + chunk_int_length) * 2);
				if (image->data == NULL)
				{
					fprintf(stderr, "There is not enough memory for new data of IDAT\n");
					return ERROR_OUT_OF_MEMORY;
				}
				image->max_length = (image->max_length + chunk_int_length) * 2;
			}
			for (int i = 0; i < chunk_int_length; i++)
			{
				image->data[i + image->current_length] = buffer[i];
			}
			image->current_length += chunk_int_length;
			free(buffer);
			there_is_IDAT = true;
		}
		else if (compare_two_arrays(chunk_type, IEND, 4))
		{
			if (chunk_int_length != 0)
			{
				fprintf(stderr, "length of IEND isn't 0\n");
				return ERROR_DATA_INVALID;
			}
			if (fread(crc, sizeof(unsigned char), 4, in) != 4)
			{
				fprintf(stderr, "There is no crc\n");
				return ERROR_OUT_OF_MEMORY;
			}
			break;
		}
		else if (compare_two_arrays(chunk_type, PLTE, 4))
		{
			if (image->color_type != 3)
			{
				fprintf(stderr, "This color isn't correct with PLTE\n");
				return ERROR_DATA_INVALID;
			}
			image->plte = realloc(image->plte, sizeof(unsigned char) * (chunk_int_length));
			if (chunk_int_length % 3 != 0 || chunk_int_length > 798)
			{
				fprintf(stderr, "length of PLTE isn't correct \n");
				return ERROR_DATA_INVALID;
			}
			unsigned char *buffer = malloc(sizeof(unsigned char) * chunk_int_length + 4);
			if (fread(buffer, sizeof(unsigned char), chunk_int_length + 4, in) != chunk_int_length + 4)
			{
				fprintf(stderr, "There is not enough memory for data of IDAT\n");
				return ERROR_OUT_OF_MEMORY;
			}
			for (int i = 0; i < chunk_int_length; i++)
			{
				image->plte[i] = buffer[i];
			}
			image->plte_length = chunk_int_length;
			free(buffer);
		}
		else
		{
			for (int i = 0; i < chunk_int_length + 4; i++)
			{
				if (!fread(unsupported_data, sizeof(unsigned char), 1, in))
				{
					fclose(in);
					fprintf(stderr, "%s chunk is not supported\n", chunk_type);
					return ERROR_UNSUPPORTED;
				}
			}
		}
	}
	unsigned char chars_after_IEND[1];
	if (fread(chars_after_IEND, sizeof(unsigned char), 1, in) != 0)
	{
		fprintf(stderr, "After IEND there is a data\n");
		return ERROR_OUT_OF_MEMORY;
	}
	if (!there_is_IDAT)
	{
		fprintf(stderr, "There is no data\n");
		return ERROR_DATA_INVALID;
	}
	return SUCCESS;
}

int calculate_size(struct Image *image)
{
	if (image->color_type == 0)
	{
		return 1;
	}
	else
	{
		return 3;
	}
}

int decompress(FILE *out, struct Image *image)
{
	int bytes_size = calculate_size(image);
	unsigned char *buffer = malloc(sizeof(unsigned char) * (bytes_size * image->width * image->height + image->height));
	if (buffer == NULL)
	{
		free(image->data);
		fprintf(stderr, "Not enough memory for decompressing\n");
		return ERROR_OUT_OF_MEMORY;
	}
#ifdef ZLIB
	uLong buffer_length = bytes_size * image->width * image->height + image->height;
	if (uncompress((Bytef *)buffer, &buffer_length, (Bytef *)image->data, (uLong)image->current_length) != Z_OK)
	{
		free(buffer);
		free(image->data);
		fprintf(stderr, "Decompression failed\n");
		return ERROR_UNKNOWN;
	}
#elif defined LIBDEFLATE
	size_t buffer_length = bytes_size * image->width * image->height + image->height;
	if (libdeflate_zlib_decompress(libdeflate_alloc_decompressor(), image->data, image->current_length, buffer, buffer_length, &buffer_length) !=
		LIBDEFLATE_SUCCESS)
	{
		free(buffer);
		free(image->data);
		fprintf(stderr, "LIBDEFLATE failed");
		return ERROR_UNKNOWN;
	}
#endif
	for (int i = 0; i < image->height; i++)
	{
		int type_of_filter = buffer[i * (image->width * bytes_size + 1)];
		switch (type_of_filter)
		{
		case 1:
			for (int j = bytes_size + 1; j < image->width * bytes_size + 1; j++)
			{
				buffer[(image->width * bytes_size + 1) * i + j] += buffer[i * (image->width * bytes_size + 1) + j - bytes_size];
			}
			break;
		case 2:
			for (int j = 1; i != 0 && j < image->width * bytes_size + 1; j++)
			{
				buffer[(image->width * bytes_size + 1) * i + j] += buffer[(i - 1) * (image->width * bytes_size + 1) + j];
			}
			break;
		case 3:
			filtration_average(image, bytes_size, buffer, i);
			break;
		case 4:
			filtration_paeth(image, bytes_size, buffer, i);
			break;
		}
	}
	if (image->color_type == 0)
	{
		fprintf(out, "P5\n%i %i\n255\n", image->width, image->height);
	}
	else if (image->color_type == 2)
	{
		fprintf(out, "P6\n%i %i\n255\n", image->width, image->height);
	}
	else if (image->color_type == 3)
	{
		// that part i deleted, because it doesn't work
	}
	for (int i = 0; i < image->height; i++)
	{
		for (int j = 1; j < (image->width * bytes_size) + 1; j++)
		{
			if (!fwrite(&buffer[i * (image->width * bytes_size + 1) + j], sizeof(unsigned char), 1, out))
			{
				free(buffer);
				fclose(out);
				fprintf(stderr, "error while writing");
				return ERROR_UNKNOWN;
			}
		}
	}
	free(buffer);
	return SUCCESS;
}
void filtration_average(const struct Image *image, int bytes_size, unsigned char *buffer, int i)
{
	for (int j = 1; j < image->width * bytes_size + 1; j++)
	{
		if (i == 0)
		{
			if (j > bytes_size + 1)
			{
				buffer[(image->width * bytes_size + 1) * i + j] +=
					(unsigned char)((buffer[i * (image->width * bytes_size + 1) + j - bytes_size] / 2) -
									(buffer[i * (image->width * bytes_size + 1) + j - bytes_size] / 2) % 2);
			}
			continue;
		}
		if (j < bytes_size + 1)
		{
			buffer[(image->width * bytes_size + 1) * i + j] +=
				(unsigned char)((buffer[(i - 1) * (image->width * bytes_size + 1) + j] / 2) -
								(buffer[(i - 1) * (image->width * bytes_size + 1) + j] / 2) % 2);
			continue;
		}
		buffer[i * (image->width * bytes_size + 1) + j] +=
			(unsigned char)(((buffer[(image->width * bytes_size + 1) * i + j - bytes_size] + buffer[(i - 1) * (image->width * bytes_size + 1) + j]) / 2) -
							((buffer[(image->width * bytes_size + 1) * i + j - bytes_size] + buffer[(i - 1) * (image->width * bytes_size + 1) + j]) / 2) % 2);
	}
}
void filtration_paeth(const struct Image *image, int bytes_size, unsigned char *buffer, int i)
{
	for (int j = 1; j < image->width * bytes_size + 1; j++)
	{
		if (i == 0)
		{
			if (j - 1 > bytes_size)
			{
				buffer[i * (image->width * bytes_size + 1) + j] += buffer[i * (image->width * bytes_size + 1) + j - bytes_size];
			}
			continue;
		}
		if (j - 1 < bytes_size)
		{
			buffer[i * (image->width * bytes_size + 1) + j] += buffer[(i - 1) * (image->width * bytes_size + 1) + j];
			continue;
		}
		unsigned char a1 = buffer[i * (image->width * bytes_size + 1) + j - bytes_size];
		unsigned char a2 = buffer[(i - 1) * (image->width * bytes_size + 1) + j];
		unsigned char a3 = buffer[(i - 1) * (image->width * bytes_size + 1) + j - bytes_size];
		unsigned char res;
		int p = a1 + a2 - a3;
		if (abs(p - a1) <= abs(p - a2) && abs(p - a1) <= abs(p - a3))
		{
			res = a1;
		}
		else if (abs(p - a2) <= abs(p - a3))
		{
			res = a2;
		}
		else
		{
			res = a3;
		}
		buffer[i * (image->width * bytes_size + 1) + j] += res;
	}
}

int main(int argc, char *argv[])
{
	if (argc != 3)
	{
		fprintf(stderr, "The parameter or number of parameters (argv) is incorrect\n");
		return ERROR_PARAMETER_INVALID;
	}
	FILE *in = fopen(argv[1], "rb");
	if (in == NULL)
	{
		fprintf(stderr, "The input file is invalid\n");
		return ERROR_CANNOT_OPEN_FILE;
	}
	int exit = parse_signature(in);
	if (exit)
	{
		return exit;
	}
	struct Image image;
	image.width = 0;
	image.height = 0;
	image.color_type = 0;
	image.filtration_type = 0;
	image.max_length = 1024;
	image.current_length = 0;
	exit = parse_IHDR(in, &image);
	if (exit)
	{
		return exit;
	}
	image.data = malloc((1024) * sizeof(unsigned char));
	if (image.data == NULL)
	{
		fprintf(stderr, "There is not enough memory for data of IDAT\n");
		return ERROR_OUT_OF_MEMORY;
	}
	image.plte = malloc((1) * sizeof(unsigned char));
	if (image.plte == NULL)
	{
		fprintf(stderr, "There is not enough memory\n");
		return ERROR_OUT_OF_MEMORY;
	}
	exit = parse_chucks(in, &image);
	if (exit)
	{
		return exit;
	}
	fclose(in);
	FILE *out = fopen(argv[2], "wb");
	if (out == NULL)
	{
		fprintf(stderr, "The output file is invalid\n");
		return ERROR_CANNOT_OPEN_FILE;
	}
	exit = decompress(out, &image);
	if (exit)
	{
		fprintf(stderr, "The decompression ended with error\n");
		return exit;
	}
	free(image.data);
	fclose(out);
	return SUCCESS;
}
