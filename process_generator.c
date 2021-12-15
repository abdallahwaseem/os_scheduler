#include "headers.h"
#include <unistd.h>
#include <string.h>

void clearResources();
void ReadFile(struct Queue *q);
int ChooseAlgorithms(char *argv[]);
void InitializeClock(char *argv[]);
int CreateMsgQueueIPC();
void SendToScheduler(struct Queue *q, int msgq_id, int SchedulerPiD);
void SendToQueueIPC(processData Data, int msq_id);
int msgq_id_Global;
void handleAlarm(){};
//void handleINT(){exit(0);};


int main(int argc, char *argv[])
{
    //signal(SIGINT, handleINT);
    signal(SIGCHLD, clearResources);
    signal(SIGALRM,handleAlarm);
    // TODO Initialization
    // 1. Read the input files.
    struct Queue *All_Processes = createQueue();
    ReadFile(All_Processes);
    // 2. Ask the user for the chosen scheduling algorithm and its parameters, if there are any.
    int SchedulerPiD = ChooseAlgorithms(argv);
    // 4. Use this function after creating the clock process to initialize clock
    InitializeClock(argv);
    initClk();
    // To get time use this
    int x = getClk();
    printf("current time is %d\n", x);
    // TODO Generation Main Loop
    // 5. Create a data structure for processes and provide it with its parameters.
    //Done it is in All_Processes and read file
    // 6. Send the information to the scheduler at the appropriate time.
    msgq_id_Global = CreateMsgQueueIPC();
    SendToScheduler(All_Processes, msgq_id_Global, SchedulerPiD);
    pause();
    // 7. Clear clock resources and kill the Scheduler
    //kill(SchedulerPiD, SIGINT);
    //clearResources(msgq_id);
    //return 0;
   
}

void clearResources()
{
    // TODO Clears all resources in case of interruption
    int pid, status;
    pid = wait(&status);
    //printf("\n process killed , %d\n",pid);
    if(WIFEXITED(status))
    {
        msgctl(msgq_id_Global, IPC_RMID, (struct msqid_ds *)0);
        destroyClk(true);
    }
    //return 0;
}

void SendToScheduler(struct Queue *All_Processes, int msgq_id, int SchedulerPiD)
{
    //this loops means that there is still processes in Queue
    while (!isEmptyQueue(All_Processes))
    {
        int wakeSchedule = false;
        int x = getClk();
        int ArrivalTop = peekQueue(All_Processes).arrivaltime;
        while (ArrivalTop == x)
        {
            /// Send to IPCS :
            wakeSchedule = true;
            processData Msg = peekQueue(All_Processes);
            SendToQueueIPC(Msg, msgq_id);
            deQueue(All_Processes);
            if (!isEmptyQueue(All_Processes))
                ArrivalTop = peekQueue(All_Processes).arrivaltime;
            else
                ArrivalTop = -1;
        }
        //wake after
        if (wakeSchedule)
        {
            wakeSchedule = false;
            kill(SchedulerPiD, SIGALRM);
        }
        //int wakeTime = peekQueue(All_Processes).arrivaltime - x-1;
        alarm(1);
        //SetTimer(NULL, 0, 500, (TIMERPROC)& NULL);
        pause();
    }
}

void SendToQueueIPC(processData Data, int msq_id)
{
    msgBuff message;
    message.mtype = 1;
    message.Data = Data;
    int send_val = msgsnd(msq_id, &message, sizeof(message.Data), !IPC_NOWAIT);
    if (send_val == -1)
        perror("Error in send");
}

void InitializeClock(char *argv[])
{
    int pid = fork();
    if (pid == 0)
    {
        execve("clk.out", argv, NULL);
    }
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
            temp.waitingtime = 0;
            temp.remainingtime = processBuffer[2];
            temp.status = waiting;
            enQueue(q, temp);
        }
    }
    // close the file
    //Will enQueue a last process as a flag, so that the scheduler will know when to clos
    processData temp;
    temp.priority = LastProcessPriorty;
    temp.remainingtime = LastProcessPriorty;
    temp.status = lastProcess;
    enQueue(q, temp);
    fclose(fp);
}

int ChooseAlgorithms(char *argv[])
{
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
    return pid;
}