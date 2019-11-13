/* Global definition for the example character device driver */

int init_module(void);
void cleanup_module(void);
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);
static long device_ioctl(struct file *file, unsigned int ioctl_num, unsigned long);

#define SUCCESS 0
#define DEVICE_NAME "opsysmem"	/* Dev name as it appears in /proc/devices   */
#define MAX_MESSAGE_SIZE 4096 // Max length of each message in bytes

// Message Queue Data Structures
typedef struct mqueue_node {
    struct mqueue_node *next;   // Pointer to the next item in the queue
    size_t msize;               // Size of the message in bytes
    char *message;              // Message string
} mqueue_node;

typedef struct mqueue {
    struct mqueue_node *front;  // Pointer to front of the queue
    struct mqueue_node *back;   // Pointer to the back of the queue
    size_t tsize;               // Total size in bytes of all messages in the queue
} mqueue;

// Message Queue Functions
void mqueue_init(mqueue *);                // Initialise new queue
int mqueue_push(mqueue *, char *, size_t); // Add new message to back of queue
mqueue_node *mqueue_pop(mqueue *);                // Remove message from front of queue
void mqueue_destroy(mqueue *);             // Destroy queue and deallocate all memory

/*
 * Global variables are declared as static, so are global within the file.
 */
struct cdev *my_cdev;
dev_t dev_num;

static int Major;		    /* Major number assigned to our device driver */
static int Device_Open = 0;	/* Is device open. Used to prevent multiple access to device */
static mqueue *Message_Queue;
static size_t max_total_size = 2097152; // Default to 2 MiB

static struct file_operations fops = {
    .open = device_open,
    .read = device_read,
    .write = device_write,
    .unlocked_ioctl = device_ioctl,
    .release = device_release
};
