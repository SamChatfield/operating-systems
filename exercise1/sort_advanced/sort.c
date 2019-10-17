#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_STRINGS 20

char *getline()
{
	size_t linesize = 0;
	size_t len = 0;
	char *line = NULL;

	do
	{
		linesize += 20;
		// printf("linesize: %d\n", linesize);
		line = realloc(line, linesize);
		// if (fgets(line+last, linesize, stdin) == NULL)
		// 	return line;
		fgets(line + len, 20, stdin);
		len = strlen(line);
		// printf("line: '%s'\n", line);
		// printf("len: %d\n", strlen(line));
	} while (!feof(stdin) && line[len - 1] != '\n');

	return line;
}

int cmpstr(const void *a, const void *b)
{
	const char **str1 = (const char **)a;
	const char **str2 = (const char **)b;
	return strcmp(*str1, *str2);
}

int main(int argc, char **argv)
{
	char *strings[MAX_STRINGS];

	int len, linecount;
	char *sp, *linebuf;

	// Read in the strings from stdin, accounting for case of no newline at the end
	linecount = 0;
	while ((linebuf = getline()) != NULL)
	{
		len = strlen(linebuf);
		if (len == 0)
		{
			free(linebuf);
			break;
		}
		printf("linecount: %d, len: %d, line: '%s'\n", linecount, len, linebuf);
		if (linebuf[len - 1] == '\n')
		{
			// If the line has a newline at the end, replace it with null byte
			linebuf[len - 1] = '\0';
		}
		else
		{
			// If there is no newline at the end, append a null byte and increment line length
			linebuf[len] = '\0';
			len++;
		}
		if (linecount >= MAX_STRINGS || (sp = malloc(len)) == NULL)
		{
			fprintf(stderr, "Error: number of lines exceeded maximum of %d\n", MAX_STRINGS);
			return 1;
		}
		else
		{
			strncpy(sp, linebuf, len);
			strings[linecount++] = sp;
		}
		free(linebuf);
	}

	// Sort the strings using Q-Sort library function
	qsort(strings, linecount, sizeof(const char *), cmpstr);

	// Print the (now sorted) strings
	for (int i = 0; i < linecount; i++)
	{
		printf("%s\n", strings[i]);
		free(strings[i]);
	}

	return 0;
}
