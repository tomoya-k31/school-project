#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

int main(int argc, char *argv[])
{
	if (argc != 3) {
		fprintf(stderr, "usage:%s source destination\n", argv[0]);
		return 1;	
	}
	FILE* fin = fopen(argv[1],"r");
	if(!fin){perror("fopen");return 1;}
	FILE* fout = fopen(argv[2],"w");
	if(!fout){perror("fopen");return 1;}
	int c;
	while((c = fgetc(fin)) != EOF){
		fputc(c,fout);
	}
	if (fclose(fin) == EOF) { perror("fclose"); return 1; }
	if (fclose(fout) == EOF) { perror("fclose"); return 1; }
	return 0;
}
