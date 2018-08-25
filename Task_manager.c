#include <stdio.h> 
//#include <unistd.h> 
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


void activateTask(task* n);
void terminateTask();	
task* create(char priority, char autostart, void (*task_begin)(void), char state, task* next);
task* prepend(char priority, char autostart, void (*task_begin)(void), char state, task* head);
task* checkAutoStart(task* headT);
task* insertion_sort(task* head);
void taskA(void);
void taskB(void);
void taskC(void);
void taskD(void);
void taskE(void);
void taskF(void);
typedef void (*callback)(task* data);
//void execute(task* head, callback f);
void execute(task* head);
void (*apuntador_tarea_A)(void) = taskA;
void (*apuntador_tarea_B)(void) = taskB;
void (*apuntador_tarea_C)(void) = taskC;

/********************************************************************************************************/
task* head_tasks = NULL;
task* head_auto = NULL;
task* taskRunning = NULL;
callback activate_task = activateTask;
task* tmp = NULL;
task* task_A = NULL;
task* task_B = NULL;
task* task_C = NULL;

/********************************************************************************************************/

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
	char counter=0;
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
        	printf("Elemento borrado con prioridad %d\n", cursor->priority);
            return cursor;
        }
        cursor = cursor->next;
    }
    return NULL;
}

task* checkAutoStart(task* headT){
	task* cursorT = headT;
    //task* headA = NULL;
    task* aux = NULL;
    char as=0;
    
    while(cursorT != NULL)
    {
    	if(cursorT->autostart == 1){
    		head_auto = prepend(cursorT->priority, cursorT->autostart, cursorT->task_begin, 1, head_auto);
    		aux = cursorT;
    		as=1;
    	}
    	cursorT = cursorT->next;
    	if(as){
    		headT = remove_any(headT, aux);
    		as=0;
    	}
    }
    head_tasks = headT;
    head_auto = insertion_sort(head_auto);
    return head_auto;
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

//void execute(task* head, callback f)
void execute(task* head)
{
	head->task_begin();
	/*
    task* cursor = head;
    while(cursor != NULL)
    {
    	f(cursor);
    	cursor = cursor->next;
        
    }*/
}

void activateTask(task* n)
{
	n->state = 1;
	//n->task_begin();
	execute(n);
}

void terminateTask(){
	//prepend(taskRunning->priority,0,taskRunning->task_begin,0,head_tasks);
}

void chainTask(task* n){
	//terminateTask();
	activateTask(n);
	//execute(n);
}

void taskA(void){
	printf("Soy tarea A \n");
	activate_task(task_B);
	//head_auto = insertion_sort(head_auto);
}

void taskB(void){
	printf("Soy tarea B \n");
	chainTask(task_C);
}

void taskC(void){
	printf("Soy tarea C \n");
	//terminateTask();
}

int main(int argc, char const *argv[])
{
	task_A = prepend(1, 1, apuntador_tarea_A, 0, task_A);
	task_B = prepend(2, 0, apuntador_tarea_B, 0, task_A);
	task_C = prepend(3, 0, apuntador_tarea_C, 0, task_B);
	head_tasks = task_C;

	printf("Lista de autostart \n");
	execute(checkAutoStart(head_tasks));
	//printf("Lista principal \n");	
	//execute(head_tasks, activate_task);
	return 0;
}
