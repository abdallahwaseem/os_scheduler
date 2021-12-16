#include "headers.h"
#include <time.h>
void EndScheduler();
void handlerCHLD(int signum);
void handlerALRM(int signum);
FILE *CreateFileAndOpen();

int msgq_id;
int runningProccessRuntime;
struct Queue *ReadyProccessQueue;
int isFirstChild = true;
FILE *OutputFile;
processData CurrentProcess;
int currentProcessID;
int ProcessStartAt;
int isFinished = 0;
int Quantum = 0;

int main(int argc, char *argv[])
{
    Quantum = strtol(argv[1], NULL, 10);
    initClk();
    // HandlerINT to kill the scheduler after process generator finishes
    // signal(SIGINT, handlerINT);
    // handlerCHLD after the process is completed
    signal(SIGCHLD, handlerCHLD);
    // THe process generator will send this signal to wake the scheduler to add processes to the readyQueue
    signal(SIGALRM, handlerALRM);

    // signal(SIGALRM, sigalrm_handlerINT);

    OutputFile = CreateFileAndOpen();
    // Initialize message queue
    msgq_id = CreateMsgQueueIPC();
    // Create The PQ for processes

    ReadyProccessQueue = createQueue();
    // For the rest of the processes

    while (1)
    {
        pause();
    }
}

void processCreator()
{
    // runningProccessRuntime = pq->data.remainingtime;
    if (isEmptyQueue(ReadyProccessQueue))
        return;
    processData top = peekQueue(ReadyProccessQueue);
    CurrentProcess = top;
    char runProcesstimeChar[10];
    sprintf(runProcesstimeChar, "%d", top.remainingtime);
    char Quant[10];
    sprintf(Quant, "%d", Quantum);
    char *argve[] = {"process.out", runProcesstimeChar, Quant, NULL};
    // printf("\n New Process is created\n");
    ProcessStartAt = getClk();
    int pid = fork();
    currentProcessID = pid;
    if (pid == 0)
    {
        execve(argve[0], &argve[0], NULL);
    }
    deQueue(ReadyProccessQueue);
    // Print in scheduler
    // HPF only
    if (CurrentProcess.status == paused)
    {
        CurrentProcess.waitingtime = CurrentProcess.waitingtime + getClk() - CurrentProcess.Pausedattime;
        fprintf(OutputFile, "At time %d process %d Resumed arr %d total %d remain %d wait %d\n", getClk(), CurrentProcess.id, CurrentProcess.arrivaltime, CurrentProcess.runningtime, CurrentProcess.remainingtime, CurrentProcess.waitingtime);
    }
    else
    {
        CurrentProcess.waitingtime = getClk() - CurrentProcess.arrivaltime;
        fprintf(OutputFile, "At time %d process %d started arr %d total %d remain %d wait %d\n", getClk(), CurrentProcess.id, CurrentProcess.arrivaltime, CurrentProcess.runningtime, CurrentProcess.remainingtime, CurrentProcess.waitingtime);
    }
    CurrentProcess.status = running;
}

void handlerCHLD(int signum)
{
    int pid, status;
    pid = wait(&status);
    // printf("\n process killed , %d\n",pid);
    if (WIFEXITED(status))
    {
        int currentTime = getClk();
        CurrentProcess.remainingtime = CurrentProcess.remainingtime - (currentTime - ProcessStartAt);
        if (CurrentProcess.remainingtime == 0)
        {
            int TA = currentTime - CurrentProcess.arrivaltime;
            float WTA = (float)TA / CurrentProcess.runningtime;
            fprintf(OutputFile, "At time %d process %d Finished arr %d total %d remain 0 wait %d  TA %d  WTA %.2f\n", currentTime, CurrentProcess.id, CurrentProcess.arrivaltime, CurrentProcess.runningtime, CurrentProcess.waitingtime, TA, WTA);
        }
        else
        {
            fprintf(OutputFile, "At time %d process %d Stopped arr %d total %d remain %d wait %d\n", currentTime, CurrentProcess.id, CurrentProcess.arrivaltime, CurrentProcess.runningtime, CurrentProcess.remainingtime, CurrentProcess.waitingtime);
            CurrentProcess.Pausedattime = currentTime;
            CurrentProcess.status = paused;
            enQueue(ReadyProccessQueue, CurrentProcess);
        }

        if (isEmptyQueue(ReadyProccessQueue) && isFinished == 1)
            EndScheduler();

        processCreator();
    }
    signal(SIGCHLD, handlerCHLD);
}

void EndScheduler()
{
    destroyClk(false);
    fclose(OutputFile);
    exit(getpid());
}

void handlerALRM(int signum)
{
    // Always Sleep bas haysah be signal, the signal SGNCHIL or signal men the process_generator an fe had 3ala the queue
    msgBuff message;
    // printf("\n Sahyt \n");
    int rec_val = msgrcv(msgq_id, &message, sizeof(message.Data), 0, IPC_NOWAIT);
    while (rec_val != -1)
    {
        if (message.Data.status != lastProcess)
        {

            enQueue(ReadyProccessQueue, message.Data);
        }
        else
        {
            isFinished = 1;
        }
        rec_val = msgrcv(msgq_id, &message, sizeof(message.Data), 0, IPC_NOWAIT);
    }

    if (isFirstChild)
    {
        // Creater Process
        isFirstChild = false;
        processCreator();
    }
    // Check if the the process that came is better than the current running

    signal(SIGALRM, handlerALRM);
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