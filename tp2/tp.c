/* GPLv2 (c) Airbus */
#include <debug.h>
#include <info.h>
#include <intr.h>
#include <segmem.h>
extern void resume_from_intr();
/*
IDT- les 32 premieres sont les exception du systeme
A partir du 32 on trouve les IRQ. Certains generent des codes d'err et certains ne le font pas.
@F      0


Pousse dans la pile:
flags
CS
EIP     <- esp

si c une erreur: ca rajoute un code d'erreur a la fin.

L'idee est de dire que les gestionneur d'err sont les petites fonctions dans idt.s
push -1: c un faux code d'erreur
puis on rajoute le numéro de l'IT.

common: sauvegrade tous les reg.++
esp va pointer en pile a l'endroit ou on a tout sauvegardé. (stack layout)
struct qui contient tout ca.

Qd on crée une fct en c le compilateur empile encore des trucs. 
C'est pour ca que notre code ne marche pas.
En plus elles font un RET a la fin. Ce RET n'a aucun sens dans notre cas.
on doit realigner la pile avec un leave puis faire un IRET


/// sjekke i deassemblert versjon hvilken adresse jeg hopper til.


///q7 message de debug apres BP trigger: 

*/


extern info_t *info;

// http://www.osdever.net/bkerndev/Docs/idt.htm
//https://manybutfinite.com/ 
// http://www.jamesmolloy.co.uk/tutorial_html/4.-The%20GDT%20and%20IDT.html



// When an interrupt occurs, the CPU does the following:

//     Push the current address (contents of the Instruction Pointer) onto the stack; also, push the processor flags (but not all the other processor registers)
//     Jump to the address of the ISR (Interrupt Service Routine), which is specified in the Interrupt Descriptor Table.

// Stack layout when we enter the handler: 
// ss      <-  if change of priviledge
// esp     <-  if change of priviledge
// flags
// CS
// EIP             <- esp (we want sp to point here in out #BP case)
// errorcode 


// The ISR should do the following:

//     Push any registers which it intends to alter (or, push all registers)
//     Handle the interrupt
//     Reenable interrupts
//     Pop any registers which it pushed
//     Use the IRET instructions, which pops the CPU flags and Instruction Pointer value from the stack (and thus returns to whatever was executing when the interrupt occured).

void bp_handler() 
{
    asm volatile ("pusha");
    debug("\n\n\n\n");
    debug("BP handler\n");
    asm volatile ("popa");
    asm volatile ("add $4, %esp"); //depiler error code
    asm volatile ("iret"); // iret depile eip, cs et eflags
}

void bp_trigger() 
{
    debug("\n\n\n\n");
    debug("BP trigger\n");
    asm("INT3"); 
    //int3();
    debug("\n\n\n\n");
    debug("BP trigger retour\n");
}

void displayIdt(int_desc_t *IDT){
     for (int i = 0; i < 5; i++){
    //   uint64_t  offset_1:16;    /* bits 00-15 of the isr offset */
    //   uint64_t  selector:16;    /* isr segment selector */
    //   uint64_t  ist:3;          /* stack table: only 64 bits */
    //   uint64_t  zero_1:5;       /* must be 0 */
    //   uint64_t  type:4;         /* interrupt/trap gate */
    //   uint64_t  zero_2:1;       /* must be zero */
    //   uint64_t  dpl:2;          /* privilege level */
    //   uint64_t  p:1;            /* present flag */
    //   uint64_t  offset_2:16;    /* bits 16-31 of the isr offset */
        debug("Offset : 0x%x\n", (IDT[i].offset_1 + ((IDT[i].offset_2)<<16)));
        debug("Selector : 0x%x\n", IDT[i].selector & 0xFFFF);
        debug("Ist : 0x%x\n", IDT[i].ist & 0x7);
        debug("zero_1 : 0x%x\n", IDT[i].zero_1 & 0x1F);
        debug("zero_2 : 0x%x\n", IDT[i].zero_2 & 0x1);
        debug("type : 0x%x\n", IDT[i].type & 0xF);
        debug("dpl : 0x%x\n", IDT[i].dpl & 0x3);
        debug("p : 0x%x\n", IDT[i].p & 0x1);
        debug("\n");
    }
}

void tp()
{
    idt_reg_t * idt_r;
    get_idtr(idt_r);
    int_desc_t * _IDT; 
    debug("Adresse de chargement IDT : 0x%x\n", idt_r->addr);

    _IDT = idt_r->desc;
    displayIdt(_IDT);
   
    debug("\n");
    debug("\n");
    debug("\n");
    debug("\n");

    int_desc(&_IDT[3], gdt_krn_seg_sel(1), (offset_t)bp_handler);

    // set_idtr(idt_);
    displayIdt(_IDT);

    bp_trigger();

}
