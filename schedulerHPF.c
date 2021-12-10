#include "headers.h"

int main(int argc, char *argv[])
{
    initClk();

    // TODO implement the scheduler High Priority First:)
    printf("\nThis is HPF\n");
    // upon termination release the clock resources

    destroyClk(true);
}
