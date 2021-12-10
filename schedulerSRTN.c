#include "headers.h"

int main(int argc, char *argv[])
{
    initClk();

    // TODO implement the scheduler Shortest Remaining Time Next:)
    printf("\nThis is SRTN\n");

    // upon termination release the clock resources

    destroyClk(true);
}
