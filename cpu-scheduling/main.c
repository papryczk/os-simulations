#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 128
#define INITIAL_CAPACITY 8

typedef struct {
	int arrivalTime;
	int duration;
} Process;

Process* parseInput(int* outCount) {
	char buffer[BUFFER_SIZE];
	int processCount = 0;
	int capacity = INITIAL_CAPACITY;
	int lineCount = 0;

	Process* processes = malloc(capacity * sizeof(Process));
	if (processes == NULL) {
		fprintf(stderr, "Memory allocation failed!\n");
		return NULL;
	}

	while (fgets(buffer, sizeof(buffer), stdin) != NULL) {
		lineCount++;
		if (buffer[0] == '\n') continue;
		if (processCount >= capacity) {
			capacity *= 2;
			Process* tmp = realloc(processes, capacity*sizeof(Process));
			if (tmp == NULL) {
				fprintf(stderr, "Memory allocation failed!\n");
				free(processes);
				return NULL;
			}
			processes = tmp;
		}
		if(sscanf(buffer, "%d %d", &processes[processCount].arrivalTime, &processes[processCount].duration)==2) {
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

int main(int argc, char* argv[]) {
	int processCount = 0;
	Process* processes = parseInput(&processCount);
	if(processes == NULL) return 1;



	free(processes);
	return 0;
}
