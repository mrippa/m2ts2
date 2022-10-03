#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>

#include "mythread.h"
#include "m2ts323.h"

#define handle_error_en(en, msg) \
    do                           \
    {                            \
        errno = en;              \
        perror(msg);             \
        exit(EXIT_FAILURE);      \
    } while (0)

#define handle_error(msg)   \
    do                      \
    {                       \
        perror(msg);        \
        exit(EXIT_FAILURE); \
    } while (0)

struct thread_info
{                        /* Used as argument to thread_start() */
    pthread_t thread_id; /* ID returned by pthread_create() */
    int thread_num;      /* Application-defined thread # */
    char *argv_string;   /* From command-line argument */
};

struct thread_info *m2tinfo;

/* Thread start function: display address near top of our stack,
   and return upper-cased copy of argv_string */

static void *
thread_start(void *arg)
{
    struct thread_info *tinfo = arg;
    char *uargv, *p;
    int i;

    printf("Thread %d: top of stack near %p; argv_string=%s\n",
           tinfo->thread_num, &p, tinfo->argv_string);

    uargv = strdup(tinfo->argv_string);
    if (uargv == NULL)
        handle_error("strdup");

    for (p = uargv; *p != '\0'; p++)
        *p = toupper(*p);

    if (tinfo->thread_num == 2)
    {

        if (!c_block323.bInitialized)
            printf("\n>>> ERROR: BOARD ADDRESS NOT SET <<<\n");
        else
        {
            for (i = 0; i < 101; i++)
            {
                printf("%12.6f\n", ((((double)c_block323.s_cor_buf[0][i]) * 20.0) / (double)65536.0) + (-10.0));
            }
        }
    }
    return uargv;
}

/* M2Acquire
 *
 *
 */
static void* M2AcqAP323_run()
{

    if(adc_running)
    {
        //printf("\n>>>ERROR: thread called with ADC Running\n");
        handle_error("ADC Running");
    }
    adc_running = 1;

    if (!c_block323.bInitialized)
    {
        printf("\n>>> ERROR: BOARD ADDRESS NOT SET <<<\n");
        handle_error("ADC BOARD"); 
    }

    calibrateAP323(&c_block323, AZ_SELECT);  /* get auto-zero values */
    calibrateAP323(&c_block323, CAL_SELECT); /* get calibration values */

    if (hflag == 0 && c_block323.int_mode != 0)
    {
        handle_error("ADC NO_INT");
    }

    printf("Start M2AcqAP323_run\n");
    while (adc_running)
    {
        convertAP323(&c_block323); /* convert the board */

        mccdAP323(&c_block323); /* correct input data */
    }

    adc_running = 0;
    printf("End M2AcqAP323_run\n");

    return NULL;
}


int setM2AcqTInfo(void *arg) {

    m2tinfo = arg;

    return 0;
}

int M2AcqStart() {

    pthread_join(m2tinfo->thread_id, NULL);

    printf("M2AcqStart finished\n");

    return(0);
}

int tsetup()
{
    int s, tnum, num_threads;
    struct thread_info *tinfo;
    pthread_attr_t attr;
    int stack_size;
    void *res;

    int argc = 3;
    char *argv[] = {"t0", "t1", "t2"};

    /* The "-s" option specifies a stack size for our threads */

    stack_size = -1;

    num_threads = argc - optind;
    num_threads += 1; /* Add one more thread for m2 acquisition*/

    /* Initialize thread creation attributes */
    s = pthread_attr_init(&attr);
    if (s != 0)
        handle_error_en(s, "pthread_attr_init");

    if (stack_size > 0)
    {
        s = pthread_attr_setstacksize(&attr, stack_size);
        if (s != 0)
            handle_error_en(s, "pthread_attr_setstacksize");
    }

    /* Allocate memory for pthread_create() arguments */

    tinfo = calloc(num_threads, sizeof(struct thread_info));
    if (tinfo == NULL)
        handle_error("calloc");

    /* Create one thread for each command-line argument */

    for (tnum = 0; tnum < num_threads; tnum++)
    {
        tinfo[tnum].thread_num = tnum + 1;
        tinfo[tnum].argv_string = argv[optind + tnum];

        /* The pthread_create() call stores the thread ID into
           corresponding element of tinfo[] */

        s = pthread_create(&tinfo[tnum].thread_id, &attr,
                           &thread_start, &tinfo[tnum]);
        if (s != 0)
            handle_error_en(s, "pthread_create");
    }

    /*Create a final thread for acquisiton. tnum is the last thread*/
    s = pthread_create(&tinfo[tnum].thread_id, &attr, &M2AcqAP323_run, NULL);

    if (s != 0)
        handle_error_en(s, "pthread_create m2Acq");

    setM2AcqTInfo(&tinfo[tnum]);

    /* Destroy the thread attributes object, since it is no
       longer needed */
    s = pthread_attr_destroy(&attr);
    if (s != 0)
        handle_error_en(s, "pthread_attr_destroy");

    /* Now join with each thread, and display its returned value */

    for (tnum = 0; tnum < num_threads - 1; tnum++) /* Don't start the final m2Acq thread yet*/
    {

        printf("Starting tnum... %d\n", tnum);
        s = pthread_join(tinfo[tnum].thread_id, &res);
        if (s != 0)
            handle_error_en(s, "pthread_join");

        printf("Joined with thread %d; returned value was %s\n",
               tinfo[tnum].thread_num, (char *)res);
        free(res); /* Free memory allocated by thread */
    }

    free(tinfo);

    return 0;
}

#ifndef MATT_TEST
int main()
{

    tsetup();
    printf("Hello world\n");

    exit(EXIT_SUCCESS);
}
#endif
