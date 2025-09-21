#pragma once

#include <arpa/inet.h>
#include <ctype.h>
#include <dirent.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

long int 自身结构体;
uintptr_t 基址头;
pid_t 进程 = 0;

class c_driver {
private:
    int has_upper = 0;
    int has_lower = 0;
    int has_symbol = 0;
    int has_digit = 0;
    int fd;
    pid_t pid;

    typedef struct _COPY_MEMORY {
        pid_t pid;
        uintptr_t addr;
        void *buffer;
        size_t size;
    } COPY_MEMORY, *PCOPY_MEMORY;

    typedef struct _MODULE_BASE {
        pid_t pid;
        char *name;
        uintptr_t base;
    } MODULE_BASE, *PMODULE_BASE;

    enum OPERATIONS {
        OP_READ_MEM = 601,
        OP_WRITE_MEM = 602,
        OP_MODULE_BASE = 603
    };

public:
    c_driver() {
        fd = socket(AF_INET, SOCK_DGRAM, 0);
        if (fd == -1) {
            perror("[-] 打开失败");
            exit(EXIT_FAILURE);
        }
    }

    ~c_driver() {
        if (fd > 0) {
            close(fd);
        }
    }

    void initialize(pid_t process) { pid = process; }

    bool read(uintptr_t addr, void *buffer, size_t size) {
        COPY_MEMORY cm{};
        cm.pid = pid;
        cm.addr = addr;
        cm.buffer = buffer;
        cm.size = size;

        if (ioctl(fd, OP_READ_MEM, &cm) != 0) {
            return false;
        }
        return true;
    }

    bool write(uintptr_t addr, void *buffer, size_t size) {
        COPY_MEMORY cm{};
        cm.pid = pid;
        cm.addr = addr;
        cm.buffer = buffer;
        cm.size = size;

        if (ioctl(fd, OP_WRITE_MEM, &cm) != 0) {
            return false;
        }
        return true;
    }

    template <typename T> T read(uintptr_t addr) {
        T res{};
        if (this->read(addr, &res, sizeof(T))) {
            return res;
        }
        return {};
    }

    template <typename T> bool write(uintptr_t addr, T value) {
        return this->write(addr, &value, sizeof(T));
    }

    uintptr_t get_module_base(char *name) {
        MODULE_BASE mb{};
        char buf[0x100];
        strcpy(buf, name);
        mb.pid = pid;
        mb.name = buf;

        if (ioctl(fd, OP_MODULE_BASE, &mb) != 0) {
            return 0;
        }
        return mb.base;
    }
};

static c_driver *driver = new c_driver();

static pid_t 获取进程(char *name) {
    FILE *fp = nullptr;
    pid_t process = 0;
    char cmd[0x100] = "pidof ";
    strcat(cmd, name);
    fp = popen(cmd, "r");
    if (fp) {
        if (fscanf(fp, "%d", &process) != 1) {
            process = 0;
        }
        pclose(fp);
    }
    return process;
}

