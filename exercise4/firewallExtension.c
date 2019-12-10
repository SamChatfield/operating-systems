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
    fw_prog_list *pl;

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
    } else {
        printk(KERN_INFO "Firewall extension module loaded\n");
    }

    Port_List = (fw_port_list *) kmalloc(sizeof(fw_port_list), GFP_KERNEL);
    if (!Port_List) return -1;

    // TODO: REMOVE
    pl = (fw_prog_list *) kmalloc(sizeof(fw_prog_list), GFP_KERNEL);
    if (!pl) {
        kfree(Port_List);
        return -1;
    }
    pl->next = NULL;
    pl->program = "/usr/bin/telnet.netkit";

    Port_List->next = NULL;
    Port_List->port = 80;
    Port_List->prog_list = pl;
    // TODO: END REMOVE

    // A non 0 return means init_module failed; module can't be loaded.
    return errno;
}

void cleanup_module(void)
{
    // Restore everything to normal
    nf_unregister_hook(&firewallExtension_ops);

    // Free all memory associated with the Port_List
    cleanup_portlist();

    remove_proc_entry(PROC_ENTRY_FILENAME, NULL);
    printk(KERN_INFO "/proc/%s removed\n", PROC_ENTRY_FILENAME);

    printk(KERN_INFO "Firewall extensions module unloaded\n");
}

void cleanup_portlist(void)
{
    fw_port_list *port_list, *tmp_port_list;
    fw_prog_list *prog_list, *tmp_prog_list;

    printk(KERN_DEBUG "Cleaning up Port_List\n");

    port_list = Port_List;

    while (port_list) {
        prog_list = port_list->prog_list;

        while (prog_list) {
            printk(KERN_DEBUG "Freeing '%s'\n", prog_list->program);
            tmp_prog_list = prog_list;
            prog_list = prog_list->next;
            // TODO: Free tmp_prog_list->program if that becomes heap allocated
            kfree(tmp_prog_list);
        }

        tmp_port_list = port_list;
        port_list = port_list->next;
        kfree(tmp_port_list);
    }

    Port_List = NULL;
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

        // Determine port
        // If port is in firewall_ports_list
            // Determine program
            // If program in port's firewall_programs_list
                // NF_ACCEPT
            // Else
                // NF_DROP
        // NF_ACCEPT

        dst_port = ntohs(tcp->dest);
        port_list = Port_List;

        while (port_list) {
            if (port_list->port == dst_port) {
                // Determine program
                // char *program;
                struct program *prog;
                size_t prog_len;
                fw_prog_list *prog_list;

                prog = find_executable();
                // program = find_executable(program);
                prog_len = strlen(prog->name);

                prog_list = port_list->prog_list;

                while (prog_list) {
                    if (strncmp(prog->name, prog_list->program, prog_len) == 0) {
                        // Program found in program list for current port, accept packet
                        printk(KERN_INFO "Program '%s' found in list for port '%u', accepting\n", prog->name, (unsigned int) dst_port);
                        kfree(prog->buffer);
                        kfree(prog);
                        return NF_ACCEPT;
                    }
                    prog_list = prog_list->next;
                }

                // Program not found in program list, so drop packet
                printk(KERN_INFO "Program '%s' not found in list for port '%u', dropping\n", prog->name, (unsigned int) dst_port);
                kfree(prog->buffer);
                kfree(prog);
                tcp_done(sk);
                return NF_DROP;
            }
            port_list = port_list->next;
        }

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
    printk(KERN_INFO "program_buf pointer: %p, program pointer: %p\n", (void *) prog_buf, (void *) prog_name);

    path_put(&path);

    prog->buffer = prog_buf;
    prog->name = prog_name;

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
    printk(KERN_DEBUG "procfs_read\n");
    return -1;
}

ssize_t procfs_write(struct file *file, const char __user *buffer, size_t count, loff_t *offset)
{
    printk(KERN_DEBUG "procfs_write\n");
    return -1;
}
