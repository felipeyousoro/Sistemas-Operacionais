#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/slab.h>

struct birthday {
       int day;
       int month;
       int year;

       struct list_head list;
};

static LIST_HEAD(birthday_list);

struct birthday *birthday_new(int day, int month, int year)
{
       struct birthday *new_birthday;

       new_birthday = kmalloc(sizeof(*new_birthday), GFP_KERNEL);

       new_birthday->day = day;
       new_birthday->month = month;
       new_birthday->year = year;

       INIT_LIST_HEAD(&new_birthday->list);

       return new_birthday;
}

/* This function is called when the module is loaded. */
int simple_init(void){

       struct birthday *persons[5], *ptr;

       int i;
       for(i = 0; i < 5; i++) {
               persons[i] = birthday_new(i+1, 2*(i+1), 2000 + i);
               list_add_tail(&persons[i]->list, &birthday_list);
       }

       list_for_each_entry(ptr, &birthday_list, list) {
               printk(KERN_INFO "day: %d, month: %d, year: %d\n",
                       ptr->day, ptr->month, ptr->year);
       }

       return 0;
}

/* This function is called when the module is removed. */
void simple_exit(void) {
	
       struct birthday *ptr, *next;

       list_for_each_entry_safe(ptr, next, &birthday_list, list) {
               list_del(&ptr->list);
               kfree(ptr);
       }
}

/* Macros for registering module entry and exit points. */
module_init( simple_init );
module_exit( simple_exit );

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Simple Module");
MODULE_AUTHOR("SGG");

