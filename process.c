#include "headers.h"

/* Modify this file as needed*/
int remainingtime;

int main(int agrc, char * argv[])
{
    initClk();
    
    //TODO it needs to get the remaining time from somewhere
    //remainingtime = ??;
    int startClock = getClk();
    remainingtime = atoi(argv[1]);
    while (remainingtime > 0)
    {
        // remainingtime = ??;
        remainingtime = remainingtime + startClock - getClk();
    }
    
    destroyClk(false);
    
    return 0;
}
