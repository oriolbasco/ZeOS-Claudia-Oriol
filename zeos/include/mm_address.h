#ifndef MM_ADDRESS_H
#define MM_ADDRESS_H

#define TOTAL_PAGES 1024
#define PAG_LOG_INIT_CODE (PAG_LOG_INIT_DATA+NUM_PAG_DATA)
#define NUM_PAG_CODE 8
#define PAG_LOG_INIT_DATA (L_USER_START>>12)
#define NUM_PAG_DATA 20
#define PAGE_SIZE 0x1000

#define FORK_TEMP_PAGE (NUM_PAG_DATA + NUM_PAG_CODE)
#define HEAP_FIRST_PAGE (FORK_TEMP_PAGE + 1)

/* Memory distribution */
/***********************/


#define KERNEL_START     0x10000
#define L_USER_START        0x400000
#define USER_ESP	L_USER_START+(NUM_PAG_DATA)*0x1000

#define PAGE(x) (x>>12)

#endif

