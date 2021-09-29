#include <stdio.h>

// get a random number
int getRmNo(int min, int max) {
	return (rand() % (max - min + 1)) + min;
}

// write log file
void writeLog(char *line) { 
	FILE * fp;
	fp = fopen (LOGFILE,"a");
	fprintf (fp,"%s\n",line); 
	fclose (fp);
}

// Job handle  in CPU, include: arrival and finish
void CPUEvent(Event evTask) {
	char str[80];
	if(evTask.iType == STATUS_ARRIVAL) {
		sprintf(str, "At time %d, Job%d arrive CPU Queue.", curTime, evTask.iJobNumber);
		writeLog(str);


		rndTime = getRmNo(gbCf.ARRIVE_MIN, gbCf.ARRIVE_MAX) + curTime; numJobs++;
		priority_Push(&EventQ, createEvent(rndTime, numJobs, STATUS_ARRIVAL), &EventNodes);
		fifo_Push(&jnCPU, evTask, &cpuNodes); // job sent to the CPU fifo
		cpu_total++;

		if (cpuState == STATUS_IDLE) {
			evTask = queue_Pop(&jnCPU, &cpuNodes);
			cpu_total--;
			rndTime = getRmNo(gbCf.CPU_MIN, gbCf.CPU_MAX) + curTime;
			sprintf(str, "At time %d, Job%d go into CPU.", curTime, evTask.iJobNumber);
			priority_Push(&EventQ, createEvent(rndTime, evTask.iJobNumber, STATUS_DONE), &EventNodes);
			writeLog(str);
			cpuState = STATUS_BUSY;
			cpu_old = curTime;
		}
	} else {
		cpuState = STATUS_IDLE;
		int time = curTime - cpu_old;
		cpu_time += time;
		cpu_job++;
		if(time > cpu_job_max)
			cpu_job_max = time;

		sprintf(str, "At time %d, Job%d finish at CPU.", curTime, evTask.iJobNumber);
		writeLog(str);

		int quit = ((rand()%10) <  (gbCf.QUIT_PROB * 10));
		if(quit) {
			sprintf(str, "At time %d, Job%d finish and exit.", curTime, evTask.iJobNumber);
			writeLog(str);
		} else {
			if ((rand()%10) >  (gbCf.NETWORK_PROB * 10)){
				priority_Push(&EventQ, createEvent(curTime, evTask.iJobNumber, DISK_STATUS_ARRIVAL), &EventNodes);
			}
			else{
				priority_Push(&EventQ, createEvent(curTime, evTask.iJobNumber, NETWORK_STATUS_ARRIVAL), &EventNodes);
			}
		}
	}
}

// Job handle  in Disk, include: arrival and finish Disk 1,2
void DiskEvent(Event evTask) {
	char str[80];
	if (evTask.iType == DISK_STATUS_ARRIVAL) {
		int sendTo = 0;

       	if(disk1Nodes < disk2Nodes) {
			sendTo = 1;
		} else if(disk1Nodes > disk2Nodes) {
			sendTo = 2;
		} else {
			sendTo = (rand() <  0.5 * 1.3) + 1;
		}

		if(sendTo == 1) {
			fifo_Push(&jnDisk1, evTask, &disk1Nodes);
			sprintf(str, "At time %d, Job%d arrives to Disk 1.", curTime, evTask.iJobNumber);
			writeLog(str);
			disk1_total++;
			if(disk1State == STATUS_IDLE){
				Event job = queue_Pop(&jnDisk1, &disk1Nodes);
				rndTime = getRmNo(gbCf.DISK1_MIN, gbCf.DISK1_MAX) + curTime;
				priority_Push(&EventQ, createEvent(rndTime, job.iJobNumber, DISK_STATUS_DONE), &EventNodes);
				disk1 = evTask.iJobNumber;
				disk1State = STATUS_BUSY;
				disk1_old = curTime;
			}
		} else if(sendTo == 2) {
			fifo_Push(&jnDisk2, evTask, &disk2Nodes);
			sprintf(str, "At time %d, Job%d arrives to Disk 2.", curTime, evTask.iJobNumber);
			writeLog(str);
			disk2_total++;
			if(disk2State == STATUS_IDLE) {
				Event job = queue_Pop(&jnDisk2, &disk2Nodes);
				rndTime = getRmNo(gbCf.DISK2_MIN, gbCf.DISK2_MAX) + curTime;
				priority_Push(&EventQ, createEvent(rndTime, job.iJobNumber, DISK_STATUS_DONE), &EventNodes);
				disk2 = evTask.iJobNumber;
				disk2State = STATUS_BUSY;
				disk2_old = curTime;
			}
		}
	} else {
		priority_Push(&EventQ, createEvent(curTime, evTask.iJobNumber, STATUS_ARRIVAL), &EventNodes);
		if(disk1 == evTask.iJobNumber) {
			int time = curTime - disk1_old;
			disk1_time += time;
			disk1State = STATUS_IDLE;
			disk1Nodes = 0;
			disk1_job++;
			if(time > disk1_job_max)
				disk1_job_max = time;
			sprintf(str, "At time %d, Job%d finish at Disk 1.", curTime, evTask.iJobNumber);
			writeLog(str);
		} else if(disk2 == evTask.iJobNumber) {
				int time = curTime - disk2_old;
				disk2_time += time;
				disk2State = STATUS_IDLE;
				disk2Nodes = 0;
				disk2_job++;
				if(time > disk2_job_max)
					disk2_job_max = time;
			sprintf(str, "At time %d, Job%d finish at Disk 2.", curTime, evTask.iJobNumber);
			writeLog(str);
		}
	}
}

// Job handle in network, include: arrival and finish
void NetworkEvent(Event evTask) {
	char str[80];
	if (evTask.iType == NETWORK_STATUS_ARRIVAL) {
		fifo_Push(&jnNetwork, evTask, &networkNodes);
		sprintf(str, "At time %d, Job%d arrives to Network.", curTime, evTask.iJobNumber);
		writeLog(str);
		network_total++;
		if(networkState == STATUS_IDLE){
			Event job = queue_Pop(&jnNetwork, &networkNodes);
			rndTime = getRmNo(gbCf.NETWORK_MIN, gbCf.NETWORK_MAX) + curTime;
			priority_Push(&EventQ, createEvent(rndTime, job.iJobNumber, NETWORK_STATUS_DONE), &EventNodes);
			network = evTask.iJobNumber;
			networkState = STATUS_BUSY;
			network_old = curTime;
		}
	} else {
		priority_Push(&EventQ, createEvent(curTime, evTask.iJobNumber, STATUS_ARRIVAL), &EventNodes);
		int time = curTime - network_old;
		network_time += time;
		networkState = STATUS_IDLE;
		networkNodes = 0;
		network_job++;
		if(time > network_job_max)
			network_job_max = time;
		sprintf(str, "At time %d, Job%d finish at Network.", curTime, evTask.iJobNumber);
		writeLog(str);
	
	}
}
