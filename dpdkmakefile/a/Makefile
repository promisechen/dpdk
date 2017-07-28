#本例子展示了不使用dpdk编译环境的独立makefile文件（DPDK静态库方式），基于dpdk1.7.0+l2fwd
#注意引用静态库的顺序问题，被包含的静态库放在后面
#-Wl,--whole-archive 将所有符号进行连接,f否则无法启动端口，据说是网卡绑定使用了gcc 构造函数的特性，在执行main函数前设置了网卡信息
#每种网卡驱动都调用PMD_REGISTER_DRIVER，这是一个构造函数的宏。这个宏调用了函数rte_eal_driver_register，只是把所有类型驱动挂在到dev_driver_list
#链表中。这几个函数就会在main函数执行之前执行。
#如何不想使用-Wl,--whole-archive，可以在在rte_eal_init之前加入如下代码
#如下内容：最好也去掉PMD_REGISTER_DRIVER 宏中的 __attribute__((constructor, used))
#extern  struct rte_driver rte_vmxnet3_driver;
#extern struct rte_driver pmd_igb_drv;
#extern struct rte_driver em_pmd_drv;
#
#extern struct rte_driver pmd_igbvf_drv;
#extern struct rte_driver rte_i40e_driver;
#extern struct rte_driver rte_i40evf_driver;
#extern struct rte_driver rte_ixgbe_driver;
#extern struct rte_driver rte_ixgbevf_driver;
#extern struct rte_driver pmd_pcap_drv;
#extern struct rte_driver pmd_ring_drv;
#extern struct rte_driver rte_virtio_driver;
#extern struct rte_driver rte_vmxnet3_driver;
#extern struct rte_driver pmd_xenvirt_drv;
#int clxxxx_dev11(void);
#
##if 1
#int clxxxx_dev11(void)
#{
#   rte_eal_driver_register(&em_pmd_drv);
#   rte_eal_driver_register(&pmd_igb_drv);
#   rte_eal_driver_register(&rte_vmxnet3_driver);
#   rte_eal_driver_register(&pmd_igbvf_drv);
#   rte_eal_driver_register(&rte_i40e_driver);
#   rte_eal_driver_register(&rte_i40evf_driver);
#   rte_eal_driver_register(&rte_ixgbe_driver);
#   rte_eal_driver_register(&rte_ixgbevf_driver);
#   
#   //rte_eal_driver_register(&pmd_pcap_drv);
#   rte_eal_driver_register(&pmd_ring_drv);
#   rte_eal_driver_register(&rte_virtio_driver);
#
#   //rte_eal_driver_register(&pmd_xenvirt_drv);
#    return 0;
#}
##endif

#WORK_DIR是本程序的目录
#ROOT_DIR是dpdk的编译环境的根目录，
WORK_DIR=/root/dpdkmake/
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
    LIBDPDK =  $(ROOT_DIR)/lib
    LIBS += -lethdev
    LIBS += -lrte_mbuf
#    LIBS += -lrte_pmd_af_packet
    LIBS += -lrte_pmd_i40e
    LIBS += -lrte_pmd_vmxnet3_uio
    LIBS += -lrte_sched
    LIBS += -lrte_eal
    LIBS += -lrte_kvargs
    LIBS += -lrte_mempool
    LIBS += -lrte_pmd_bond
    LIBS += -lrte_pmd_ixgbe
    LIBS += -lrte_port
 #   LIBS += -lrte_table
    LIBS += -lrte_cfgfile
    LIBS += -lrte_hash
    LIBS += -lrte_meter
    LIBS += -lrte_pmd_e1000
    LIBS += -lrte_pmd_ring
 #   LIBS += -lrte_power
    LIBS += -lrte_timer
    LIBS += -lrte_malloc
#    LIBS += -lrte_pmd_enic
    LIBS += -lrte_pmd_virtio_uio
    LIBS += -lrte_ring
    LIBS += -lrte_lpm
    LIBS += -lrte_ip_frag
    LIBS += -lrte_cmdline
  #  LIBS += -lrte_acl
    LIBS += -lrt
    LIBS += -lm -ldl
    LIBS +=  -lpthread
BIN_OBJECT = l2fwd

OBJ = main.o 



CFLAGS += -O3 -I$(WORK_DIR)/include -I$(ROOT_DIR)/include
CFLAGS += -include $(ROOT_DIR)/include/rte_config.h -msse4.2


all:$(OBJ)
	$(CC) $(CFLAGS)   -o $(BIN_OBJECT) $(OBJ)  -lrt -Wl,--whole-archive -L$(LIBDPDK) $(LIBS) -Wl,--no-whole-archive
	cp -rf $(BIN_OBJECT) $(BIN_DIR)

$(OBJ):%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@  -lrt -Wl,--whole-archive -L$(LIBDPDK)  $(LIBS) -Wl,--no-whole-archive

clean:
	rm -rf *.o
