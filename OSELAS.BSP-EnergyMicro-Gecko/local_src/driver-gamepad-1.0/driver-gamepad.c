#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/ioport.h>
#include <linux/fs.h>
#include <linux/signal.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/miscdevice.h>
#include <linux/interrupt.h>
#include <linux/uaccess.h>
#include <asm/io.h>

#include "efm32gg.h"

/* 
 * Need these constants because we need to offset so we can use the memomry remap.
 * The reason for casting only some of the constants to uint32 is because
 * only some of them are pointers, the rest are ints.
 */
#define GPIO_ADDR_START GPIO_PA_BASE
#define GPIO_ADDR_SIZE (uint32_t)(GPIO_IFC) + 4 - GPIO_PA_BASE
#define GPIO_PC_OFFSET GPIO_PC_BASE - GPIO_PA_BASE
#define GPIO_MODEL_OFFSET (uint32_t)(GPIO_PA_MODEL) - GPIO_PA_BASE
#define GPIO_DOUT_OFFSET (uint32_t)(GPIO_PA_DOUT) - GPIO_PA_BASE
#define GPIO_DIN_OFFSET (uint32_t)(GPIO_PA_DIN) - GPIO_PA_BASE
#define GPIO_EXTIPSELL_OFFSET (uint32_t)(GPIO_EXTIPSELL) - GPIO_PA_BASE
#define GPIO_EXTIRISE_OFFSET (uint32_t)(GPIO_EXTIRISE) - GPIO_PA_BASE
#define GPIO_EXTIFALL_OFFSET (uint32_t)(GPIO_EXTIFALL) - GPIO_PA_BASE
#define GPIO_IEN_OFFSET (uint32_t)(GPIO_IEN) - GPIO_PA_BASE
#define GPIO_IF_OFFSET (uint32_t)(GPIO_IF) - GPIO_PA_BASE
#define GPIO_IFC_OFFSET (uint32_t)(GPIO_IFC) - GPIO_PA_BASE

// Function prototypes
static int my_open(struct inode *inode, struct file *filp);
static int my_release(struct inode *inode, struct file *filp);
static ssize_t my_read(struct file *filp, char __user *buff, size_t count,
		       loff_t *offp);
static ssize_t my_write(struct file *filp, const char __user *buff,
			size_t count, loff_t *offp);
static irqreturn_t GPIO_interrupt(int irq, void *dev_id);
static int my_probe(struct platform_device *dev);
static int my_remove(struct platform_device *dev);
static int __init gamepad_init(void);
static void __exit gamepad_cleanup(void);

// Global variables. Many of these are needed as globals only because they need to be
// freed in the cleanup function.
static struct miscdevice miscdev;
static struct fasync_struct *pasync_queue;
static uint32_t gpio_map_return;
static int gpio_irq_even;
static int gpio_irq_odd;

// Debugging, Set debug variable to 1 to enable.
static int debug = 1;
static void debug_str(char *msg)
{
	if (debug)
		printk("%s\n", msg);
}
static void debug_int(int msg)
{
	if (debug)
		printk("%d\n", msg);
}

/**
 * Open function for file operations. Will be called when user program is
 * starting to read device file
 */
static int my_open(struct inode *inode, struct file *filp)
{
	debug_str("Opening");
	return 0;
}

/**
 * Release function for file operations. Will be called when user program is done
 * reading device file
 */
static int my_release(struct inode *inode, struct file *filp)
{
	debug_str("Releasing");
	return 0;
}

/**
 * Read function for file operations. Will be called when the device file is read from.
 * It reads the button bits and then sends it to user space program that is reading 
 * the file
 */
static ssize_t my_read(struct file *filp, char __user *buff, size_t count,
		       loff_t *offp)
{
	uint8_t res;
	res = ioread8((uint32_t *)(gpio_map_return + GPIO_PC_OFFSET +
				   GPIO_DIN_OFFSET));
	debug_str("Driver button result:");
	debug_int(res);
	if (copy_to_user(buff, &res, 1) != 0) {
		printk(KERN_WARNING "Error when copying data to user space\n");
		return -EFAULT;
	}
	(*offp)++;
	return 1;
}

/**
 * Write function for file operations. Will be called when the device file is written to. 
 * Not used since it makes no sense to write to buttons.
 */
static ssize_t my_write(struct file *filp, const char __user *buff,
			size_t count, loff_t *offp)
{
	debug_str("Writing");
	return 0;
}

static int exer_fasync(int fd, struct file *pfile, int mode)
{
	return fasync_helper(fd, pfile, mode, &pasync_queue);
}

static struct file_operations my_fops = { .owner = THIS_MODULE,
					  .read = my_read,
					  .write = my_write,
					  .open = my_open,
					  .release = my_release,
					  .fasync = exer_fasync };

/**
 * GPIO interrupt handler. This function will fire a SIGIO signal 
 * to let the user space program now that a button has been pressed
 * or released.
 */
static irqreturn_t GPIO_interrupt(int irq, void *dev_id)
{
	unsigned int GPIO_IF_res;

	printk("IN interrupt handler\n");
	debug_str("Interrupt fired");
	debug_str("Setting interrupt as handled, reading from gpio_if");
	GPIO_IF_res = ioread32((uint32_t *)(gpio_map_return + GPIO_IF_OFFSET));
	debug_str("Writing gpio if to gpio ifc");
	iowrite32(GPIO_IF_res, (uint32_t *)(gpio_map_return + GPIO_IFC_OFFSET));

	// Send SIGIO
	debug_str("Firing SIGIO signal");
	if (pasync_queue) {
		kill_fasync(&pasync_queue, SIGIO, POLL_IN);
	}

	debug_str("Interrupt handled, returning IRQ_HANDLED");
	return IRQ_HANDLED;
}

/**
 * This function sets up the driver with hardware access, interrupts, file
 * operations etc.. Because this is a platform driver, this function will 
 * only called after the driver has made sure that a correct platform 
 * device exist
 */
static int my_probe(struct platform_device *dev)
{
	// Need to allocate variables here because C90 restrictions
	unsigned int gpio_ien_tmp;
	uint32_t start_addr;
	uint32_t end_addr;
	struct resource *res;

	printk("platform driver my_probe running\n");
	res = platform_get_resource(dev, IORESOURCE_MEM, 0);
	gpio_irq_even = platform_get_irq(dev, 0);
	gpio_irq_odd = platform_get_irq(dev, 1);

	start_addr = res->start;
	end_addr = res->end - res->start;

	printk("GPIO start addr: %d\n", GPIO_ADDR_START);
	printk("GPIO addr size: %d\n", GPIO_ADDR_SIZE);
	printk("platform dev start addr: %d\n", res->start);
	printk("platform dev end addr: %d\n", res->end);
	printk("platform dev addr size: %d\n", res->end - res->start);

	// Request memory region for gpio. This is actually not strictly neede, but
	// is good practice so that drivers do not access same mem regions
	printk("Allocating memory region for GPIO\n");
	if (request_mem_region(start_addr, end_addr, "GPIO") == NULL) {
		printk(KERN_WARNING
		       "An error occured! Could not reserve memory region for GPIO\n");
		return 1;
	}

	// Map I/O address space to new address space that we will use for hardware access.
	// This is actually not strictly needed since I/O is memory mapped on the
	// EFM32GG, but still good practice.
	printk("Initializing io memory remap for GPIO\n");
	gpio_map_return = (uint32_t)ioremap_nocache((resource_size_t)start_addr,
						    end_addr);

	// Make driver visible to user space and register as char device
	printk("Making driver visible to user space\n");
	miscdev.minor = MISC_DYNAMIC_MINOR;
	miscdev.name = "Gamepad";
	miscdev.fops = &my_fops;
	misc_register(&miscdev);

	// Button setup
	printk("Writing to gpio to enable buttons. Using GPIO mode low offset %d \n",
	       GPIO_MODEL_OFFSET);
	iowrite32((unsigned int)0x33333333,
		  (uint32_t *)(gpio_map_return + GPIO_PC_OFFSET +
			       GPIO_MODEL_OFFSET));

	// Enable internal pull-up for buttons by writing 0xff to GPIOPCDOUT
	printk("Setting internal pull up resistors with GPIO dout offset %d\n",
	       GPIO_DOUT_OFFSET);
	iowrite32((unsigned int)0xff,
		  (uint32_t *)(gpio_map_return + GPIO_PC_OFFSET +
			       GPIO_DOUT_OFFSET));

	// Should be placed before hardware generates interrupts
	printk("Enabling interrupt for even GPIO irq number %d\n",
	       gpio_irq_even);
	if (request_irq(gpio_irq_even, GPIO_interrupt, 0, "GPIO IRQ even",
			NULL) < 0) {
		printk(KERN_WARNING "Interrupt handler error for even GPIO!\n");
	}
	printk("Enabling interrupt for GPIO irq number %d\n", gpio_irq_odd);
	if (request_irq(gpio_irq_odd, GPIO_interrupt, 0, "GPIO IRQ odd", NULL) <
	    0) {
		printk(KERN_WARNING "Interrupt handler error for odd GPIO\n");
	}

	// GPIO interrupt setup
	// DONT LOG BEYOND THIS PART IN THIS FUNCTION!!!
	// Logging will cause an interrupt to happen in the middle of interrupt setup,
	// which will cause modprobe to crash
	iowrite32((unsigned int)0x22222222,
		  (uint32_t *)(gpio_map_return + GPIO_EXTIPSELL_OFFSET));

	iowrite32((unsigned int)0xff,
		  (uint32_t *)(gpio_map_return + GPIO_EXTIRISE_OFFSET));

	iowrite32((unsigned int)0xff,
		  (uint32_t *)(gpio_map_return + GPIO_EXTIFALL_OFFSET));

	gpio_ien_tmp =
		ioread32((uint32_t *)(gpio_map_return + GPIO_IEN_OFFSET)) |
		0xff;

	iowrite32((unsigned int)gpio_ien_tmp,
		  (uint32_t *)(gpio_map_return + GPIO_IEN_OFFSET));

	return 0;
}

static const struct of_device_id my_of_match[] = {
	{
		.compatible = "tdt4258",
	},
	{},
};

MODULE_DEVICE_TABLE(of, my_of_match);

static struct platform_driver my_driver = {
  .probe = my_probe,
  .remove = my_remove,
  .driver = {
    .name = "my",
    .owner = THIS_MODULE,
    .of_match_table = my_of_match,
  },
};

/**
 * This functions cleans up and frees memory for the platform driver
 */
static int my_remove(struct platform_device *dev)
{
	printk("platform driver cleanup\n");
	platform_driver_unregister(&my_driver);
	misc_deregister(&miscdev);
	free_irq(gpio_irq_even, NULL);
	free_irq(gpio_irq_odd, NULL);
	printk("platform driver cleanup complete\n");
	return 0;
}

/*
 * This function is the first function to be called when the driver is loaded.
 * Since this driver is a platform driver, all this function does is checking 
 * that a correct platform device exists, registers the driver as a platform 
 * driver, then delegates most of the setup to the platform driver specific 
 * setup function.
 */
static int __init gamepad_init(void)
{
	int driver_reg_res;

	printk("Initializing gamepad driver\n");
	driver_reg_res = platform_driver_probe(&my_driver, my_probe);
	if (driver_reg_res != 0) {
		printk(KERN_WARNING
		       "Failed to register platform device, err code: %d\n",
		       driver_reg_res);
	}
	return driver_reg_res;
}

/**
 * The last function to be called when driver exists. In this driver, all cleanup
 * is found in the platform specific remove function.
 */
static void __exit gamepad_cleanup(void)
{
	printk("Gamepad driver __exit cleanup function\n");
}

module_init(gamepad_init);
module_exit(gamepad_cleanup);

MODULE_DESCRIPTION("Device driver for gamepad");
MODULE_LICENSE("GPL");
