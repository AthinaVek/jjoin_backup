#ifndef master_H_
#define master_H_
    #include <stdio.h>
    #include <stdlib.h> // malloc, (s)rand etc.
    #include <stdint.h>
    #include <string.h>
    #include <stdarg.h>
    #include <inttypes.h>
    #include <time.h>
    #include <math.h>
    #include <assert.h>
    #include <sys/stat.h>  // stat
    #include <sys/mman.h>  // mmap
    #include <fcntl.h>     // open
    #include <unistd.h>    // close
    #include <pthread.h>

    // sorting operation definitions
    #define BITS 64             // size of our data(64-bit unsigned integers)
    #define CHUNK_SIZE 8
    #define FIRST_CHUNK  ((BITS/CHUNK_SIZE)-1)
    #define MAX_BUCKET_SIZE (BITS * 1024)

    // random data generation definitions
    #define DEFAULT_TABLE_SIZE 5
    #define TABLES 2

    // merge-join operation definitions
    #define JOIN_FACTOR 0.1
    #define PRECISION 100
    #define MAX_TABLE_SIZE (1024 * 1024)
    #define OUT "out.csv"
    #define LOG "log.txt"

    // utilities
    #define MAX(a,b) ((a) > (b) ? (a) : (b))

    #define MAX_PATH 512
    #define PATH_TERM "done"
    #define QUERY_DELIM "|"

    #define MATRIX_INITIAL_CAPACITY 4
    #define SORTED 1
    #define NOT_SORTED UINT64_MAX



    #define MAX_QUERY 100
    #define REL_DELIM " "
    #define PRED_DELIM "&"
    #define PROJ_DELIM " "
    #define FILTERS 3
    #define BATCH_TERM "F"

    #define NUM_OF_THREADS 8

    #define LIST_START 0
    #define LIST_END 1

    #define VECTOR_STARTING_SIZE 2

    typedef struct vector64{
        uint64_t next;
        uint64_t size;
        uint64_t *data;
    }vector64_t;

    int vector_init(void *);
    int vector_insert(void *, uint64_t);
    int vector_delete(void *, uint64_t);
    int vector_isfull(void *);
    int vector_print(const void *);
    int vector_free(void *);

    typedef struct tuple{
        uint64_t key;
        uint64_t payload;
    }tuple_t;

    typedef struct field{
        char *name;
        size_t size;
    }field_t;

    typedef struct matrix{
        tuple_t **data;
        tuple_t *col_map;
        uint64_t rows;
        uint64_t cols;
    }matrix_t;

    typedef struct header{
        field_t *fields;
        uint8_t size;
    }header_t;

    typedef struct table{

        // data
        header_t header;
        tuple_t *tuples;

        // meta-data
        uint64_t next;
        uint64_t size;
        char *name;
    }table_t;

    typedef struct relation{
        uint64_t rows;
        uint64_t cols;
        uint64_t *data;
    }rel_t;

    typedef struct node{
        void *data;
        struct node *next;
        struct node *prev;
    }node_t;

    typedef struct list{

        // field type properties
        node_t* head;
        node_t* tail;
        uint64_t length;
        char *name;
        size_t node_size;

        // method type properties
        int (*init)(void*, va_list);
        int (*print)(void*);
        int (*free_data)(void *);
    }list_t;


    typedef struct pred{
        tuple_t left;
        tuple_t right;
        char operation;
    }pred_t;

    typedef struct query{
        rel_t **rels;
        uint64_t total;
        list_t preds;
        list_t projs;
    }query_t;


    typedef struct job_scheduler job_scheduler;

    typedef struct query_job {
        query_t *query;
        job_scheduler *js;
    }query_job;

    typedef query_job queueType;

    typedef struct {
        int front;
        int back;
        int size;
        queueType** buffer;
    } queue_t;

    typedef struct job_scheduler {
        char bar;
        char done;
        size_t num_of_threads;
        pthread_t *threads;
        pthread_mutex_t queueLock;
        pthread_cond_t cond_nonempty;
        pthread_cond_t cond_empty;
        pthread_barrier_t pbar;
        queue_t q;
    } job_scheduler;


    // declarations of member functions
    int tuple_init(void *);
    void tuple_set(void*, uint64_t, uint64_t);
    int tuple_free(void *);
    int tuple_copy(const void*, void*);
    int tuple_swap(void*, void*);
    int tuple_compare(const void*, const void*);
    int tuple_print(void *);
    int tuple_isempty(const void *);
    int tuple_randomize(void *);
    int tuple_cmp(const void *, const void*);

    #include <errno.h>

    // bit manipulation
    uint64_t rand_bits();
    uint64_t get_chunk(const uint64_t, const uint8_t, const uint8_t);
    uint64_t nearest_power_of2(uint64_t , uint8_t);
    int is_power_of2(uint64_t);

    // memory management
    int64_t line_counter(FILE *);
    int reset(void *);
    void memswap(void *, void *, size_t);

    // string related utilities
    int string_rename(char **, const char*);
    int string_count(char *, char);
    int fgets_crop(char *, unsigned int, FILE*);
    char *find_char(char *, char *, int);

    // error handling
    int perror_return(const char*, int);
    int error_return(int, const char *, ...);

    // data structures definitions


    // member methods
    int table_init_wrap(void *, va_list);
    int table_init(void *, const uint64_t, const char *, const header_t *);
    int table_insert(void *, tuple_t *);
    int table_is_empty(const table_t *);
    int table_is_full(const void *);
    int table_clear(table_t *);
    int table_print(void *);
    int table_free(void *);
    int table_equals(const table_t *, const table_t *);
    int table_clone(const table_t*, table_t *);
    int table_copy(const table_t*, const tuple_t *, table_t*, uint64_t);
    int table_map(table_t *, int (*)(void *));

    // helper methods
    int table_randomize(table_t *);
    int table_by_row(table_t *);
    int table_reset(table_t *, const char *, const header_t *);
    int table_reducer(const void*, void*, ...);
    int table_to_mx(table_t*, matrix_t *, uint64_t, tuple_t*, uint64_t);
    int mx_to_table(matrix_t*, table_t *, uint64_t, tuple_t*, uint64_t);

    // sort-merge-join method declarations
    int table_sort_wrapper(table_t *);
    int table_sort(table_t*, table_t *, const tuple_t*, tuple_t*); // radix
    int table_split(table_t *, table_t *, const table_t *, const tuple_t*, const tuple_t *, const uint64_t); // radix
    int table_quick_sort(table_t *, const tuple_t *);
    uint64_t table_partition(table_t *, const tuple_t *);

    int set_histogram(const void *, table_t *, const tuple_t*, const tuple_t*, uint64_t);
    int set_prefix_sum(table_t *, uint64_t *);

    // matrix-integration methods
    int mx_radix(matrix_t *, uint64_t);
    int matrix_sort(matrix_t *, table_t *, uint64_t, const tuple_t *, tuple_t *);
    int matrix_split(matrix_t *, table_t *, uint64_t, const table_t *,
                     const tuple_t*, const tuple_t *, const uint64_t);



    // member methods
    int rel_init(rel_t *, const char *);
    int rel_init_wrap(void *, va_list);
    int rel_print_by_row(void *);
    int rel_reduce(const rel_t *, void *, uint64_t, int (*)(void *, const void *));

    // utilities
    int rel_add(void *, const void *);





    // member methods
    int list_init(list_t*, const char *, const size_t,
                 int (*)(void*, va_list),// tableconstructor
                 int (*)(void*), int (*)(void*));
    int list_insert(list_t*, uint8_t, ...);
    int list_add_start(list_t *, node_t *);
    int list_add_end(list_t *, node_t *);
    int list_is_empty(const list_t *);
    int list_print(void *);
    int list_free(void *);

    void *list_get(list_t*, unsigned int);
    void list_sort(void *, int (*)(const void*, const void*));
    int list_reduce(const list_t *, void *, int (*)(const void*, void*, ...));
    node_t *nodelist_mergesort(node_t *, int (*)(const void *, const void *));
    node_t *nodelist_merge(node_t *, node_t *, int (*)(const void *, const void *));
    void node_inresult(node_t **, node_t **);


    // projection member methods
    int proj_init(void *, va_list);
    int proj_print(void*);

    // predicate member methods
    int pred_init(void *, va_list);
    int pred_print(void*);
    int pred_cmp(const void *, const void *);

    // query member methods
    int query_init(void *, va_list);
    int query_print(void *);
    int query_destroy(void *);

    // utility methods
    int is_join(const char*);
    int is_pred_join(const pred_t*);

    // operations
    int query_exec(void *);
    int query_filter(query_t *, pred_t *, matrix_t*);
    int query_filter_all(rel_t *, pred_t *, matrix_t*);
    int query_filter_existing(query_t *, pred_t *, matrix_t*);
    int query_join_existing(query_t *, pred_t *, matrix_t*);
    int filter_pass(uint64_t, uint64_t, char);
    int query_join(query_t *, pred_t *, matrix_t*);
    uint64_t query_projection(query_t *, tuple_t *, matrix_t*, uint8_t);
    uint64_t pred_value(pred_t *, rel_t *, uint64_t);


    void queue_create(queue_t *q, int size);

    void queue_destroy(queue_t *q);

    int queue_empty(queue_t *q);

    int queue_full(queue_t *q);

    int queue_push(queue_t *q, queueType *element);

    queueType *queue_pop(queue_t *q);

    void job_scheduler_threadWork(job_scheduler *js, void *arg);
    /**
     * Initializes the JobScheduler with the number of open threads.
     * Returns true if everything done right false else.
     */
    char job_scheduler_init(job_scheduler *js);

    /**
     * Free all resources that the are allocated by JobScheduler
     * Returns true if everything done right false else.
     */
    char job_scheduler_destroy(job_scheduler *js);

    /**
     * Waits until executed all jobs in the queue.
     */
    void job_scheduler_barrier(job_scheduler *js);

    /**
     * Add a job in the queue and returns a JobId
     */
    int job_scheduler_schedule(job_scheduler *js, query_job *qj);

    /**
     * Waits until all threads finish their job, and after that close all threads.
     */
    void job_scheduler_stop(job_scheduler *js);

    void query_job_init(query_job *qj, query_t *query, void *js);

    int query_job_run(query_job *qj);


    int list_foreach(list_t *list, int (*)(void *, job_scheduler *), job_scheduler *js);
    int list_foreach_query(list_t *list, job_scheduler *js);

    // batch management methods
    int batch_list_init(void *, va_list);
    int batch_list_populate(void *, void *);
    int batch_exec(void *, job_scheduler *);



    // member methods
    int matrix_init(void *, uint64_t);
    int matrix_set(void *, tuple_t*, uint64_t, uint64_t);
    int matrix_setrow(void *, tuple_t *, uint64_t);
    tuple_t * matrix_get(void *, uint64_t, uint64_t);
    uint64_t matrix_finalize(void *);
    void matrix_print(void*);
    void matrix_rowprint(void *, uint64_t);
    void matrix_free(void*);
    int matrix_isempty(void *);

    // utilities
    void matrix_create_space(void *, uint64_t);
    void matrix_resize(void*, int64_t);
    int matrix_randomize(matrix_t *, uint64_t, uint64_t);
    int matrix_empty_row(void *, uint64_t);
    void matrix_content_swap(matrix_t *, matrix_t *);

    // sorting-methods
    uint64_t matrix_col_size(void *, uint64_t);
    int matrix_qsort(matrix_t *, uint64_t, const tuple_t *);
    int matrix_test_order(matrix_t *, uint64_t);
    uint64_t matrix_partition(matrix_t *, uint64_t, const tuple_t *);





    int header_init(header_t *, const field_t*, const uint8_t);
    int header_reset(header_t *, const header_t*);
    int header_print(header_t *header);
    int header_free(header_t *header);

    // relation methods
    int rel_list_init(list_t *);

    // join operation methods
    int load_table(table_t *, const rel_t *, uint64_t);
    int load_matrix_full(matrix_t *, const rel_t *, tuple_t *);
    int load_matrix_col_from_rel(matrix_t *, const rel_t *, tuple_t*);
    int load_all(matrix_t *, table_t *, tuple_t *, tuple_t *, rel_t **);
    int join(const table_t *, const table_t *, list_t*);
    int mx_join(matrix_t *, uint64_t, const table_t *, uint64_t);

#endif
