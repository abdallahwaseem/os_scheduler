#include "headers.h"
#include <time.h>

/* Modify this file as needed*/
int remainingtime;
void handleINT(int);
int main(int agrc, char * argv[])
{
 
    signal(SIGINT,handleINT);
    initClk();
    
    //TODO it needs to get the remaining time from somewhere
    //remainingtime = ??;
    int startClock = getClk();
    remainingtime = atoi(argv[1]);
    //while (remainingtime > 0)
    while(clock() < remainingtime * CLOCKS_PER_SEC)
    {
        // remainingtime = ??;
        //remainingtime = remainingtime + startClock - getClk();
    }
    
    destroyClk(false);
    
    return 0;
}


void handleINT(int segnum)
{
    destroyClk(false);
    exit(getpid());
}

