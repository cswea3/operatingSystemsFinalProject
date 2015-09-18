// Written by Craig Swearingen for CSC389
// Introduction to Operating Systems - University of Illinois Springfield

#include "linkedlist.h"
#include <stdbool.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>


/*
	A structure to hold different variables that help control the path of the program
*/
typedef struct ControlUnit {
	bool inUseByMale;
	int stallsInUse;
	int totalUsedMale;
	int totalUsedFemale;
	int totalUsedCurrentOccupancy;
} ControlUnit;

const int stallCount = 4;
const int performanceVariable = 6;
const int MAX = 100;
sem_t countingSem;
sem_t release_mutex;
sem_t counting_mutex;
volatile ControlUnit *cu;
int TOTAL = 0;
pthread_t *threads;
bool release = false;
bool automatic = false;
List *list;
int releaseCount = 0;
int thread_count = 0;


void invalidCLArg();
bool IsNumber(char *arg);
void automated(char** argv);
void manual();
void man_wants_to_enter(node *current);
void woman_wants_to_enter(node *current);
void man_wants_to_leave();
void woman_wants_to_leave();
void printCycle();
void bathroom(node *current_node);
void automaticRelease();

int main(int argc, char **argv) {

	/*
	Check if a command line argument was passed and is in the correct format.
	If none is passed it is in manual mode, call manual.
	If one is passed it is in automatic mode, call automated.
	If more is passed it is invalid, call invalidCLArg
	*/
	if(argc == 1){
		manual();
	} else if(argc == 2) {
		automatic = true;
		automated(argv);
	} else {
		invalidCLArg();
	}

	return 0;
}

void automated(char** argv){
	int result;
	int totalPeople = 0;
	srand(time(NULL));
	int r;
	result = sem_init(&countingSem,0,4);
	result = sem_init(&counting_mutex,0,1);
	result = sem_init(&release_mutex,0,1);

	cu = malloc(sizeof(ControlUnit));
	cu->stallsInUse = 0;
	cu->totalUsedMale = 0;
	cu->totalUsedFemale = 0;
	cu->totalUsedCurrentOccupancy = 0;
	result = sem_init(&release_mutex,0,1);


	if(IsNumber(argv[1])) {
		totalPeople = atoi(argv[1]);
		if(totalPeople < 20 || totalPeople > 100)
				invalidCLArg();
	} else {
		invalidCLArg();
	}

	threads = malloc(sizeof(pthread_t) * totalPeople);

	list = createLinkedList();
	for(int i =0; i < totalPeople; i++) {
		r = rand() % 2;
		pushLinkedList(list, r);
		}
	printList(list);
	cu->inUseByMale = (list->first->data == MALE);


	
	node *currentNode;
	for(int i = 0; i < totalPeople; i++){
		//currentNode = removeFirstLinkedList(list);
		if(isNextMale(list)){
			man_wants_to_enter(currentNode);
		}
		else{
			woman_wants_to_enter(currentNode);
		}
	}
	

}

void manual(){

	// Create and initialize variables
	cu = malloc(sizeof(ControlUnit));
	int *test = malloc(sizeof(int));
	
	int result = 0;
	bool loop = true;
	char choice;
	char line[256];
	threads = malloc(sizeof(pthread_t) * MAX);
	result = sem_init(&countingSem,0,stallCount);
	result = sem_init(&release_mutex,0,1);
	result = sem_init(&counting_mutex,0,1);
	list = createLinkedList();
	cu->stallsInUse = 0;
	cu->totalUsedMale = 0;
	cu->totalUsedFemale = 0;
	cu->totalUsedCurrentOccupancy = 0;
	
	

	while(loop){
		//Total holds the cycle count
		TOTAL++;
		//print menu
		printf("Enter M to add a male to the line\n");
		printf("Enter F to add a female to the line\n");
		printf("Enter 1 to let someone in the bathroom if able to\n");
		printf("Enter 2 to let someone leave the bathroom if able to\n");
		printf("Enter X to leave program\n");
		//get input
		if(fgets(line,sizeof(line), stdin))
			choice = line[0];	
		node *current_node;
		switch(choice){
			case 'M':
				// add a male to the list
				pushLinkedList(list,MALE);
				break;
			case 'F':
				//add a female to the list
				pushLinkedList(list,FEMALE);
				break;
			case '1':
				// Try to let someone in the bathroom

				// test is used to see if the counting semaphore is full, it counts up to the stall count
				result = sem_getvalue(&countingSem, test );

				//list is empty
				if(list->first==NULL){
						printf("There isn't anyone in line!\n");
						TOTAL--;
				//semaphore is all used
				}else if(*test == 0){
					printf("The stalls are full\n");
				//no stalls are in use and test would be same as stall count
				//the following else if test the control unit structure and creates the appropriate threads
				}else if(cu->stallsInUse == 0){
					cu->totalUsedCurrentOccupancy = 0;
					if(list->first->data == MALE){
						pthread_create(&threads[thread_count], NULL, man_wants_to_enter, current_node);
						thread_count++;
						cu->totalUsedCurrentOccupancy++;
					}else if(list->first->data == FEMALE){
						pthread_create(&threads[thread_count], NULL, woman_wants_to_enter, current_node);
						thread_count++;
						cu->totalUsedCurrentOccupancy++;
					}
				}else if(cu->inUseByMale && list->first->data == MALE){
						pthread_create(&threads[thread_count], NULL, man_wants_to_enter, current_node);
						thread_count++;
						cu->totalUsedCurrentOccupancy++;
				}else if(!(cu->inUseByMale) && list->first->data == FEMALE ){
						pthread_create(&threads[thread_count], NULL, woman_wants_to_enter, current_node);
						thread_count++;
						cu->totalUsedCurrentOccupancy++;
				}else if(cu->inUseByMale && list->first->data == FEMALE){
						// To make the most of concurrency sometimes a person is skipped if there is a member
						// of the opposite sex in the bathroom, there is room in the bathroom, and there
						// is a member of the opposite sex in line behind the person
						// The amount of people that can be let around is set in the performance variable
						if(performanceVariable > cu->totalUsedCurrentOccupancy){
							pthread_create(&threads[thread_count], NULL, man_wants_to_enter, current_node);
							thread_count++;
							cu->totalUsedCurrentOccupancy++;
						}else{
							// This prevents one person from constantly being skipped. The amount of people
							// of one sex let in, in a row is the same as the performance variable
							printf("You must let some females use the bathroom! Starvation Prevention!\n");
							TOTAL--;
						}
				}else if(!(cu->inUseByMale) && list->first->data == MALE){
					// To make the most of concurrency sometimes a person is skipped if there is a member
						// of the opposite sex in the bathroom, there is room in the bathroom, and there
						// is a member of the opposite sex in line behind the person
						// The amount of people that can be let around is set in the performance variable
					if(performanceVariable > cu->totalUsedCurrentOccupancy){
							pthread_create(&threads[thread_count], NULL, woman_wants_to_enter, current_node);
							thread_count++;
							cu->totalUsedCurrentOccupancy++;
						}else{
							// This prevents one person from constantly being skipped. The amount of people
							// of one sex let in, in a row is the same as the performance variable
							printf("You must let some males use the bathroom! Starvation Prevention!\n");
							TOTAL--;
						}
				}
				break;
			case '2':
			// lets someone leave the bathroom
				if(cu->stallsInUse == 0){
					printf("There is no one in the bathroom.\n");
					TOTAL--;
				}else if(cu->inUseByMale){
					man_wants_to_leave();
				}else{
					woman_wants_to_leave();
				}
				break;
			case 'X':
				loop = false;
				break;
		}
		sleep(2);
		printCycle();
	}

}


void man_wants_to_enter(node *current_node){
	int result = 0;
	current_node = removeLinkedList(list,MALE);
	if(current_node == NULL){
		printf("There are no more males in line\n");
		return;
	}
	int *test = malloc(sizeof(int));
	result = sem_getvalue(&countingSem, test );

	if(automatic){
		sem_post(&countingSem);
		result = pthread_create(&threads[thread_count],NULL,bathroom,current_node);
		
		thread_count++;
		int timeSpentInBathroom = rand() %4 + 1;
		sleep(timeSpentInBathroom);
			man_wants_to_leave();	
			sem_wait(&counting_mutex);
			cu->stallsInUse--;
			cu->totalUsedMale--;
			TOTAL++;
			printCycle();
			sem_post(&counting_mutex);
	}else{
		sem_wait(&countingSem);
		sem_wait(&counting_mutex);
		cu->stallsInUse++;
		cu->inUseByMale = true;
		cu->totalUsedMale++;
		sem_post(&counting_mutex);
		bathroom(current_node);
		sem_wait(&counting_mutex);
		cu->stallsInUse--;
		cu->totalUsedMale--;
		sem_post(&counting_mutex);
		sem_post(&countingSem);
	}
	
}

void woman_wants_to_enter(node *current_node){

	int result = 0;
	current_node = removeLinkedList(list,FEMALE);
	if(current_node == NULL){
		printf("There are no more females in line\n");
		return;
	}
	int *test = malloc(sizeof(int));
	result = sem_getvalue(&countingSem, test );
	
	if(automatic){
		sem_post(&countingSem);
		result = pthread_create(&threads[thread_count],NULL,bathroom,current_node);
		
		thread_count++;
		int timeSpentInBathroom = rand() %4 + 1;
		sleep(timeSpentInBathroom);
			woman_wants_to_leave();	
			sem_wait(&counting_mutex);
			cu->stallsInUse--;
			cu->totalUsedFemale--;
			TOTAL++;
			printCycle();
			sem_post(&counting_mutex);
	}else{	
		sem_wait(&countingSem);
		sem_wait(&counting_mutex);
		cu->stallsInUse++;
		cu->totalUsedFemale++;
		cu->inUseByMale = false;
		sem_post(&counting_mutex);
		bathroom(current_node);
		sem_wait(&counting_mutex);
		cu->stallsInUse--;
		cu->totalUsedFemale--;
		sem_post(&counting_mutex);
		sem_post(&countingSem);
	}

}
 void bathroom(node *current_node) {
 		if(automatic && current_node->data == MALE){
 			sem_wait(&counting_mutex);
 			TOTAL++;
 			cu->stallsInUse++;
 			cu->totalUsedMale++;
 			printCycle();
 			sem_post(&counting_mutex);
 		}else if(automatic && current_node->data == FEMALE){
 			sem_wait(&counting_mutex);
 			TOTAL++;
 			cu->stallsInUse++;
 			cu->totalUsedFemale++;
 			printCycle();
 			sem_post(&counting_mutex);
 		}

	
 		sem_wait(&release_mutex);
 		while(!release){
 			sem_post(&release_mutex);

 			sem_wait(&release_mutex);
 		}
 		free(current_node);
 		releaseCount++;
    	release = false;
 		sem_post(&release_mutex);
 	
 }

void man_wants_to_leave(){
	sem_wait(&release_mutex);
	release = true;
	sem_post(&release_mutex);
}

 void woman_wants_to_leave(){
 	sem_wait(&release_mutex);
	release = true;
	sem_post(&release_mutex);
 }

 void printCycle() {
	printf("************************************\n");
	printf("Cycle Number: %d\n",TOTAL);
	printf("Current Line: \n");
	printList(list);
	printf("Current Occupancy: \n");
	//sem_wait(&counting_mutex);
	printf("Males: %d\n", cu->totalUsedMale);
	printf("Females: %d\n", cu->totalUsedFemale);
	//sem_post(&counting_mutex);
	printf("************************************\n");
}

void invalidCLArg() {
	printf("You passed an invalid argument!\n");
	printf("The correct usage  for automatic operation is $./project2 n\n");
	printf("Where n is a number 20-100\n");
	printf("Or $./project2 for manual operation\n");
	exit(EXIT_FAILURE);
}

bool IsNumber(char *arg) {
   for(int i = 0; i < strlen(arg); i ++) {     
      if (arg[i] < 48 || arg[i] > 57)
         return false;
   }
   return true;
}

