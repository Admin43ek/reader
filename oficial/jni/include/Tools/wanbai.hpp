#define NEKO_H
#include <sys/fcntl.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/utsname.h> 


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
		void* buffer;
		size_t size;
	} COPY_MEMORY, *PCOPY_MEMORY;

	typedef struct _MODULE_BASE {
		pid_t pid;
		char* name;
		uintptr_t base;
	} MODULE_BASE, *PMODULE_BASE;

	enum OPERATIONS {
		OP_INIT_KEY = 0x800,
		OP_READ_MEM = 0x801,
		OP_WRITE_MEM = 0x802,
		OP_MODULE_BASE = 0x803,
	};
	char *kernel()
  {
    struct utsname unameData;      
    if (uname(&unameData) == 0){
        return unameData.release; //sysname：操作系统名称  nodename：计算机网络名称  release：操作系统发行版本  version：操作系统内核版本  machine：硬件架构  domainname：域名
    }else{
    printf("\033[31m");
        printf("[-]无法获取内核版本。");
		exit(0);
        return NULL;
    }
  }
char* get_dev() {//by.温柔
    char* command = "for dir in /proc/*/; do cmdline_file=\"cmdline\"; comm_file=\"comm\"; proclj=\"$dir$cmdline_file\"; proclj2=\"$dir$comm_file\"; if [[ -f \"$proclj\" && -f \"$proclj2\" ]]; then cmdline=$(head -n 1 \"$proclj\"); comm=$(head -n 1 \"$proclj2\"); if echo \"$cmdline\" | grep -qE '^/data/[a-z]{6}$'; then sbwj=$(echo \"$comm\"); open_file=\"\"; for file in \"$dir\"/fd/*; do link=$(readlink \"$file\"); if [[ \"$link\" == \"/dev/$sbwj (deleted)\" ]]; then open_file=\"$file\"; break; fi; done; if [[ -n \"$open_file\" ]]; then nhjd=$(echo \"$open_file\"); sbid=$(ls -L -l \"$nhjd\" | sed 's/\\([^,]*\\).*/\\1/' | sed 's/.*root //'); echo \"/dev/$sbwj\"; rm -Rf \"/dev/$sbwj\"; mknod \"/dev/$sbwj\" c \"$sbid\" 0; break; fi; fi; fi; done";
    FILE* file = popen(command, "r");
    if (file == NULL) {
        return NULL;
    }
    char result[512];
    fgets(result, sizeof(result), file);
    pclose(file);
    int len = strlen(result);
    if (len > 0 && result[len - 1] == '\n') {
        result[len - 1] = '\0';
    }
    return strdup(result);
    }
	int open_driver() {
	char dev_path[64];
	strcpy(dev_path,get_dev());
	fd = open(dev_path, O_RDWR);
	if (fd>0){
	printf("驱动文件：%s\n", dev_path);
	unlink(dev_path);
	return 0;
	} else {
	printf("无法找到驱动文件！\n");
	return -1;
	}
	return -1;
	}
	public:
	c_driver() {
		open_driver();
		if (fd <= 0) {
			printf("[-] open driver failed\n");
			exit(0);
		}
	}
	~c_driver()
	{
		if (fd > 0)
			close(fd);
	}

	void initialize(pid_t pid)
	{
		this->pid = pid;
	}

    bool initKey(const char* key) {
        if (ioctl(fd, OP_INIT_KEY, key) != 0) {
            return false;
        }
        return true;
    }

    bool read(uintptr_t addr, void* buffer, size_t size) {
        COPY_MEMORY cm;
        cm.pid = this->pid;
        cm.addr = addr;
        cm.buffer = buffer;
        cm.size = size;

        if (ioctl(fd, OP_READ_MEM, &cm) != 0) {
            return false;
        }
        return true;
    }

    bool write(uintptr_t addr, void* buffer, size_t size) {
        COPY_MEMORY cm;
        cm.pid = this->pid;
        cm.addr = addr;
        cm.buffer = buffer;
        cm.size = size;

        if (ioctl(fd, OP_WRITE_MEM, &cm) != 0) {
            return false;
        }
        return true;
    }

    template <typename T>
    T read(uintptr_t addr) {
        T res;
        if (this->read(addr, &res, sizeof(T)))
            return res;
        return {};
        
    }
template <class T> T WriteAddress(long int addr, T value)
    {
    char lj[128];
    sprintf(lj, "/proc/%d/mem", pid);
    long int handle = open(lj, O_RDWR | O_SYNC);
    pwrite64(handle, &value, sizeof(T), addr);
    close(handle);
    return 0;
    }
    template <typename T>
    bool write(uintptr_t addr, T value) {
        return this->write(addr, &value, sizeof(T));
    }

    uintptr_t getModuleBase(const char* name) {
        MODULE_BASE mb;
        mb.pid = this->pid;
        mb.name = strdup(name);

        if (ioctl(fd, OP_MODULE_BASE, &mb) != 0) {
            free(mb.name);
            return 0;
        }
		
        uintptr_t base = mb.base;
        free(mb.name);
        return base;
    }
};

static c_driver* driver = new c_driver();