#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFFER_SIZE 128
#define INITIAL_CAPACITY 8

typedef struct {
	int id;
	int arrivalTime;
	int duration;
	int startTime;
	int isDone;
} Process;

Process *parseInput(int *outCount) {
	char buffer[BUFFER_SIZE];
	int processCount = 0;
	int capacity = INITIAL_CAPACITY;
	int lineCount = 0;

	Process *processes = malloc(capacity * sizeof(Process));
	if (processes == NULL) {
		fprintf(stderr, "Memory allocation failed!\n");
		return NULL;
	}

	while (fgets(buffer, sizeof(buffer), stdin) != NULL) {
		lineCount++;
		if (buffer[0] == '\n') continue;
		if (processCount >= capacity) {
			capacity *= 2;
			Process *tmp = realloc(processes, capacity*sizeof(Process));
			if (tmp == NULL) {
				fprintf(stderr, "Memory allocation failed!\n");
				free(processes);
				return NULL;
			}
			processes = tmp;
		}
		if(sscanf(buffer, "%d %d", &processes[processCount].arrivalTime, &processes[processCount].duration)==2) {
			processes[processCount].isDone = 0;
			processes[processCount].id = processCount;
			processCount++;
		} else {
			fprintf(stderr, "Incorrect line format in line %d\n", lineCount);
			free(processes);
			return NULL;
		}

	}
	*outCount = processCount;
	return processes;

}

int comparProcesses(const void *a, const void *b) {
	const Process *processA = (const Process*)a;
	const Process *processB = (const Process*)b;

	return (processA->arrivalTime - processB->arrivalTime);
}

void algoFCFS(Process processes[], int processCount) {
	int currentTime = 0;
	int i;
	for(i=0;i < processCount;i++) {
		if(currentTime < processes[i].arrivalTime) currentTime = processes[i].arrivalTime;
		processes[i].startTime = currentTime;
		currentTime += processes[i].duration;
		processes[i].isDone = 1;
	}
}

void algoLCFS(Process processes[], int processCount) {
	int currentTime=0;
	int processCompleted = 0;
	while(processCompleted < processCount) {
		int lastCome = -1, lastComeTime = -1;
		int i;
		for(i=0;i<processCount;i++) {
			if(processes[i].isDone == 0 && processes[i].arrivalTime <= currentTime && processes[i].arrivalTime > lastComeTime) {
				lastCome = i;
				lastComeTime = processes[i].arrivalTime;
			}
		}
		if(lastCome == -1) {
			for(i=0;i<processCount;i++) {
				if(processes[i].isDone == 0) {
					currentTime = processes[i].arrivalTime;
					lastCome = i;
					break;
				}
			}
		}
		processes[lastCome].isDone = 1;
		processes[lastCome].startTime = currentTime;
		processCompleted++;
		currentTime += processes[lastCome].duration;
	}
}

int main(int argc, char *argv[]) {
	char algorithm = '\x00';
	int opt;
	while ((opt = getopt(argc, argv, "hfl")) != -1) {
		switch (opt) {
			case 'f': algorithm = 'f'; break;
			case 'l': algorithm = 'l'; break;
			case 'h':	// fall-through
			case '?':
				printf("HELP\n");	// TODO
				return 0;

		}
	}
	if (algorithm == '\x00') {
		fprintf(stderr, "No algorithm chosen! Use -h flag for help!\n");
		return 1;
	}

	int processCount = 0;
	Process *processes = parseInput(&processCount);
	if(processes == NULL) return 1;
	
	qsort(processes, processCount, sizeof(Process), comparProcesses);

	switch (algorithm) {
		case 'f':
			algoFCFS(processes, processCount);
			break;
		case 'l':
			algoLCFS(processes, processCount);
			break;
	}
	printf("ID,Time of arrival,Duration,Time of start,Time of finish,Waiting time,Turnaround\n");
	int i;
	for(i=0;i<processCount;i++) {
		int finishTime = processes[i].startTime+processes[i].duration;
		printf("%i,%i,%i,%i,%i,%i,%i\n",
				processes[i].id,
				processes[i].arrivalTime,
				processes[i].duration,
				processes[i].startTime,
				finishTime,
				processes[i].startTime - processes[i].arrivalTime,
				finishTime - processes[i].arrivalTime
		);
	}
	free(processes);
	return 0;
}
