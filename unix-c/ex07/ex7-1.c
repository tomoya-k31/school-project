#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	char* fn;
	if(scanf("%ms",&fn)<=0){perror("scanf");return 1;}
	FILE* fp = fopen(fn, "w");
	free(fn); 
	if (fp == NULL) { perror("fopen"); return 1; }
	int i=1;
	while(i<=10)
		fprintf(fp,"%d ",i++);
	fprintf(fp,"\n");
	if (fclose(fp) == EOF) { perror("fclose"); return 1; }
	return 0;
}

