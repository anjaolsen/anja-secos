/* GPLv2 (c) Airbus */
#include <debug.h>
#include <info.h>
#include <segmem.h>


// Tips: https://wiki.osdev.org/Global_Descriptor_Table
// http://www.osdever.net/bkerndev/Docs/gdt.htm 
// We can use an entry in the GDT to generate segment 
// violation exceptions that give the kernel an opportunity
//  to end a process that is doing something it shouldn't.
// 
// The GDT itself is a list of 64-bit long entries. 
// These entries define where in memory that the allowed 
// region will start, as well as the limit of this region, 
// and the access privileges associated with this entry. 
// One common rule is that the first entry in your GDT, 
// entry 0, is known as the NULL descriptor. No segment register 
// should be set to 0, otherwise this will cause a General 
// Protection fault, and is a protection feature of the processor. 
// The General Protection Fault and several other types of 
// 'exceptions' will be explained in detail under the section 
// on Interrupt Service Routines (ISRs).

#define CSD_TYPE 0x9A
#define DSD_TYPE 0x92

extern info_t *info;

void gdt_setup_descriptor(seg_desc_t * seg_p, unsigned long base, unsigned long limit, uint8_t type){//}, ,  unsigned char access, unsignes char gran){
    // debug("Avant chgmt:  0x%lx       Adresse:  0x%x   \n", *seg_p, seg_p);
    uint8_t * oct = (uint8_t*)seg_p;
    
    oct[2] = base & 0xFF;
    oct[3] =  (base >> 8) & 0xFF;
    oct[4] =  (base >> 16) & 0xFF;
    oct[7] =  (base >> 24) & 0xFF;
 
    oct[0] = limit & 0xFF;
    oct[1] = (limit >> 8)  & 0xFF;
    oct[6] = (limit >> 16)  & 0xFF;

    oct[5] = type & 0xFF;
    seg_p->l = 1&0x1;

    //debug("Apres chgmt:  0x%lx       Adresse:  0x%x   \n", *seg_p, seg_p);
    //NB:  oct is 2 bytes as for now. look into this
    //https://wiki.osdev.org/GDT_Tutorial#How_can_we_do_that.3F 
    //http://www.osdever.net/bkerndev/Docs/gdt.htm
    //http://tuttlem.github.io/2014/07/11/a-gdt-primer.html 
    // https://manybutfinite.com/post/cpu-rings-privilege-and-protection/
}

void display_gdt(gdt_reg_t gdt){

    unsigned short gdtr_size = gdt.limit + 1;
    
    int nombre_de_segments = gdtr_size / sizeof(seg_desc_t);
    debug("nb:  %d   \n", nombre_de_segments);

    seg_desc_t * seg_ptr = (seg_desc_t *)gdt.addr;
    for (int i = 0; i < nombre_de_segments; i++){
        
        uint32_t adr = ((seg_ptr->base_1)&0xFF) + (((seg_ptr->base_2)&0xF)<<16) + (((seg_ptr->base_3)&0xF)<<24); 
        debug("Adr seg-desc:  0x%x   \n", adr);

        uint32_t lim = (seg_ptr->limit_1) + ((seg_ptr->limit_2)<<16);
        debug("Limite seg-desc:  0x%x   \n", lim);

        uint32_t seg_type = ((seg_ptr->type)&0xF) + ((seg_ptr->s&0x1)<< 4) + ((seg_ptr->dpl&0x3)<<5) + ((seg_ptr->p&0x1)<<7);
        debug("Type seg-desc:  0x%x   \n", seg_type);

        debug("Long mode (32bits)  0x%x   \n", seg_ptr->l);

        seg_ptr++;
        
 
     // uint64_t    limit_1:16;    /* bits 00-15 of the segment limit */
     // uint64_t    base_1:16;     /* bits 00-15 of the base address */
     // uint64_t    base_2:8;      /* bits 16-23 of the base address */
     // uint64_t    type:4;        /* segment type */
     // uint64_t    s:1;           /* descriptor type */
     // uint64_t    dpl:2;         /* descriptor privilege level */
     // uint64_t    p:1;           /* segment present flag */
     // uint64_t    limit_2:4;     /* bits 16-19 of the segment limit */
     // uint64_t    avl:1;         /* available for fun and profit */
     // uint64_t    l:1;           /* longmode */
     // uint64_t    d:1;           /* default length, depend on seg type */
     // uint64_t    g:1;           /* granularity */
     // uint64_t    base_3:8;      /* bits 24-31 of the base address */

        debug("------------------------------------------------\n", seg_type);

    }
}

void tp()
{
    gdt_reg_t gdt_addr;
    get_gdtr(gdt_addr); //l'@ de la première entrée
    //gdt_reg_t * gdt_ptr = &gdt_addr;
    debug("Limite :  0x%x, Adr de base : 0x%x\n", gdt_addr.limit, gdt_addr.addr);
    //gdt_ptr++;
    //debug("Limite :  0x%x, Adr de base : 0x%x\n", gdt_ptr->limit, gdt_ptr->addr);
    
    debug("==============================================\n");
    debug("==============Affichage contenu GDT===========\n");
    debug("==============================================\n");


    display_gdt(gdt_addr);


    debug("==============================================\n");
    debug("==============Configuration GDT===============\n");
    debug("==============================================\n");

    // remettre le pointeur au debut
    //seg_desc_t seg_ptr = (seg_desc_t *)gdt_addr.addr;
    
    
    //gdt_setup_descriptor(seg_ptr+2, 0x300000, 1000, DSD_TYPE);

    gdt_reg_t gdt_addr_new;
    seg_desc_t gdt_tab[3];
    if ((int )&gdt_tab[0] % 8 != 0){
        debug("Not aligned! addr = 0x%x \n", gdt_tab);
    }
    gdt_addr_new.desc = &gdt_tab[0];
    gdt_addr_new.limit = 3 * sizeof(seg_desc_t) - 1;

    debug("Limite :  0x%x, Adr de base : 0x%lx\n", gdt_addr_new.limit, gdt_addr_new.addr);

    gdt_tab[0] = (seg_desc_t) *gdt_addr.desc ;
    gdt_setup_descriptor(&gdt_tab[1], 0x00000, 0xfffff, 0x9A);
    gdt_setup_descriptor(&gdt_tab[2], 0x00000, 0xfffff, 0x92);

    display_gdt(gdt_addr_new);

    set_gdtr(gdt_addr_new); 




}

