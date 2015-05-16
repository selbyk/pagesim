#ifndef PAGESIM_H
#define PAGESIM_H

/**
 * Data structures
 */
// List for page tables and victim lists
LIST_HEAD(Frame_List, Frame);

// stuct to hold Frame info
typedef struct Frame
{
        LIST_ENTRY(Frame) frames;
        int index;
        int page;
        time_t time;
        int extra;
} Frame;

// stuct to hold Algorithm data
typedef struct {
        int hits;
        int misses;
        struct Frame_List page_table;
        struct Frame_List victim_list;
        Frame *last_victim;
} Algorithm_Data;

// an Algorithm 
typedef struct {
        const char *label;
        int (*algo)(Algorithm_Data *data);
        int selected;
        Algorithm_Data *data;
} Algorithm;

/**
 * Init/cleanup functions
 */
int init();
Algorithm_Data *create_algo_data_store();
Frame *create_empty_frame(int index);
int cleanup();

/**
 * Control functions
 */
int event_loop();
int page(int page_ref);
int get_ref();
int add_victim(struct Frame_List *victim_list, struct Frame *frame);

/**
 * Algorithm functions
 */
int RANDOM(Algorithm_Data *data);
int FIFO(Algorithm_Data *data);
int LRU(Algorithm_Data *data);
int CLOCK(Algorithm_Data *data);
int NFU(Algorithm_Data *data);
int AGING(Algorithm_Data *data);

/**
 * Output functions
 */
int print_help(const char *binary);
int print_list(struct Frame *head, const char* index_label, const char* value_label);
int print_stats(Algorithm algo);
int print_summary(Algorithm algo);

#endif
