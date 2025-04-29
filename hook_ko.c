#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/fs.h>
#include <asm/paravirt.h> // for write_cr0/read_cr0 on x86

MODULE_LICENSE("GPL");

unsigned long **sys_call_table;
asmlinkage long (*original_sys_openat)(int dfd, const char __user *filename, int flags, umode_t mode);

asmlinkage long hooked_sys_openat(int dfd, const char __user *filename, int flags, umode_t mode) {
    char fname[256];
    long ret;

    // copy filename from userspace
    if (strncpy_from_user(fname, filename, sizeof(fname)) > 0) {
        printk(KERN_INFO "intercepted openat(): %s\n", fname);
    }

    // call the real syscall
    ret = original_sys_openat(dfd, filename, flags, mode);

    // modify return value if needed (e.g., deny certain files)
    if (strstr(fname, "secret.txt")) {
        printk(KERN_INFO "blocking access to secret.txt\n");
        return -EACCES; // Permission denied
    }

    return ret;
}

static unsigned long **find_sys_call_table(void) {
    // normally use kallsyms_lookup_name (needs export), 
    // or hardcode symbol address (bad idea).
    // this function must be implemented depending on your environment.

    //FOR EXAMPLE in a pseudocode style
    unsigned long i;
    unsigned long **sct;

    for (i = some_address; i < max_address; i += sizeof(void *)) {
        sct = (unsigned long **) i;

        if (sct[__NR_close] == (unsigned long *) sys_close)
            return sct;
    }

    return NULL;
}

static int __init hook_init(void) {
    sys_call_table = find_sys_call_table();
    if (!sys_call_table) {
        printk(KERN_ERR "cannot find sys_call_table\n");
        return -1;
    }

    // disable write protection
    write_cr0(read_cr0() & (~0x10000));

    // hook
    original_sys_openat = (void *)sys_call_table[__NR_openat];
    sys_call_table[__NR_openat] = (unsigned long *)hooked_sys_openat;

    // re-enable write protection
    write_cr0(read_cr0() | 0x10000);

    printk(KERN_INFO "syscall hooking loaded\n");
    return 0;
}

static void __exit hook_exit(void) {
    // restore original syscall
    write_cr0(read_cr0() & (~0x10000));
    sys_call_table[__NR_openat] = (unsigned long *)original_sys_openat;
    write_cr0(read_cr0() | 0x10000);

    printk(KERN_INFO "syscall hooking unloaded\n");
}

module_init(hook_init);
module_exit(hook_exit);
