#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char *getline()
{
	size_t linesize = 0;
	size_t len = 0;
	char *line = NULL;

	do {
		linesize += 20;
		line = (char *) realloc(line, linesize * sizeof(char));
		if (!line) exit(1);

		if (fgets(line+len, 20, stdin) == NULL) {
			free(line);
			return NULL;
		}

		len = strlen(line);
	} while (!feof(stdin) && line[len-1] != '\n');

	return line;
}

int cmpstr(const void *a, const void *b)
{
	const char **str1 = (const char **) a;
	const char **str2 = (const char **) b;
	return strcmp(*str1, *str2);
}

int main(int argc, char **argv)
{
	char **strings = NULL;

	int len;
	char *sp, *linebuf;
	size_t linesmax = 0;
	size_t linecount = 0;

	// Read in the strings from stdin, accounting for case of no newline at the end
	while ((linebuf = getline()) != NULL) {
		len = strlen(linebuf);
		if (len == 0) {
			free(linebuf);
			break;
		}
		if (linebuf[len-1] == '\n') {
			// If the line has a newline at the end, replace it with null byte
			linebuf[len-1] = '\0';
		} else {
			// If there is no newline at the end, append a null byte and increment line length
			linebuf[len++] = '\0';
		}

		sp = (char *) malloc(len * sizeof(char));
		if (!sp) return 1;

		if (linecount == linesmax) {
			linesmax += 20;
			strings = (char **) realloc(strings, linesmax * sizeof(char *));
			if (!strings) return 1;
		}

		strncpy(sp, linebuf, len);
		strings[linecount++] = sp;

		free(linebuf);
	}

	// Sort the strings using Q-Sort library function
	qsort(strings, linecount, sizeof(char *), cmpstr);

	// Print the (now sorted) strings
	for (int i = 0; i < linecount; i++) {
		printf("%s\n", strings[i]);
		free(strings[i]);
	}

	free(strings);

	return 0;
}
