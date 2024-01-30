#include <stdio.h>
#include <stdlib.h>
#include <minix/syslib.h>
 
int main(int argc, char **argv)
{
    sef_startup();
 
    endpoint_t ept;
    unsigned flags;

    int r = sys_sample(flags, ept);
    
    printf("Hello, World %d!\n", r);
    return EXIT_SUCCESS;
}