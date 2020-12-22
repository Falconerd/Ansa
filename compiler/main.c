#include <inttypes.h>
#include "./lexer.c"

uint8_t *src = NULL;

int main(int argc, const char* argv[]) {
	//if (argc < 2) return -1;
	puts("Compiler: Let us try and open the provided file and put it into a buffer...");
	//FILE *f = fopen(argv[1], "rb");
	FILE *f = fopen("./test.ans", "rb");
	size_t bufsize;
	if (NULL != f) {
		if (fseek(f, 0L, SEEK_END) == 0) {
			bufsize = ftell(f);
			if (-1 == bufsize) {
				fputs("Error calculating file length with ftell", stderr);
			}

			src = malloc(sizeof(uint8_t) * (bufsize + 1));

			if (fseek(f, 0L, SEEK_SET) != 0) {
				fputs("Error seeking to start of file", stderr);
			}

			size_t new_len = fread(src, sizeof(uint8_t), bufsize, f);
			if (ferror(f) != 0) {
				fputs("Error reading file", stderr);
			} else {
				src[new_len++] = '\0';
			}
		}
		fclose(f);
	}

	printf("Fantastic. We have a file with a length of %lu. The first character is '%c' and the last character is '%c'.\n", strlen(src), src[0], src[strlen(src)-2]);

	puts("Now let us run the lexer on it and see what we can make of it.");

	lex(src, bufsize);
}
