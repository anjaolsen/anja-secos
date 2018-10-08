/* GPLv2 (c) Airbus */
#include <debug.h>
#include <info.h>
#include <segmem.h>

extern info_t *info;

// http://www.osdever.net/bkerndev/Docs/idt.htm
//https://manybutfinite.com/ 


void bp_handler() 
{
    debug("BP handler\n");

}

void bp_trigger() 
{
    debug("BP trigger\n");
    asm("INT3"); //$
    //int3();
    debug("BP trigger\n");
}


void tp()
{
    //q1)

    // int a = 1;
    // int b = 0;


    // while (1) {
    //     a = a/b;
    // }

    //resultat : 

    // IDT event
    //  . int    #0
    //  . error  0xffffffff
    //  . cs:eip 0x8:0x303f82
    //  . ss:esp 0x303052:0x302008
    //  . eflags 0x6

    // - GPR
    // eax     : 0x1
    // ecx     : 0x304920
    // edx     : 0x0
    // ebx     : 0x2be40
    // esp     : 0x301fc4
    // ebp     : 0x301fe8
    // esi     : 0x2bfc2
    // edi     : 0x2bfc3

    // Exception: Divide by zero
    // cr0 = 0x11
    // cr4 = 0x0

    // -= Stack Trace =-
    // 0x30305a
    // 0x303020
    // 0x8c85
    // 0x72bf0000
    // fatal exception !

    //3.1

    idt_reg_t * idt_;
    get_idtr(idt_);

    debug("Adresse de chargement IDT : 0x%x\n", idt_);
    bp_trigger();

    //3.2


}
