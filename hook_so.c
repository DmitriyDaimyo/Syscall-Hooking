#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <stdarg.h>

int open(const char *pathname, int flags, ...) {
    static int (*real_open)(const char *, int, ...) = NULL;
    va_list args;
    mode_t mode = 0;
    int ret;

    if (!real_open)
        real_open = dlsym(RTLD_NEXT, "open");

    // if O_CREAT is used, mode argument must be extracted
    if (flags & O_CREAT) {
        va_start(args, flags);
        mode = va_arg(args, mode_t);
        va_end(args);
        ret = real_open(pathname, flags, mode);
    } else {
        ret = real_open(pathname, flags);
    }

    // now we have the real syscall result
    if (ret >= 0) {
        printf("[*] opened file: %s (fd=%d)\n", pathname, ret);

        // if the file is "secret.txt", fake an error
        if (strstr(pathname, "secret.txt")) {
            close(ret); // Close the real fd
            errno = ENOENT;
            return -1;  // 'No such file or directory'
        }
    }

    return ret;
}
