#include "head.h"
#include "stdio.h"

int main(int argc, char* argv[])
{
	printf("is test\n");
	int a=23,b=2;
	printf("%d + %d = %d\n",a, b, add(a,b));
	return 0;
}
