#include "types.h"
#include "riscv.h"
#include "param.h"
#include "defs.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;


  argint(0, &n);
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

#define LAB_PGTBL 1
#ifdef LAB_PGTBL
int
sys_pgaccess(void)
{
  uint64 vaddr;
  uint64 abits;
  int page_num;
  argaddr(0, &vaddr);
  argint(1, &page_num);
  argaddr(2, &abits);
  if (page_num > 64 || page_num < 0)
    return -1;
  struct proc *p = myproc();
  pagetable_t pgtbl = p->pagetable;
  uint64 res = 0;
  
  for (int i = 0; i < page_num; i++) {
    if (vaddr+i*PGSIZE >= MAXVA)
      return -1;
    pte_t *pte = walk(pgtbl, vaddr+i*PGSIZE, 0);
    if (!pte)
      continue;
    if ((*pte & PTE_V) == 0)
      continue;
    if ((*pte & PTE_U) == 0)
      continue;
    if (*pte & PTE_A) {
      res |= (1L << i);
      *pte &= ~PTE_A; //clear PTE_A
    }
  }
  if(copyout(p->pagetable, abits, (char *)&res, (page_num + 8 - 1)/8) < 0)
    return -1;
  return 0;
}
#endif

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}
