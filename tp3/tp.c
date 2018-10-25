/* GPLv2 (c) Airbus */

#include <debug.h>
#include <segmem.h>
#include <string.h>
#include <info.h>

extern info_t *info;

#define c0_idx  1
#define d0_idx  2
#define c3_idx  3
#define d3_idx  4
#define ts_idx  5

#define c0_sel  gdt_krn_seg_sel(c0_idx)
#define d0_sel  gdt_krn_seg_sel(d0_idx)
#define c3_sel  gdt_usr_seg_sel(c3_idx)
#define d3_sel  gdt_usr_seg_sel(d3_idx)
#define ts_sel  gdt_krn_seg_sel(ts_idx)


seg_desc_t GDT[6];
tss_t      TSS;

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
#define tss_dsc(_dSc_,_tSs_)                                            \
   ({                                                                   \
      raw32_t addr    = {.raw = _tSs_};                                 \
      (_dSc_)->raw    = sizeof(tss_t);                                  \
      (_dSc_)->base_1 = addr.wlow;                                      \
      (_dSc_)->base_2 = addr._whigh.blow;                               \
      (_dSc_)->base_3 = addr._whigh.bhigh;                              \
      (_dSc_)->type   = SEG_DESC_SYS_TSS_AVL_32;                        \
      (_dSc_)->p      = 1;                                              \
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

   /* fptr32_t fptr;
   fptr.offset = (uint32_t) userland;
   fptr.segment = c3_sel;

   farjump(fptr); */

   //general protection fault.

   //3.4 

   /* asm volatile (
      "push %0    \n" // ss
      "push %%ebp \n" // esp
      "pushf      \n" // eflags
      "push %1    \n" // cs
      "push %2    \n" // eip
      "iret"
      ::
       "i"(d3_sel),   //ss
       "i"(c3_sel),   //cs
       "r"(&userland) //eip
       ); */

   /* déclenche TSS invalid car le mov cr0 génère un exception #GP avec
    * changement de niveau de privilège, donc le CPU regarde le TSS pour
    * charger la pile ring0 (TSS.esp0), et comme nous n'avons pas
    * configuré de TSS dans notre GDT ... ca plante.
    */

      //    qemu: fatal: invalid tss type
      // EAX=00304317 EBX=0002be40 ECX=00304d40 EDX=00000003
      // ESI=0002bfc2 EDI=0002bfc3 EBP=00301fe4 ESP=00301fe4
      // EIP=0030431a EFL=00000082 [--S----] CPL=3 II=0 A20=1 SMM=0 HLT=0
      // ES =0023 00000000 ffffffff 00cff300 DPL=3 DS   [-WA]
      // CS =001b 00000000 ffffffff 00cffa00 DPL=3 CS32 [-R-]
      // SS =0023 00000000 ffffffff 00cff300 DPL=3 DS   [-WA]
      // DS =0023 00000000 ffffffff 00cff300 DPL=3 DS   [-WA]
      // FS =0023 00000000 ffffffff 00cff300 DPL=3 DS   [-WA]
      // GS =0023 00000000 ffffffff 00cff300 DPL=3 DS   [-WA]
      // LDT=0000 00000000 0000ffff 00008200 DPL=0 LDT
      // TR =0000 00000000 0000ffff 00008b00 DPL=0 TSS32-busy
      // GDT=     00305940 0000002f
      // IDT=     00305140 000007ff
      // CR0=00000011 CR2=00000000 CR3=00000000 CR4=00000000
      // DR0=00000000 DR1=00000000 DR2=00000000 DR3=00000000 
      // DR6=ffff0ff0 DR7=00000400
      // CCS=00000080 CCD=fffffff2 CCO=EFLAGS  
      // EFER=0000000000000000
      // FCW=037f FSW=0000 [ST=0] FTW=00 MXCSR=00001f80
      // FPR0=0000000000000000 0000 FPR1=0000000000000000 0000
      // FPR2=0000000000000000 0000 FPR3=0000000000000000 0000
      // FPR4=0000000000000000 0000 FPR5=0000000000000000 0000
      // FPR6=0000000000000000 0000 FPR7=0000000000000000 0000
      // XMM00=00000000000000000000000000000000 XMM01=00000000000000000000000000000000
      // XMM02=00000000000000000000000000000000 XMM03=00000000000000000000000000000000
      // XMM04=00000000000000000000000000000000 XMM05=00000000000000000000000000000000
      // XMM06=00000000000000000000000000000000 XMM07=00000000000000000000000000000000
      // ../utils/rules.mk:58: recipe for target 'qemu' failed

      TSS.s0.esp = get_ebp(); //stack pointer of ring0
      TSS.s0.ss  = d0_sel; //ring 0 data selector
      tss_dsc(&GDT[ts_idx], (offset_t)&TSS);
      set_tr(ts_sel);

      asm volatile (
            "push %0    \n" // ss
            "push %%ebp \n" // esp
            "pushf      \n" // eflags
            "push %1    \n" // cs
            "push %2    \n" // eip
            "iret"
            ::
            "i"(d3_sel),
            "i"(c3_sel),
            "r"(&userland)
            );


//    debug("lol 0x%lx 0x%lx \n", fptr.offset, fptr.segment);
}
