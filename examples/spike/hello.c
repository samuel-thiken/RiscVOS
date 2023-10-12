#include <stdio.h>


int main(void) {
	int a=42;
	a++;
	printf("Hello words! %d\n", a);
	if (a==43)
		return 0;
	else
		return -1;
}
