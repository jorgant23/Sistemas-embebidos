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

task* tarea_actual; // 0:A, 1:B, 2:C

void activateTask(task* n);
task* terminateTask();
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
void execute(task* head, callback f);
void (*apuntador_tarea_A)(void) = taskA;
void (*apuntador_tarea_B)(void) = taskB;
void (*apuntador_tarea_C)(void) = taskC;
void (*apuntador_tarea_D)(void) = taskD;
void (*apuntador_tarea_E)(void) = taskE;
void (*apuntador_tarea_F)(void) = taskF;

void activateTask(task* n)
{
	//n->state = 1;
	n->task_begin();
	//checkAutoStart(head);
}

task* terminateTask(){
	tarea_actual->state= 0;
	return tarea_actual;
}

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

task* remove_any(task* head)
{
	task* nd = head->next;
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
        if(cursor->next = nd)
            break;
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

task* checkAutoStart(task* headT){
	task* cursorT = headT;
    task* headA = NULL;
    
    while(cursorT != NULL)
    {
    	if(cursorT->autostart == 1){
    		headA = prepend(cursorT->priority, cursorT->autostart, cursorT->task_begin, 1, headA);
    	}
    	cursorT = cursorT->next;
    }
    headA = insertion_sort(headA);
    return headA;
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

void execute(task* head, callback f)
{
    tarea_actual = head;
    while(tarea_actual != NULL)
    {
    	f(tarea_actual);
    	tarea_actual = tarea_actual->next;
        
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

void taskD(void){
	printf("Soy tarea D \n");
}

void taskE(void){
	printf("Soy tarea E \n");
}

void taskF(void){
	printf("Soy tarea F \n");
}

int main(int argc, char const *argv[])
{
	callback activate_task = activateTask;
	
	task* head = NULL;
	task* task_A = NULL;
	task* task_B = NULL;
	task* task_C = NULL;
	task* task_D = NULL;
	task* task_E = NULL;
	task* task_F = NULL;
	task_A = prepend(1, 0, apuntador_tarea_A, 0, task_A);
	task_B = prepend(3, 1, apuntador_tarea_B, 0, task_A);
	task_C = prepend(2, 1, apuntador_tarea_C, 0, task_B);
	task_D = prepend(4, 0, apuntador_tarea_D, 0, task_C);
	task_E = prepend(6, 1, apuntador_tarea_E, 0, task_D);
	task_F = prepend(5, 1, apuntador_tarea_F, 0, task_E);
	head = task_F;
	printf("Lista principal \n");
	execute(head, activate_task);
	printf("Lista ordenada \n");
	execute(checkAutoStart(head) , activate_task);
	printf("Elemento borrado \n");
	//remove_front(task_F);
	//remove_back(task_A);	
	execute(head, activate_task);
	printf("%p\n",task_A->next);
	return 0;
}
