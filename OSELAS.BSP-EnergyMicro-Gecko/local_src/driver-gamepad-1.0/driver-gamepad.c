#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/ioport.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <asm/io.h>

#include "efm32gg.h"

/* 
 * Need these constants because we need to offset so we can use the memomry remap.
 * The reason for casting only some of the constants to uint32 is because
 * only some of them are pointers, the rest are ints.
 */
#define GPIO_ADDR_START         GPIO_PA_BASE
#define GPIO_ADDR_SIZE          (uint32_t)(GPIO_IFC) + 4 - GPIO_PA_BASE
#define GPIO_PC_OFFSET          GPIO_PC_BASE - GPIO_PA_BASE
#define GPIO_MODEL_OFFSET       (uint32_t)(GPIO_PA_MODEL) - GPIO_PA_BASE
#define GPIO_DOUT_OFFSET        (uint32_t)(GPIO_PA_DOUT) - GPIO_PA_BASE
#define CMU_ADDR_START          CMU_BASE2
#define CMU_ADDR_SIZE           (uint32_t)(CMU_HFPERCLKEN0) + 4 - CMU_BASE2 
#define CMU_HFPERCLKEN0_OFFSET  (uint32_t)(CMU_HFPERCLKEN0) - CMU_BASE2

// Global variables for init and cleanup functions
static dev_t *devno;
static struct class *cl;

// Global variables to access hardware in read function
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
  unsigned int res;
  res = ioread32(gpioMapReturn + 72 + 28);
  printk("%d\n", res);
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
  printk("Hello from gamepad driver init function. Initializing...\n");
  printk("This is the new one");

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
  if (request_mem_region(GPIO_ADDR_START, GPIO_ADDR_SIZE, "GPIO") == NULL)  {
    printk("An error occured! Could not reserve memory region for GPIO\n");
    return 1;
  }
  printk("Allocating memory region for CMU\n");
  cmuAlloc = "CMU";
  if (request_mem_region(CMU_ADDR_START, CMU_ADDR_SIZE, "CMU") == NULL)  {
    printk("An error occured! Could not reserve memory region for CMU\n");
    return 1;
  }

  // This is our io address space, but dont read it directlu, use accessor functions
  printk("Initializing io memory remap for GPIO\n");
  gpioMapReturn = ioremap_nocache((resource_size_t) GPIO_ADDR_START, GPIO_ADDR_SIZE);
  printk("Initializing io memory remap for CMU\n");
  cmuMapReturn = ioremap_nocache((resource_size_t) CMU_ADDR_START, CMU_ADDR_SIZE);
  
  // Get device version number
  printk("Getting device number\n");
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
  printk("Setting up CMU\n");
  CMU_HFPER = ioread32(cmuMapReturn + CMU_HFPERCLKEN0_OFFSET); 
  result = CMU_HFPER | CMU2_HFPERCLKEN0_GPIO;
  printk("Writing result: %d to cmu\n", result);
  iowrite32(result, (cmuMapReturn + CMU_HFPERCLKEN0_OFFSET));

	// Button setup 
  printk("writing to gpio to enable buttons as input with offset %d \n", GPIO_MODEL_OFFSET);
  iowrite32(
      (unsigned int) 0x33333333, 
      (gpioMapReturn + GPIO_PC_OFFSET + GPIO_MODEL_OFFSET));

  // Enable internal pull-up for buttons by writing 0xff to GPIOPCDOUT
  printk("Setting internal pull up resistors with pc offset %d , dout %d \n", GPIO_PC_OFFSET, GPIO_DOUT_OFFSET);
  iowrite32(
      (unsigned int) 0xff, 
      (gpioMapReturn + GPIO_PC_OFFSET + GPIO_DOUT_OFFSET));

  printk("Init function complete, driver should now be visible under /dev\n");
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

