#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/ioport.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <asm/io.h>


#include "efm32gg.h"

dev_t *devno;
struct class *cl;


static int my_open (struct inode *inode, struct  file *filp) {
  printk("opening\n");
  return 0;
}

static int my_release (struct inode *inode, struct  file *filp) {
  printk("releasing\n");
  return 0;
}

static ssize_t my_read (struct  file *filp, char __user *buff, size_t count, loff_t *offp) {
  printk("reading");
  return 0;
}

static ssize_t my_write (struct  file *filp, char __user *buff, size_t count, loff_t *offp) {
  printk("writing");
  return 0;
}

static struct file_operations my_fops = {
  .owner = THIS_MODULE,
  .read = my_read,
  .write = my_write,
  .open = my_open,
  .release = my_release
};


/**
 * struct cdev should have an owner field that should be 
 * set to THIS_MODULE
 */
struct cdev my_cdev = {
  .owner = THIS_MODULE
};


/*
 * template_init - function to insert this module into kernel space
 *
 * This is the first of two exported functions to handle inserting this
 * code into a running kernel
 *
 * Returns 0 if successfull, otherwise -1
 */

static int __init gamepad_init(void)
{
  // Need to allocate variables here because C90 restrictions
  int alloc_chrdevice_result;
  int dev_major;
  int dev_minor;
  int mapReturn;
  int cdev_result;
  char *name;

  printk("Hello World, here is your module speaking\n");

  // Request memory. Dont know if 1024 byte is correct, just a guess.
  *name = "GPIO";
  if (request_mem_region(GPIO_PA_BASE, 1024, name) == NULL)  {
    printk("An error occured! Could not reserve memory region");
    return 1;
  }

  // This is our io address space, but dont read it directlu, use accessor functions
  mapReturn = ioremap_nocache((resource_size_t) GPIO_PA_BASE, 1024);
  
  // Get device version number
  alloc_chrdevice_result = alloc_chrdev_region(devno, 0, 1, "device_name");
  if (alloc_chrdevice_result < 0) {
    printk(KERN_WARNING "Gamepad driver: Can't get major %d\n", dev_major);
  }
  dev_major = MAJOR(*devno);
  dev_minor = MINOR(*devno);

  // Initialize as char driver
  cdev_init(&my_cdev, &my_fops);
  cdev_result = cdev_add(&my_cdev, *devno, 1);
  if (cdev_result < 0) {
    printk(KERN_WARNING "Gamepad driver: Failed to add character device");
  } 

  // Make driver visible to user space
  cl = class_create(THIS_MODULE, "Gamepad");
  device_create(cl, NULL, *devno , NULL, "Gamepad");

  return 0;
}


static void __exit gamepad_cleanup(void)
{
	 printk("Gamepad module running cleanup...\n");
   unregister_chrdev_region(*devno, 1);
   device_destroy(cl, *devno);
   class_destroy(cl);
	 printk("Cleanup complete\n");
}

module_init(gamepad_init);
module_exit(gamepad_cleanup);

MODULE_DESCRIPTION("Device driver for gamepad");
MODULE_LICENSE("GPL");

