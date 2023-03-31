#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/kthread.h>
#include <linux/module.h>
#include <linux/kallsyms.h>
#include <linux/delay.h>
#include <asm/pgtable.h>

#define ENTER() printk(KERN_DEBUG "%s() Enter", __func__)
#define EXIT() printk(KERN_DEBUG "%s() Exit", __func__)
#define ERR(fmt, args...) printk(KERN_ERR "%s()-%d: " fmt "\n", __func__, __LINE__, ##args)
#define DBG(fmt, args...) printk(KERN_DEBUG "%s()-%d: " fmt "\n", __func__, __LINE__, ##args)

static struct task_struct *test_kthread = NULL; 

void k_pte_get(void)
{
	int a = 0;
	unsigned long va = &a;
	pte_t *pte;

	struct mm_struct init_mm;
	unsigned long init_mm_address = (struct mm_struct *)kallsyms_lookup_name("init_mm");
	printk("init_mm_address:%lx\n", init_mm_address);
	memcpy(&init_mm, init_mm_address, sizeof(struct mm_struct));

	pte = pte_offset_kernel(pmd_offset(pud_offset(p4d_offset(pgd_offset(&init_mm, va), va), va), va), va);

	unsigned long pte_val = pte_val(*pte);
	unsigned long pfn = pte_pfn(*pte);
	unsigned long pa = pfn * PAGE_SIZE + va % PAGE_SIZE;
	printk("va:%016lx, pfn:%016lx, pte_val:%016lx, pa:%016lx\n", va, pfn, pte_val, pa);
}

void p_pte_get(void)
{
	pte_t *pte;
	struct task_struct *task = pid_task(find_get_pid(189), PIDTYPE_PID);
	struct mm_struct *mm = task->mm;

	unsigned long va = mm->start_code;

	pte = pte_offset_kernel(pmd_offset(pud_offset(p4d_offset(pgd_offset(mm, va), va), va), va), va);

	unsigned long pte_val = pte_val(*pte);
	unsigned long pfn = pte_pfn(*pte);
	unsigned long pa = pfn * PAGE_SIZE + va % PAGE_SIZE;
	printk("va:%016lx, pfn:%016lx, pte_val:%016lx, pa:%016lx\n", va, pfn, pte_val, pa);
}


static int kthread_test_func(void *data) // 定义一个内核线程要执行的函数
{
	k_pte_get();

	p_pte_get();


	while (!kthread_should_stop())
	{
		// printk("kthread is running\n");
		msleep(5000);
	}

	// EXIT();
	return 0;
}

static __init int kthread_test_init(void)
{
	// ENTER();

	test_kthread = kthread_run(kthread_test_func, NULL, "kthread-test"); // 创建线程kthread-test，并且运行
	if (!test_kthread)
	{
		printk("kthread_run fail");
		return -ECHILD;
	}

	// EXIT();
	return 0;
}

static __exit void kthread_test_exit(void)
{
	// ENTER();
	if (test_kthread)
	{
		printk("kthread_stop\n");
		kthread_stop(test_kthread); // 停止内核线程
		test_kthread = NULL;
	}

	// EXIT();
}

module_init(kthread_test_init);
module_exit(kthread_test_exit);

MODULE_AUTHOR("yzc,2922165563@qq.com");
MODULE_LICENSE("GPL");
