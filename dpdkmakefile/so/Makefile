#本例子展示了不使用dpdk编译环境的独立makefile文件(dpdk动态库方式)，基于dpdk1.7.0+l2fwd
#1.DPDK默认是编译成静态库的，改成动态库只需要把config/common_linuxapp文件中
#    CONFIG_RTE_BUILD_SHARED_LIB=n修改成CONFIG_RTE_BUILD_SHARED_LIB=y就行了。
#    指定CONFIG_RTE_BUILD_COMBINE_LIBS=y 可以打成一个独立的so
#    so文件存放在dpdk-1.7.0/x86_64-native-linuxapp-gcc/lib/libintel_dpdk.so
#2.cp /root/dpdk-1.7.0/x86_64-native-linuxapp-gcc/lib/libintel_dpdk.so /usr/local/lib/libintel_dpdk.so 
#ldconfig 
#3.写这个makefile 注意 CFLAGS += -include $(ROOT_DIR)/include/rte_config.h
#WORK_DIR是本程序的目录
#ROOT_DIR是dpdk的编译环境的根目录，如下。。
WORK_DIR=/root/dpdkmakefile/
ROOT_DIR=/root/dpdk-1.7.0/x86_64-native-linuxapp-gcc/
ifeq ($(WORK_DIR),)
$(error "Please define RTE_SDK environment variable")
endif


shell = /bin/sh

LIB_DIR = $(ROOT_DIR)/lib/   #该目录中是dpdk生成的所有dpdk静态库 include目录下是各种.h文件。。

BIN_DIR = $(WORK_DIR)/bin/

OBJECT_DIR = $(WORK_DIR)/obj/ 

APP_DIR = $(WORK_DIR)/libc/  

RM = rm -fr
#****************************************************************************

CC = gcc

AR = ar rc


#lib
    
    LIBS += -lrt
    LIBS += -lm -ldl
    LIBS +=  -lpthread
	LIBS +=  -lintel_dpdk
BIN_OBJECT = l2fwd

OBJ = main.o 

CFLAGS += -g -O0 -I$(WORK_DIR)/include -I$(ROOT_DIR)/include
CFLAGS += -include $(ROOT_DIR)/include/rte_config.h 


all:$(OBJ)
	$(CC) $(CFLAGS)   -o $(BIN_OBJECT) $(OBJ)  -lrt   $(LIBS) 
	cp -rf $(BIN_OBJECT) $(BIN_DIR)

$(OBJ):%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@  -lrt  $(LIBS) 

clean:
	rm -rf *.o
