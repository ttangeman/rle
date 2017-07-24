#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

typedef struct
{
    size_t size;
    uint8_t *memory;
} mm_file;

mm_file mmap_file(FILE *f)
{
    if (!f) {
        fprintf(stderr, "Unable to open file!\n");
        fclose(f);
        exit(2);
    }

    mm_file result = {0};

    fseek(f, 0, SEEK_END);
    result.size = ftell(f);
    rewind(f);

    result.memory = (uint8_t *) malloc(result.size);

    fread(result.memory, 1, result.size, f);
    fclose(f);

    return(result);
}

void encode(char *infn, char *outfn)
{
    FILE *inf = fopen(infn, "r");
    mm_file m = mmap_file(inf);

    size_t size = m.size;
    uint8_t *memory = m.memory;

    FILE *outf = fopen(outfn, "w");

    for (int i = 0; i < size; ) {
        uint8_t count = 1;
        uint8_t byte = memory[i];
        while (memory[++i] == byte && count < 255)
            count++;
        fputc(count, outf);
        fputc(byte, outf);
    }

    fclose(outf);
    free(m.memory);
}

void decode(char *infn, char *outfn)
{
    FILE *inf = fopen(infn, "r");
    mm_file m = mmap_file(inf);

    size_t size = m.size;
    uint8_t *memory = m.memory;
    uint8_t *outmem = malloc(255);
    uint8_t *base = outmem;

    FILE *outf = fopen(outfn, "w");

    for (int i = 0, j = 1; i < size && j < size; i += 2, j += 2) {
        uint8_t count = memory[i];
        uint8_t byte = memory[j];
        memset(outmem, byte, count);
        fwrite(outmem, 1, count, outf);
        outmem = base;
    }

    fclose(outf);
    free(m.memory);
    free(outmem);
}

int main(int argc, char **argv)
{
    if (argc != 4) {
        printf("Usage: %s [OPTION] [INPUT] [OUTPUT]\n", argv[0]);
        printf("Options:\n");
        printf("\tencode\n");
        printf("\tdecode\n");
        exit(1);
    }

    char *infn = argv[2];
    char *outfn = argv[3];

    if (strcmp(argv[1], "encode") == 0) {
        encode(infn, outfn);
    } else if (strcmp(argv[1], "decode") == 0) {
        decode(infn, outfn);
    } else {
        fprintf(stderr, "No valid option detected.\n");
        exit(1);
    }
}
