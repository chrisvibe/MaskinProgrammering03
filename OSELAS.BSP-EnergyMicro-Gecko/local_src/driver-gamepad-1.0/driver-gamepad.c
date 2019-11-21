#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/ioport.h>
#include <linux/fs.h>
#include <linux/signal.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/interrupt.h>
#include <linux/uaccess.h>
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
#define GPIO_EXTIPSELL_OFFSET   (uint32_t)(GPIO_EXTIPSELL) - GPIO_PA_BASE
#define GPIO_EXTIRISE_OFFSET    (uint32_t)(GPIO_EXTIRISE) - GPIO_PA_BASE
#define GPIO_EXTIFALL_OFFSET    (uint32_t)(GPIO_EXTIFALL) - GPIO_PA_BASE
#define GPIO_IEN_OFFSET         (uint32_t)(GPIO_IEN) - GPIO_PA_BASE
#define GPIO_IF_OFFSET          (uint32_t)(GPIO_IF) - GPIO_PA_BASE
#define GPIO_IFC_OFFSET         (uint32_t)(GPIO_IFC) - GPIO_PA_BASE

// Other constants
#define GPIO_EVEN_IRQ_NUM       17
#define GPIO_ODD_IRQ_NUM        18


// Global variables for init and cleanup functions
static dev_t *devno;
static struct class *cl;

// Global variables to access hardware in read function
static int gpioMapReturn;
static int cmuMapReturn;

// For sending SIGIO
struct fasync_struct *pasync_queue;


// Debugging. Set debug variable to 1 to enable.
static int debug = 1;
static void debugStr(char *msg) {
  if (debug) 
    printk("%s\n", msg);
}
static void debugInt(int msg) {
  if (debug) 
    printk("%d\n", msg);
}


static int my_open (struct inode *inode, struct  file *filp) {
  debugStr("opening");
  return 0;
}

static int my_release (struct inode *inode, struct  file *filp) {
  debugStr("releasing");
  return 0;
}

static ssize_t my_read (struct  file *filp, char __user *buff, size_t count, loff_t *offp) {
  unsigned int res;
  res = ioread32(gpioMapReturn + 72 + 28);
  debugStr("Read:");
  debugInt(res);
  debugStr("sizeof Read:");
  debugInt(sizeof(res));
  debugStr("Loff_t:");
  debugInt(*offp);
  debugStr("count:");
  debugInt(count);
  if (*offp == 0)
  {
      if (copy_to_user(buff, &res, 4) != 0)
          return -EFAULT;
      else
      {
          *offp = 4;
          return 1;
      }
  }
  return 0;
}

static ssize_t my_write (struct  file *filp, char __user *buff, size_t count, loff_t *offp) {
  debugStr("writing");
  return 0;
}

static int exer_fasync(int fd, struct file *pfile, int mode)
{
     struct fasync_struct **fapp = &pasync_queue;
     return fasync_helper(fd, pfile, mode, fapp);
}

static struct file_operations my_fops = {
  .owner = THIS_MODULE,
  .read = my_read,
  .write = my_write,
  .open = my_open,
  .release = my_release,
  .fasync = exer_fasync
};


/**
 * Struct cdev should have an owner field that should be 
 * set to THIS_MODULE
 */
struct cdev my_cdev = {
  .owner = THIS_MODULE
};


/**
 * GPIO interrupt handler
 */
irqreturn_t GPIO_interrupt(int irq, void *dev_id, struct pt_regs *regs) {
  unsigned int GPIO_IF_res;

  debugStr("Interrupt fired");
  debugStr("Setting interrupt as handled, reading from gpio_if");
  GPIO_IF_res = ioread32(gpioMapReturn + GPIO_IF_OFFSET);
  debugStr("Writing gpio if to gpio ifc");
  iowrite32(
      GPIO_IF_res,
      (gpioMapReturn + GPIO_IFC_OFFSET));


  // Send SIGIO
  debugStr("Firing SIGIO signal");
  struct fasync_struct **fapp = &pasync_queue;
  kill_fasync(fapp, SIGIO, POLL_IN);


  debugStr("Interrupt handled, returning IRQ_HANDLED");
  return IRQ_HANDLED;
}


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
  printk("Initializing gamepad driver\n");

  // Need to allocate variables here because C90 restrictions
  int alloc_chrdevice_result;
  int cdev_result;
  char *gpioAlloc;
  char *cmuAlloc;

  unsigned int CMU_HFPER;
  unsigned int result;

  unsigned int gpio1;
  unsigned int gpio2;

  // Request memory region for gpio. This is actually not strictly neede, but 
  // is good practice so that drivers do not access same mem regions
  printk("Allocating memory region for GPIO\n");
  gpioAlloc = "GPIO";
  if (request_mem_region(GPIO_ADDR_START, GPIO_ADDR_SIZE, "GPIO") == NULL)  {
    printk(KERN_WARNING "An error occured! Could not reserve memory region for GPIO\n");
    return 1;
  }
  printk("Allocating memory region for CMU\n");
  cmuAlloc = "CMU";
  if (request_mem_region(CMU_ADDR_START, CMU_ADDR_SIZE, "CMU") == NULL)  {
    printk(KERN_WARNING "An error occured! Could not reserve memory region for CMU\n");
    return 1;
  }

  // Map I/O address space to new address space that we will use for hardware access. 
  // This is actually not strictly needed since I/O is memory mapped on the 
  // EFM32GG, but still good practice.
  printk("Initializing io memory remap for GPIO\n");
  gpioMapReturn = ioremap_nocache((resource_size_t) GPIO_ADDR_START, GPIO_ADDR_SIZE);
  printk("Initializing io memory remap for CMU\n");
  cmuMapReturn = ioremap_nocache((resource_size_t) CMU_ADDR_START, CMU_ADDR_SIZE);
  
  // Get device version number
  printk("Getting device number\n");
  alloc_chrdevice_result = alloc_chrdev_region(devno, 0, 1, "device_name");
  if (alloc_chrdevice_result < 0) {
    printk(KERN_WARNING "Gampead driver: Can't get device numbers\n");
  }

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

  
  printk("Setting up GPIO, using port C offset %d\n", GPIO_PC_OFFSET);

	// Button setup 
  printk("Writing to gpio to enable buttons. Using GPIO mode low offset %d \n", GPIO_MODEL_OFFSET);
  iowrite32(
      (unsigned int) 0x33333333, 
      (gpioMapReturn + GPIO_PC_OFFSET + GPIO_MODEL_OFFSET));

  // Enable internal pull-up for buttons by writing 0xff to GPIOPCDOUT
  printk("Setting internal pull up resistors with GPIO dout offset %d\n", GPIO_DOUT_OFFSET);
  iowrite32(
      (unsigned int) 0xff, 
      (gpioMapReturn + GPIO_PC_OFFSET + GPIO_DOUT_OFFSET));


  // Should be placed before hardware generates interrupts
  printk("Enabling interrupt for even GPIO irq number %d\n", GPIO_EVEN_IRQ_NUM);
  if (request_irq(GPIO_EVEN_IRQ_NUM, 
        GPIO_interrupt,
        0,
        "GPIO IRQ even",
        NULL) < 0) {
    printk(KERN_WARNING "Interrupt handler error for even GPIO!\n");
  }
  printk("Enabling interrupt for GPIO irq number %d\n", GPIO_ODD_IRQ_NUM);
  if (request_irq(GPIO_ODD_IRQ_NUM, 
        GPIO_interrupt,
        0,
        "GPIO IRQ odd",
        NULL) < 0) {
    printk(KERN_WARNING "Interrupt handler error for odd GPIO\n");
  }


  //*GPIO_EXTIPSELL = 0x22222222;
  printk("Setting gpio extipsell\n");
  iowrite32(
      (unsigned int) 0x22222222,
      (gpioMapReturn + GPIO_EXTIPSELL_OFFSET));

	//*GPIO_EXTIRISE = 0xff; 
  printk("Setting gpio extirise\n");
  iowrite32(
      (unsigned int) 0xff,
      (gpioMapReturn + GPIO_EXTIRISE_OFFSET));

	//*GPIO_EXTIFALL = 0xff;
  printk("Setting gpio extifall\n");
  iowrite32(
      (unsigned int) 0xff,
      (gpioMapReturn + GPIO_EXTIFALL_OFFSET));

	// *GPIO_IEN |= 0xff;
  // CMU setup
  printk("Setting gpio IEN\n");
  gpio1 = ioread32(gpioMapReturn + GPIO_IEN_OFFSET); 
  gpio2 = gpio1 | 0xff;
  iowrite32(
      (unsigned int) gpio2,
      (gpioMapReturn + GPIO_IEN_OFFSET));




  printk("Init function complete, driver should now be visible under /dev\n");
  return 0;
}


static void __exit gamepad_cleanup(void)
{
	 printk("Gamepad driver cleanup\n");
   free_irq(17, NULL);
   free_irq(18, NULL);
   unregister_chrdev_region(*devno, 1);
   device_destroy(cl, *devno);
   class_destroy(cl);
	 printk("Cleanup complete\n");
}

module_init(gamepad_init);
module_exit(gamepad_cleanup);

MODULE_DESCRIPTION("Device driver for gamepad");
MODULE_LICENSE("GPL");

