/* File: malloc.c
 * --------------
 * ***** This function implements malloc allocator *****
 */


 /*
 * The code given below is simple "bump" allocator from lecture.
 * An allocation request is serviced by using sbrk to extend
 * the heap segment.
 * It does not recycle memory (free is a no-op) so when all the
 * space set aside for the heap is consumed, it will not be able
 * to service any further requests.
 *
 * This code is given here just to show the very simplest of
 * approaches to dynamic allocation. You will replace this code
 * with your own heap allocator implementation.
 */

 #include "malloc.h"
 #include "memmap.h"
 #include "printf.h"
 #include <stddef.h> // for NULL
 #include "strings.h"
 
 /*
  * Data variables private to this module used to track
  * statistics for debugging/validate heap:
  *    count_allocs, count_frees, total_bytes_requested
  */
 static int count_allocs, count_frees, total_bytes_requested;
 
 struct header{
     unsigned int size;
     unsigned int status; //1 is used, 0 is freed
 };
 
 static void *cur_heap_end =  &__heap_start;
 
 /*
  * The segment of memory available for the heap runs from &__heap_start
  * to &__heap_max (symbols from memmap.ld establish these boundaries)
  *
  * The variable cur_head_end is initialized to &__heap_start and this
  * address is adjusted upward as in-use portion of heap segment
  * enlarges. Because cur_head_end is qualified as static, this variable
  * is not stored in stack frame, instead variable is located in data segment.
  * The one variable is shared by all and retains its value between calls.
  */
 
 // Call sbrk to enlarge in-use heap area - currently does nothing
 void *sbrk(size_t nbytes) {
     return cur_heap_end;
 }
 
 // Simple macro to round up x to multiple of n.
 // The efficient but tricky bitwise approach it uses
 // works only if n is a power of two -- why?
 #define roundup(x,n) (((x)+((n)-1))&(~((n)-1)))
 
 void *malloc (size_t nbytes) {
     nbytes = roundup(nbytes, 8);
     //specific looping to start finding a free segment
         //case 1: if block is big enough, it is either "big enough" for one block (8+nbytes) or two blocks (16+nbytes)
         //case 2: add at the end because "nothing" fitting exists
         //warning - we cannot add greater than max length
     
     unsigned long counter = 0; //tracks block size availability
     unsigned long alloc_del = 0; //when we replace blocks, we need to subtract from total allocation
     int prevOpen = 0;//prevOpen tracks consecutivity of open blocks
     unsigned long n = 0; //tracks mem positiion
     char * start = (char *) (&__heap_start);//beginning memory locaiton
     char * savedChange = start; //tracks memory location for block replacement
     while (n < (total_bytes_requested)){ //count and find available posiiton
         int size = ((struct header *) (start + n)) -> size;
         int status = ((struct header *) (start +n)) -> status;
         if((status == 0)){
             if(!prevOpen){
                 counter += (8+size);
                 alloc_del++;
             }
             else{
                 prevOpen = 0;
                 savedChange = (start + n);
                 counter += (8+size);
                 alloc_del++;
             }
         }
         else{
             prevOpen = 1;
             counter = 0;
             alloc_del = 0;
         }
         if(counter >= (nbytes+8)){ //replace with one block
             break;
         }
         else{
             n+=(8+size);
         }
     }
 
     //block is added
     if((n >=  (total_bytes_requested)) & (counter == 0)){ //block added at the end
         if(( (total_bytes_requested+nbytes+8) > ((unsigned long) (&__heap_max) -  (unsigned long) (&__heap_start)))){ //return null for too big value
             return NULL;
         }
         struct header *newOne = (struct header *) (start + n);
         newOne -> size = nbytes;
         newOne -> status = 1;
         void * returnHeap = ( (char *) newOne + 8);
         count_allocs++;
         total_bytes_requested+=nbytes;
         total_bytes_requested+=8;
         cur_heap_end = start + total_bytes_requested;
         return returnHeap;
     }
     else{ //a replacement block is made in the middle of our heap 
         if(( (((unsigned long ) savedChange ) - (unsigned long ) start)+nbytes+8) >((unsigned long) (&__heap_max) -  (unsigned long) (&__heap_start)) ){ //return null for too big value
             return NULL;
         }
         if(( (((unsigned long ) savedChange ) - (unsigned long ) start)+nbytes+8) > total_bytes_requested){ // could extend beyond end, and so we need to add to total bytes
             total_bytes_requested = ( (((unsigned long ) savedChange ) - (unsigned long ) start)+nbytes+8); 
         }
         int remains = counter - (nbytes+8); 
         ((struct header *) savedChange )-> size = nbytes; //adds first block
         ((struct header *) savedChange )-> status = 1;
         void* returnHeap =  ( savedChange + 8);
         count_allocs++;
         count_allocs-=alloc_del;
         count_frees-=alloc_del;
         cur_heap_end = start + total_bytes_requested;
 
         if(remains >= 8){ //adds the second block we split from if there's space
             struct header * remainThing =  (struct header *) (savedChange + nbytes + 8);
             remainThing -> size = remains-8;
             remainThing -> status = 0;
             count_allocs++;
             count_frees++;
         }
         return  returnHeap;
     }
 }
 
 //frees memory
 void free (void *ptr) {
     if(ptr != NULL){
         struct header * in = (struct header * )((char *) ptr - (8));
         count_frees++;
         in -> status = 0;

         unsigned long counter = 0; //tracks block size availability
         unsigned long alloc_del = 0; //when we replace blocks, we need to subtract from total allocation
         unsigned long n = ((unsigned long) in - (unsigned long) &__heap_start); //tracks mem positiion
         char * start = (char *) (&__heap_start);//beginning memory locaiton
         while (n < (total_bytes_requested)){ //count and find available posiiton
             unsigned int size = ((struct header *) (start + n)) -> size;
             unsigned int status = ((struct header *) (start +n)) -> status;
             if((status == 0)){
                 counter += (8+size);
                 alloc_del++;
             }
             else{
                 break;
             }
            
             n+=(8+size);
         }


         ((struct header *) in ) -> size = (counter - 8);
         count_allocs++;
         count_frees++;
         count_allocs-=alloc_del;
         count_frees-=alloc_del;
     }
 }
 
 void heap_dump (const char *label) {
     printf("\n---------- HEAP DUMP (%s) ----------\n", label);
     printf("Heap segment at %p - %p\n", &__heap_start, sbrk(0));
     unsigned long n = 0;
     char * start = (char *) (0x44000000);//beginning memory locaiton
     while (n < (total_bytes_requested)){ //count and find available posiiton
         int size = ((struct header *) (start + sizeof(char)*n)) -> size;
         int status = ((struct header *) (start +sizeof(char)*n)) -> status;
         printf("\nBlock size: %d; Block status: %d; Block storage: %s\n", size, status,(start + sizeof(char)*n+8));
         n+= (8+size);
     }
     printf("\n----------  END DUMP (%s) ----------\n", label);
     printf("Stats: %d in-use (%d allocs, %d frees), %d total payload bytes requested\n\n",
         count_allocs - count_frees, count_allocs, count_frees, total_bytes_requested);
 }
 
 void malloc_report (void) {
     printf("\n=============================================\n");
     printf(  "         Mini-Valgrind Malloc Report         \n");
     printf(  "=============================================\n");
     /***** TODO EXTENSION: Your code goes here if implementing extension *****/
 }
 
 void report_damaged_redzone (void *ptr) {
     printf("\n=============================================\n");
     printf(  " **********  Mini-Valgrind Alert  ********** \n");
     printf(  "=============================================\n");
     printf("Attempt to free address %p that has damaged red zone(s):", ptr);
     /***** TODO EXTENSION: Your code goes here if implementing extension *****/
 }