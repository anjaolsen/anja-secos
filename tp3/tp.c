/* GPLv2 (c) Airbus */

// commentaire : je me permets de repartir de la correction du tp1
// Ca petmet de mieux comprendre tous les élements de la correction

#include <debug.h>
#include <segmem.h>
#include <string.h>
#include <info.h>

extern info_t *info;

#define c0_idx  1
#define d0_idx  2
#define c3_idx  3
#define d3_idx  4

#define c0_sel  gdt_krn_seg_sel(c0_idx)
#define d0_sel  gdt_krn_seg_sel(d0_idx)
#define c3_sel  gdt_usr_seg_sel(c3_idx)
#define d3_sel  gdt_usr_seg_sel(d3_idx)


seg_desc_t GDT[6];

#define gdt_flat_dsc(_dSc_,_pVl_,_tYp_)                                 \
   ({                                                                   \
      (_dSc_)->raw     = 0;                                             \
      (_dSc_)->limit_1 = 0xffff;                                        \
      (_dSc_)->limit_2 = 0xf;                                           \
      (_dSc_)->type    = _tYp_;                                         \
      (_dSc_)->dpl     = _pVl_;                                         \
      (_dSc_)->d       = 1;                                             \
      (_dSc_)->g       = 1;                                             \
      (_dSc_)->s       = 1;                                             \
      (_dSc_)->p       = 1;                                             \
   })

#define c0_dsc(_d) gdt_flat_dsc(_d,0,SEG_DESC_CODE_XR)
#define d0_dsc(_d) gdt_flat_dsc(_d,0,SEG_DESC_DATA_RW)
#define c3_dsc(_d) gdt_flat_dsc(_d,3,SEG_DESC_CODE_XR)
#define d3_dsc(_d) gdt_flat_dsc(_d,3,SEG_DESC_DATA_RW)

void show_gdt()
{
   gdt_reg_t gdtr;
   size_t    i,n;

   get_gdtr(gdtr);
   n = (gdtr.limit+1)/sizeof(seg_desc_t);
   for(i=0 ; i<n ; i++) {
      seg_desc_t *dsc   = &gdtr.desc[i];
      uint32_t    base  = dsc->base_3<<24 | dsc->base_2<<16 | dsc->base_1;
      uint32_t    limit = dsc->limit_2<<16| dsc->limit_1;
      debug("GDT[%d] = 0x%llx | base 0x%x | limit 0x%x | type 0x%x\n",
            i, gdtr.desc[i].raw, base, limit, dsc->type);
   }
   debug("--\n");
}

void init_gdt()
{
   gdt_reg_t gdtr;

   GDT[0].raw = 0ULL;

   c0_dsc( &GDT[c0_idx] );
   d0_dsc( &GDT[d0_idx] );
   c3_dsc( &GDT[c3_idx] );
   d3_dsc( &GDT[d3_idx] );

   gdtr.desc  = GDT;
   gdtr.limit = sizeof(GDT) - 1;
   set_gdtr(gdtr);

   set_cs(c0_sel);

   set_ss(d0_sel); //chargement de d3_sel ici: general protection fault.

   //chargements des registres suivants avec le descritpeur de ring 3 : rien ne change
   set_ds(d0_sel);
   set_es(d0_sel);
   set_fs(d0_sel);
   set_gs(d0_sel);
}

void userland()
{
   asm volatile ("mov %eax, %cr0");
}

void tp()
{
   show_gdt();
   init_gdt();

   set_ds(d3_sel);
   set_es(d3_sel);
   set_fs(d3_sel);
   set_gs(d3_sel);



   // 3.3
//    fptr32_t fptr = {.segment = c3_sel, .offset = (uint32_t)userland}; 
//    farjump(fptr);


//    3.3 - Essayez d'effectuer un "far jump" vers la fonction
// "userland()". Pour cela il faut charger dans "CS" le sélecteur de code
// ring 3 et dans EIP l'adresse de la fonction "userland()". Vous pouvez
// utiliser le type "fptr32_t" et la fonction "farjump()" de notre noyau

//    set_cs(c3_sel);

   fptr32_t fptr;
   fptr.offset = (uint32_t) userland;
   fptr.segment = c3_sel;

//    fptr32_t fptr = {.segment = c3_sel, .offset = (uint32_t)userland}; 
   farjump(fptr);
//    debug("lol 0x%lx 0x%lx \n", fptr.offset, fptr.segment);
//    farjump(fptr);
}
