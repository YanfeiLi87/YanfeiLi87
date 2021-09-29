#include <stdio.h>
#include <time.h>
#include "df.h"
#include "process.c"



int main(int argc, char *argv[]) {
	clock_t clkBegin = clock();	//get begin time


	getParam(argc,argv);	//get param from txt file

	//start print out configuation 
	FILE * fp;
	fp = fopen (RUNLOGFILE,"w");
	fprintf (fp,"Configuation from user input file: \n");
	fprintf (fp,"Seed: %d\n", gbCf.SEED);
	fprintf (fp,"QUIT_PROB: %f\n", gbCf.QUIT_PROB);
	fprintf (fp,"NETWORK_PROB: %f\n", gbCf.NETWORK_PROB);
	fprintf (fp,"INIT_TIME: %d\n", gbCf.INIT_TIME);
	fprintf (fp,"FIN_TIME: %d\n", gbCf.FIN_TIME);
	fprintf (fp,"ARRIVE_MAX: %d\n", gbCf.ARRIVE_MAX);
	fprintf (fp,"ARRIVE_MIN: %d\n", gbCf.ARRIVE_MIN);
	fprintf (fp,"CPU_MAX: %d\n", gbCf.CPU_MAX);
	fprintf (fp,"CPU_MIN: %d\n", gbCf.CPU_MIN);
	fprintf (fp,"DISK1_MAX: %d\n", gbCf.DISK1_MAX);
	fprintf (fp,"DISK1_MIN: %d\n", gbCf.DISK1_MIN);
	fprintf (fp,"DISK2_MAX: %d\n", gbCf.DISK2_MAX);
	fprintf (fp,"DISK2_MIN: %d\n", gbCf.DISK2_MIN);
	fprintf (fp,"NETWORK_MAX: %d\n", gbCf.NETWORK_MAX);
	fprintf (fp,"NETWORK_MIN: %d\n", gbCf.NETWORK_MIN);
	fprintf (fp,"-------------------------------------------\n\n");
	fclose (fp);
	
	//start print out the log
	fp = fopen (LOGFILE,"w");
	fprintf (fp,"----------------------------------------------\n"); 
	fclose (fp);
	
	
	
	//init
	srand(gbCf.SEED);	
	curTime = gbCf.INIT_TIME;
	total_time = gbCf.TTL_TIME;
	
	priority_Push(&EventQ, createEvent(gbCf.INIT_TIME, 1, STATUS_ARRIVAL), &EventNodes);	//job1 arrival
	priority_Push(&EventQ, createEvent(gbCf.FIN_TIME, 0, STATUS_FINISH), &EventNodes);		//program done

    //push event task into cpu 
	while(EventNodes != 0 && curTime < gbCf.FIN_TIME) {
		evTask = queue_Pop(&EventQ, &EventNodes); 
		curTime = evTask.iId;
		
        //Handle Event  
		switch(evTask.iType) {
			case STATUS_ARRIVAL: CPUEvent(evTask); 
            break;
			case STATUS_DONE: CPUEvent(evTask); 
            break;
			case DISK_STATUS_ARRIVAL: DiskEvent(evTask); 
            break;
			case DISK_STATUS_DONE: DiskEvent(evTask); 
            break;
            case NETWORK_STATUS_ARRIVAL: NetworkEvent(evTask); 
            break;
            case NETWORK_STATUS_DONE: NetworkEvent(evTask); 
            break;
			case STATUS_FINISH: 
            break;
		}
		
		// for run log 
		iterations_total++;
		priority_average += priority_total;
		cpu_average += cpu_total;
		disk1_average += disk1_total;
		disk2_average += disk2_total;
		network_average += network_total;
		getMaxValue();
	}

	clock_t clkEnd = clock();  //get end time
	
	RunStatistics();
	
	double time_spent = (double)(clkEnd-clkBegin) / CLOCKS_PER_SEC;
	
	//print and write status
	printf("Program finish. \nTotal spending time is %fs\n", time_spent);

	fp = fopen (RUNLOGFILE,"a");
	fprintf (fp,"Total spending time is %fs\n", time_spent); 
	fprintf (fp,"----------------------------------------------\n", time_spent); 
	fclose (fp);

	return 0;
}

//get param from txt file
void getParam(int argc, char *argv[]){
   	//Open config file	
	FILE *fp;
	char strLine[30];	
	int iLine=1;
	
	if((fp=fopen(argv[1],"rt"))== NULL)	{ 
		printf("Open Falied!"); 
	}
	
	while (!feof(fp)){
        fgets(strLine,30,fp);
        
		char str[30];
        
        sscanf(strLine, "%*[^=]=%[^\n]", str);
      	
	   	switch (iLine){
     		case 1: gbCf.SEED=atoi(str);
    		break;
    		case 2: gbCf.QUIT_PROB=atof(str);
    		break;
    		case 3: gbCf.NETWORK_PROB=atof(str);
    		break;
    		case 4: gbCf.INIT_TIME=atoi(str);
    		break;
    		case 5: gbCf.FIN_TIME=atoi(str);
    		break;
    		case 6: gbCf.ARRIVE_MAX=atoi(str);
    		break;
    		case 7: gbCf.ARRIVE_MIN=atoi(str);
    		break;
    		case 8: gbCf.CPU_MAX=atoi(str);
    		break;
    		case 9: gbCf.CPU_MIN=atoi(str);
    		break;
    		case 10: gbCf.DISK1_MAX=atoi(str);
    		break;
    		case 11: gbCf.DISK1_MIN=atoi(str);
    		break;
    		case 12: gbCf.DISK2_MAX=atoi(str);
    		break;
    		case 13: gbCf.DISK2_MIN=atoi(str);
    		break;
    		case 14: gbCf.NETWORK_MAX=atoi(str);
    		break;
    		case 15: gbCf.NETWORK_MIN=atoi(str);
    		break;
		}
		iLine++;
    }	
    
    gbCf.TTL_TIME = gbCf.FIN_TIME - gbCf.INIT_TIME;
		
}



void writeStats(char *line) { 
	FILE * fp;
	fp = fopen (RUNLOGFILE,"a");
	fprintf (fp,"%s\n",line); 
	fclose (fp);
}

// CPU Disk Network queue 
// first in first out queue 
void fifo_Push(JobNode** head_ref, Event data, int* total) {
	JobNode* new_JobNode = (JobNode*) malloc(sizeof(JobNode));
	new_JobNode->job = data;
	new_JobNode->next = NULL;

	JobNode *last = *head_ref;
	(*total)++; //to add a new Event in linked list
	if (*head_ref == NULL) {
		*head_ref = new_JobNode;
		return;
	}
	while (last->next != NULL) {
		last = last->next;
    }
	last->next = new_JobNode;
	return;
}

// priority queue 
void priority_Push(JobNode** head_ref, Event data, int* total) {
	priority_total++;
	JobNode* temp = (JobNode*) malloc(sizeof(JobNode));
	temp->job = data;
	temp->next = NULL;

	int priority = temp->job.iId; 
	(*total)++;

	// start here	
	JobNode *last = *head_ref;

	if (*head_ref == NULL) { 
		*head_ref = temp;
		return;
	} else if(priority < last->job.iId) {
		temp->next = last;
		*head_ref = temp; 
		return;
	}

	//from head to tail
	while (last->next != NULL && priority >= last->next->job.iId) {
		last = last->next;
    }
	temp->next = last->next;
	last->next = temp;
	return;
}

// pop Job Node function
Event queue_Pop(JobNode** head_ref, int* total) {
	JobNode *temp = *head_ref;
	*head_ref = (*head_ref)->next;
	Event job = temp->job;
	free(temp); 
	(*total)--; 
	return job;
}

// create Events function
Event createEvent(int time, int iJobNumber, int type) {
	Event job;
	job.iId = time;
	job.iJobNumber = iJobNumber;
	job.iType = type;
	return job; 
}



// finds all the max values for the statistics 
void getMaxValue() {
	if(priority_max < priority_total)
		priority_max = priority_total;

	if(cpu_max < cpu_total)
		cpu_max = cpu_total;

	if(disk1_max < disk1_total)
		disk1_max = disk1_total;

	if(disk2_max < disk2_total)
		disk2_max = disk2_total;
		
	if(network_max < network_total)
		network_max = network_total;	
}


// statistics and print
void RunStatistics() {
	char str[80];
	
	//write Statistics data to run log file
	FILE * fp;
	fp = fopen (RUNLOGFILE,"a");

	//Average sizes of queues
	fprintf (fp,"Average sizes of queues: \n\n"); 
	fprintf(fp, "Priority queue average size: %.2f\n", priority_average/iterations_total);
	fprintf(fp, "CPU queue average size: %.2f\n", cpu_average/iterations_total);
	fprintf(fp,"Disk1 queue average size: %.2f\n", disk1_average/iterations_total);
	fprintf(fp,"Disk2 queue average size: %.2f\n", disk2_average/iterations_total);
	fprintf(fp, "Network queue average size: %.2f\n", network_average/iterations_total);
	fprintf (fp,"----------------------------------\n\n");

	//Max sizes of of queues
	fprintf(fp, "Max sizes of of queues:\n\n");
	fprintf(fp, "Priority queue max size: %d\n", priority_max);
	fprintf(fp, "CPU queue max size: %d\n", cpu_max);
	fprintf(fp, "Disk1 queue max size: %d\n", disk1_max);
	fprintf(fp, "Disk2 queue max size: %d\n", disk2_max);
	fprintf(fp, "Network queue max size: %d\n", network_max);
	fprintf (fp,"----------------------------------\n\n");

	//Average response time of components
	fprintf(fp, "Average response time of components:\n\n");
	fprintf(fp, "CPU average response time: %.2f\n", cpu_time/cpu_job);
	fprintf(fp, "Disk1 average response time: %.2f\n", disk1_time/disk1_job);
	fprintf(fp, "Disk2 average response time: %.2f\n", disk2_time/disk2_job);
	fprintf(fp, "Network average response time: %.2f\n", network_time/network_job);
	fprintf (fp,"----------------------------------\n\n");		

	//Max response time of components
	fprintf(fp, "Max response time of components:\n\n");
	fprintf(fp, "CPU max Response Time: %d\n", cpu_job_max);
	fprintf(fp, "Disk1 max Response Time: %d\n", disk1_job_max);
	fprintf(fp, "Disk2 max Response Time: %d\n", disk2_job_max);
	fprintf(fp, "Network max Response Time: %d\n", network_job_max);
	fprintf (fp,"----------------------------------\n\n");

	//Utilization
	fprintf(fp, "Utilization: (Process Time / Total Time)\n\n");
	fprintf(fp, "CPU utilization: %.2f, %.2f: %.2f\n", cpu_time, total_time, cpu_time/total_time);
	fprintf(fp, "Disk1 utilization: %.2f, %.2f: %.2f\n", disk1_time, total_time, disk1_time/total_time);
	fprintf(fp, "Disk2 utilization: %.2f, %.2f: %.2f\n", disk2_time, total_time, disk2_time/total_time);
	fprintf(fp, "Network utilization: %.2f, %.2f: %.2f\n", network_time, total_time, network_time/total_time);
	fprintf (fp,"----------------------------------\n\n");
	
	//Throughput
	fprintf(fp, "Throughput: (Process Job count / Total Time)\n\n");
	fprintf(fp, "Throughput of CPU %d, %.2f,: %.2f\n", cpu_job, total_time, cpu_job/total_time);
	fprintf(fp, "Throughput of DISK1 %d, %.2f,: %.2f\n", disk1_job, total_time, disk1_job/total_time);
	fprintf(fp, "Throughput of DISK2 %d, %.2f,: %.2f\n", disk2_job, total_time, disk2_job/total_time);
	fprintf(fp, "Throughput of Network %d, %.2f,: %.2f\n", network_job, total_time, network_job/total_time);
	fprintf (fp,"----------------------------------\n\n");		
	
	fclose (fp);
}





