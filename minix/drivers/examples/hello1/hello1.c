#include <stdio.h>
#include <stdlib.h>
#include <minix/syslib.h>
 
int main(int argc, char **argv)
{
    sef_startup();
 
    endpoint_t et;
    unsigned flag;

    int r = sys_sample( flag, et);
    
    printf("Hello, World!\n");
    return EXIT_SUCCESS;
}