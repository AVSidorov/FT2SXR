#ifndef _SYSTEMH_
#define _SYSTEMH_

#include <linux/version.h>

#ifdef DZYTOOLS_2_4_X
        #define VMA_OFFSET(vma)  ((vma)->vm_pgoff << PAGE_SHIFT)
#else
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,9)
    #define VMA_OFFSET(vma)  ((vma)->vm_pgoff) 		 //from 2.6.10.x
    #define remap_page_range remap_pfn_range
#else
    #define VMA_OFFSET(vma)  ((vma)->vm_pgoff << PAGE_SHIFT) //up to 2.6.9.x
#endif
#endif

int adp_open( struct inode *inode, struct file *file );

int adp_close( struct inode *inode, struct file *file );

ssize_t adp_read( struct file *file,
			 char *buffer,
			 size_t length,
			 loff_t *offset );

ssize_t adp_write( struct file *file,
                          const char *buffer,
                          size_t length,
                          loff_t *offset );

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,37)
long adp_ioctl( struct file *file, unsigned int cmd, unsigned long arg );
#else
int adp_ioctl( struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg );
#endif

int  adp_mmap(struct file *filp, struct vm_area_struct *vma );
int  adp_fasync(int fd, struct file *filp, int on);
int  adp_proc_info(char *buf, char **start, off_t offset, int lenght, int *eof, void *data);
void adp_remove_proc( char *name );
unsigned int adp_poll(struct file *filp, poll_table *wait);

#endif
