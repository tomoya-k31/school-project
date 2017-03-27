#include <stdio.h>

int mystrcmp(const char *s1, const char *s2)
{
  while(*s1 == *s2)
    {
      if (*s1 == '\0'){
	return 0;
      }
      s1++;s2++;
    }
  return *s1 - *s2;    
}

int main(int argc, char *argv[])
{
  printf("%d\n", mystrcmp("abc", "abc")); /* 0 */
  printf("%d\n", mystrcmp("aa", "abc")); /*
					   負
					 */
  printf("%d\n", mystrcmp("ab", "abc")); /*
					   負
					 */
  printf("%d\n", mystrcmp("ac", "abc")); /*
					   正
					 */
  printf("%d\n", mystrcmp("", ""));
  /* 0 */
}
