#include "headers.h"
void handler(int signum);

int main(int argc, char *argv[])
{
    initClk();
    signal(SIGINT, handler);
    // TODO implement the scheduler High Priority First:)
    printf("\nThis is HPF\n");
    // upon termination release the clock resources
    //destroyClk(true);
    while(1)
    {

    }
    
}

void handler(int signum)
{
    destroyClk(true);
    exit(getpid());
}