#if LINUX_VERSION_CODE <= KERNEL_VERSION(4, 3, 0)
#error "Kernel version < 4.4 not supported!"
#endif

// Make IPv4 addresses readable
#define NIPQUAD(addr)             \
    ((unsigned char *) &addr)[0], \
    ((unsigned char *) &addr)[1], \
    ((unsigned char *) &addr)[2], \
    ((unsigned char *) &addr)[3]

#define PROC_ENTRY_FILENAME "firewallExtension"
#define EXE_BUFSIZ 80
#define NEW_RULES_FLAG "NEW"
#define END_RULES_FLAG "END"
#define KILL_RULES_FLAG "KIL"
#define RULES_FLAG_LEN 3

typedef struct fw_prog_list {
    struct fw_prog_list *next;
    char *program;
} fw_prog_list;

typedef struct fw_port_list {
    struct fw_port_list *next;
    uint16_t port;
    struct fw_prog_list *prog_list;
} fw_port_list;

struct program {
    char *buffer;
    char *name;
};

int init_module(void);
void cleanup_module(void);
fw_port_list *init_portlist(void);
void cleanup_portlist(fw_port_list *);
unsigned int FirewallExtensionHook(void *, struct sk_buff *, const struct nf_hook_state *);
struct program *find_executable(void);
int procfs_open(struct inode *, struct file *);
int procfs_close(struct inode *, struct file *);
ssize_t procfs_read(struct file *, char *, size_t, loff_t *);
ssize_t procfs_write(struct file *, const char *, size_t, loff_t *);

static struct nf_hook_ops firewallExtension_ops = {
    .hook = FirewallExtensionHook,
    .pf = PF_INET,
    .priority = NF_IP_PRI_FIRST,
    .hooknum = NF_INET_LOCAL_OUT
};

DEFINE_MUTEX(proc_lock);
static int Device_Open = 0;
static fw_port_list *Port_List = NULL;
static fw_port_list *Port_List_Tmp = NULL;
static struct proc_dir_entry *Proc_File = NULL;
static struct file_operations Proc_File_Fops = {
    .owner   = THIS_MODULE,
    .open 	 = procfs_open,
    .release = procfs_close,
    .read    = procfs_read,
    .write 	 = procfs_write,
};
