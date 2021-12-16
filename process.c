#include "headers.h"
#include <time.h>

/* Modify this file as needed*/
int remainingtime;
void handleINT(int);
int main(int agrc, char *argv[])
{

    signal(SIGINT, handleINT);
    initClk();

    // TODO it needs to get the remaining time from somewhere
    // remainingtime = ??;
    int startClock = getClk();
    int Quantum = 0;
    remainingtime = atoi(argv[1]);
    if (agrc > 2)
    {
        Quantum = atoi(argv[2]);
    }

    // while (remainingtime > 0)
    while (clock() < remainingtime * CLOCKS_PER_SEC)
    {
        int runtime = getClk() - startClock;
        if (agrc > 2 && ((remainingtime < Quantum && runtime == remainingtime) || (Quantum == runtime)))
        {
            raise(SIGINT);
        }

        // remainingtime = ??;
        // remainingtime = remainingtime + startClock - getClk();
    }

    destroyClk(false);

    return 0;
}

void handleINT(int segnum)
{

    destroyClk(false);
    exit(getpid());
}
