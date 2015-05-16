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
        LIST_ENTRY(Frame) frames; // frames node, next
        int index; // frame position in list... not really needed
        int page; // page frame points to, -1 is empty
        time_t time; // time added/accessed
        int extra; // extra field for per-algo use
} Frame;

// stuct to hold Algorithm data
typedef struct {
        int hits; // number of times page was found in page table
        int misses; // number of times page wasn't found in page table
        struct Frame_List page_table; // List to hold frames in page table
        struct Frame_List victim_list; // List to hold frames that were replaced in page table
        Frame *last_victim; // Holds last frame used as a victim to make inserting to victim list faster
} Algorithm_Data;

// an Algorithm 
typedef struct {
        const char *label; // Algorithm name
        int (*algo)(Algorithm_Data *data); // Pointer to algorithm function
        int selected; // Should algorithm be run, 1 or 0
        Algorithm_Data *data; // Holds algorithm data to pass into algorithm function
} Algorithm;

/**
 * Init/cleanup functions
 */
int init(); // init lists and variable, set up config defaults, and load configs
Algorithm_Data *create_algo_data_store(); // returns empty algorithm data
Frame *create_empty_frame(int index); // returns empty frame
int cleanup(); // frees allocated memory

/**
 * Control functions
 */
int event_loop(); // loops for each page call
int page(int page_ref); // page all algos with page ref
int get_ref(); // get next page ref however you like
int add_victim(struct Frame_List *victim_list, struct Frame *frame); // add victim frame to a victim list

/**
 * Output functions
 */
int print_help(const char *binary); // prints help screen
int print_list(struct Frame *head, const char* index_label, const char* value_label); // prints a list
int print_stats(Algorithm algo); // detailed stats
int print_summary(Algorithm algo); // one line summary

/**
 * Algorithm functions
 */
int RANDOM(Algorithm_Data *data);
int FIFO(Algorithm_Data *data);
int LRU(Algorithm_Data *data);
int CLOCK(Algorithm_Data *data);
int NFU(Algorithm_Data *data);
int AGING(Algorithm_Data *data);

#endif
