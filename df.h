
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


//job and device status
#define STATUS_ARRIVAL 0
#define STATUS_DONE 1
#define STATUS_FINISH 99

#define STATUS_IDLE 0
#define STATUS_BUSY 1

#define DISK_STATUS_ARRIVAL 10
#define DISK_STATUS_DONE 11
#define NETWORK_STATUS_ARRIVAL 20
#define NETWORK_STATUS_DONE 21

#define LOGFILE "log.txt"
#define RUNLOGFILE "runlog.txt"


// Config var struct
typedef struct {
	int SEED;
	float QUIT_PROB;	//job quit probability.
	float NETWORK_PROB; // probability for a job to disk or network

	int INIT_TIME;		//Initial time
	int	FIN_TIME;		//Final time
	int TTL_TIME;		//Total time
	int ARRIVE_MAX;		//Job Arrivate time (maximum)
	int ARRIVE_MIN;        //Job Arrivate time (minimum)

	int CPU_MAX; 		//CPU handling time (maximum)
	int CPU_MIN;		//CPU handling time (minimum)
	int DISK1_MAX;			//Disk 1 handling time (maximum)
	int DISK1_MIN;			//Disk 1 handling time (minimum)
	int DISK2_MAX;			//Disk 2 handling time (maximum)
	int DISK2_MIN;			//Disk 2 handling time (minimum)
	int NETWORK_MAX;			//Network handling time (maximum)
	int NETWORK_MIN;			//Network handling time (minimum)

} Param;

// Event struct
typedef struct Event{
	int iId; 		//job timestamp
	int iJobNumber;	//job number
	int iType;		//job type
} Event;

//JobNode struct for all queues
typedef struct JobNode{
	Event job; 			//Event infor
	struct JobNode *next;	//JobNode next pointer
} JobNode;


//Global var
Param gbCf;	//system Params values get from user file
int curTime = 0;	//timestamp
int rndTime = 0;	//rand time
Event evTask;		//event

//JobNode start number
int cpuNodes = 0, disk1Nodes = 0, disk2Nodes = 0, networkNodes = 0, EventNodes = 0;
//jobs starts number
int numJobs = 1;

//init status
int cpuState = STATUS_IDLE, disk1State = STATUS_IDLE, disk2State = STATUS_IDLE, networkState=STATUS_IDLE;

// define Job Node
JobNode *jnCPU, *jnDisk1, *jnDisk2, *jnNetwork;
JobNode* EventQ;


// creates event
// job id , job number, job type
Event createEvent(int, int, int);


//statistics var
int job_Numbers = 0;
int disk1, disk2,network;
int priority_total = 0, cpu_total = 0, disk1_total = 0, disk2_total = 0, network_total = 0;
int priority_max = 0, cpu_max = 0, disk1_max = 0, disk2_max = 0,network_max=0;
int iterations_total = 0;
float priority_average = 0, cpu_average = 0, disk1_average = 0, disk2_average = 0, network_average=0;
float cpu_time = 0, disk1_time = 0, disk2_time = 0,network_time=0;
int cpu_old = 0, disk1_old = 0, disk2_old = 0, network_old;
float total_time = 0;
int cpu_job = 0, disk1_job = 0, disk2_job = 0,network_job=0;
int cpu_job_max = 0, disk1_job_max = 0, disk2_job_max = 0,network_job_max=0;


// function

//get param from file
void getParam(int, char **);

// priority queue linked list push
void priority_Push(JobNode**, Event, int*);

// first in first out linked list push
void fifo_Push(JobNode**, Event, int*);

//both linked list queues pop event
Event queue_Pop(JobNode**, int*);

// get a random number
int getRmNo(int min, int max);

// handle event
void CPUEvent(Event);
void DiskEvent(Event);
void NetworkEvent(Event);

// statistics data
void getMaxValue();

// print data
void RunStatistics();







