#include <stdio.h> 
#include <unistd.h> 
#include <stdlib.h>
#include <string.h>

typedef struct task{
	int data; //1: a, 2
	char priority;
	char autostart;
	float *return_addr;
	float *task_begin;
	char state;
	struct task* next;
}task;

typedef void (*callback)(task* data);
typedef void (*apuntador_tarea);

void taskA(void);
void taskB(void);
void taskC(void);

void activateTask(task* n)
{
    if(n != NULL){
        if(n->data == 1){
        	taskA();
        }
        else if (n->data == 2)
        {
        	taskB();
        }
        else if (n->data == 3)
        {
        	taskC();
        }
    }
}

task* create(int data, task* next, float *task_begin){
	task* new_task = (task*)malloc(sizeof(task));
    if(new_task == NULL)
    {
        printf("Error creating a new task.\n");
        exit(0);
    }
    new_task->data = data;
    new_task->next = next;
    new_task->task_begin = task_begin;
    return new_task;
}

/*task* create(int data, task* next){
	task* new_task = (task*)malloc(sizeof(task));
    if(new_task == NULL)
    {
        printf("Error creating a new task.\n");
        exit(0);
    }
    new_task->data = data;
    new_task->next = next;

    return new_task;
}*/

task* prepend(int data, task* head, float *task_begin)
{
    task* new_task = create(data,head, task_begin);
    head = new_task;	
    return head;
}

/*task* prepend(int data, task* head)
{
    task* new_task = create(data,head);
    head = new_task;	
    return head;
}*/

void traverse(task* head, callback f)
{
    task* cursor = head;
    while(cursor != NULL)
    {
    	f(cursor);
    	//printf("HOla amigos putos %d\n", cursor->data);
        cursor = cursor->next;
        
    }
}

void execute(task* head, callback f)
{
    task* cursor = head;
    while(cursor != NULL)
    {
    	f(cursor);
    	//printf("HOla amigos putos %d\n", cursor->data);
        cursor = cursor->next;
        
    }
}

void taskA(void){
	printf("Soy tarea A \n");
}

void taskB(void){
	printf("Soy tarea B \n");
}

void taskC(void){
	printf("Soy tarea C \n");
}

int main(int argc, char const *argv[])
{
	callback activate_task = activateTask;
	task* head = NULL;
	task* task_A = NULL;
	task* task_B = NULL;
	task* task_C = NULL;
	//apuntador_tarea ptA;
	//void (*ptA)(void) = taskA;
	//ptA = taskA;
	apuntador_tarea ptA = taskA;
	printf("%p\n", ptA);
	apuntador_tarea ptB = taskB;
	printf("%p\n", ptB);
	apuntador_tarea ptC = taskC;
	printf("%p\n", ptC);
	printf("***********************************\n");
	task_A = prepend(1, task_A, ptA);
	task_B = prepend(2, task_A, ptB);
	task_C = prepend(3, task_B, ptC);
	head = task_C;
	//execute(head, activate_task);
	printf("%p\n", task_A->task_begin);
	printf("%p\n", task_B->task_begin);
	printf("%p\n", task_C->task_begin);
	printf("***********************************\n");
	//taskA();
	/*a =8;
	int *pa ;
	pa = &a;
	printf("%d",&pa);*/
	return 0;
}
