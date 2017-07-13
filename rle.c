#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

void compress(char *infn, char *outfn)
{
    uint8_t *memory;
    size_t size;
    FILE *inf = fopen(infn, "r");

    if (!inf) {
        fprintf(stderr, "Unable to open input file!\n");
        exit(2);
    }

    fseek(inf, 0, SEEK_END);
    size = ftell(inf);
    rewind(inf);

    memory = malloc(size);

    fread(memory, 1, size, inf);
    fclose(inf);

    FILE *outf = fopen(outfn, "w");
    if (!outf) {
        fprintf(stderr, "Unable to open output file!\n");
        exit(2);
    }

    for (int i = 0; i < size; ) {
        uint8_t count = 1;
        uint8_t byte = memory[i];
        while (memory[++i] == byte && count < 255)
            count++;
        fputc(count, outf);
        fputc(byte, outf);
    }

    fclose(outf);
    free(memory);
}

void decompress(char *infn, char *outfn)
{
    uint8_t *memory;
    size_t size;
    FILE *inf = fopen(infn, "r");

    if (!inf) {
        fprintf(stderr, "Unable to open input file!\n");
        exit(2);
    }

    fseek(inf, 0, SEEK_END);
    size = ftell(inf);
    rewind(inf);

    memory = malloc(size);

    fread(memory, 1, size, inf);
    fclose(inf);

    FILE *outf = fopen(outfn, "w");
    if (!outf) {
        fprintf(stderr, "Unable to open output file!\n");
        exit(2);
    }

    for (int i = 0, j = 1; i < size && j < size; i += 2, j += 2) {
        uint8_t count = memory[i];
        uint8_t byte = memory[j];

        for (int k = 0; k < count; k++)
            fputc(byte, outf);
    }

    fclose(outf);
    free(memory);
}

int main(int argc, char **argv)
{
    if (argc != 4) {
        printf("Usage: %s <OPTION> <INPUT> <OUTPUT>\n", argv[0]);
        printf("Options:\n");
        printf("\tcompress\n");
        printf("\tdecompress\n");
        exit(1);
    }

    char *infn = argv[2];
    char *outfn = argv[3];

    if (strcmp(argv[1], "compress") == 0) {
        compress(infn, outfn);
    } else if (strcmp(argv[1], "decompress") == 0) {
        decompress(infn, outfn);
    } else {
        fprintf(stderr, "No valid option detected.\n");
        exit(1);
    }
}
