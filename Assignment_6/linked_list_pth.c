#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

int thread_count;
int task_count;
pthread_rwlock_t rwlock;
struct list_node_s
{
    int data;
    struct list_node_s *next;
};
struct list_node_s *head = NULL;

int Find_member(int value);
int insert(int value);
int delete(int value);
void *Task_queue(void *rank);
void *Create_list();
void sortList(struct list_node_s *head);
void printList(struct list_node_s *head);

int main(int argc, char *argv[])
{
    long thread;
    pthread_t *thread_handles;

    pthread_rwlock_init(&rwlock, NULL);

    printf("How many threads? \n");
    scanf("%d", &thread_count);
    thread_handles = malloc(thread_count * sizeof(pthread_t));
    printf("How many tasks? \n");
    scanf("%d", &task_count);

    printf("----------------------Linked List----------------------\n");

    Create_list();

    printf("\n");

    for (thread = 0; thread < thread_count; thread++)
    {
        pthread_create(&thread_handles[thread], NULL, Task_queue, (void *)thread);
    }

    for (thread = 0; thread < thread_count; thread++)
    {
        pthread_join(thread_handles[thread], NULL);
    }

    pthread_rwlock_destroy(&rwlock);
    return 0;
}

void *Task_queue(void *rank)
{
    long my_rank = (long)rank;
    int local_task_count = task_count / thread_count;
    srand(time(0) + my_rank);
    for (int i = 0; i < local_task_count; i++)
    {
        int random_task = (rand() % 3 + 1);
        int r = rand() % 100;
        switch (random_task)
        {
        case 1:
            // printList(head);
            pthread_rwlock_rdlock(&rwlock);
            int found = Find_member(r);
            pthread_rwlock_unlock(&rwlock);
            printf("Thread %ld searching %d in list:\n", my_rank, r);
            (found) ? printf("%d is a member \n", r) : printf("%d is a not member \n", r);
            break;
        case 2:
            // printList(head);
            pthread_rwlock_wrlock(&rwlock);
            insert(r);
            pthread_rwlock_unlock(&rwlock);
            printf("Thread %ld inserting %d into list:\n", my_rank, r);
            printList(head);
            break;
        case 3:
            // printList(head);
            pthread_rwlock_wrlock(&rwlock);
            int result = delete (r);
            pthread_rwlock_unlock(&rwlock);
            printf("Thread %ld performing deleting %d in List:\n", my_rank, r);
            if (result)
            {
                printList(head);
            }
            else
            {
                printf("%d isn't a node in the list \n", r);
            }
        default:
            break;
        }
    }
    return NULL;
}

void *Create_list()
{
    for (int i = 0; i < 12; i++)
    {
        int rnd = rand() % 100 + 1;
        if (!insert(rnd))
        {
            i--;
        }
    }
    printList(head);
    return NULL;
}

void printList(struct list_node_s *head)
{
    struct list_node_s *current = head;
    while (current != NULL)
    {
        printf("%d  ", current->data);
        current = current->next;
    }
    printf("NULL \n");
}

int Find_member(int value)
{
    struct list_node_s *curr_p = head;
    while (curr_p != NULL && curr_p->data < value)
        curr_p = curr_p->next;

    if (curr_p == NULL || curr_p->data > value)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

int insert(int value)
{
    struct list_node_s *curr_p = head;
    struct list_node_s *pred_p = NULL;
    struct list_node_s *temp_p = NULL;

    while (curr_p != NULL && curr_p->data < value)
    {
        pred_p = curr_p;
        curr_p = curr_p->next;
    }

    if (curr_p == NULL || curr_p->data > value)
    {
        temp_p = malloc(sizeof(struct list_node_s));
        temp_p->data = value;
        temp_p->next = curr_p;

        if (pred_p == NULL)
        {
            head = temp_p;
        }
        else
        {
            pred_p->next = temp_p;
        }
        return 1;
    }
    else
    {
        return 0;
    }
} /*insert*/

int delete(int value)
{
    struct list_node_s *curr_p = head;
    struct list_node_s *pred_p = NULL;

    while (curr_p != NULL && curr_p->data < value)
    {
        pred_p = curr_p;
        curr_p = curr_p->next;
    }

    if (curr_p != NULL && curr_p->data == value)
    {
        if (pred_p == NULL)
        {
            head = curr_p->next;
            free(curr_p);
        }
        else
        {
            pred_p->next = curr_p->next;
            free(curr_p);
        }
        return 1;
    }
    else
    {
        return 0;
    }
}