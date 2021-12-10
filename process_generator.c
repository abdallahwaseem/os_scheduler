#include "headers.h"
#include <unistd.h>
#include <string.h>
void clearResources(int);
void ReadFile(struct Queue *q);

int main(int argc, char *argv[])
{
    signal(SIGINT, clearResources);
    // TODO Initialization
    // 1. Read the input files.
    struct Queue *All_Processes = createQueue();
    ReadFile(All_Processes);
    // 2. Ask the user for the chosen scheduling algorithm and its parameters, if there are any.
    printf("\nPlease enter scheduling algorithm : \n1)HPF\n2)SRTN\n3)RR)\n ");
    int algo;
    char quantum[10];
    scanf("%d", &algo);
    if (algo == 3)
    {
        // Round Robin
        printf("\nPlease enter Quantum: \n");
        scanf("%s", quantum);
    }
    char *argve[] = {"schedulerRR.out", quantum, NULL};
    // 3. Initiate and create the scheduler and clock processes.
    int pid = fork();
    if (pid == 0)
    {
        switch (algo)
        {
        case 1:
            // Non-preemptive Highest Priority First
            execve("schedulerHPF.out", argv, NULL);
            break;
        case 2:
            execve("schedulerSRTN.out", argv, NULL);
            // Shortest Remaining time Next
            break;
        case 3:
            // Round Robin
            execve(argve[0], &argve[0], NULL);
            break;

        default:
            break;
        }
    }
    // 4. Use this function after creating the clock process to initialize clock
    initClk();
    // To get time use this
    int x = getClk();
    printf("current time is %d\n", x);
    // TODO Generation Main Loop
    // 5. Create a data structure for processes and provide it with its parameters.
    // 6. Send the information to the scheduler at the appropriate time.
    // 7. Clear clock resources
    destroyClk(true);
}

void clearResources(int signum)
{
    // TODO Clears all resources in case of interruption

    // TODO Clears all resources in case of interruption
}

void ReadFile(struct Queue *q)
{
    char *filename = "processes.txt";
    FILE *fp = fopen(filename, "r");

    if (fp == NULL)
    {
        printf("Error: could not open file %s", filename);
        return;
    }

    // reading line by line, max 256 bytes
    const unsigned MAX_LENGTH = 256;
    char buffer[MAX_LENGTH];

    while (fgets(buffer, MAX_LENGTH, fp))
    {
        if (buffer[0] != '#')
        {
            processData temp;
            int processBuffer[4];
            int index = 0;
            char *token = strtok(buffer, "\t");
            // Keep printing tokens while one of the
            // delimiters present in str[].
            while (token != NULL)
            {
                processBuffer[index] = atoi(token);
                token = strtok(NULL, "\t");
                index++;
            }
            temp.id = processBuffer[0];
            temp.arrivaltime = processBuffer[1];
            temp.runningtime = processBuffer[2];
            temp.priority = processBuffer[3];
            enQueue(q, temp);
        }
    }
    // close the file
    fclose(fp);
}