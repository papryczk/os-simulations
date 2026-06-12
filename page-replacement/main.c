#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>

#define BUFFER_SIZE 128
#define INITIAL_CAPACITY 8

typedef struct {
	int pageNumber;
	int timestamp;
} FrameSlot;

int *parseInput(int *outCount, int *frameSize) {
	char buffer[BUFFER_SIZE];
	int queueLength = 0;
	int capacity = INITIAL_CAPACITY;
	int lineCount = 1;

	// Initial allocation of processes table in memory
	int *queue = malloc(capacity * sizeof(int));
	if (queue == NULL) {
		fprintf(stderr, "Memory allocation failed!\n");
		return NULL;
	}

	// Get frameSize
	fgets(buffer, sizeof(buffer), stdin);
	if(sscanf(buffer, "%d", frameSize) != 1) {
		fprintf(stderr, "Incorrect Line format in the first line!\n");
		free(queue);
		return NULL;
	}

	while (fgets(buffer, sizeof(buffer), stdin) != NULL) {
		lineCount++;
		if (buffer[0] == '\n') continue;

		// Increase allocated memory for processes table if its too long - dinamic size
		if (queueLength >= capacity) {
			capacity *= 2;
			int *tmp = realloc(queue, capacity*sizeof(int));
			if (tmp == NULL) {
				fprintf(stderr, "Memory allocation failed!\n");
				free(queue);
				return NULL;
			}
			queue = tmp;
		}

		// Taking input from sdtin
		if(sscanf(buffer, "%d", &queue[queueLength])==1) {
			queueLength++;
		} else {
			fprintf(stderr, "Incorrect line format in line %d\n", lineCount);
			free(queue);
			return NULL;
		}

	}
	*outCount = queueLength;
	return queue;
}

int main(int argc, char *argv[]) {
	char algorithm = '\x00';
	int opt;

	// Parsing input flags
	while ((opt = getopt(argc, argv, "hfl")) != -1) {
		switch (opt) {
			case 'f': algorithm = 'f'; break;
			case 'l': algorithm = 'l'; break;
			case 'h':	// fall-through
			case '?':
				printf("Page Replacement Simulator\n\n");
				printf("Usage: cat data.txt | ./main [OPTION]\n");
				printf("Or:    ./main [OPTION] < data.txt\n\n");
				printf("Options:\n");
				printf("  -f    Run First In First Out (FIFO) paging\n");
				printf("  -l    Run Least Recently Used (LRU) paging\n");
				printf("  -h    Display this help message and exit\n\n");
				printf("Input Format:\n");
				printf("Pass data through standard input (stdin). First line should represent\n");
				printf("frame size, the rest is sequential list of pages asked by programme\n");
				return 0;

		}
	}
	if (algorithm == '\x00') {
		fprintf(stderr, "No algorithm chosen! Use -h flag for help!\n");
		return 1;
	}
	int queueLength = 0, frameSize = 0, pageFaults = 0, currentTime = 0;

	int *queue = parseInput(&queueLength, &frameSize);
	if(queue == NULL) return 1;

	FrameSlot frame[frameSize];
	int i;
	for(i=0;i<frameSize;i++) {
		frame[i].pageNumber = -1;
		frame[i].timestamp = 0;
	}

	// Main simulation loop
	for(i=0;i<queueLength;i++) {
		currentTime++;
		int isHit=0;
		int j;

		// Check for hits
		for(j=0;j<frameSize;j++) {
			if(frame[j].pageNumber == queue[i]) {
				isHit = 1;
				if(algorithm=='l') frame[j].timestamp = currentTime;
				break;
			}
		}

		// Page Fault logic
		int hadFreeSpace = 0;
		if(!isHit) {
			pageFaults++;
			for(j=0;j<frameSize;j++) {
				if(frame[j].pageNumber == -1) {
					hadFreeSpace = 1;
					frame[j].pageNumber = queue[i];
					frame[j].timestamp = currentTime;
					break;
				}
			}

			// Kicking page from frame
			if(!hadFreeSpace) {
				int oldestTimestamp = INT_MAX, oldestIndex = 0;
				for(j=0;j<frameSize;j++) {
					if(frame[j].timestamp < oldestTimestamp) {
						oldestTimestamp = frame[j].timestamp;
						oldestIndex = j;
					}
				}
				frame[oldestIndex].pageNumber = queue[i];
				frame[oldestIndex].timestamp = currentTime;
			}
		}

		printf("Step %d: [", currentTime);
		int isFirst = 1;
		for(int k = 0; k < frameSize; k++) {
			if(frame[k].pageNumber != -1) {
				if(!isFirst) {
					printf(", ");
				}
				printf("%d", frame[k].pageNumber);
				isFirst = 0;
			}
		}
		printf("]\n");
	}
	
	printf("Page Faults: %d\n", pageFaults);

	free(queue);
	return 0;
}
