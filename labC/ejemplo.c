#include <stdio.h>

int main() {
    int x = 10;      // Declare an integer
    int *p = &x;     // Declare a pointer 'p' and point it to the address of 'x'
    
    printf("%d /n", *p); // 'Dereference' p to get the value at that address
    
    return 0;
}
