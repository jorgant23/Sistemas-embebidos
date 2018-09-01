#include <stdio.h> 
#include <stdlib.h>
#define __ASM __asm 
#define __INLINE inline 
#define __STATIC_INLINE static inline 
#include "derivative.h"

typedef struct task{
	char priority;
	char autostart;
	//void (*return_addr)(int);
	//void (*return_addr)(int);
	int *return_addr;
	void (*task_begin)(void);
	char state;
	struct task* next; //nodo
}task;

/********************************************************************************************************/

void activateTask(task* n);
void terminateTask();	
void taskA(void);
void taskB(void);
void taskC(void);
void taskD(void);
void taskE(void);
void taskF(void);
typedef void (*callback)(task* data);
void execute(task* head);
void (*apuntador_tarea_A)(void) = taskA;
void (*apuntador_tarea_B)(void) = taskB;
void (*apuntador_tarea_C)(void) = taskC;
void run(task* n);
void runWait(void);
void systemInit(void);
void run_wait(task* n);
void PORTC_init(void);
/********************************************************************************************************/

task* head_iddle = NULL;
task* head_ready = NULL;
task* head_wait = NULL;
task* taskRunning = NULL;
callback activate_task = activateTask;
task* tmp = NULL;
task* task_A = NULL;
task* task_B = NULL;
task* task_C = NULL;
task* cursorT = NULL;
task* create(char priority, char autostart, void (*task_begin)(void), char state, task* next);
task* prepend(char priority, char autostart, void (*task_begin)(void), char state, task* head);
task* checkAutoStart(task* headT);
task* insertion_sort(task* head);
int *LR_addres;

/********************************************************************************************************/

void PORTC_init(void){
	SIM_SCGC5|=SIM_SCGC5_PORTC_MASK;//Sys Clock
    PORTC_PCR6|=(PORT_PCR_MUX(1)| PORT_PCR_IRQC(0x09) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK );//GPIO | Rising Edge
    GPIOC_PDDR=0;
    NVICICPR1= 1<<(61%32);           
    NVICISER1= 1<<(61%32);
}

void PORTC_IRQHandler(void){
	PORTC_PCR6|=PORT_PCR_ISF_MASK; //CLEAN FLAG
}

__attribute__( ( always_inline ) ) __STATIC_INLINE int* __get_LR(void){ 
  __ASM volatile ("MOV %0, LR\n" : "=r" (LR_addres) ); 
 
  return(LR_addres); 
}

task* create(char priority, char autostart, void (*task_begin)(void), char state, task* next){
	task* new_task = (task*)malloc(sizeof(task));
	//task* new_task // agregado
    if(new_task == NULL)
    {
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

task* remove_front(task* head)
{
    if(head == NULL)
        return NULL;
    task* front = head;
    head = head->next;
    front->next = NULL;
    /* is this the last node in the list */
    if(front == head)
        head = NULL;
    free(front);
    return head;
}

task* remove_back(task* head)
{
    if(head == NULL)
        return NULL;
 
    task *cursor = head;
    task *back = NULL;
    while(cursor->next != NULL)
    {
        back = cursor;
        cursor = cursor->next;
    }
    if(back != NULL)
        back->next = NULL;
 
    /* if this is the last node in the list*/
    if(cursor == head)
        head = NULL;
 
    free(cursor);
 
    return head;
}

task* remove_any(task* head, task* nd)
{
	/* if the node is the first node */
    if(nd == head)
    {
        head = remove_front(head);
        return head;
    }
 
    /* if the node is the last node */
    if(nd->next == NULL)
    {
        head = remove_back(head);
        return head;
    }
 
    /* if the node is in the middle */
    task* cursor = head;
    while(cursor != NULL)
    {
    	if(cursor->next == nd){
        	break;
        }
        cursor = cursor->next;
    }
 
    if(cursor != NULL)
    {
        task* tmp = cursor->next;
        cursor->next = tmp->next;
        tmp->next = NULL;
        free(tmp);
    }
    return head;
}

task* search(task* head,char data)
{
 
    task *cursor = head;
    while(cursor!=NULL)
    {
        if(cursor->priority == data){
            return cursor;
        }
        cursor = cursor->next;
    }
    return NULL;
}


task* prepend2(task* n, task* next, char state){
    n->next= next;
    n->state = state; // ready
    return n;
}

task* prependWait(task* n, task* next){
    n->next= next;
    n->state = 2; // wait
    return n;
}

task* checkAutoStart(task* headT){
	cursorT = headT;
    task* aux = NULL;
    char as=0;
    
    while(cursorT != NULL)
    {
    	if(cursorT->autostart == 1){
    		head_ready = prepend2(cursorT, head_ready, 1);

    		aux = cursorT;
    		as=1;
    	}
    	cursorT = cursorT->next;
    	if(as){
    		headT = remove_any(headT, aux);
    		as=0;
    	}
    }
    head_iddle = headT;
    head_ready = insertion_sort(head_ready);
    return head_ready;
}

task* insertion_sort(task* head)
{
    task *x, *y, *e;
 
    x = head;
    head = NULL;
 
    while(x != NULL)
    {
        e = x;
        x = x->next;
        if (head != NULL)
        {
            if(head->priority > e->priority)
            {
                y = head;
                while ((y->next != NULL) && (e->priority < y->next->priority))
                {
                    y = y->next;
                }
                e->next = y->next;
                y->next = e;
            }
            else
            {
                e->next = head;
                head = e ;
            }
        }
        else
        {
            e->next = NULL;
            head = e ;
        }
    }
    return head;
}

void activateTask(task* new)
{
	head_ready = prepend2(new, head_ready, 1);
	head_iddle = remove_any(head_iddle, new);
	head_ready = insertion_sort(head_ready);
	if (taskRunning != NULL)
	{
		if (head_ready->priority > taskRunning->priority)
		{
			taskRunning->return_addr = __get_LR(); //LR
			head_wait = prepend(taskRunning->priority, 0, taskRunning->task_begin, 3, head_wait);
			taskRunning = head_ready;
			run(taskRunning);
		}
	}
	else{
		taskRunning = head_ready;
		taskRunning->state = 2;
		head_ready = remove_any(head_ready, taskRunning);
		run(taskRunning);
	}
}

void terminateTask(){
	if(taskRunning!=NULL){
		taskRunning->state = 0;
		//head_iddle = prepend(taskRunning->priority, 0, taskRunning->task_begin, 0, head_iddle);
		head_iddle = prepend2(taskRunning, head_iddle, 0);
		taskRunning = NULL;
	}else{
		if(head_wait != NULL){
			run_wait(head_wait); // transition from wait to run
			
		}
	}
	/*if (head_wait != NULL && head_ready == NULL)
	{
		taskRunning = head_wait;
		head_wait= remove_front(head_wait);
		taskRunning->state = 2;
		runWait();
	}*/
	
}



void run(task* n){
	head_ready = remove_any(head_ready, n);
	taskRunning=n;
	n->state=2; //running
	n->task_begin();
}

void run_wait(task* n){
	head_wait= remove_any(head_wait,n);
	taskRunning=n;
	n->state=2; //running
	
	//volver a link register de n
}

void chainTask(task* n){
	terminateTask();
	activateTask(n);
}

void systemInit(void){
	task_A = prepend(1, 1, apuntador_tarea_A, 0, task_A);
	task_B = prepend(2, 0, apuntador_tarea_B, 0, task_A);
	task_C = prepend(3, 0, apuntador_tarea_C, 0, task_B);
	head_iddle = task_C;
	run(checkAutoStart(head_iddle));
	while(taskRunning!=NULL){
		terminateTask(taskRunning);
	}
}

void taskA(void){
	activateTask(task_B);
	terminateTask();
}

void taskB(void){
	chainTask(task_C);
}

void taskC(void){
	terminateTask();
}

int main(int argc, char const *argv[])
{
	PORTC_init();
	systemInit();
	
	return 0;
}
