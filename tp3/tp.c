/* GPLv2 (c) Airbus */
#include <debug.h>
#include <info.h>
#include <segmem.h>
#include <string.h>

extern info_t *info;

#define CSD_TYPE 0x9A
#define DSD_TYPE 0x92

extern info_t *info;

//attention : limite absolue, pas relative
void gdt_setup_descriptor(seg_desc_t * seg_p, unsigned long base, unsigned long limit, uint8_t type){//}, ,  unsigned char access, unsignes char gran){
    // debug("Avant chgmt:  0x%lx       Adresse:  0x%x   \n", *seg_p, seg_p);
    uint8_t * oct = (uint8_t*)seg_p;
    
    if ((limit > 65536) && (limit & 0xFFF) != 0xFFF) {
        debug("Impossible lim 0x%lx\n", limit);
    }
    if (limit > 65536) {
       // Adjust granularity if required
        debug("Gran. Ancienne limite  0x%lx\n", limit);
        limit = limit >> 12;
        oct[6] = 0xC0;
        debug("Gran. Nouvelle limite  0x%lx\n", limit);

    } else {
       oct[6] = 0x40;
    }
    seg_p->g = 1&0x1;

    oct[2] = base & 0xFF;
    oct[3] =  (base >> 8) & 0xFF;
    oct[4] =  (base >> 16) & 0xFF;
    oct[7] =  (base >> 24) & 0xFF;
 
    oct[0] = limit & 0xFF;
    oct[1] = (limit >> 8)  & 0xFF;
    oct[6] |= (limit >> 16)  & 0xFF;

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
    debug("Numero de segments:  %d   \n", nombre_de_segments);

    seg_desc_t * seg_ptr = (seg_desc_t *)gdt.addr;
    for (int i = 0; i < nombre_de_segments; i++){
        
        uint32_t adr = ((seg_ptr->base_1)&0xFF) + (((seg_ptr->base_2)&0xF)<<16) + (((seg_ptr->base_3)&0xF)<<24); 
        debug("Adr seg-desc:  0x%x   \n", adr);

        uint32_t lim = (seg_ptr->limit_1) + ((seg_ptr->limit_2)<<16);
        debug("Limite seg-desc:  0x%x   \n", lim);

        uint32_t seg_type = ((seg_ptr->type)&0xF) + ((seg_ptr->s&0x1)<< 4) + ((seg_ptr->dpl&0x3)<<5) + ((seg_ptr->p&0x1)<<7);
        debug("Type seg-desc:  0x%x   \n", seg_type);

        debug("Long mode (32bits)  0x%x   \n", seg_ptr->l);

        debug("Granularity  0x%x   \n", seg_ptr->g);

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
    debug("Limite :  0x%x, Adr de base : 0x%x\n", gdt_addr.limit, gdt_addr.addr);

    debug("==============================================\n");
    debug("==============Configuration GDT===============\n");
    debug("==============================================\n");

    gdt_reg_t gdt_addr_new;
    seg_desc_t gdt_tab[6];
    if ((int )&gdt_tab[0] % 8 != 0){
        debug("Not aligned! T0D0. current addr = 0x%x \n", gdt_tab);
    }
    gdt_addr_new.desc = &gdt_tab[0];
    gdt_addr_new.limit = 4 * sizeof(seg_desc_t) - 1;

    debug("Limite :  0x%x, Adr de base : 0x%lx\n", gdt_addr_new.limit, gdt_addr_new.addr);

    gdt_tab[0] = (seg_desc_t) *gdt_addr.desc ;
    gdt_setup_descriptor(&gdt_tab[1], 0x00000, 0xfff0, CSD_TYPE);
    gdt_setup_descriptor(&gdt_tab[2], 0x00000, 0xfff0, DSD_TYPE);
    gdt_setup_descriptor(&gdt_tab[3], 0x600000, 0x20, DSD_TYPE);
    gdt_setup_descriptor(&gdt_tab[4], 0x700000, 0x10000, CSD_TYPE);
    //modifier type
    gdt_setup_descriptor(&gdt_tab[5], 0x800000, 0x10000, DSD_TYPE);
    //modifier type
    gdt_tab[3].g = 0&0x1;
    display_gdt(gdt_addr_new);

    set_gdtr(gdt_addr_new); 
    set_cs(0x08);
    set_ds(0x10);
    set_ss(0x10);
    
    set_fs(0x10);
    set_gs(0x10);
    //https://stackoverflow.com/questions/23978486/far-jump-in-gdt-in-bootloader

    set_es(0x18);


}