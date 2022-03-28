
#ifndef _DZYCROSS_H_
#define _DZYCROSS_H_

#ifdef DZYTOOLS_2_2_X
    #define VMA_OFFSET(vma)  ((vma)->vm_offset)
    #define DECLARE_WAIT_QUEUE_HEAD( head ) struct wait_queue *head = NULL
    typedef struct wait_queue* wait_queue_head_t;
    #define init_waitqueue_head(head) (*(head)) = NULL
    #define IORESOURCE_IO 0x001
    #define IORESOURCE_MEM 0x200
    #ifndef VM_RESERVED            // Added 2.4.0-test10
    	#define VM_RESERVED 0
    #endif
    //#define atomic_inc_and_test atomic_inc_and_test_greater_zero
    #include <linux/malloc.h>
    #include <linux/sched.h>
    #include <asm/atomic.h>
    #include <asm/io.h>
    typedef void irqreturn_t;
    typedef unsigned long dma_addr_t;    
    typedef unsigned long dma_handle_t;        
    #define IRQ_NONE
    #define IRQ_HANDLED

//---------
/**
 * atomic_inc_and_test - increment and test 
 * @v: pointer of type atomic_t
 * 
 * Atomically increments @v by 1
 * and returns true if the result is zero, or false for all
 * other cases.
 */ 
static __inline__ int atomic_inc_and_test(atomic_t *v)
{
	unsigned char c;

	__asm__ __volatile__(
		LOCK_PREFIX "incl %0; sete %1"
		:"+m" (v->counter), "=qm" (c)
		: : "memory");
	return c != 0;
}
//---------    
    
#endif

#endif //_DZYCROSS_H_