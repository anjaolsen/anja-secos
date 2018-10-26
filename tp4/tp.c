/* GPLv2 (c) Airbus */
#include <debug.h>
#include <info.h>
#include <pagemem.h>
#include <cr.h>

extern info_t *info;

// 1
void show_cr3()
{
   cr3_reg_t cr3 = {.raw = get_cr3()};
   debug("CR3 = %p\n", cr3.raw);
}

// 3
void enable_paging()
{
   uint32_t cr0 = get_cr0();
   set_cr0(cr0|CR0_PG);
}

void identity_init()
{
   // 2
   int      i;
   pde32_t *pgd = (pde32_t*)0x600000;
   pte32_t *ptb = (pte32_t*)0x601000;

   // 4
   for(i=0;i<1024;i++)
      pg_set_entry(&ptb[i], PG_KRN|PG_RW, i); //mettre les entrees de la PT en user et en RW

   memset((void*)pgd, 0, PAGE_SIZE); //mettre des 0 partout dans la PD
   //mettre l`@ de la premiere PT dans l`index 0 avec les attributs user et en RW
   pg_set_entry(&pgd[0], PG_KRN|PG_RW, page_nr(ptb)); 
//    Program Headers:
//   Type           Offset   VirtAddr   PhysAddr   FileSiz MemSiz  Flg Align
//   LOAD           0x000094 0x00300000 0x00300000 0x0000c 0x0000c RWE 0x4
//   LOAD           0x0000a0 0x00300010 0x00300010 0x00000 0x02000 RW  0x10
//   LOAD           0x0000b0 0x00302010 0x00302010 0x02ae0 0x03710 RWE 0x20

   // 6: il faut mapper les PTBs également
   pte32_t *ptb2 = (pte32_t*)0x602000;
   for(i=0;i<1024;i++)
      pg_set_entry(&ptb2[i], PG_KRN|PG_RW, i+1024);

   pg_set_entry(&pgd[1], PG_KRN|PG_RW, page_nr(ptb2));

   // 3
   set_cr3((uint32_t)pgd);
   enable_paging();

   // 5: #PF car l'adresse virtuelle 0x700000 n'est pas mappée
   debug("PTB[1] = %p\n", ptb[1].raw);

   // 7
   pte32_t  *ptb3    = (pte32_t*)0x603000; //on cree un ptr sur  la 3eme entree de la PGD
   uint32_t *target  = (uint32_t*)0xc0000000; //on cree un un ptr sur une @ virtuelle
   int      pgd_idx = pd32_idx(target); // on recupere l index dans la PGD de cette @ (shift 22 et &10 bits)
   int      ptb_idx = pt32_idx(target); //on recupere l index de la PTB de cette @ (shift 16 et &10 bits)

   memset((void*)ptb3, 0, PAGE_SIZE);
   pg_set_entry(&ptb3[ptb_idx], PG_KRN|PG_RW, page_nr(pgd)); //page_nr page directory....1000 entrees
   pg_set_entry(&pgd[pgd_idx], PG_KRN|PG_RW, page_nr(ptb3)); 

   debug("PGD[0] = %p | target = %p\n", pgd[0].raw, *target);

   // 8: mémoire partagée
   char *v1 = (char*)0x700000; //adresse virtuelle
   char *v2 = (char*)0x7ff000; //adresse virtuelle

   ptb_idx = pt32_idx(v1);
   pg_set_entry(&ptb2[ptb_idx], PG_KRN|PG_RW, 2);

   ptb_idx = pt32_idx(v2);
   pg_set_entry(&ptb2[ptb_idx], PG_KRN|PG_RW, 2);

   debug("%p = %s | %p = %s\n", v1, v1, v2, v2);

   
   // 9 : il faut également vider les TLBs
   *target = 0;
//    invalidate(target);
}


void tp()
{
    show_cr3();
    identity_init();
    // show_cr3();

}
