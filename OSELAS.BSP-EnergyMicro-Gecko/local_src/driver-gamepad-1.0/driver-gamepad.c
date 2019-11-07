/*
 * This is a demo Linux kernel module.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/ioport.h>
#include <linux/fs.h>
#include <asm/io.h>

#include "stdint.h"

dev_t *devno;
struct class *cl;


static int my_open (struct inode *inode, struct  file *filp) {
  printk("opening\n");

}


static int my_release (struct inode *inode, struct  file *filp) {
  printk("releasing\n");
}


static ssize_t my_read (struct  file *filp, char __user *buff, size_t count, loff_t *offp) {
  printk("reading");
}


static ssize_t my_write (struct  file *filp, char __user *buff, size_t count, loff_t *offp) {
  printk("writing");
}

static struct fileoperations my_fops = {
  .owner = THIS_MODULE,
  .read = my_read ,
  .write = my_write ,
  .open = my_open ,
  .release = my_release
};

struct cdev my_cdev;


/*
 * template_init - function to insert this module into kernel space
 *
 * This is the first of two exported functions to handle inserting this
 * code into a running kernel
 *
 * Returns 0 if successfull, otherwise -1
 */

static int __init template_init(void)
{
	printk("Hello World, here is your module speaking\n");

  cdev_init(&my_cdev, &my_fops);
  cdev_add(&my_cdev);

  // Request memory
  char *name = "GPIO";
  if (request_mem_region(GPIO_PA_BASE, (GPIO_PC_BASE + GPIO_FIC) - GPIO_PA_BASE, name) == NULL)  {
    printk("An error occured! Could not reserve memory region");
    return 1;
  }

  // This is our io address space, but dont read it directlu, use accessor functions
  void *mappReturn = ioremap_nocache(GPIO_PA_BASE, (GPIO_PC_BASE + GPIO_FIC));
  
  // Get device version number
  char *name = "device_name"
  alloc_chrdev_region(devno, 1, 1, name)

  // Make driver visible to user space
  cl = class_create(THIS_MODULE, ”my_class_name”);
  device_create(cl, NULL, devno , NULL, ”my_class_name”);

	return 0;
}




/*
 * template_cleanup - function to cleanup this module from kernel space
 *
 * This is the second of two exported functions to handle cleanup this
 * code from a running kernel
 */

static void __exit template_cleanup(void)
{
	 printk("Short life for a small module...\n");
   unregister_chrdev_region(*dev, 1);
}

module_init(template_init);
module_exit(template_cleanup);

MODULE_DESCRIPTION("Small module, demo only, not very useful.");
MODULE_LICENSE("GPL");

