#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <asm/uaccess.h>	/* for put_user */
#include <charDeviceDriver.h>
#include "ioctl.h"

MODULE_LICENSE("GPL");

DEFINE_MUTEX(devLock);

/*
 * This function is called whenever a process tries to do an ioctl on our
 * device file. We get two extra parameters (additional to the inode and file
 * structures, which all device functions get): the number of the ioctl called
 * and the parameter given to the ioctl function.
 *
 * If the ioctl is write or read/write (meaning output is returned to the
 * calling process), the ioctl call returns the output of this function.
 *
 */
static long device_ioctl(struct file *file,	/* see include/linux/fs.h */
		 unsigned int ioctl_num,	        /* number and param for ioctl */
		 unsigned long ioctl_param)
{
	if (ioctl_num == IOCTL_RESIZE) {
		printk(KERN_DEBUG "Got ioctl parameter %lu\n", ioctl_param);

		printk(KERN_DEBUG "Is %lu > %zu ?\n", ioctl_param, Message_Queue->tsize);
		if (ioctl_param > Message_Queue->tsize) {
			max_total_size = (size_t) ioctl_param;
			printk(KERN_DEBUG "ioctl success, new max_total_size: %zu\n", max_total_size);
			return 0;
		}
	}

	printk(KERN_DEBUG "ioctl fail\n");
	return -EINVAL;
}

/*
 * This function is called when the module is loaded
 */
int init_module(void)
{
	Major = register_chrdev(0, DEVICE_NAME, &fops);

	if (Major < 0) {
	  printk(KERN_ALERT "Registering char device failed with %d\n", Major);
	  return Major;
	}

	printk(KERN_INFO "I was assigned major number %d. To talk to\n", Major);
	printk(KERN_INFO "the driver, create a dev file with\n");
	printk(KERN_INFO "'mknod /dev/%s c %d 0'.\n", DEVICE_NAME, Major);
	printk(KERN_INFO "Try various minor numbers. Try to cat and echo to\n");
	printk(KERN_INFO "the device file.\n");
	printk(KERN_INFO "Remove the device file and module when done.\n");

	Message_Queue = (mqueue *) kmalloc(sizeof(mqueue), GFP_KERNEL);
	if (!Message_Queue) return -1;
	mqueue_init(Message_Queue);

	return SUCCESS;
}

/*
 * This function is called when the module is unloaded
 */
void cleanup_module(void)
{
	// Unregister the device
	unregister_chrdev(Major, DEVICE_NAME);

	mqueue_destroy(Message_Queue);
	kfree(Message_Queue);
	Message_Queue = NULL;
}

/*
 * Methods
 */

/*
 * Called when a process tries to open the device file, like
 * "cat /dev/mycharfile"
 */
static int device_open(struct inode *inode, struct file *file)
{
    mutex_lock(&devLock);
	printk(KERN_DEBUG "device_open\n");
    if (Device_Open) {
		mutex_unlock(&devLock);
		return -EBUSY;
    }
    Device_Open++;
    mutex_unlock(&devLock);

    try_module_get(THIS_MODULE);

    return SUCCESS;
}

/* Called when a process closes the device file. */
static int device_release(struct inode *inode, struct file *file)
{
    mutex_lock(&devLock);
	printk(KERN_DEBUG "device_release\n");
	Device_Open--;		/* We're now ready for our next caller */
	mutex_unlock(&devLock);
	/*
	 * Decrement the usage count, or else once you opened the file, you'll
	 * never get get rid of the module.
	 */
	module_put(THIS_MODULE);

	return 0;
}

/*
 * Called when a process, which already opened the dev file, attempts to
 * read from it.
 */
static ssize_t device_read(struct file *filp,	/* see include/linux/fs.h   */
			   char *buffer,	/* buffer to fill with data */
			   size_t length,	/* length of the buffer     */
			   loff_t * offset)
{
	int bytes_read = 0; // Number of bytes actually written to the buffer
	int result;         // Result of function calls
	mqueue_node *n;
	char *message_open;
	char *mp;
	size_t msize;

	printk(KERN_DEBUG "device_read\n");

	n = mqueue_pop(Message_Queue);
	if (n == NULL) {
		printk(KERN_DEBUG "!n\n");
		return -EAGAIN;
	}
	message_open = n->message;
	msize = n->msize;
	mp = message_open;
	printk(KERN_DEBUG "popped (%zu) '%.*s'\n", msize, msize, message_open);

	printk(KERN_DEBUG "pop done\n");

	while (length && bytes_read < msize) {
		result = put_user(*(mp++), buffer++);
		if (result != 0) {
			return -EFAULT;
		}
		length--;
		bytes_read++;
	}

	printk(KERN_DEBUG "popped (%d) '%.*s'\n", bytes_read, bytes_read, message_open);

	kfree(message_open);
	message_open = NULL;
	mp = NULL;
	kfree(n);
	n = NULL;

	printk(KERN_DEBUG "tsize after read: %zu\n", Message_Queue->tsize);

	/*
	 * Most read functions return the number of bytes put into the buffer
	 */
	return bytes_read;
}

/* Called when a process writes to dev file: echo "hi" > /dev/hello  */
static ssize_t
device_write(struct file *filp, const char *buff, size_t len, loff_t * off)
{
	char *message;
	unsigned int i;
	int result;

	printk(KERN_DEBUG "device_write\n");
	printk(KERN_DEBUG "len: %zu\n", len);
	printk(KERN_DEBUG "buff: '%.*s'\n", len, buff);

	if (len > MAX_MESSAGE_SIZE) {
		// Message too large, return invalid argument error
		printk(KERN_INFO "Provided message exceeded max message size\n");
		return -EINVAL;
	}

	message = (char *) kmalloc(len, GFP_KERNEL);
	if (!message) {
		printk(KERN_ALERT "Failed to allocate kernel memory for message");
		return -1;
	}

	for (i = 0; i < len; i++) {
		// printk(KERN_DEBUG "Char (%d): '%c'\n", i, buff[i]);
		message[i] = buff[i];
	}
	printk(KERN_INFO "Got (%zu) '%.*s'\n", len, len, message);

	result = mqueue_push(Message_Queue, message, len);
	if (result != 0) {
		kfree(message);
		return result;
	}

	return len;
}

/* Message Queue Functions */

// Initialise new queue
void mqueue_init(mqueue *q)
{
	printk(KERN_DEBUG "mqueue_init\n");
	q->front = NULL;
	q->back = NULL;
	q->tsize = 0;
}

// Add new message to back of queue
int mqueue_push(mqueue *q, char *m, size_t msize)
{
	mqueue_node *n;
	size_t tsize_new;

	printk(KERN_DEBUG "mqueue_push\n");
	printk(KERN_DEBUG "m: '%.*s'", msize, m);

	tsize_new = q->tsize + msize;
	printk(KERN_DEBUG "tsize_new = %zu\n", tsize_new);
	if (tsize_new > max_total_size) {
		printk(KERN_ALERT "Cannot add message, total size of all messages would be exceeded\n");
		return -EAGAIN;
	}

	n = (mqueue_node *) kmalloc(sizeof(mqueue_node), GFP_KERNEL);
	if (!n) {
		printk(KERN_ALERT "Failed to allocate space for new queue node\n");
		return -1;
	}

	n->next = NULL;
	n->msize = msize;
	n->message = m;

	if (!(q->front) && !(q->back)) {
		q->front = n;
	} else {
		q->back->next = n;
	}
	q->back = n;
	q->tsize = tsize_new;

	return 0;
}

// Remove node from front of queue and return it (we need both message and msize for read)
// NB: Both the message and the node pointers should be freed by the caller of mqueue_pop (device_read)
mqueue_node *mqueue_pop(mqueue *q)
{
	mqueue_node *n;

	printk(KERN_DEBUG "mqueue_pop\n");

	// Queue is empty
	if (!(q->front) && !(q->back)) {
		printk(KERN_DEBUG "mqueue_pop: queue empty\n");
		return NULL;
	}

	n = q->front;

	q->front = n->next;
	q->tsize -= n->msize;
	// If the queue is now empty
	if (n->next == NULL) {
		q->back = NULL;
		if (q->tsize != 0) {
			printk(KERN_ALERT "Queue was emptied but tsize != 0\n");
		}
	}

	// Caller of this function must free n and n->message later after use
	return n;
}

// Destroy queue and deallocate all memory
void mqueue_destroy(mqueue *q)
{
	mqueue_node *n = q->front;
	mqueue_node *m;

	printk(KERN_DEBUG "mqueue_destroy\n");

	// Traverse the queue and free each node
	while (n) {
		m = n;
		n = n->next;
		printk(KERN_DEBUG "Free queue node with (%zu) '%.*s'\n", m->msize, m->msize, m->message);
		kfree(m->message);
		kfree(m);
	}
}
