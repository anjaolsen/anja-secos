/* GPLv2 (c) Airbus */
#include <debug.h>
#include <info.h>
#include <mbi.h>

extern info_t   *info;
extern uint32_t __kernel_start__;
extern uint32_t __kernel_end__;


// Informations trouves sur
// https://www.gnu.org/software/grub/manual/multiboot/multiboot.html 


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

//q3 - pas de segfault!
   int * ptr = (int *) 0x003049e0;
   *ptr = 1;

   debug("%d\n", *ptr);

   ptr = (int *) 0x9fc00;
   *ptr = 1;

   start = (multiboot_memory_map_t *) mbi->mmap_addr;
   end = (multiboot_memory_map_t *) (mbi->mmap_addr + mbi->mmap_length);
  
   while ((unsigned long) start < (unsigned long) end)
   {   
        debug("Addr: 0x%x, Len: 0x%x, Type = 0x%x\n", 
                (unsigned) start->addr, (unsigned) start->len, (unsigned) start->type);
        start = (multiboot_memory_map_t *) ((unsigned long) start
                                   + sizeof(multiboot_memory_map_t)); 
                              
   }

   ptr = (int *) 0xfffc0000;
   *ptr = 1;
   while ((unsigned long) start < (unsigned long) end)
   {   
        debug("Addr: 0x%x, Len: 0x%x, Type = 0x%x\n", 
                (unsigned) start->addr, (unsigned) start->len, (unsigned) start->type);
        start = (multiboot_memory_map_t *) ((unsigned long) start
                                   + sizeof(multiboot_memory_map_t)); 
                              
   }

   //pas de sagfault parce que l on n a pas encore active les segfaults
   //il est en plein crise existensielle, juste on ne l entends pas

//q4 - hors ram
   ptr = (int *) 0x8FFFFFF;
   debug("%d\n", *ptr);
   *ptr = 1;
   debug("%d\n", *ptr);
   //ca n a pas change la valeur du contenu de ptr

}


/*

    q1
        objdump -D kernel.elf *pipe* less 

*/


/*


Addr: 0x0, Len: 0x9fc00, Type = 0x1 - available ram (interrupt vector area)
Addr: 0x9fc00, Len: 0x400, Type = 0x2 - reserved ram
Addr: 0xf0000, Len: 0x10000, Type = 0x2 - reserved ram (BIOS code area)
Addr: 0x100000, Len: 0x7ee0000, Type = 0x1 - available ram
Addr: 0x7fe0000, Len: 0x20000, Type = 0x2 - reserved ram
Addr: 0xfffc0000, Len: 0x40000, Type = 0x2 - reserved ram


*/

