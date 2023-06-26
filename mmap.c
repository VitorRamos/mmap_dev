#include <linux/compiler.h> /* __must_check */
#include <linux/init.h>       /* module_init, module_exit */
#include <linux/module.h>     /* version info, MODULE_LICENSE, MODULE_AUTHOR, printk() */

#include <linux/fs.h> 	    /* file stuff */
#include <linux/kernel.h>   /* printk() */
#include <linux/errno.h>    /* error codes */
#include <linux/module.h>   /* THIS_MODULE */
#include <linux/cdev.h>     /* char device stuff */
#include <linux/uaccess.h>  /* copy_to_user() */

#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/sched/mm.h>

static ssize_t device_file_read(
    struct file *file_ptr
    , char __user *user_buffer
    , size_t count
    , loff_t *possition)
{
    struct task_struct *task; // get current process task_struct
    struct mm_struct *mm; // get mm_struct for current process
    struct vm_area_struct *vma; // get first virtual memory area
    struct vma_entry
    {
        unsigned long start, stop, flags;
    } vma_entry;

    // printk( KERN_NOTICE "mmap_dev: Device file is read at offset = %i, read bytes count = %u\n"
    //     , (int)*possition
    //     , (unsigned int)count );

    task = current; // get current process task_struct
    mm = get_task_mm(task);
    count = 0;
    for (vma = mm->mmap; vma; vma = vma->vm_next)
    {
        // printk("%p\n", (void*)vma->vm_start);
        vma_entry.start = vma->vm_start;
        vma_entry.stop = vma->vm_end;
        vma_entry.flags = vma->vm_flags;
        if(copy_to_user(user_buffer+count, &vma_entry, sizeof(vma_entry)) != 0)
            return -EFAULT;
        count += sizeof(vma_entry);
    }
    return count;
}

/*===============================================================================================*/
static ssize_t device_file_write(
    struct file *file_ptr
    , const char __user *user_buffer
    , size_t count
    , loff_t *possition)
{    
    struct task_struct *task; // get current process task_struct
    struct mm_struct *mm; // get mm_struct for current process
    struct vm_area_struct *vma; // get first virtual memory area
    struct query
    {
        unsigned long ptr;
        size_t size;
    } u_query;
    int ret = 0;

    if (count != sizeof(struct query))
        return -EINVAL;
    ret = copy_from_user(&u_query, user_buffer, count);
    if (ret)
        return -EFAULT;

    // if (!access_ok((void __user *)u_query.ptr, u_query.size))
    //     return 1;
    // return 0;

    task = current; // get current process task_struct
    mm = get_task_mm(task);
    for (vma = mm->mmap; vma; vma = vma->vm_next)
    {
        if (vma->vm_flags & 0x1
            && u_query.ptr >= vma->vm_start 
            && u_query.ptr+u_query.size <= vma->vm_end)
            return 0;
    }
    return 1;
}

static struct file_operations simple_driver_fops = 
{
    .owner = THIS_MODULE,
    .read = device_file_read,
    .write = device_file_write,
};

static int device_file_major_number = 0;
static const char device_name[] = "mmap_dev";

static int simple_driver_init(void)
{
    int result = 0;
    printk( KERN_NOTICE "mmap_dev: Initialization started\n" );

    result = register_chrdev( 0, device_name, &simple_driver_fops );
    if( result < 0 )
    {
        printk( KERN_WARNING "mmap_dev:  can\'t register character device with errorcode = %i\n", result );
        return result;
    }

    device_file_major_number = result;
    printk( KERN_NOTICE "mmap_dev: registered character device with major number = %i and minor numbers 0...255\n" , device_file_major_number );
    return 0;
}

static void simple_driver_exit(void)
{
    printk( KERN_NOTICE "mmap_dev: Exiting\n" );
    if(device_file_major_number != 0)
        unregister_chrdev(device_file_major_number, device_name);
}

/*===============================================================================================*/
module_init(simple_driver_init);
module_exit(simple_driver_exit);

MODULE_DESCRIPTION("Simple Linux driver");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Apriorit, Inc");