#include <stdio.h>
#include <stdlib.h>
#define SIZE_OF_HEAP 3000
#define N 3
#define SIZE 8

typedef struct meta_block
{
    size_t size;
    struct meta_block *next;
} M_Block;

typedef struct node
{
    int val;
    int flag;
    int RC;
    struct node *l[N];
} Node;

M_Block *heap_start = NULL;
M_Block *free_list = NULL;
Node *ptr[SIZE];

void init_heap()
{
    heap_start = malloc(SIZE_OF_HEAP); // size of my heap
    free_list = heap_start;            // at the start free_list is pointing to heap(as heap is empty at the start)
    free_list->size = (SIZE_OF_HEAP) - sizeof(M_Block);
    free_list->next = NULL;
}

void *allocate_block(size_t size)
{
    if (heap_start == NULL)
        init_heap(); // initializing the heap and free_list

    M_Block *prev = NULL;
    M_Block *ptr = free_list;

    while (ptr != NULL)
    {
        if (ptr->size >= size) // block of size grater than requested size is found
        {
            // printf("block found and memory allocated\n");
            /*splitting the free block since the requested size is small we cannot allocate the
            total free block to it(to avoid internal fragmentation)*/
            if ((ptr->size) - size > sizeof(M_Block))
            {
                M_Block *new_splitted_block = (M_Block *)((void *)ptr + size + sizeof(M_Block));
                new_splitted_block->size = (ptr->size) - size - sizeof(M_Block);
                new_splitted_block->next = ptr->next;
                ptr->size = size;
                ptr->next = new_splitted_block;
                // printf("block is split\n");
            }

            if (prev == NULL)
            {
                free_list = ptr->next;
            }
            else
            {
                prev->next = ptr->next;
            }

            return (void *)(ptr + 1); // returning the pointer which points to the allocated space of heap and not to the meta_data
        }
        // printf("one free block checked\n");
        prev = ptr;
        ptr = ptr->next;
    }
    // if any free block is not found in the free list which satisfies the given request it returns NULL
    printf("unable to allocate memory\n");
    return NULL;
}

void free_block(void *ptr)
{
    if (ptr == NULL)
        return;

    M_Block *block = ptr;
    --block; // pointing block to meta data
    M_Block *curr = free_list;
    M_Block *prev = NULL;

    /*since we need to find where the newly freed block should be inserted
      in the free_list we have to traverse the free_list*/
    while (curr != NULL && curr < block)
    {
        prev = curr;
        curr = curr->next;
    }

    /* check if previous block and newly freed block are adjacent
        if they are adjacent merge them both by updating the size */
    if (prev != NULL && ((M_Block *)((void *)prev + sizeof(M_Block) + prev->size) == block))
    {
        printf("block is merged with previous block in free list\n");
        prev->size += sizeof(M_Block) + block->size;
        block = prev;
    }
    /* check if next block and newly freed block are adjacent
        if they are adjacent merge them both by updating the size */
    if (curr != NULL && ((M_Block *)((void *)block + sizeof(M_Block) + block->size)) == curr)
    {
        printf("block is merged with the next block in the free list\n");
        block->size += sizeof(M_Block) + curr->size;
        block->next = curr->next;
        if (prev != NULL && prev != block)
        {
            prev->next = block;
        }
        if (prev == NULL)
            free_list = block;
    }
    else
    {
        // insert newly freed block in the free_list
        block->next = curr;
        if (prev == NULL)
            free_list = block;
        else if (prev != block)
            prev->next = block;
    }
    printf("block of size %zu is freed and added to free list\n", block->size);
}

void Display_free_list()
{
    M_Block *ptr = free_list;
    printf("BLOCK_SIZE\tADDRESS");
    while (ptr)
    {
        printf("\n%ld\t\t%p", ptr->size, ptr);
        ptr = ptr->next;
    }
    printf("\n");
}

Node *create(int data, Node *l1, Node *l2, Node *l3)
{
    Node *new_node = (Node *)allocate_block(sizeof(Node));
    new_node->val = data;
    new_node->flag = 0;
    new_node->RC = 0;
    new_node->l[0] = l1;
    new_node->l[1] = l2;
    new_node->l[2] = l3;

    if (l1 != NULL)
        l1->RC += 1;
    if (l2 != NULL)
        l2->RC += 1;
    if (l3 != NULL)
        l3->RC += 1;

    return new_node;
}

/*void setlink(int source, int l1, int l2, int l3)
{
    if (l1 != -1)
    {
        ptr[source]->l[0] = ptr[l1];
        ptr[l1]->RC += 1;
    }
    if (l2 != -1)
    {
        ptr[source]->l[1] = ptr[l2];
        ptr[l2]->RC += 1;
    }
    if (l3 != -1)
    {
        ptr[source]->l[2] = ptr[l3];
        ptr[l3]->RC += 1;
    }
}*/

void print_graph()
{
    for (int i = 0; i < SIZE; i++)
    {
        if (ptr[i] != NULL)
        {
            printf("Node Value:%d  Its connections: ", ptr[i]->val);
            for (int j = 0; j < N; j++)
            {
                if (ptr[i]->l[j] != NULL)
                {
                    printf("%d ", ptr[i]->l[j]->val);
                }
            }
            printf("NULL\n");
        }
    }
}

void print_node(Node *root)
{
    if (root != NULL)
    {
        printf("%d(connected_nodes=%d) ", root->val, root->RC);
        print_node(root->l[0]);
        print_node(root->l[1]);
        print_node(root->l[2]);
    }
}

void adjacency_matrix()
{
    int matrix[SIZE + 1][SIZE + 1];
    for (int i = 0; i < (SIZE + 1); i++)
    {
        for (int j = 0; j < (SIZE + 1); j++)
        {
            matrix[i][j] = 0;
        }
    }

    for (int i = 0; i < (SIZE + 1); i++)
    {
        for (int j = 0; j < (SIZE + 1); j++)
        {
            if (i == 0 && j == 0)
            {
                matrix[i][j] = 0;
            }
            else if (j == 0 && i != 0 && ptr[i - 1] != NULL)
            {
                matrix[i][j] = ptr[i - 1]->val;
            }
            else if (i == 0 && j != 0 && ptr[j - 1] != NULL)
            {
                matrix[i][j] = ptr[j - 1]->val;
            }
            else if (ptr[j - 1] != NULL && ptr[i - 1] != NULL)
            {
                if (ptr[i - 1]->l[0] != NULL)
                {
                    if (ptr[i - 1]->l[0]->val == ptr[j - 1]->val && i != j)
                    {
                        matrix[i][j] = 1;
                    }
                }
                if (ptr[i - 1]->l[1] != NULL)
                {
                    if (ptr[i - 1]->l[1]->val == ptr[j - 1]->val && i != j)
                    {
                        matrix[i][j] = 1;
                    }
                }
                if (ptr[i - 1]->l[2] != NULL)
                {
                    if (ptr[i - 1]->l[2]->val == ptr[j - 1]->val && i != j)
                    {
                        matrix[i][j] = 1;
                    }
                }
            }
            else
            {
                matrix[i][j] = -1;
            }
        }
    }
    for (int i = 0; i < (SIZE + 1); i++)
    {
        if (matrix[i][0] != -1)
        {
            for (int j = 0; j < (SIZE + 1); j++)
            {
                if (matrix[i][j] != -1)
                    printf("%d ", matrix[i][j]); // printing the adjacency matrix
            }
            printf("\n");
        }
    }
}

/*void garbage_collection_rc()
{
    for(int i=0;i<SIZE;i++)
    {
        if(ptr[i]->RC==0)
        {
            for(int j=0;j<N;j++)
            {
                if(ptr[i]->l[j]!=NULL)
                {
                    ptr[i]->l[j]->RC -= 1;
                }
            }
            printf("Garbage:\n");
            printf("value:%d ",ptr[i]->val);
            free_block(ptr[i]);
            ptr[i]=NULL;
        }
    }
}*/

void garbage_collection_rc()
{
    int rc = 0;
    while (rc == 0)
    {
        rc = 1;
        for (int i = 0; i < SIZE; i++)
        {
            if (ptr[i] != NULL && ptr[i]->RC == 0)
            {
                for (int j = 0; j < N; j++)
                {
                    if (ptr[i]->l[j] != NULL)
                    {
                        ptr[i]->l[j]->RC -= 1;
                        if (ptr[i]->l[j]->RC == 0)
                        {
                            rc = 0;
                        }
                    }
                }
                // printf("Garbage:\n");
                printf("value:%d ", ptr[i]->val);
                free_block(ptr[i]);
                ptr[i] = NULL;
            }
        }
    }
}

void mark_nodes(Node *root)
{
    if (root != NULL)
    {
        mark_nodes(root->l[0]);
        mark_nodes(root->l[1]);
        mark_nodes(root->l[2]);
        root->flag = 1;
    }
}

void sweep()
{
    for (int i = 0; i < 8; i++)
    {
        if (ptr[i]->flag == 0)
        {
            if (ptr[i]->l[0] != NULL)
            {
                ptr[i]->l[0]->RC -= 1;
            }
            if (ptr[i]->l[1] != NULL)
            {
                ptr[i]->l[1]->RC -= 1;
            }
            if (ptr[i]->l[2] != NULL)
            {
                ptr[i]->l[2]->RC -= 1;
            }
            printf("Garbage:\n");
            printf("value:%d ", ptr[i]->val);
            free_block(ptr[i]);
            ptr[i] = NULL;
        }
    }
}

int main()
{
    int k;
    ptr[0] = create(10, NULL, NULL, NULL);
    ptr[1] = create(9, NULL, NULL, NULL);
    ptr[2] = create(2, NULL, NULL, NULL);
    ptr[3] = create(8, ptr[1], NULL, NULL);
    ptr[4] = create(1, ptr[2], ptr[1], ptr[0]);
    ptr[5] = create(3, ptr[3], ptr[0], NULL);
    ptr[6] = create(7, ptr[4], ptr[3], NULL);
    ptr[7] = create(5, ptr[4], NULL, NULL);

    /*int array[]={1,2,3,5,7,8,9,10};
    int i;
    for(i=0;i<8;i++)
    {
        Node* new_node =(Node*)allocate_block(sizeof(Node));
        new_node->val=array[i];
        new_node->l[0]=NULL;
        new_node->l[1]=NULL;
        new_node->l[2]=NULL;
        new_node->RC=0;
        new_node->flag=0;
        ptr[i]=new_node;
    }
    setlink(0,1,6,7);
    setlink(2,5,7,-1);
    setlink(3,0,-1,-1);
    setlink(4,0,5,-1);
    setlink(5,6,-1,-1);*/

    Node *root_1 = ptr[7];
    ptr[7]->RC += 1;
    Node *root_2 = ptr[4];
    ptr[4]->RC += 1;

    printf("All nodes through Root-1:\n");
    print_node(root_1);

    printf("\nAll nodes through Root-2:\n");
    print_node(root_2);

    printf("\nprinting all nodes and their connections\n");
    print_graph();

    printf("Adjacency Matrix\n");
    adjacency_matrix();

    printf("Enter the method you want for Garbage Collection: ");
    printf("1.Reference Counting\n2.Mark and Sweep\n");
    scanf("%d", &k);

    switch (k)
    {
    case 1:
    {
        printf("Garbage collection through Reference counting:\n");
        garbage_collection_rc();

        printf("Adjacency matrix after removal of garbage:\n");
        adjacency_matrix();
        break;
    }
    case 2:
    {
        printf("Garbage collection using mark and sweep method\n");
        mark_nodes(root_1);
        mark_nodes(root_2);
        sweep();

        printf("Adjacency matrix after removal of garbage:\n");
        adjacency_matrix();
    }
    default:
        break;
    }

    return 0;
}