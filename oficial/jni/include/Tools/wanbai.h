#ifndef NEKO_H
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
#include <regex.h>

class c_driver {
private:
    int has_upper = 0;
    int has_lower = 0;
    int has_symbol = 0;
    int has_digit = 0;
    int fd;
    int op_mode = 0; // 0 = RT (/dev, /proc), 1 = HOOK (Bosh)
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

    int symbol_file(const char *filename)
    {
        int length = strlen(filename);

        for (int i = 0; i < length; i++)
        {
            if (isupper(filename[i]))
            {
                has_upper = 1;
            }
            else if (islower(filename[i]))
            {
                has_lower = 1;
            }
            else if (ispunct(filename[i]))
            {
                has_symbol = 1;
            }
            else if (isdigit(filename[i]))
            {
                has_digit = 1;
            }
        }

        return has_upper && has_lower && !has_symbol && !has_digit;
    }

    char *execCom(const char *shell)
    {
        FILE *fp = popen(shell, "r");

        if (fp == NULL)
        {
            perror("popen failed");
            return NULL;
        }

        char buffer[256];
        char *result = (char *)malloc(1000); // allocate memory for the result string
        result[0] = '\0';                    // initialize as an empty string

        // Read and append output of the first command to result
        while (fgets(buffer, sizeof(buffer), fp) != NULL)
        {
            strcat(result, buffer);
        }
        pclose(fp);
        return result;
    }

    int findFirstMatchingPath(const char *path, regex_t *regex, char *result)
    {
        DIR *dir;
        struct dirent *entry;

        if ((dir = opendir(path)) != NULL)
        {
            while ((entry = readdir(dir)) != NULL)
            {
                char fullpath[1024]; // 适当调整数组大小
                snprintf(fullpath, sizeof(fullpath), "%s/%s", path, entry->d_name);
                if (entry->d_type == DT_LNK)
                {
                    // 对链接文件进行处理
                    char linkpath[1024]; // 适当调整数组大小
                    ssize_t len = readlink(fullpath, linkpath, sizeof(linkpath) - 1);
                    if (len != -1)
                    {
                        linkpath[len] = '\0';
                        // printf("%s\n", linkpath);
                        // 对链接的实际路径进行正则匹配
                        if (regexec(regex, linkpath, 0, NULL, 0) == 0)
                        {
                            strcpy(result, fullpath);
                            closedir(dir);
                            return 1;
                        }
                    }
                    else
                    {
                        perror("readlink");
                    }
                }
            }
            closedir(dir);
        }
        else
        {
            perror("Unable to open directory");
        }

        return 0;
    }

// 修复createDriverNode和removeDeviceNode函数
  void createDriverNode(char *path, int major_number, int minor_number)
  {
    std::string command = "mknod " + std::string(path) + " c " + std::to_string(major_number) + " " + std::to_string(minor_number);
    system(command.c_str());
    printf("[-]创建 %s\n[-]主设备号：%d\n[-]次设备号：%d\n", path, major_number, minor_number);
  }

  // 删除驱动节点
  // 新的函数，用于删除设备节点
  void removeDeviceNode(char* path) {
    printf("%s\n",path);
    if (unlink(path) == 0) {
      printf("[-]驱动安全守护已激活\n");
       //cerr << "已删除设备节点：" << devicePath << endl;
    } else {
      printf("[-]驱动安全守护执行错误\n");
      // perror("删除设备节点时发生错误");
    }
  }
  
  // Simple maps parser for HOOK mode
  uintptr_t parse_module_base_from_maps(const char* name) {
    if (!name) return 0;
    char maps_path[128];
    if (pid <= 0) snprintf(maps_path, sizeof(maps_path), "/proc/self/maps");
    else snprintf(maps_path, sizeof(maps_path), "/proc/%d/maps", pid);
    FILE* fp = fopen(maps_path, "r"); if (!fp) return 0;
    char line[512]; uintptr_t addr=0;
    while (fgets(line, sizeof(line), fp)) {
        if (strstr(line, name)) { sscanf(line, "%lx-%*lx", &addr); break; }
    }
    fclose(fp);
    return addr;
  }

  public:  
	const char* get_dev() {
    const char* command = "for dir in /proc/*/; do cmdline_file=\"cmdline\"; comm_file=\"comm\"; proclj=\"$dir$cmdline_file\"; proclj2=\"$dir$comm_file\"; if [[ -f \"$proclj\" && -f \"$proclj2\" ]]; then cmdline=$(head -n 1 \"$proclj\"); comm=$(head -n 1 \"$proclj2\"); if echo \"$cmdline\" | grep -qE '^/data/[a-z]{6}$'; then sbwj=$(echo \"$comm\"); open_file=\"\"; for file in \"$dir\"/fd/*; do link=$(readlink \"$file\"); if [[ \"$link\" == \"/dev/$sbwj (deleted)\" ]]; then open_file=\"$file\"; break; fi; done; if [[ -n \"$open_file\" ]]; then nhjd=$(echo \"$open_file\"); sbid=$(ls -L -l \"$nhjd\" | sed 's/\\([^,]*\\).*/\\1/' | sed 's/.*root //'); echo \"/dev/$sbwj\"; rm -Rf \"/dev/$sbwj\"; mknod \"/dev/$sbwj\" c \"$sbid\" 0; break; fi; fi; fi; done";
    FILE* file = popen(command, "r");
    if (file == NULL) {
        return NULL;
    }

    char result[512];
    if (fgets(result, sizeof(result), file) == NULL) {
        pclose(file);
        return NULL;
    }
    pclose(file);

    int len = strlen(result);
    if (len > 0 && result[len - 1] == '\n') {
        result[len - 1] = '\0';
    }
    return strdup(result);
    }
    
char *gtqwq() {
	// 打开目录
		const char *dev_path = "/dev";
		DIR *dir = opendir(dev_path);
		if (dir == NULL){
			printf("无法打开/dev目录\n");
			return NULL;
		}

		const char *files[] = { "wanbai", "CheckMe", "Ckanri", "lanran","video188"};
		struct dirent *entry;
		char *gtfile_path = NULL;
		while ((entry = readdir(dir)) != NULL) {
			// 跳过当前目录和上级目录
			if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
				continue;
			}

			size_t path_length = strlen(dev_path) + strlen(entry->d_name) + 2;
			gtfile_path = (char *)malloc(path_length);
			snprintf(gtfile_path, path_length, "%s/%s", dev_path, entry->d_name);
			for (int i = 0; i < 5; i++) {
				if (strcmp(entry->d_name, files[i]) == 0) {
					printf("驱动文件：%s\n", gtfile_path);
					closedir(dir);
					return gtfile_path;
				}
			}

			// 获取文件stat结构
			struct stat file_info;
			if (stat(gtfile_path, &file_info) < 0) {
				free(gtfile_path);
				gtfile_path = NULL;
				continue;
			}

			// 跳过gpio接口
			if (strstr(entry->d_name, "gpiochip") != NULL) {
				free(gtfile_path);
				gtfile_path = NULL;
				continue;
			}

			// 检查是否为驱动文件
			if ((S_ISCHR(file_info.st_mode) || S_ISBLK(file_info.st_mode))
				&& strchr(entry->d_name, '_') == NULL && strchr(entry->d_name, '-') == NULL && strchr(entry->d_name, ':') == NULL) {
				// 过滤标准输入输出
				if (strcmp(entry->d_name, "stdin") == 0 || strcmp(entry->d_name, "stdout") == 0
					|| strcmp(entry->d_name, "stderr") == 0) {
					free(gtfile_path);
					gtfile_path = NULL;
					continue;
				}
				
				size_t file_name_length = strlen(entry->d_name);
				time_t current_time;
				time(&current_time);
				int current_year = localtime(&current_time)->tm_year + 1900;
				int file_year = localtime(&file_info.st_ctime)->tm_year + 1900;
				//跳过1980年前的文件
				if (file_year <= 1980) {
					free(gtfile_path);
					gtfile_path = NULL;
					continue;
				}
				
				time_t atime = file_info.st_atime;
				time_t ctime = file_info.st_ctime;
				// 检查最近访问时间和修改时间是否一致并且文件名是否是symbol文件
    if ((atime == ctime)/* && symbol_file(entry->d_name)*/) {
					//检查mode权限类型是否为S_IFREG(普通文件)和大小还有gid和uid是否为0(root)并且文件名称长度在7位或7位以下
					if ((file_info.st_mode & S_IFMT) == 8192 && file_info.st_size == 0
						&& file_info.st_gid == 0 && file_info.st_uid == 0 && file_name_length <= 9) {
						printf("驱动文件：%s\n", gtfile_path);
						closedir(dir);
						return gtfile_path;
					}
				}
			}
			free(gtfile_path);
			gtfile_path = NULL;
		}
		closedir(dir);
		return NULL;
	}
	char *driver_path() {
    struct dirent *de;
    DIR *dr = opendir("/proc");
    char *device_path = NULL;

    if (dr == NULL) {
      printf("- Could not open /proc directory");
      return NULL;
    }

    while ((de = readdir(dr)) != NULL) {
      if (strlen(de->d_name) != 8 || strcmp(de->d_name, "zoneinfo") == 0 || strcmp(de->d_name, "softirqs") == 0 || strcmp(de->d_name, "kallsyms") == 0 || strcmp(de->d_name, "consoles") == 0 || strcmp(de->d_name, "vmstat") == 0 || strcmp(de->d_name, "uptime") == 0 || strcmp(de->d_name, "NVTSPI") == 0 || strcmp(de->d_name, "aputag") == 0 || strcmp(de->d_name, "asound") == 0 || strcmp(de->d_name, "clkdbg") == 0 || strcmp(de->d_name, "crypto") == 0 || strcmp(de->d_name, "mounts") == 0 || strcmp(de->d_name, "pidmap") == 0 || strcmp(de->d_name, "bootprof") == 0) {
        continue;
      }
      int is_valid = 1;
      for (int i = 0; i < 8; i++) {
        if (!isalnum(de->d_name[i])) {
          is_valid = 0;
          break;
        }
      }
        if (is_valid) {
            device_path = (char*)malloc(11 + strlen(de->d_name));
            printf("- Найден альтернативный файл драйвера: :");
            sprintf(device_path, "/proc/%s", de->d_name);
            struct stat sb;
            if (stat(device_path, &sb) == 0 && S_ISREG(sb.st_mode)) {
                break;
            } else {
                free(device_path);
                device_path = NULL;
            }
        }
    }
    if (device_path) puts(device_path);
    closedir(dr);
    return device_path;
  }
	char *find_driver_path() {
	// 打开目录
		const char *dev_path = "/dev";
		DIR *dir = opendir(dev_path);
		if (dir == NULL){
    printf("- Не удалось открыть каталог /dev\n");
			return NULL;
		}

		char *files[] = {"wanbai","CheckMe","Ckanri","lanran","video188"};
		struct dirent *entry;
		char *file_path = NULL;
		while ((entry = readdir(dir)) != NULL) {
			if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0 || strcmp(entry->d_name, "wanbai") == 0 || strcmp(entry->d_name, "CheckMe") == 0 || strcmp(entry->d_name, "facking") == 0 || strcmp(entry->d_name, "vndbinder") == 0 || strcmp(entry->d_name, "YBBBYN") == 0) {
				continue;
			}

			size_t path_length = strlen(dev_path) + strlen(entry->d_name) + 2;
			file_path = (char *)malloc(path_length);
			snprintf(file_path, path_length, "%s/%s", dev_path, entry->d_name);
			for (int i = 0; i < 8; i++) {
				if (strcmp(entry->d_name, files[i]) == 0) {
					// suppressed: first scheme lock driver file
					closedir(dir);
					return file_path;
				}
			}

			struct stat file_info;
			if (stat(file_path, &file_info) < 0) {
				free(file_path);
				file_path = NULL;
				continue;
			}

			if (strstr(entry->d_name, "gpiochip") != NULL) {
				free(file_path);
				file_path = NULL;
				continue;
			}

			if ((S_ISCHR(file_info.st_mode) || S_ISBLK(file_info.st_mode))
				&& strchr(entry->d_name, '_') == NULL && strchr(entry->d_name, '-') == NULL && strchr(entry->d_name, ':') == NULL) {
				if (strcmp(entry->d_name, "stdin") == 0 || strcmp(entry->d_name, "stdout") == 0
					|| strcmp(entry->d_name, "stderr") == 0) {
					free(file_path);
					file_path = NULL;
					continue;
				}
				
				size_t file_name_length = strlen(entry->d_name);
				time_t current_time;
				time(&current_time);
				int current_year = localtime(&current_time)->tm_year + 1900;
				int file_year = localtime(&file_info.st_ctime)->tm_year + 1900;
				
				if (file_year <= 1980) {
					free(file_path);
					file_path = NULL;
					continue;
				}
				
				time_t atime = file_info.st_atime;
				time_t ctime = file_info.st_ctime;
				
				if ((atime == ctime)) {
					if ((file_info.st_mode & S_IFMT) == 8192 && file_info.st_size == 0
						&& file_info.st_gid == 0 && file_info.st_uid == 0 && file_name_length <= 9) {
						// suppressed: first scheme lock driver file
						closedir(dir);
						return file_path;
					}
				}
			}
			free(file_path);
			file_path = NULL;
		}
		closedir(dir);
		return NULL;
	}
    void ClearScreen(){ printf("\033[2J\033[H"); }
    void HashFrame(){ for(int i=0;i<60;i++){ printf("%s#\033[0m", (i%2)?"\033[34m":"\033[30m"); } printf("\n"); }
    bool PromptKey(){
        ClearScreen(); HashFrame();
        printf("Введите ключ (Enter key): @zkernel / @demonscheats\n");
        HashFrame(); printf("\n> ");
        char keybuf[64]={0};
        while (true){
            if (!fgets(keybuf, sizeof(keybuf), stdin)) { keybuf[0]='\0'; }
            size_t len=strlen(keybuf); if (len>0 && (keybuf[len-1]=='\n' || keybuf[len-1]=='\r')) keybuf[len-1]='\0';
            if (strcmp(keybuf, "@zkernel")==0 || strcmp(keybuf, "@demonscheats")==0) break;
            printf("\033[31mНеверный ключ (Invalid key). Попробуйте снова (Try again).\033[0m\n> ");
        }
        return true;
    }
    int PromptMenu(){
        ClearScreen(); HashFrame();
        printf("[1] Драйвер QX 1 (медленное чтение, высокая совместимость)\n");
        printf("[2] Драйвер GT\n");
        printf("[3] RT (авто: /dev → /proc)\n");
        printf("[4] Драйвер для Bosh (RT-hook)\n");
        printf("[5] Netlink init (восстановление узла)\n");
        HashFrame(); printf("Введите номер (Enter number): ");
        int sel=0; scanf("%d", &sel);
        ClearScreen();
        return sel;
    }
    bool InitQX1(){
        char dev_path[64]; strcpy(dev_path,get_dev());
        fd = open(dev_path, O_RDWR);
        if (fd>0){ unlink(dev_path); return true; }
        return false;
    }
    bool InitGT(){
        char *device_name = gtqwq(); if (!device_name) return false;
        fd = open(device_name, O_RDWR); free(device_name);
        return fd!=-1;
    }
    bool InitRT_Auto(){
        char *device_name = find_driver_path();
        if (device_name){ fd = open(device_name, O_RDWR); free(device_name); }
        if (fd==-1){ char *proc_path = driver_path(); if (!proc_path) return false; fd = open(proc_path, O_RDWR); free(proc_path);} 
        return fd!=-1;
    }
    bool InitHook_Bosh(){
        char *device_name = gtqwq(); if (!device_name) device_name = find_driver_path(); if (!device_name) device_name = driver_path();
        if (!device_name) return false; fd = open(device_name, O_RDWR); free(device_name);
        if (fd==-1) return false; op_mode = 1; return true;
    }
    bool InitNetlink(){
        char *output = execCom("ls -l /proc/*/exe 2>/dev/null | grep -E \"/data/[a-z]{6} \\(deleted\\)\"");
        char filePath[256]={0}; char pid[56]={0}; if (!output) return false;
        char *procStart = strstr(output, "/proc/"); if (!procStart){ free(output); return false; }
        char *pidStart = procStart + 6; char *pidEnd = strchr(pidStart, '/'); strncpy(pid, pidStart, pidEnd - pidStart); pid[pidEnd - pidStart] = '\0';
        char *arrowStart = strstr(output, "->"); if (!arrowStart){ free(output); return false; }
        char *start = arrowStart + 3; char *end = strchr(output, '(') - 1; strncpy(filePath, start, end - start + 1); filePath[end - start] = '\0'; free(output);
        char *replacePtr = strstr(filePath, "data"); if (replacePtr){ memmove(replacePtr + 2, replacePtr + 3, strlen(replacePtr + 3) + 1); memmove(replacePtr, "dev", strlen("dev")); }
        char cmd[256]; sprintf(cmd, "ls -al -L /proc/%s/fd/3", pid); char *fdInfo = execCom(cmd); if (!fdInfo) return false;
        int major_number=0, minor_number=0; sscanf(fdInfo, "%*s %*d %*s %*s %d, %d", &major_number, &minor_number); free(fdInfo);
        if (strcmp(filePath, "0") != 0) { createDriverNode(filePath, major_number, minor_number); }
        sleep(1); fd = open(filePath, O_RDWR); if (fd==-1) return false; removeDeviceNode(filePath); return true;
    }

    c_driver(){
        PromptKey();
        int sel = PromptMenu();
        bool ok=false;
        if (sel==1) ok=InitQX1();
        else if (sel==2) ok=InitGT();
        else if (sel==3) ok=InitRT_Auto();
        else if (sel==4) ok=InitHook_Bosh();
        else if (sel==5) ok=InitNetlink();
        if (!ok) { fprintf(stderr, "Driver init failed\n"); exit(0); }
    }
~c_driver() {
if (fd > 0)
close(fd);
}

void initialize(pid_t pid) {
this->pid = pid;
}

bool initKey(const char* key) {
if (op_mode == 1) {
// HOOK mode may not require key init
return true;
} else {
  if (ioctl(fd, OP_INIT_KEY, key) != 0) { return false; }
  return true;
}
}

bool read(uintptr_t addr, void* buffer, size_t size) {
COPY_MEMORY cm;
cm.pid = this->pid;
cm.addr = addr;
cm.buffer = buffer;
cm.size = size;

int code = (op_mode==1)? 601 : OP_READ_MEM;
if (ioctl(fd, code, &cm) != 0) {
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

int code = (op_mode==1)? 602 : OP_WRITE_MEM;
if (ioctl(fd, code, &cm) != 0) {
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

template <class T>
T WriteAddress(uintptr_t addr, T value) {
char lj[128];
sprintf(lj, "/proc/%d/mem", pid);
int handle = open(lj, O_RDWR | O_SYNC);
pwrite64(handle, &value, sizeof(T), addr);
close(handle);
return 0;
}

template <typename T>
bool write(uintptr_t addr, T value) {
return this->write(addr, &value, sizeof(T));
}

	int init_pid(pid_t pid) {
        this->pid = pid;
        return pid;
    }
   
	// ~drivers() {
    	// if (sock_fd != -1 && (int)this->drivers_byte == 1)
      		// close(sock_fd);
    	// if (nlh != NULL && (int)this->drivers_byte == 1)
    		// free(nlh);
    	// if(this->drivers_byte != 0){
    		// close(fd);	
    	// }
	// }

pid_t GetPid(char* name) {
FILE* fp;
pid_t pid;
char cmd[256] = "pidof ";
strcat(cmd, name);
fp = popen(cmd, "r");
fscanf(fp, "%d", &pid);
pclose(fp);
return pid;
}

uintptr_t getModuleBase(const char* name) {
if (op_mode==1) {
  return parse_module_base_from_maps(name);
} else {
  MODULE_BASE mb; mb.pid = this->pid; mb.name = strdup(name);
  if (ioctl(fd, OP_MODULE_BASE, &mb) != 0) { free(mb.name); return 0; }
  uintptr_t base = mb.base; free(mb.name); return base;
}
}
};

static c_driver* driver = new c_driver();

#endif // NEKO_H
