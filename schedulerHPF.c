#include "headers.h"
void handler(int signum);

int runningProccessRuntime;

void sigalrm_handler(int signum)
{
    printf("\ninside handler\n");
    runningProccessRuntime = 0;
}

int main(int argc, char *argv[])
{
    initClk();
    signal(SIGINT, handler);

    signal(SIGALRM, sigalrm_handler);
    // Initialize message queue
    key_t key_id = ftok("file_hayet3emel", 66);
    int msgqid = msgget(key_id, 0644);
    struct msgBuff message;

    // For the first process received
    int rec_val;
    rec_val = msgrcv(msgqid, &message, sizeof(message.mtext), 0, !IPC_NOWAIT);
    NodePQ *pq = newNodePQ(message.Data, message.Data.priority);
    runningProccessRuntime = pq->data.remainingtime;

    char runProcesstimeChar[10];
    sprintf(runProcesstimeChar, "%d", runningProccessRuntime);
    char *argve[] = {"process.out", runProcesstimeChar, NULL};
    int pid = fork();
    if (pid == 0)
    {
        execve(argve[0], &argve[0], NULL);
    }
    alarm(runningProccessRuntime);
    pause();

    // For the rest of the processes
    while (1)
    {
        rec_val = msgrcv(msgqid, &message, sizeof(message.mtext), 0, !IPC_NOWAIT);
        if (rec_val == -1)
        {
            perror("Error in receive");
        }
        pushPQ(pq, message.Data, message.Data.priority);
        if (message.count != 0)
        {
            for (int i = 0; i < message.count; i++)
            {
                rec_val = msgrcv(msgqid, &message, sizeof(message.mtext), 0, !IPC_NOWAIT);
                pushPQ(pq, message.Data, message.Data.priority);
            }
        }
        // TODO Get from the process generator

        if (runningProccessRuntime == 0)
        {
            processCreator();
            // runningProccessRuntime = pq->data.remainingtime;
            // argve[] = {"process.out", runProcesstimeChar, NULL};
            // sprintf(runProcesstimeChar, "%d", runningProccessRuntime);
            // char *argve[] = {"process.out", runProcesstimeChar, NULL};
            // int pid = fork();
            // if (pid == 0)
            // {
            //     execve(argve[0], &argve[0], NULL);
            // }
            // popPQ(pq);
            // alarm(runningProccessRuntime);
            // pause();
        }
        else
        {
            // dequeue from ready queue

            // add in shared memory

            // run process
        }
    }

    printf("\nThis is HPF\n");
    // upon termination release the clock resources
    printf("\nAbout to destroy the shared memory area !\n");
    destroyClk(true);
}

void handler(int signum)
{
    destroyClk(true);
    exit(getpid());
}

void processCreator()
{
    runningProccessRuntime = pq->data.remainingtime;
    argve[] = {"process.out", runProcesstimeChar, NULL};
    sprintf(runProcesstimeChar, "%d", runningProccessRuntime);
    char *argve[] = {"process.out", runProcesstimeChar, NULL};
    int pid = fork();
    if (pid == 0)
    {
        execve(argve[0], &argve[0], NULL);
    }
    popPQ(pq);
    alarm(runningProccessRuntime);
    pause();
}