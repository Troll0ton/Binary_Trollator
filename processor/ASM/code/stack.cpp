#include "ASM/include/stack.h"

//-----------------------------------------------------------------------------

int stack_ctor_ (struct Stack *stk,     int capacity_ctor, const char* stk_name,
                  const char* file_name, int stk_line                           )
{
    stk->canary_left  = LEFT_CANARY;
    stk->canary_right = RIGHT_CANARY;
    stk->capacity_stk = capacity_ctor;
    stk->size_stk = 0;

    (stk->Stack_info).name = stk_name;
    (stk->Stack_info).file = file_name;
    (stk->Stack_info).line = stk_line;
    (stk->Stack_info).error_codes = 0;
    (stk->Stack_info).cur_status = "OK";

    if (capacity_ctor >= 0)
    {
        stk->buffer_stk = (double*) calloc (1, capacity_ctor * sizeof (double));
    }

    stk->hash = calculate_hash (stk);
    verificate_stack (stk, stk->hash);

    return 0;
}

//-----------------------------------------------------------------------------

void stack_dtor (struct Stack *stk)
{
    int32_t new_hash = calculate_hash (stk);

    verificate_stack (stk, new_hash);

    free (stk->buffer_stk);

    stk->hash = -1;
}

//-----------------------------------------------------------------------------

void stack_push (struct Stack *stk, double elem)
{
    int32_t new_hash = calculate_hash (stk);

    verificate_stack (stk, new_hash);

    if(stk->size_stk + 1 > stk->capacity_stk)
    {
        stack_resize (stk, stk_increase);
    }

    stk->buffer_stk[stk->size_stk] = elem;

    (stk->size_stk)++;

    stk->hash = calculate_hash (stk);
}

//-----------------------------------------------------------------------------

void stack_resize (struct Stack *stk, int opt_resize)
{
    if (opt_resize == stk_increase)
    {
        if(stk->capacity_stk == 0)
        {
            stk->capacity_stk = 2;
        }

        stk->capacity_stk *= 2;

        stk->buffer_stk = (double*) recalloc (stk->buffer_stk, stk->capacity_stk, stk->size_stk, sizeof (double));
    }

    else if (opt_resize == stk_decrease)
    {
        if(stk->capacity_stk >= 2)
        {
            stk->capacity_stk /= 2;
            stk->buffer_stk = (double*) recalloc (stk->buffer_stk, stk->capacity_stk, stk->size_stk, sizeof (double));
        }
    }

    stk->hash = calculate_hash (stk);
}

//-----------------------------------------------------------------------------

double stack_pop (struct Stack *stk)
{
    int32_t new_hash = calculate_hash (stk);

    verificate_stack (stk, new_hash);

    double elem_del = stk->buffer_stk[stk->size_stk - 1];
    stk->buffer_stk[stk->size_stk - 1] = 0;

    stk->size_stk--;

    if(stk->size_stk <= stk->capacity_stk / 4)
    {
        stack_resize (stk, stk_decrease);
    }

    stk->hash = calculate_hash (stk);

    return elem_del;
}

//-----------------------------------------------------------------------------

void verificate_stack (struct Stack *stk, int32_t new_hash)
{
    check_errors (stk, new_hash);

    if ((stk->Stack_info).error_codes != 0)
    {
        (stk->Stack_info).cur_status = "ERROR";
    }
}

//-----------------------------------------------------------------------------

int32_t calculate_hash (struct Stack *stk)
{
    int32_t sum = 0;
    int32_t hash_par = 17;

    for(int i = 0; i < stk->capacity_stk; i++)
    {
        sum += (int32_t)(stk->buffer_stk[i] * 100 * hash_par);
        sum %= UINT_MAX;
        hash_par *= 17;
    }

    return sum;
}

//-----------------------------------------------------------------------------

void debug_stack (struct Stack *stk)
{
    handle_errors  (stk);
    stack_dump_ (stk);
}

//-----------------------------------------------------------------------------

void handle_errors (struct Stack *stk)
{
    if((stk->Stack_info).error_codes == 0)
    {
        printf ("OK\n");
    }

    else
    {
        for(int i = 0; i < NUM_OF_MIS; i++)
        {
            if((stk->Stack_info).error_codes & error_arr[i].error_code)
            {
                printf ("%d %s\n", error_arr[i].error_code, error_arr[i].error_output);
            }
        }
    }
}

//-----------------------------------------------------------------------------

void stack_dumps (struct Stack *stk_, FILE *log_file)
{
    fprintf (log_file,
             "%s[%p](%s) at %s, LINE - %d \n"
             "{                           \n"
             "    canary_open  = %x       \n"
             "    canary_close = %x       \n"
             "    hash         = %x       \n"
             "    size_stk     = %d       \n"
             "    capacity_stk = %d       \n"
             "                            \n"
             "    buffer_stk[%p]          \n"
             "    {                       \n",
             (stk_->Stack_info).name + 1,    stk_,
             (stk_->Stack_info).cur_status, (stk_->Stack_info).file,
             (stk_->Stack_info).line,
             stk_->canary_left,
             stk_->canary_right,
             stk_->hash,
             stk_->size_stk,
             stk_->capacity_stk,
             stk_->buffer_stk);

        for(int pos = 0; pos < stk_->capacity_stk; pos++)
        {
            fprintf (log_file,
                 "          [%d] = %lg        \n",
                 pos, stk_->buffer_stk[pos]);
        }

        fprintf (log_file,
             "    }                       \n"
             "}                           \n\n");

        fprintf (log_file, "_____________________________________________________________\n\n");
}

//-----------------------------------------------------------------------------

void check_errors (struct Stack *stk, int32_t new_hash)
{
    if(stk->canary_left != LEFT_CANARY)    (stk->Stack_info).error_codes |= ERR_CAN_1;
    if(stk->canary_right != RIGHT_CANARY)  (stk->Stack_info).error_codes |= ERR_CAN_2;
    if(stk->buffer_stk == NULL)            (stk->Stack_info).error_codes |= ERR_MEMBUF;
    if(stk->capacity_stk < stk->size_stk)  (stk->Stack_info).error_codes |= ERR_OVERF;
    if(stk->capacity_stk < 0)              (stk->Stack_info).error_codes |= ERR_CAP;
    if(stk->size_stk < 0)                  (stk->Stack_info).error_codes |= ERR_SIZE;
    if(stk == NULL)                        (stk->Stack_info).error_codes |= ERR_MEMSTK;
    if(stk->hash != new_hash)              (stk->Stack_info).error_codes |= ERR_HASH;
}

//-----------------------------------------------------------------------------

void stack_dump_ (struct Stack *stk)
{
    FILE *log_file = fopen ("log.txt", "w+");

    fprintf (log_file,
             "%s[%p](%s) at %s, LINE - %d \n"
             "{                           \n"
             "    canary_open  = %x       \n"
             "    canary_close = %x       \n"
             "    hash         = %x       \n"
             "    size_stk     = %d       \n"
             "    capacity_stk = %d       \n"
             "                            \n"
             "    buffer_stk[%p]          \n"
             "    {                       \n",
             (stk->Stack_info).name + 1,    stk,
             (stk->Stack_info).cur_status, (stk->Stack_info).file,
             (stk->Stack_info).line,
             stk->canary_left,
             stk->canary_right,
             stk->hash,
             stk->size_stk,
             stk->capacity_stk,
             stk->buffer_stk);

    for(int pos = 0; pos < stk->capacity_stk; pos++)
    {
        fprintf (log_file,
                 "          [%d] = %lg        \n",
                 pos, stk->buffer_stk[pos]);
    }

    fprintf (log_file,
             "    }                       \n"
             "}                           \n");

    fclose (log_file);
}

//-----------------------------------------------------------------------------

void *recalloc (void *buffer, int capacity, int size, int size_of_type)
{
    char *pointer = (char*) realloc ((char*) buffer, capacity * size_of_type);

    if(capacity > size)
    {
        memset (pointer + size * size_of_type, 0, (capacity - size) * size_of_type);
    }

    return (void*) pointer;
}

//-----------------------------------------------------------------------------


