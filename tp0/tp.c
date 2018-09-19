/* GPLv2 (c) Airbus */
#include <debug.h>
#include <info.h>
#include <mbi.h>

extern info_t   *info;
extern uint32_t __kernel_start__;
extern uint32_t __kernel_end__;


// Informations tires de https://www.gnu.org/software/grub/manual/multiboot/multiboot.html 

// If bit 6 in the ‘flags’ word is set, then the ‘mmap_*’ fields are valid,
// and indicate the address and length of a buffer containing a memory map 
// of the machine provided by the bios. ‘mmap_addr’ is the address, and 
// ‘mmap_length’ is the total size of the buffer. The buffer consists of one 
// or more of the following size/structure pairs (‘size’ is really used for 
// skipping to the next pair):

//              +-------------------+
//      -4      | size              |
//              +-------------------+
//      0       | base_addr         |
//      8       | length            |
//      16      | type              |
//              +-------------------+

// where ‘size’ is the size of the associated structure in bytes, which can be 
// greater than the minimum of 20 bytes. ‘base_addr’ is the starting address. 
// ‘length’ is the size of the memory region in bytes. ‘type’ is the variety of 
// address range represented, where a value of 1 indicates available ram, value 
// of 3 indicates usable memory holding ACPI information, value of 4 indicates 
// reserved memory which needs to be preserved on hibernation, value of 5 indicates
//  a memory which is occupied by defective RAM modules and all other values currently
// indicated a reserved area.

// The map provided is guaranteed to list all standard ram that should be available 
// for normal use. 

void tp()
{
   debug("kernel mem [0x%x - 0x%x]\n", &__kernel_start__, &__kernel_end__);
   debug("MBI flags 0x%x\n", info->mbi->flags);
   multiboot_info_t * mbi =  (info->mbi);
   multiboot_memory_map_t *start;
   multiboot_memory_map_t *end;

   start = (multiboot_memory_map_t *) mbi->mmap_addr;
   end = (multiboot_memory_map_t *) (mbi->mmap_addr + mbi->mmap_length);
    while ((unsigned long) start < (unsigned long) end)
   {   
        debug("Addr: 0x%x, Len: 0x%x, Type = 0x%x\n", 
                (unsigned) start->addr, (unsigned) start->len, (unsigned) start->type);
        start = (multiboot_memory_map_t *) ((unsigned long) start
                                   + sizeof(multiboot_memory_map_t));                                  
   }

    
        //    multiboot_memory_map_t *mmap;

        //    for (mmap = (multiboot_memory_map_t *) mbi->mmap_addr;
        //         (unsigned long) mmap < mbi->mmap_addr + mbi->mmap_length;
        //         mmap = (multiboot_memory_map_t *) ((unsigned long) mmap
        //                                  + mmap->size + sizeof (mmap->size)))
        //      debug (" size = 0x%x, base_addr = 0x%x%08x,"
        //              " length = 0x%x%08x, type = 0x%x\n",
        //              (unsigned) mmap->size,
        //              (unsigned) (mmap->addr >> 32),
        //              (unsigned) (mmap->addr & 0xffffffff),
        //              (unsigned) (mmap->len >> 32),
        //              (unsigned) (mmap->len & 0xffffffff),
        //              (unsigned) mmap->type);
         

}
