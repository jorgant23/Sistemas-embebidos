#include <stdio.h> 
#include <unistd.h> 
#include <stdlib.h>
#include <string.h>

typedef struct task{
	char priority;
	char autostart;
	float *return_addr;
	void (*task_begin)(void);
	char state;
	struct task* next; //nodo
}task;

task* tarea_actual; // 0:A, 1:B, 2:C

void taskA(void);
void taskB(void);
void taskC(void);
typedef void (*callback)(task* data);
void (*apuntador_tarea_A)(void) = taskA;
void (*apuntador_tarea_B)(void) = taskB;
void (*apuntador_tarea_C)(void) = taskC;

void activateTask(task* n)
{
	n->task_begin();
}

task* terminateTask(){
	tarea_actual->state= 0;
	return tarea_actual;
}

//act

void chainTask(task* n){

}

task* create(char priority, char autostart, void (*task_begin)(void), char state, task* next){
	task* new_task = (task*)malloc(sizeof(task));
    if(new_task == NULL)
    {
        printf("Error creating a new task.\n");
        exit(0);
    }
    new_task->priority = priority;
    new_task->autostart = autostart;
    new_task->task_begin = task_begin;
    new_task->state = state;
    new_task->next = next;
    return new_task;
}

task* prepend(char priority, char autostart, void (*task_begin)(void), char state, task* head)
{
    task* new_task = create(priority, autostart, task_begin, state, head);
    head = new_task;	
    return head;
}

task* checkAutoStart(task* headT){
	task* cursorT = headT;
    task* headA = NULL;
    
    while(cursorT != NULL)
    {
    	if(cursorT->autostart == 1){
    		headA = prepend(cursorT->priority, cursorT->autostart, cursorT->task_begin, cursorT->state, headA);
    	}
    	cursorT = cursorT->next;
    }

    return headA;
}

void execute(task* head, callback f)
{
    tarea_actual = head;
    while(tarea_actual != NULL)
    {
    	f(tarea_actual);
    	tarea_actual = tarea_actual->next;
        
    }
}

/*void execute(task* head, callback f)
{
    task* cursor = head;
    while(cursor != NULL)
    {
    	f(cursor);
    	cursor = cursor->next;
        
    }
}*/

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
	task_A = prepend(1, 1, apuntador_tarea_A, 0, task_A);
	task_B = prepend(2, 0, apuntador_tarea_B, 0, task_A);
	task_C = prepend(3, 0, apuntador_tarea_C, 0, task_B);
	head = task_C;
	//apuntador_tarea_A();
	execute(checkAutoStart(head), activate_task);
	execute(head, activate_task);
	//printf("%p\n", task_A->task_begin);
	//task_A->task_begin();	
	return 0;
}
