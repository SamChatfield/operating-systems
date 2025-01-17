#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/skbuff.h>
#include <linux/compiler.h>
#include <net/tcp.h>
#include <linux/namei.h>
#include <linux/version.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/dcache.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>
#include "firewallExtension.h"

MODULE_LICENSE("GPL");

int init_module(void)
{
    int errno;

    // Create /proc file
    Proc_File = proc_create_data(PROC_ENTRY_FILENAME, 0644, NULL, &Proc_File_Fops, NULL);
    if (Proc_File == NULL) {
        printk(KERN_ALERT "Error: Could not initialize /proc/%s\n", PROC_ENTRY_FILENAME);
        return -ENOMEM;
    }
    printk(KERN_INFO "/proc/%s created\n", PROC_ENTRY_FILENAME);

    // Register hook
    errno = nf_register_hook(&firewallExtension_ops);
    if (errno) {
        printk(KERN_INFO "Firewall extension could not be registered!\n");
        return errno;
    }
    printk(KERN_INFO "Firewall extension module loaded\n");

    // Initialise the Port_List
    down_write(&rules_sem);
    Port_List = init_portlist();
    if (!Port_List) {
        up_write(&rules_sem);
        return -ENOMEM;
    }
    up_write(&rules_sem);

    // A non 0 return means init_module failed; module can't be loaded.
    return errno;
}

void cleanup_module(void)
{
    // Restore everything to normal
    nf_unregister_hook(&firewallExtension_ops);

    // Free all memory associated with the Port_List
    down_write(&rules_sem);
    cleanup_portlist(Port_List);
    if (Port_List_Tmp)
        cleanup_portlist(Port_List_Tmp);
    up_write(&rules_sem);

    remove_proc_entry(PROC_ENTRY_FILENAME, NULL);
    printk(KERN_INFO "/proc/%s removed\n", PROC_ENTRY_FILENAME);

    printk(KERN_INFO "Firewall extensions module unloaded\n");
}

/* Initialise the Port_List
 * Values of next and prog_list pointers initialised to NULL
 * Port number initialised to 0 to signify an empty fw_port_list node
 */
fw_port_list *init_portlist(void)
{
    fw_port_list *port_list;

    printk(KERN_DEBUG "Initialising port list\n");

    port_list = (fw_port_list *) kmalloc(sizeof(fw_port_list), GFP_KERNEL);
    if (!port_list)
        return NULL;
    port_list->next = NULL;
    port_list->port = 0;
    port_list->prog_list = NULL;

    return port_list;
}

/* Free all memory assoicated with Port_List and NULL the pointer */
void cleanup_portlist(fw_port_list *port_list_head)
{
    fw_port_list *port_list, *tmp_port_list;
    fw_prog_list *prog_list, *tmp_prog_list;

    printk(KERN_DEBUG "Cleaning up port list\n");

    port_list = port_list_head;

    while (port_list) {
        printk(KERN_DEBUG "Freeing port node: '%u'\n", (unsigned int) port_list->port);
        prog_list = port_list->prog_list;

        while (prog_list) {
            printk(KERN_DEBUG "Freeing prog node: '%s'\n", prog_list->program);
            tmp_prog_list = prog_list;
            prog_list = prog_list->next;
            kfree(tmp_prog_list->program);
            kfree(tmp_prog_list);
        }

        tmp_port_list = port_list;
        port_list = port_list->next;
        kfree(tmp_port_list);
    }
}

unsigned int FirewallExtensionHook(void *priv, struct sk_buff *skb, const struct nf_hook_state *state)
{
    struct tcphdr *tcp;
    struct tcphdr _tcph;
    struct sock *sk;
    struct mm_struct *mm;

    sk = skb->sk;
    if (!sk) {
        printk(KERN_INFO "firewall: netfilter called with empty socket!\n");
        return NF_ACCEPT;
    }

    if (sk->sk_protocol != IPPROTO_TCP) {
        printk(KERN_INFO "firewall: netfilter called with non-TCP-packet.\n");
        return NF_ACCEPT;
    }

    // Get the tcp-header for the packet
    tcp = skb_header_pointer(skb, ip_hdrlen(skb), sizeof(struct tcphdr), &_tcph);
    if (!tcp) {
        printk(KERN_INFO "Could not get tcp-header!\n");
        return NF_ACCEPT;
    }
    if (tcp->syn) {
        struct iphdr *ip;
        uint16_t dst_port;
        fw_port_list *port_list;

        printk(KERN_INFO "firewall: Starting connection \n");
        ip = ip_hdr(skb);
        if (!ip) {
            printk(KERN_INFO "firewall: Cannot get IP header!\n!");
        } else {
            printk(KERN_INFO "firewall: Destination address = %u.%u.%u.%u\n", NIPQUAD(ip->daddr));
        }
        printk(KERN_INFO "firewall: destination port = %d\n", ntohs(tcp->dest));

        if (in_irq() || in_softirq() || !(mm = get_task_mm(current))) {
            printk(KERN_INFO "Not in user context - retry packet\n");
            return NF_ACCEPT;
        }
        mmput(mm);

        dst_port = ntohs(tcp->dest);

        down_read(&rules_sem);
        port_list = Port_List;

        while (port_list) {
            if (port_list->port == dst_port) {
                // Determine program
                // char *program;
                struct program *prog;
                size_t prog_len;
                fw_prog_list *prog_list;

                prog = find_executable();
                prog_len = strlen(prog->name);

                prog_list = port_list->prog_list;

                while (prog_list) {
                    if (strncmp(prog->name, prog_list->program, prog_len) == 0) {
                        // Program found in program list for current port, accept packet
                        up_read(&rules_sem);
                        printk(KERN_INFO "Program '%s' found in list for port '%u', accepting\n", prog->name, (unsigned int) dst_port);
                        kfree(prog->buffer);
                        kfree(prog);
                        return NF_ACCEPT;
                    }
                    prog_list = prog_list->next;
                }

                // Program not found in program list, so drop packet
                up_read(&rules_sem);
                printk(KERN_INFO "Program '%s' not found in list for port '%u', dropping\n", prog->name, (unsigned int) dst_port);
                kfree(prog->buffer);
                kfree(prog);
                tcp_done(sk);
                return NF_DROP;
            }
            port_list = port_list->next;
        }

        up_read(&rules_sem);
        return NF_ACCEPT;
    }

    return NF_ACCEPT;
}

struct program *find_executable()
{
    struct path path;
    pid_t mod_pid;
    struct program *prog;
    char *prog_buf;
    char *prog_name;

    char cmdlineFile[EXE_BUFSIZ];
    int res;

    printk(KERN_INFO "findExecutable\n");
    // Current is pre-defined pointer to task structure of currently running task
    mod_pid = current->pid;
    snprintf(cmdlineFile, EXE_BUFSIZ, "/proc/%d/exe", mod_pid);
    res = kern_path(cmdlineFile, LOOKUP_FOLLOW, &path);
    if (res) {
        printk(KERN_INFO "Could not get dentry for %s\n", cmdlineFile);
        return NULL;
    }

    prog = (struct program *) kmalloc(sizeof(struct program), GFP_KERNEL);
    if (!prog) {
        printk(KERN_ALERT "Failed to allocate space for program struct\n");
        return NULL;
    }
    prog_buf = (char *) kmalloc(EXE_BUFSIZ * sizeof(char), GFP_KERNEL);
    if (!prog_buf) {
        printk(KERN_ALERT "Failed to allocate space for program buffer\n");
        kfree(prog);
        return NULL;
    }

    prog_name = d_path(&path, prog_buf, EXE_BUFSIZ);
    printk(KERN_INFO "Program name is %s\n", prog_name);
    printk(KERN_INFO "Program len is %d", strlen(prog_name));

    path_put(&path);

    prog->buffer = prog_buf;
    prog->name = prog_name;

    // Caller must later free prog->buffer and prog
    return prog;
}

int procfs_open(struct inode *inode, struct file *file)
{
    printk(KERN_DEBUG "procfs_open\n");

    mutex_lock(&proc_lock);
    if (Device_Open) {
		mutex_unlock(&proc_lock);
		return -EAGAIN;
    }
    Device_Open++;
    mutex_unlock(&proc_lock);

	try_module_get(THIS_MODULE);

	return 0;
}

int procfs_close(struct inode *inode, struct file *file)
{
    printk(KERN_DEBUG "procfs_close\n");

    mutex_lock(&proc_lock);
	Device_Open--;
	mutex_unlock(&proc_lock);

    module_put(THIS_MODULE);
    return 0;
}

ssize_t procfs_read(struct file *file, char __user *buffer, size_t count, loff_t *offset)
{
    fw_port_list *port_list;
    fw_prog_list *prog_list;
    printk(KERN_DEBUG "procfs_read\n");

    // Trigger printk of firewall rules line-by-line
    down_read(&rules_sem);
    port_list = Port_List;

    // Loop through all ports in Port_List
    while (port_list) {
        // Loop through all programs for the current Port_List item
        prog_list = port_list->prog_list;

        while (prog_list) {
            printk(KERN_INFO "Firewall rule: %u %s\n", (unsigned int) port_list->port, prog_list->program);
            prog_list = prog_list->next;
        }
        port_list = port_list->next;
    }
    up_read(&rules_sem);
    return 0;
}

ssize_t procfs_write(struct file *file, const char __user *buffer, size_t count, loff_t *offset)
{
    int errno;
    unsigned int i;
    char *kbuffer, *buf_port_p, *buf_prog_p;
    size_t port_len, prog_len;
    char *program;
    uint16_t port;
    fw_port_list *port_list, *new_port_list;
    fw_prog_list *new_prog_list;

    printk(KERN_DEBUG "procfs_write\n");

    // If buffer is NEW flag: initialise temporary new Port_List_Tmp
    if (count >= RULES_FLAG_LEN && strncmp(buffer, NEW_RULES_FLAG, RULES_FLAG_LEN) == 0) {
        printk(KERN_DEBUG "Buffer started with NEW flag\n");

        down_write(&rules_sem);
        Port_List_Tmp = init_portlist();
        if (!Port_List_Tmp) {
            printk(KERN_DEBUG "init_portlist failed\n");
            return -ENOMEM;
        }

        return count;
    }
    // If buffer is END flag: swap in Port_List_Tmp to Port_List
    else if (count >= RULES_FLAG_LEN && strncmp(buffer, END_RULES_FLAG, RULES_FLAG_LEN) == 0) {
        printk(KERN_DEBUG "Buffer started with END flag\n");

        // Cleanup old port list
        cleanup_portlist(Port_List);
        // Swap in the new port list from Port_List_Tmp to Port_List
        Port_List = Port_List_Tmp;
        Port_List_Tmp = NULL;
        up_write(&rules_sem);

        return count;
    }
    // If buffer is KIL flag: abort the creation of Port_List_Tmp and free memory
    else if (count >= RULES_FLAG_LEN && strncmp(buffer, KILL_RULES_FLAG, RULES_FLAG_LEN) == 0) {
        printk(KERN_DEBUG "Buffer started with KIL flag\n");

        // Cleanup Port_List_Tmp
        cleanup_portlist(Port_List_Tmp);
        Port_List_Tmp = NULL;

        up_write(&rules_sem);
        return count;
    }
    printk(KERN_DEBUG "Buffer was not a flag\n");

    // Create a kernel buffer with correct size
    kbuffer = (char *) kmalloc(sizeof(char) * count, GFP_KERNEL);
    if (!kbuffer) {
        printk(KERN_ALERT "Failed to allocate kernel buffer\n");
        cleanup_portlist(Port_List_Tmp);
        Port_List_Tmp = NULL;
		return -ENOMEM;
    }

    // Copy from user line-by-line into kernel buffer
    errno = copy_from_user(kbuffer, buffer, count);
    if (errno) {
        printk(KERN_ALERT "Failed to copy_from_user\n");
        kfree(kbuffer);
        cleanup_portlist(Port_List_Tmp);
        Port_List_Tmp = NULL;
        return errno;
    }
    kbuffer[count-1] = '\0';
    prog_len = strlen(kbuffer);

    // Use strsep on kernel buffer to split into two tokens by the space
    buf_prog_p = kbuffer;
    buf_port_p = strsep(&buf_prog_p, " ");
    printk(KERN_DEBUG "kbuffer    (%p) [%d]: %s\n", kbuffer, strlen(kbuffer), kbuffer);
    printk(KERN_DEBUG "buf_port_p (%p) [%d]: %s\n", buf_port_p, strlen(buf_port_p), buf_port_p);
    printk(KERN_DEBUG "buf_prog_p (%p) [%d]: %s\n", buf_prog_p, strlen(buf_prog_p), buf_prog_p);

    // Convert first token to integer
    port_len = strlen(buf_port_p);
    printk(KERN_DEBUG "Calculated port_len = %zu\n", port_len);
    kstrtouint(buf_port_p, 10, (unsigned int *) &port);
    printk(KERN_DEBUG "port: %u\n", (unsigned int) port);

    // Copy second token to a new memory location which will be pointed to by the program list
    prog_len -= port_len;
    printk(KERN_DEBUG "Calculated prog_len = %zu\n", prog_len);
    program = (char *) kmalloc(sizeof(char) * prog_len, GFP_KERNEL);
    if (!program) {
        printk(KERN_ALERT "Failed to allocate program string\n");
        kfree(kbuffer);
        cleanup_portlist(Port_List_Tmp);
        Port_List_Tmp = NULL;
        return -ENOMEM;
    }
    for (i = 0; i < prog_len; i++) {
        program[i] = kbuffer[i+port_len+1];
    }

    // Walk through Port_List_Tmp and add the new rule

    port_list = Port_List_Tmp;

    while (port_list) {
        if (port_list->port == 0 || port_list->port == port) {
            new_prog_list = (fw_prog_list *) kmalloc(sizeof(fw_prog_list), GFP_KERNEL);
            if (!new_prog_list) {
                printk(KERN_ALERT "Failed to allocate space for new program list element\n");
                kfree(program);
                kfree(kbuffer);
                cleanup_portlist(Port_List_Tmp);
                Port_List_Tmp = NULL;
                return -ENOMEM;
            }
            new_prog_list->next = port_list->prog_list;
            new_prog_list->program = program;

            // If this is the first entry into the port list, initialise the other values
            if (port_list->port == 0) {
                printk(KERN_DEBUG "Empty port_list, init next and port values\n");
                port_list->next = NULL;
                port_list->port = port;
            }
            // Point port's prog_list to the new element
            port_list->prog_list = new_prog_list;

            printk(KERN_DEBUG "Added program to front of port %u prog_list\n", (unsigned int) port_list->port);
            goto OUT;
        }
        port_list = port_list->next;
    }
    // Got to end of port_list and not found, add new entry to port list
    new_prog_list = (fw_prog_list *) kmalloc(sizeof(fw_prog_list), GFP_KERNEL);
    if (!new_prog_list) {
        printk(KERN_ALERT "Failed to allocate space for new program list element\n");
        kfree(program);
        kfree(kbuffer);
        cleanup_portlist(Port_List_Tmp);
        Port_List_Tmp = NULL;
        return -ENOMEM;
    }
    new_prog_list->next = NULL;
    new_prog_list->program = program;

    new_port_list = (fw_port_list *) kmalloc(sizeof(fw_port_list), GFP_KERNEL);
    if (!new_port_list) {
        printk(KERN_ALERT "Failed to allocate space for new port list element\n");
        kfree(new_prog_list);
        kfree(program);
        kfree(kbuffer);
        cleanup_portlist(Port_List_Tmp);
        Port_List_Tmp = NULL;
        return -ENOMEM;
    }
    new_port_list->next = Port_List_Tmp;
    new_port_list->port = port;
    new_port_list->prog_list = new_prog_list;
    Port_List_Tmp = new_port_list;
    printk(KERN_DEBUG "Added new port and prog list elements for %u to front of Port_List_Tmp\n", (unsigned int) Port_List_Tmp->port);

    OUT:
        kfree(kbuffer);
        return count;
}
