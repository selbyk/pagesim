/*
   Page Replacement Algorithms
   Author: Selby Kendrick
   Description: Simulation of common page replacement algorithms used by
   operating systems to manage memory usage
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/queue.h>
#include "pagesim.h"

/**
 * Configuration variables
 */
int num_frames = 10; // Number of avaliable pages in page tables
int debug = 0; // Debug bool, 1 shows verbose output
int printrefs = 0; // Print refs bool, 1 shows output after each page ref
int max_pages = 1000; // Max number of page refs to test
// Page reference string
int page_refs[28] = { 0, 1, 2, 3, 2, 4, 5, 3, 4, 1, 6, 3, 7, 8, 7, 8, 4, 9, 7, 8, 1, 2, 9, 5, 4, 5, 0, 2 };
int num_refs = 28; // Number of page refs in page reference string

/**
 * Array of algorithm functions that can be enabled
 */
Algorithm algos[6] = { {"RANDOM", &RANDOM, 0, NULL},
                       {"FIFO", &FIFO, 0, NULL},
                       {"LRU", &LRU, 0, NULL},
                       {"CLOCK", &CLOCK, 0, NULL},
                       {"NFU", &NFU, 0, NULL},
                       {"AGING", &AGING, 0, NULL} };

/**
* Runtime variables, don't touch
*/
int counter = 0; // "Time" as number of loops calling page_refs 0...num_refs (used as i in for loop)
int last_page_ref = -1; // Last ref
size_t num_algos = 0; // Number of algorithms in algos, calculated in init()

/**
 * int main(int argc, char *argv[])
 *
 * @param argc {int} number of commandline terms eg {'./pagesim' => argc=1}
 * @param argv {char **} arguments passed in
 *
 * Run algo if given correct arguments, else terminate with error
 */
int main ( int argc, char *argv[] )
{
        init();
        if ( !(argc >= 3 && argc <= 5) ) /* argc should be 3-5 for correct execution */
                print_help(argv[0]);
        else
        {
                num_frames = atoi(argv[2]);
                if ( num_frames < 1 ) /* argc should be 3 for correct execution */
                {
                        num_frames = 1;
                        printf( "Number of page frames must be at least 1, setting to 1\n");
                }
                if ( argc > 3 )
                {
                        if(atoi(argv[3]) == 1 || atoi(argv[3]) == 0)
                                printrefs = atoi(argv[3]);
                        else
                                printf( "Printrefs must be 1 or 0, ignoring\n");
                }
                if ( argc > 4 )
                {
                        if(atoi(argv[4]) == 1 || atoi(argv[3]) == 0)
                                debug = atoi(argv[4]);
                        else
                                printf( "Debug must be 1 or 0, ignoring\n");
                }
                switch(argv[1][0])
                {
                case 'L':
                case 'l':
                        algos[0].selected = 1;
                        break;
                case 'C':
                case 'c':
                        algos[0].selected = 1;
                        break;
                case 'A':
                case 'a':
                case 'T':
                case 't':
                        printrefs = 1;
                        num_refs = -1;
                        size_t i = 0;
                        for (i = 0; i < num_algos; i++)
                                algos[i].selected = 1;
                        break;
                default:
                        printf( "%s algorithm is invalid choice or not yet implemented\n", argv[1]);
                        print_help(argv[0]);
                        return 1;
                }
                event_loop();
        }
        cleanup();
        return 0;
}

/**
 * int init()
 *
 * Initialize lists and variables
 *
 * @return 0
 */
int init()
{
        // Calculate number of algos
        num_algos = sizeof(algos)/sizeof(Algorithm);
        size_t i = 0;
        for (i = 0; i < num_algos; ++i)
        {
                algos[i].data = create_algo_data_store();
        }
        return 0;
}

/**
 * Algorithm_Data* create_algo_data_store(int num_frames)
 *
 * Creates an empty Algorithm_Data to init an Algorithm
 *
 * @return {Algorithm_Data*} empty Algorithm_Data struct for an Algorithm
 */
Algorithm_Data *create_algo_data_store()
{
        Algorithm_Data *data = malloc(sizeof(Algorithm_Data));
        data->hits = 0;
        data->misses = 0;
        data->last_victim = NULL;
        /* Initialize Lists */
        LIST_INIT(&(data->page_table));
        LIST_INIT(&(data->victim_list));
        /* Insert at the page_table. */
        Frame *framep = create_empty_frame(0);
        LIST_INSERT_HEAD(&(data->page_table), framep, frames);
        /* Build the rest of the list. */
        size_t i = 0;
        for (i = 1; i < num_frames; ++i)
        {
                Frame *insert_frame = create_empty_frame(i);
                LIST_INSERT_AFTER(framep, insert_frame, frames);
                framep = framep->frames.le_next;
        }
        return data;
}

/**
 * Frame* create_empty_frame(int num_frames)
 *
 * Creates an empty Frame for page table list
 *
 * @return {Frame*} empty Frame entry for page table list
 */
Frame* create_empty_frame(int index)
{
        Frame *framep = malloc(sizeof(Frame));
        framep->index = index;
        framep->page = -1;
        time(&framep->time);
        framep->extra = 0;
        return framep;
}

/**
 * int event_loop()
 *
 * page all selected algorithms with input ref
 *
 * @param page_ref {int} page to ref
 *
 * @return 0
 */
int event_loop()
{
        if(num_refs == -1)
        {
                time_t stop = time(NULL) + 10;
                srand(stop);
                while(counter < max_pages)
                {
                        page(get_ref());
                        usleep(1000);
                        ++counter;
                }
        }
        else
                while(counter < num_refs && counter < max_pages)
                        page(page_refs[counter++]);
        size_t i = 0;
        for (i = 0; i < num_algos; i++)
                if(algos[i].selected==1) {
                        print_summary(algos[i]);
                }
        return 0;
}

/**
 * int cleanup()
 *
 * Clean up memory
 *
 * @return 0
 */
int cleanup()
{
        size_t i = 0;
        for (i = 0; i < num_algos; i++)
        {
                /* Clean up memory, delete the list */
                while (algos[i].data->page_table.lh_first != NULL)
                        LIST_REMOVE(algos[i].data->page_table.lh_first, frames);
                while (algos[i].data->victim_list.lh_first != NULL)
                        LIST_REMOVE(algos[i].data->victim_list.lh_first, frames);
        }
        return 0;
}

/**
 * int get_ref()
 *
 * get a random ref
 *
 * @return {int}
 */
int get_ref()
{
        return rand() % 12;
}

/**
 * int page()
 *
 * page all selected algorithms with input ref
 *
 * @param page_ref {int} page to ref
 *
 * @return 0
 */
int page(int page_ref)
{
        last_page_ref = page_ref;
        size_t i = 0;
        for (i = 0; i < num_algos; i++)
                if(algos[i].selected==1) {
                        algos[i].algo(algos[i].data);
                        if(printrefs == 1)
                                print_stats(algos[i]);
                }
        return 0;
}

/**
 * int add_victim(struct Frame_List *victim_list, struct Frame *frame)
 *
 * Add victim frame evicted from page table to list of victims
 *
 * @param index {Frame_List} list of victims
 * @param page {Frame} page frame evicted
 *
 * @retun 0
 */
int add_victim(struct Frame_List *victim_list, struct Frame *frame)
{
        if(debug)
                printf("Victim index: %d, Page: %d\n", frame->index, frame->page);
        struct Frame *victim = malloc(sizeof(Frame));
        *victim = *frame;
        victim->index = 1;
        LIST_INSERT_HEAD(victim_list, victim, frames);
        return 0;
}

/**
 * int print_list()
 *
 * Print list
 *
 * @param head {Frame} head of frame list
 * @param index_label {const char*} label for index frame field
 * @param value_label {const char*} label for value frame field
 *
 * @retun 0
 */
int print_list(struct Frame *head, const char* index_label, const char* value_label)
{
        int colsize = 9, labelsize;
        struct Frame *framep;
        // Determine lanbel col size from text
        if (strlen(value_label) > strlen(index_label))
                labelsize = strlen(value_label) + 1;
        else
                labelsize = strlen(index_label) + 1;
        /* Forward traversal. */
        printf("%-*s: ", labelsize, index_label);
        for (framep = head; framep != NULL; framep = framep->frames.le_next)
        {
                printf("%*d", colsize, framep->index);
        }
        printf("\n%-*s: ", labelsize, value_label);
        for (framep = head; framep != NULL; framep = framep->frames.le_next)
        {
                if(framep->page == -1)
                        printf("%*s", colsize, "_");
                else
                        printf("%*d", colsize, framep->page);
        }
        printf("\n%-*s: ", labelsize, "Extra");
        for (framep = head; framep != NULL; framep = framep->frames.le_next)
        {
                printf("%*d", colsize, framep->extra);
        }
        printf("\n%-*s: ", labelsize, "Time");
        for (framep = head; framep != NULL; framep = framep->frames.le_next)
        {
                printf("%*d", colsize, (int32_t) (framep->time%200000000));
        }
        printf("\n\n");
        return 0;
}

/**
 * int RANDOM(Algorithm_Data *data)
 *
 * @param page_table {Frame_List} page table
 * @param victim_list {Frame_List} victim list
 *
 * RANDOM Page Replacement Algorithm
 */
int RANDOM(Algorithm_Data *data)
{
        struct Frame *framep = data->page_table.lh_first,
                     *victim = NULL;
        int rand_victim = rand() % num_frames;
        int fault = 0;
        /* Find target (hit), empty page index (miss), or victim to evict (miss) */
        while (framep != NULL && framep->page > -1 && framep->page != last_page_ref) {
                if(framep->index == rand_victim) // rand
                        victim = framep;
                framep = framep->frames.le_next;
        }
        if(framep == NULL)
        { // It's a miss, kill our victim
                if(debug) printf("Victim selected: %d, Page: %d\n", victim->index, victim->page);
                add_victim(&data->victim_list, victim);
                victim->page = last_page_ref;
                time(&victim->time);
                victim->extra = counter;
                fault = 1;
        }
        else if(framep->page == -1)
        { // Use free page table index
                framep->page = last_page_ref;
                time(&framep->time);
                framep->extra = counter;
                fault = 1;
        }
        else if(framep->page == last_page_ref)
        { // The page was found! Hit!
                time(&framep->time);
                framep->extra = counter;
        }
        if(debug)
        {
                printf("Page Ref: %d\n", last_page_ref);
                for (framep = data->page_table.lh_first; framep != NULL; framep = framep->frames.le_next)
                        printf("Slot: %d, Page: %d, Time used: %d\n", framep->index, framep->page, framep->extra);
        }
        if(fault == 1) data->misses++; else data->hits++;
        return fault;
}

/**
 * int FIFO(Algorithm_Data *data)
 *
 * @param page_table {Frame_List} page table
 * @param victim_list {Frame_List} victim list
 *
 * FIFO Page Replacement Algorithm
 */
int FIFO(Algorithm_Data *data)
{
        struct Frame *framep = data->page_table.lh_first,
                     *victim = NULL;
        int fault = 0;
        /* Find target (hit), empty page index (miss), or victim to evict (miss) */
        while (framep != NULL && framep->page > -1 && framep->page != last_page_ref) {
                if(victim == NULL || framep->time > victim->time)
                { // No victim yet or frame older than victim
                        victim = framep;
                }
                framep = framep->frames.le_next;
        }
        /* Make a decision */
        if(framep == NULL)
        { // It's a miss, kill our victim
                if(debug) printf("Victim selected: %d, Page: %d\n", victim->index, victim->page);
                add_victim(&data->victim_list, victim);
                victim->page = last_page_ref;
                time(&victim->time);
                victim->extra = counter;
                fault = 1;
        }
        else if(framep->page == -1)
        { // Can use free page table index
                framep->page = last_page_ref;
                time(&framep->time);
                framep->extra = counter;
                fault = 1;
        }
        else if(framep->page == last_page_ref)
        { // The page was found! Hit!
                time(&framep->time);
                framep->extra = counter;
        }
        if(fault == 1) data->misses++; else data->hits++;
        return fault;
}


/**
 * int LRU(Algorithm_Data *data)
 *
 * @param page_table {Frame_List} page table
 * @param victim_list {Frame_List} victim list
 *
 * LRU Page Replacement Algorithm
 */
int LRU(Algorithm_Data *data)
{
        struct Frame *framep = data->page_table.lh_first,
                     *victim = NULL;
        int fault = 0;
        /* Find target (hit), empty page index (miss), or victim to evict (miss) */
        while (framep != NULL && framep->page > -1 && framep->page != last_page_ref) {
                if(victim == NULL || framep->time < victim->time)
                        victim = framep; // No victim yet or frame older than victim
                framep = framep->frames.le_next;
        }
        /* Make a decision */
        if(framep == NULL)
        { // It's a miss, kill our victim
                if(debug) printf("Victim selected: %d, Page: %d\n", victim->index, victim->page);
                add_victim(&data->victim_list, victim);
                victim->page = last_page_ref;
                time(&victim->time);
                victim->extra = counter;
                fault = 1;
        }
        else if(framep->page == -1)
        { // Can use free page table index
                framep->page = last_page_ref;
                time(&framep->time);
                framep->extra = counter;
                fault = 1;
        }
        else if(framep->page == last_page_ref)
        { // The page was found! Hit!
                time(&framep->time);
                framep->extra = counter;
        }
        if(fault == 1) data->misses++; else data->hits++;
        return fault;
}

/**
 * int CLOCK(Algorithm_Data *data)
 *
 * @param page_table {Frame_List} page table
 * @param victim_list {Frame_List} victim list
 *
 * CLOCK Page Replacement Algorithm
 */
int CLOCK(Algorithm_Data *data)
{
        static Frame *clock_hand = NULL; // Clock needs a hand
        Frame *framep = data->page_table.lh_first;
        int fault = 0;
        /* Forward traversal. */
        /* Find target (hit), empty page slot (miss), or victim to evict (miss) */
        while(framep != NULL && framep->page > -1 && framep->page != last_page_ref)
                framep = framep->frames.le_next;
        /* Make a decision */
        if(framep != NULL)
        {
                if(framep->page == -1)
                {
                        framep->page = last_page_ref;
                        framep->extra = 0;
                        fault = 1;
                }
                else
                { // Found the page, update its R bit to 0
                        framep->extra = 0;
                }
        }
        else // Use the hand to find our victim
        {
                while(clock_hand == NULL || clock_hand->extra == 0)
                {
                        if(clock_hand == NULL)
                        {
                                clock_hand = data->page_table.lh_first;
                        }
                        else
                        {
                                clock_hand->extra = 1;
                                clock_hand = clock_hand->frames.le_next;
                        }
                }
                add_victim(&data->victim_list, clock_hand);
                clock_hand->page = last_page_ref;
                clock_hand->extra = 0;
                fault = 1;
        }
        if(fault == 1) data->misses++; else data->hits++;
        return fault;
}

/**
 * int NFU(Algorithm_Data *data)
 *
 * @param page_table {Frame_List} page table
 * @param victim_list {Frame_List} victim list
 *
 * NFU Page Replacement Algorithm
 */
int NFU(Algorithm_Data *data)
{
        struct Frame *framep = data->page_table.lh_first,
                     *victim = NULL;
        int fault = 0;
        /* Find target (hit), empty page index (miss), or victim to evict (miss) */
        while (framep != NULL && framep->page > -1 && framep->page != last_page_ref) {
                if(victim == NULL || framep->extra < victim->extra)
                        victim = framep; // No victim or frame used fewer times
                framep = framep->frames.le_next;
        }
        /* Make a decision */
        if(framep == NULL)
        { // It's a miss, kill our victim
                add_victim(&data->victim_list, victim);
                victim->page = last_page_ref;
                time(&victim->time);
                victim->extra = 0;
                fault = 1;
        }
        else if(framep->page == -1)
        { // Can use free page table index
                framep->page = last_page_ref;
                time(&framep->time);
                framep->extra = 0;
                fault = 1;
        }
        else if(framep->page == last_page_ref)
        { // The page was found! Hit!
                time(&framep->time);
                framep->extra++;
        }
        if(fault == 1) data->misses++; else data->hits++;
        return fault;
}

/**
 * int AGING(Algorithm_Data *data)
 *
 * @param page_table {Frame_List} page table
 * @param victim_list {Frame_List} victim list
 *
 * AGING Page Replacement Algorithm
 */
int AGING(Algorithm_Data *data)
{
        struct Frame *framep = data->page_table.lh_first,
                     *victim = NULL;
        int fault = 0;
        /* Find target (hit), empty page index (miss), or victim to evict (miss) */
        while (framep != NULL && framep->page > -1 && framep->page != last_page_ref) {
                framep->extra /= 2;
                if(victim == NULL || framep->extra < victim->extra)
                        victim = framep; // No victim or frame used rel less
                framep = framep->frames.le_next;
        }
        /* Make a decision */
        if(framep == NULL)
        { // It's a miss, kill our victim
                add_victim(&data->victim_list, victim);
                victim->page = last_page_ref;
                time(&victim->time);
                victim->extra = 0;
                fault = 1;
        }
        else if(framep->page == -1)
        { // Can use free page table index
                framep->page = last_page_ref;
                time(&framep->time);
                framep->extra = 0;
                fault = 1;
        }
        else if(framep->page == last_page_ref)
        { // The page was found! Hit!
                time(&framep->time);
                framep->extra = framep->extra+10000000;
                while (framep->frames.le_next != NULL) {
                        framep = framep->frames.le_next;
                        framep->extra /= 2;
                }
        }
        if(fault == 1) data->misses++; else data->hits++;
        return fault;
}

/**
 * int print_help()
 *
 * @param num_frames {int} number of page frames in simulated page table
 *
 * Function to print results after algo is ran
 */
int print_help(const char *binary)
{
        printf( "usage: %s algorithm num_frames show_process debug\n", binary);
        printf( "   algorithm    - page algorithm to use {LRU, CLOCK}\n");
        printf( "   num_frames   - number of page frames {int > 0}\n");
        printf( "   show_process - print page table after each ref is processed {1 or 0}\n");
        printf( "   debug        - verbose debugging output {1 or 0}\n");
        return 0;
}

/**
 * int print_stats()
 *
 * Function to print results after algo is ran
 */
int print_stats(Algorithm algo)
{
        print_summary(algo);
        print_list(algo.data->page_table.lh_first, "Frame #", "Page Ref");
        return 0;
}

/**
 * int print_summary()
 *
 * Function to print summary report of an Algorithm
 */
int print_summary(Algorithm algo)
{
        printf("%s Algorithm\n", algo.label);
        printf("Frames in Mem: %d, ", num_frames);
        printf("Hits: %d, ", algo.data->hits);
        printf("Misses: %d, ", algo.data->misses);
        printf("Hit Ratio: %f\n", (double)algo.data->hits/(double)(algo.data->hits+algo.data->misses));
        return 0;
}
