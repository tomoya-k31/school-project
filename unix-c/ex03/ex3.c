#include <stdio.h>
#include <string.h>
#include <limits.h>

void print(unsigned int x);

int main(int argc, char *argv[])
{
  printf("CHAR_BIT = %d\n",CHAR_BIT);
  printf("%d\n",65000);
  print(65000);
  printf("\n");
}

void print(unsigned int x)
{
  int nbits = sizeof(unsigned int)*8;
  unsigned int msb = 1 << (nbits - 1);
  for (int i = 1; i <= nbits; i++) {
    /* ここに 何か書く */
    
    //x&msb ? printf("1") : printf("0");
    int y = x&msb ? 1 : 0 ;
    printf("%d",y);

    if (i % 8 == 0) putchar(' ');
    x <<= 1; /* xを1ビット左シフト */
  }
}
