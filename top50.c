#include <stdio.h>      /* printf */
#include <stdlib.h>     /* qsort */
#include <string.h>

struct EPVal
{
	double a;
	char b;
}aa[6];

static int compareIt (const void * a, const void * b)
{
	struct EPVal *ia = (struct EPVal *)a;
	struct EPVal *ib = (struct EPVal *)b;

  if (ia->a > ib->a) return -1;
  else if (ia->a < ib->a) return 1;
  else return 0;  
}

char* substr(const char *src, int m, int n)
{
	int len = n - m;

	char *dest = (char*)malloc(sizeof(char) * (len + 1));

	for (int i = m; i < n && (*src != '\0'); i++)
	{
		*dest = *(src + i);
		dest++;
	}

	*dest = '\0';

	return dest - len;
}

int l = 4;
int checkSubstring(char *text, int i1, int i2) {
	char *str1 = substr(text, i1 - l + 1, i1 + 1);
	char *str2 = substr(text, i2 - l + 1, i2 + 1);
	int cmp = ( strcmp(str1, str2) == 0 );
	return cmp;
}

int main ()
{
	char *txt = "Helloguysllogarithm";

	printf("%d\n", checkSubstring(txt, 5, 11));

}