#include "headers.h"

int main(int argc, char *argv[])
{
    initClk();

    // TODO implement the scheduler Round Robin:)
    printf("\nThis is RR\n");
    printf("\n%s\n", argv[1]);
    // upon termination release the clock resources

    destroyClk(true);
}
