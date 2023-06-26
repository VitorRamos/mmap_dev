#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

struct vma_entry
{
    unsigned long start, stop, flags;
};

int main()
{
    void* x = malloc(1024);
    int fd = open("/dev/mmap_dev", O_RDONLY);
    struct vma_entry entries[1024];
    int res = read(fd, entries, 1024*sizeof(struct vma_entry));
    printf("%d\n", res);
    for(int i=0; i<res/sizeof(struct vma_entry); i++)
    {
        printf("%p %p %ld\n", (void*)entries[i].start, (void*)entries[i].stop, entries[i].flags);
    }

    // void* x = malloc(1024);
    // int fd = open("/proc/etx/etx_proc", O_RDONLY);
    // long start[1024];
    // read(fd, &start, 1024*sizeof(long));
    // for(int i=0; i<10; i++)
    // {
    //     printf("%p\n", (void*)start[i]);
    // }
}