#include <stdio.h>
#include "getline.h"

int main()
/* 
 * just echo user input lines, letting user edit them and move through
 * history list
 */
{
    char *p;

	do
	{
		p = getlineplus("jimosql> ", "");
		gl_histadd(p);
		fputs(p, stdout);

		if (!strncmp(p, "exit", 4)) break;
	}
	while (*p != 0);

	return 0;
}
