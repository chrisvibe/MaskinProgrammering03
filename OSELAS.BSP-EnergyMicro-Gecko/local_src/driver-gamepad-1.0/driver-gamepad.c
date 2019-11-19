#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/ioport.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <asm/io.h>


#include "efm32gg.h"

static dev_t *devno;
static struct class *cl;

static int gpioMapReturn;
static int cmuMapReturn;

static int my_open (struct inode *inode, struct  file *filp) {
  printk("opening\n");
  return 0;
}

static int my_release (struct inode *inode, struct  file *filp) {
  printk("releasing\n");
  return 0;
}

static ssize_t my_read (struct  file *filp, char __user *buff, size_t count, loff_t *offp) {
  printk("reading\n");
  unsigned int res;
  res = ioread32(gpioMapReturn + 72 + 28);
  printk("Got result: %d\n", res);
  return 0;
}

static ssize_t my_write (struct  file *filp, char __user *buff, size_t count, loff_t *offp) {
  printk("writing\n");
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
 * gamepad_init - function to insert this module into kernel space
 *
 * This is the first of two exported functions to handle inserting this
 * code into a running kernel
 *
 * Returns 0 if successfull, otherwise -1
 */

static int __init gamepad_init(void)
{
  printk("Hello from gamepad driver init function. Initializing...\n")

  // Need to allocate variables here because C90 restrictions
  int alloc_chrdevice_result;
  int dev_major;
  int dev_minor;
  int cdev_result;
  char *gpioAlloc;
  char *cmuAlloc;

  unsigned int CMU_HFPER;
  unsigned int result;


  printk("Allocating memory region for GPIO\n");
  // Request memory. Dont know if 1024 byte is correct, just a guess.
  gpioAlloc = "GPIO";
  if (request_mem_region(GPIO_PA_BASE, 1024, gpioAlloc) == NULL)  {
    printk("An error occured! Could not reserve memory region for GPIO\n");
    return 1;
  }
  printk("Allocating memory region for CMU\n");
  cmuAlloc = "CMU";
  if (request_mem_region(CMU_BASE2, 1024, cmuAlloc) == NULL)  {
    printk("An error occured! Could not reserve memory region for CMU\n");
    return 1;
  }

  // This is our io address space, but dont read it directlu, use accessor functions
  printk("Initializing io memory remap for GPIO\n");
  gpioMapReturn = ioremap_nocache((resource_size_t) GPIO_PA_BASE, 1024);
  printk("Initializing io memory remap for CMU\n");
  cmuMapReturn = ioremap_nocache((resource_size_t) CMU_BASE2, 1024);
  
  printk("Getting device number\n");
  // Get device version number
  alloc_chrdevice_result = alloc_chrdev_region(devno, 0, 1, "device_name");
  if (alloc_chrdevice_result < 0) {
    printk(KERN_WARNING "Gamepad driver: Can't get major %d\n", dev_major);
  }
  dev_major = MAJOR(*devno);
  dev_minor = MINOR(*devno);

  // Initialize as char driver
  printk("Initializing as char driver\n");
  cdev_init(&my_cdev, &my_fops);
  cdev_result = cdev_add(&my_cdev, *devno, 1);
  if (cdev_result < 0) {
    printk(KERN_WARNING "Gamepad driver: Failed to add character device\n");
  } 

  
  // Make driver visible to user space
  printk("Making driver visible to user space\n");
  cl = class_create(THIS_MODULE, "Gamepad");
  device_create(cl, NULL, *devno , NULL, "Gamepad");

  // CMU setup
  // NOTE!!! Here, CMU_HFPERCLKEN0 is a pointer while CMU2_HFPERCLKEN0_GPIO is an int
  printk("Setting up CMU\n");
  CMU_HFPER = ioread32(cmuMapReturn + 68); 
  result = CMU_HFPER | CMU2_HFPERCLKEN0_GPIO;
  printk("Writing result: %d to cmu\n", result);
  iowrite32(result, (cmuMapReturn + 68));

	// Button setup 
  printk("writing to gpio to enable buttons as input\n");
  iowrite32((unsigned int) 0x33333333, (gpioMapReturn + 72 + 4));

  // Enable internal pull-up for buttons by writing 0xff to GPIOPCDOUT
  printk("Setting internal pull up resistors\n");
  iowrite32((unsigned int) 0xff, (gpioMapReturn + 72 + 12));

  printk("Init function complete, driver should now be visible under /dev\n")
  return 0;
}


static void __exit gamepad_cleanup(void)
{
	 printk("Gamepad driver cleanup\n");
   unregister_chrdev_region(*devno, 1);
   device_destroy(cl, *devno);
   class_destroy(cl);
	 printk("Cleanup complete\n");
}

module_init(gamepad_init);
module_exit(gamepad_cleanup);

MODULE_DESCRIPTION("Device driver for gamepad");
MODULE_LICENSE("GPL");

