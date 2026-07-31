/* SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: Copyright (C) 2026 Riversoft Studios */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int is_empty_or_whitespace(const char* str)
{
	while (*str) {
		if (!isspace((unsigned char)*str)) {
			return 0;
		}
		str++;
	}
	return 1;
}

int main(void)
{
	FILE* file = fopen(".config", "r");

	if (file == NULL) {
		perror("Error opening .config file");
		return EXIT_FAILURE;
	}

	char line[BUFSIZ];

	while (fgets(line, sizeof(line), file)) {
		line[strcspn(line, "\r\n")] = '\0';

		if (is_empty_or_whitespace(line)) {
			continue;
		}
		if (line[0] == '#') {
			if (line[1] != '\0') {
				printf("/*%s */\n", &line[1]);
			}
		} else {
			char* equals_sign = strchr(line, '=');

			if (equals_sign != NULL) {
				*equals_sign = '\0';
				char* name = line;
				char* option = equals_sign + 1;

				printf("#define %s %s\n", name, option);
			}
		}
	}

	fclose(file);
	return EXIT_SUCCESS;
}
