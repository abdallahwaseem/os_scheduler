#include "headers.h"
#include <time.h>
void EndScheduler();
void handlerCHLD(int signum);
void handlerALRM(int signum);
FILE *CreateFileAndOpen();
FILE *CreatePerfFileAndOpen();

int msgq_id;
int runningProccessRuntime;
struct QueuePQ *ReadyProccessQueue;
int isFirstChild = true;
FILE *OutputFile;
FILE *OutputFilePref;
processData CurrentProcess;
int currentProcessID;
int ProcessStartAt;
int isFinished = 0;
int noOfProcesses = 0;
float sumWTA = 0.0;
float sumWait = 0.0;
float StdWTA = 0.0;
int totalRunningTime = 0;
float sumWTASQ = 0;

int main(int argc, char *argv[])
{
    noOfProcesses = atoi(argv[1]);

    initClk();
    // HandlerINT to kill the scheduler after process generator finishes
    // signal(SIGINT, handlerINT);
    // handlerCHLD after the process is completed
    signal(SIGCHLD, handlerCHLD);
    // THe process generator will send this signal to wake the scheduler to add processes to the readyQueue
    signal(SIGALRM, handlerALRM);

    // signal(SIGALRM, sigalrm_handlerINT);

    OutputFile = CreateFileAndOpen();
    OutputFilePref = CreatePerfFileAndOpen();

    // Initialize message queue
    msgq_id = CreateMsgQueueIPC();
    // Create The PQ for processes

    ReadyProccessQueue = createQueuePQ();
    // For the rest of the processes

    while (1)
    {
        pause();
    }
}

void processCreator()
{
    // runningProccessRuntime = pq->data.remainingtime;
    if (isEmptyQueuePQ(ReadyProccessQueue))
        return;
    processData top = peekQueuePQ(ReadyProccessQueue);
    CurrentProcess = top;
    char runProcesstimeChar[10];
    sprintf(runProcesstimeChar, "%d", top.remainingtime);
    char *argve[] = {"process.out", runProcesstimeChar, NULL};
    // printf("\n New Process is created\n");
    ProcessStartAt = getClk();
    int pid = fork();
    currentProcessID = pid;
    if (pid == 0)
    {
        execve(argve[0], &argve[0], NULL);
    }
    deQueuePQ(ReadyProccessQueue);
    // Print in scheduler
    // HPF only
    if (CurrentProcess.status == paused)
    {
        CurrentProcess.waitingtime = CurrentProcess.waitingtime + getClk() - CurrentProcess.Pausedattime;
    }
    else
    {
        CurrentProcess.waitingtime = getClk() - CurrentProcess.arrivaltime;
    }

    if (CurrentProcess.status == paused)
    {
        fprintf(OutputFile, "At time %d process %d Resumed arr %d total %d remain %d wait %d\n", getClk(), CurrentProcess.id, CurrentProcess.arrivaltime, CurrentProcess.runningtime, CurrentProcess.remainingtime, CurrentProcess.waitingtime);
    }
    else
        fprintf(OutputFile, "At time %d process %d started arr %d total %d remain %d wait %d\n", getClk(), CurrentProcess.id, CurrentProcess.arrivaltime, CurrentProcess.runningtime, CurrentProcess.remainingtime, CurrentProcess.waitingtime);
    CurrentProcess.status = running;
}

void handlerCHLD(int signum)
{
    int pid, status;
    pid = wait(&status);
    // printf("\n process killed , %d\n",pid);
    if (WIFEXITED(status))
    {
        if (CurrentProcess.status != paused)
        {
            int TA = getClk() - CurrentProcess.arrivaltime;
            float WTA = (float)TA / CurrentProcess.runningtime;
            fprintf(OutputFile, "At time %d process %d Finished arr %d total %d remain 0 wait %d  TA %d  WTA %.2f\n", getClk(), CurrentProcess.id, CurrentProcess.arrivaltime, CurrentProcess.runningtime, CurrentProcess.waitingtime, TA, WTA);
            totalRunningTime += CurrentProcess.runningtime;
            sumWTA += WTA;
            sumWait += CurrentProcess.waitingtime;
            sumWTASQ += pow(WTA, 2);
        }

        if (peekQueuePQ(ReadyProccessQueue).status == lastProcess)
        {
            int finishTime = getClk();
            printf("\n wta squared = %f  sum wta all squared = %f\n", sumWTASQ, sumWTA);
            float CPUUti = (float)((totalRunningTime)*100) / finishTime;
            float avgWait = sumWait / noOfProcesses;
            float avgWTA = sumWTA / noOfProcesses;
            float sumofWTAsuqared = pow(sumWTA, 2) / pow(noOfProcesses, 2);
            StdWTA = sqrt((sumWTASQ / noOfProcesses) - sumofWTAsuqared);
            fprintf(OutputFilePref, "CPU utilization = %.1f %%\nAvg WTA= %.2f\nAvg Waiting = %.2f\nStd WTA = %.2f", CPUUti, avgWTA, avgWait, StdWTA);
            EndScheduler();
        }
        processCreator();
    }
    signal(SIGCHLD, handlerCHLD);
}

void EndScheduler()
{
    destroyClk(false);
    fclose(OutputFile);
    fclose(OutputFilePref);
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
        enQueuePQ(ReadyProccessQueue, message.Data, message.Data.remainingtime);
        rec_val = msgrcv(msgq_id, &message, sizeof(message.Data), 0, IPC_NOWAIT);
    }

    if (isFirstChild)
    {
        // Creater Process
        isFirstChild = false;
        processCreator();
    }
    // Check if the the process that came is better than the current running
    else
    {
        processData Top = peekQueuePQ(ReadyProccessQueue);
        int ProcessNewRemainingTime = CurrentProcess.remainingtime - (getClk() - ProcessStartAt);
        if (Top.remainingtime < ProcessNewRemainingTime)
        {
            fprintf(OutputFile, "At time %d process %d Stopped arr %d total %d remain %d wait %d\n", getClk(), CurrentProcess.id, CurrentProcess.arrivaltime, CurrentProcess.runningtime, ProcessNewRemainingTime, CurrentProcess.waitingtime);
            CurrentProcess.remainingtime = ProcessNewRemainingTime;
            CurrentProcess.Pausedattime = getClk();
            CurrentProcess.status = paused;
            enQueuePQ(ReadyProccessQueue, CurrentProcess, CurrentProcess.remainingtime);
            kill(currentProcessID, SIGINT);
            // processCreator();
        }
    }
    signal(SIGALRM, handlerALRM);
}

FILE *CreateFileAndOpen()
{
    char *filename = "processOutputSRTN.txt";

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
FILE *CreatePerfFileAndOpen()
{
    char *filename = "schedulerSRTN_perf.txt";

    // open the file for writing
    FILE *fp = fopen(filename, "w");
    if (fp == NULL)
    {
        printf("Error opening the file %s", filename);
        return NULL;
    }
    return fp;
}