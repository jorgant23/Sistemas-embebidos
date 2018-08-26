#include <stdio.h> 
//#include <unistd.h> 
#include <stdlib.h>
#include <string.h>

typedef struct task{
	char priority;
	char autostart;
	void (*return_addr)(void);
	//float *return_addr;
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
void linkRegister(void);
typedef void (*callback)(task* data);
//void execute(task* head, callback f);
void execute(task* head);
void (*apuntador_tarea_A)(void) = taskA;
void (*apuntador_tarea_B)(void) = taskB;
void (*apuntador_tarea_C)(void) = taskC;
void (*apuntador_LR)(void) = linkRegister;
void run(task* n);
void runWait(void);
//void return_addr(void);
void systemInit(void);

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
//head_wait;
//head_ready;
//head_ready;
//head_iddle;
//head_iddle;

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

task* printTask(task* head)
{
 	task* cursor = head;
 	//printf("%p\n", cursor);
    while(cursor != NULL)
    {
    	printf("%p\n", cursor);
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
    		head_ready = prepend(cursorT->priority, cursorT->autostart, cursorT->task_begin, 1, head_ready);
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

//task* prepend(char priority, char autostart, void (*task_begin)(void), char state, task* head)
void activateTask(task* new)
{
	head_ready = prepend(new->priority, 0, new->task_begin, 1, head_ready);
	head_iddle = remove_any(head_iddle, new);
	head_ready = insertion_sort(head_ready);
	if (taskRunning != NULL)
	{
		if (new->priority > taskRunning->priority)
		{
			taskRunning->return_addr = apuntador_LR; //LR
			//taskRunning->state = 3;
			head_wait = prepend(taskRunning->priority, 0, taskRunning->task_begin, 3, head_wait);
			taskRunning = head_ready;
			taskRunning->state = 2;
			head_ready = remove_any(head_ready, taskRunning);
			run(taskRunning);
		}
	}
	else{
		taskRunning = head_ready;
		taskRunning->state = 2;
		head_ready = remove_any(head_ready, taskRunning);
		printf("La tarea actual es %p\n", taskRunning);
		run(taskRunning);
	}
}

/********************************************************************************************************/
// 							La tarea A se manda a llamar dos veces, una al iniciar el sistema y otra cuando taskRunning se iguala a la tarea A
/********************************************************************************************************/
/********************************************************************************************************/
/********************************************************************************************************/

void terminateTask(){
	taskRunning->return_addr = NULL;
	head_iddle = prepend(taskRunning->priority, 0, taskRunning->task_begin, 0, head_iddle);
	taskRunning = NULL;
	if (head_wait != NULL)
	{
		taskRunning = head_wait;
		head_wait= remove_front(head_wait);
		taskRunning->state = 2;
		runWait();
	}
	else{
		head_iddle = prepend(taskRunning->priority, 0, taskRunning->task_begin, 0, head_iddle);
		taskRunning = NULL;
	}
}



void run(task* n){
	n->task_begin();
}

void runWait(void){
	taskRunning->return_addr();
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
	printf("Apuntador A %p\n", task_A);
	printf("Apuntador B %p\n", task_B);
	printf("Apuntador C %p\n", task_C);
	printf("Antes de ejecutar \n");
	printf("Tareas de iddle \n");
	printTask(head_iddle);
	printf("Tareas de ready \n");
	printTask(head_ready);
	printf("Tareas de wait \n");
	printTask(head_wait);
	//printf("Lista de autostart \n");
	run(checkAutoStart(head_iddle));
}

void linkRegister(void){
	printf("Hola, soy el LR \n");
	printf("Tareas de iddle \n");
	printTask(head_iddle);
	printf("Tareas de ready \n");
	printTask(head_ready);
	printf("Tareas de wait \n");
	printTask(head_wait);
	//terminateTask();
}

void taskA(void){
	printf("Soy tarea A \n");
	activateTask(task_B);
	terminateTask();
	printf("Termine A \n");
	printf("Tareas de iddle \n");
	printTask(head_iddle);
	printf("Tareas de ready \n");
	printTask(head_ready);
	printf("Tareas de wait \n");
	printTask(head_wait);
}

void taskB(void){
	printf("Soy tarea B \n");
	chainTask(task_C);
}

void taskC(void){
	printf("Soy tarea C \n");
	terminateTask();
}

int main(int argc, char const *argv[])
{
	systemInit();
	
	//execute(checkAutoStart(head_iddle));
	//printf("Lista principal \n");	
	//execute(head_iddle, activate_task);
	return 0;
}
