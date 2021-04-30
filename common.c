/* common.c
 *
 * Author: Yangkai Wang 
 * wang_yangkai@163.com
 *
 * Coding in 2014/12/25
 */

#include <stdio.h>

extern void dumptxt(char *buf, int len)
{
	char * p;

	for (p = buf; p < buf + len; p++) {
		if(*p > 31 && *p < 127)
			printf("%c", *p);
		else if(*p == '\t')
			printf("\t");
		else if(*p == 13)
			printf("\\r");
		else if(*p == '\n')
			printf("\\n\n"); /*printf("\\n");*/
		else
			printf("\\%02x", *p);
	}
}

extern void dumptxt_oneline(char *buf, int len)
{
	char * p;

	for (p = buf; p < buf + len; p++) {
		if(*p > 31 && *p < 127)
			printf("%c", *p);
		else if(*p == '\t')
			printf("\t");
		else if(*p == 13)
			printf("\\r");
		else if(*p == '\n')
			printf("\\n");
		else
			printf("\\%02x", *p);
	}
}

extern void dumphex(char *buf, int len)
{
	int i, c;

#define PER_LINE_LEN 32

	while (len > 0) {
		printf("%p: ", buf);
		for (i = 0;  i < PER_LINE_LEN;  i++) {
			if (i < len) {
				printf("%02X ", buf[i] & 0xff);
			} else {
				printf("   ");
			}
			if (i == 7)
				printf(" ");
		}
		printf(" |");
		for (i = 0;  i < PER_LINE_LEN;  i++) {
			if (i < len) {
				c = buf[i] & 0xff;
				if ((c < 0x20) || (c >= 0x7f))
					c = '.';
			} else {
				c = ' ';
			}
			printf("%c", c);
		}
		printf("|\n");
		len -= PER_LINE_LEN;
		buf += PER_LINE_LEN;
	}
}
