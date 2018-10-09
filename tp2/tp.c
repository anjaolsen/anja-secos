/* GPLv2 (c) Airbus */
#include <debug.h>
#include <info.h>
#include <intr.h>
#include <segmem.h>

extern info_t *info;

// http://www.osdever.net/bkerndev/Docs/idt.htm
//https://manybutfinite.com/ 
// http://www.jamesmolloy.co.uk/tutorial_html/4.-The%20GDT%20and%20IDT.html


void bp_handler() 
{
    debug("\n\n\n\n");
    debug("BP handler\n");
    debug("\n\n\n\n");

}

void bp_trigger() 
{

    

// When an interrupt occurs, the CPU does the following:

//     Push the current address (contents of the Instruction Pointer) onto the stack; also, push the processor flags (but not all the other processor registers)
//     Jump to the address of the ISR (Interrupt Service Routine), which is specified in the Interrupt Descriptor Table.

// The ISR should do the following:

//     Push any registers which it intends to alter (or, push all registers)
//     Handle the interrupt
//     Reenable interrupts
//     Pop any registers which it pushed
//     Use the IRET instructions, which pops the CPU flags and Instruction Pointer value from the stack (and thus returns to whatever was executing when the interrupt occured).


    debug("\n\n\n\n");
    debug("BP trigger\n");
    asm("INT3"); 
    //int3();
    debug("\n\n\n\n");
    debug("BP trigger2\n");
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

    

    idt_reg_t * idt_;
    get_idtr(idt_);
    int_desc_t * old_IDT; 
    debug("Adresse de chargement IDT : 0x%x\n", idt_);

    old_IDT = idt_->desc;
    for (int i = 0; i < 10; i++){
        debug("Offset : 0x%x\n", (old_IDT[i].offset_1) + ((old_IDT[i].offset_2)<<16));
    }
    debug("\n");

    // old_IDT[3].offset_1 = (offset_t)&bp_handler & 0xFFFF;
    // old_IDT[3].offset_2 = ((offset_t)&bp_handler>>16) & 0xFFFF;
    // ou :
    int_desc(&old_IDT[3], gdt_krn_seg_sel(1), (offset_t)bp_handler);

    // set_idtr(idt_);
    for (int i = 0; i < 10; i++){
        debug("Offset : 0x%x\n", (old_IDT[i].offset_1) + ((old_IDT[i].offset_2)<<16));
    }


    bp_trigger();




}
