#include "headers.h"
void handlerINT(int signum);
void handlerCHLD(int signum);
void handlerALRM(int signum);
FILE *CreateFileAndOpen();

int msgq_id;
int runningProccessRuntime;
struct QueuePQ *ReadyProccessQueue;
int isFirstChild = true;
FILE *OutputFile;
processData CurrentProcess;

int main(int argc, char *argv[])
{
    initClk();
    //HandlerINT to kill the scheduler after process generator finishes
    signal(SIGINT, handlerINT);
    //handlerCHLD after the process is completed
    signal(SIGCHLD, handlerCHLD);
    //THe process generator will send this signal to wake the scheduler to add processes to the readyQueue
    signal(SIGALRM, handlerALRM);

    //signal(SIGALRM, sigalrm_handlerINT);

    OutputFile = CreateFileAndOpen();
    // Initialize message queue
    msgq_id = CreateMsgQueueIPC();
    //Create The PQ for processes
    ReadyProccessQueue = createQueuePQ();
    ///////////////////////////////////////
    //////Will be needed for process Generator
    // char runProcesstimeChar[10];
    // sprintf(runProcesstimeChar, "%d", runningProccessRuntime);
    // char *argve[] = {"process.out", runProcesstimeChar, NULL};
    // int pid = fork();
    // if (pid == 0)
    // {
    //     execve(argve[0], &argve[0], NULL);
    // }
    // alarm(runningProccessRuntime);
    // pause();
    /////////////////////////////////////////////

    // For the rest of the processes

    while (1)
    {
        pause();
    }
}

void processCreator()
{
    //runningProccessRuntime = pq->data.remainingtime;
    if (isEmptyQueuePQ(ReadyProccessQueue))
        return;
    processData top = peekQueuePQ(ReadyProccessQueue);
    CurrentProcess = top;
    char runProcesstimeChar[10];
    sprintf(runProcesstimeChar, "%d", top.remainingtime);
    char *argve[] = {"process.out", runProcesstimeChar, NULL};
    int pid = fork();
    if (pid == 0)
    {
        execve(argve[0], &argve[0], NULL);
    }
    deQueuePQ(ReadyProccessQueue);
    //Print in scheduler
    //HPF only
    CurrentProcess.waitingtime = getClk() - CurrentProcess.arrivaltime;
    fprintf(OutputFile, "#At time %d process %d started arr %d total %d remain %d wait %d\n", getClk(), CurrentProcess.id, CurrentProcess.arrivaltime, CurrentProcess.runningtime, CurrentProcess.remainingtime, CurrentProcess.waitingtime);
}

void handlerINT(int signum)
{
    destroyClk(true);
    fclose(OutputFile);
    exit(getpid());
}

void handlerCHLD(int signum)
{
    fprintf(OutputFile, "#At time %d process %d Finished arr %d total %d remain 0 wait %d\n", getClk(), CurrentProcess.id, CurrentProcess.arrivaltime, CurrentProcess.runningtime, CurrentProcess.waitingtime);
    processCreator();
}

void handlerALRM(int signum)
{
    //Always Sleep bas haysah be signal, the signal SGNCHIL or signal men the process_generator an fe had 3ala the queue
    msgBuff message;
    printf("\n Sahyt \n");
    int rec_val = msgrcv(msgq_id, &message, sizeof(message.Data), 0, IPC_NOWAIT);
    while (rec_val != -1)
    {
        enQueuePQ(ReadyProccessQueue, message.Data, message.Data.priority);
        rec_val = msgrcv(msgq_id, &message, sizeof(message.Data), 0, IPC_NOWAIT);
    }
    if (isFirstChild)
    {
        //Creater Process
        isFirstChild = false;
        processCreator();
    }
}

FILE *CreateFileAndOpen()
{
    char *filename = "processOutput.txt";

    // open the file for writing
    FILE *fp = fopen(filename, "w");
    if (fp == NULL)
    {
        printf("Error opening the file %s", filename);
        return NULL;
    }
    // // write to the text file
    // for (int i = 0; i < 10; i++)
    //     fprintf(fp, "This is the line #%d\n", i + 1);

    // // close the file
    // fclose(fp);
    fprintf(fp, "#At time x process y state arr w total z remain y wait k\n");
    return fp;
}