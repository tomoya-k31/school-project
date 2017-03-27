#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
	char s[16];
	while (fgets(s, sizeof(s), stdin) != NULL) {
		/* ここに追加*/
		char *ret = strchr(s,'\n');
		if(ret) *ret = '\0';
		fputs(s, stdout);
	}
	return 0;
}
