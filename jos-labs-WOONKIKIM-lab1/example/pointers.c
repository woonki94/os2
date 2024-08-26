#include <stdio.h>
#include <stdlib.h>

void
f(void)
{
    int a[4]; //initiallizing array which has same meaning as int *a, memory allocated in the stack
    int *b = malloc(16); // Allocating memory using malloc, in heap
    int *c; // pointer not initialized
    int i;
    

    printf("1: a = %p, b = %p, c = %p\n", a, b, c); //prints out physical memory of each pointer

    c = a; //allocating same physical memory of a to c
    for (i = 0; i < 4; i++)
	a[i] = 100 + i; // putting values in memory. a[0] = 100, a[1] = 101, a[2] = 102, a[2] = 103
    c[0] = 200; // putting values in c[0] which is same as a[0]. a[0] = c[0] = 200
    printf("2: a[0] = %d, a[1] = %d, a[2] = %d, a[3] = %d\n",
	   a[0], a[1], a[2], a[3]);

    c[1] = 300; // c[1] = a[1] = 300
    *(c + 2) = 301;// *(c + 2) = c[2] = a[2] = 301
    3[c] = 302; // 3[c] = *(3 + c) = *(3 + c) = c[3] = a[3] = 302
    printf("3: a[0] = %d, a[1] = %d, a[2] = %d, a[3] = %d\n",
    a[0], a[1], a[2], a[3]);

    c = c + 1; 
    *c = 400;
    printf("4: a[0] = %d, a[1] = %d, a[2] = %d, a[3] = %d\n",
	   a[0], a[1], a[2], a[3]);

    c = (int *) ((char *) c + 1);
    *c = 500;
    printf("5: a[0] = %d, a[1] = %d, a[2] = %d, a[3] = %d\n",
	   a[0], a[1], a[2], a[3]);

    b = (int *) a + 1;
    c = (int *) ((char *) a + 1);
    printf("6: a = %p, b = %p, c = %p\n", a, b, c);
}

int
main(int ac, char **av)
{
    f();
    return 0;
}
