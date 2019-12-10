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

int init_module(void);
void cleanup_module(void);
unsigned int FirewallExtensionHook(void *, struct sk_buff *, const struct nf_hook_state *);
char *find_executable(void);
// char *find_executable(char *);
int procfs_open(struct inode *, struct file *);
int procfs_close(struct inode *, struct file *);
ssize_t procfs_write(struct file *file, const char __user *buffer, size_t count, loff_t *offset);

typedef struct fw_prog_list {
    struct fw_prog_list *next;
    char *program;
} fw_prog_list;

typedef struct fw_port_list {
    struct fw_port_list *next;
    uint16_t port;
    struct fw_prog_list *prog_list;
} fw_port_list;

static struct nf_hook_ops firewallExtension_ops = {
    .hook = FirewallExtensionHook,
    .pf = PF_INET,
    .priority = NF_IP_PRI_FIRST,
    .hooknum = NF_INET_LOCAL_OUT
};

static fw_port_list *Port_List = NULL;
static struct proc_dir_entry *Proc_File = NULL;
static struct file_operations Proc_File_Fops = {
    .owner   = THIS_MODULE,
    .open 	 = procfs_open,
    .release = procfs_close,
    .write 	 = procfs_write,
};
