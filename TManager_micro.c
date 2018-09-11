#include <stdio.h> 
#include <stdlib.h>
#define __ASM __asm 
#define __INLINE inline 
#define __STATIC_INLINE static inline 
#include "derivative.h"
#include "os.h"
#include "user.h"

__attribute__( ( always_inline ) ) __STATIC_INLINE  uint32_t* __get_LR(void){ 
	__ASM volatile ("MOV %0, LR\n" : "=r" (LR_address)); 
	return(LR_address); 
}

task* create(char priority, char autostart, void (*task_begin)(void), char state, task* next){
	task* new_task = (task*)malloc(sizeof(task));
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
	if (taskRunning != NULL)
	{
		taskRunning->return_addr = __get_LR(); 
		task* aux = new;
		head_iddle = remove_any(head_iddle, new); // aqui copia C con todo y next (C->A) y la lista iddle (B->C->A)
		head_ready = prepend2(aux, head_ready, 1);
		head_ready = insertion_sort(head_ready);
		if (head_ready->priority > taskRunning->priority)
		{
			head_ready = prepend2(taskRunning, head_ready, 1);
			head_ready = insertion_sort(head_ready);
			run(head_ready);
		}
	}
	else{
		task* aux = new;
		head_iddle = remove_any(head_iddle, new); // aqui copia C con todo y next (C->A) y la lista iddle (B->C->A)
		head_ready = prepend2(aux, head_ready, 1);
		head_ready = insertion_sort(head_ready);
		run(head_ready);
	}
}

void terminateTask(){
	if(taskRunning!=NULL){
		taskRunning->state = 0;
		head_iddle = prepend2(taskRunning, head_iddle, 0);
		taskRunning = NULL;
	}else{
		if(head_wait != NULL){
			run_wait(head_wait); // transition from wait to run

		}
	}
}

void run(task* n){
	taskRunning = n;
	head_ready = remove_any(head_ready, n);
	n->state=2; //running
	n->task_begin();
}

void run_wait(task* n){
	head_wait= remove_any(head_wait,n);
	taskRunning=n;
	n->state=2; //running
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
	while(head_ready != NULL){
		__ASM volatile ("BX %0\n": : "r" (head_ready->return_addr));
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
	systemInit();
	return 0;
}
