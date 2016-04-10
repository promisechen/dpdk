
http://www.cnblogs.com/MerlinJ/p/4074391.html

http://www.cnblogs.com/ding-linux-coder/p/4922583.html


log
====

样例
=================

相关的外部接口及变量
---------------------

函数调用
---------

主要接口描述
------------

rte_eal_cpu_init
=================
该函数主要是解析/sys/devices/system/cpu文件，获取物理及逻辑核心，并填充cpu_info信息。
主要设置了全局变量rte_config的lcore_count lcore_role,该字段是当前机器所有逻辑核心数。
    设置了全局变量lcore_config的core cpu_set相关信息
该函数将输出如下信息

"
EAL: Detected lcore 0 as core 0 on socket 0

EAL: Detected lcore 1 as core 0 on socket 0

EAL: Detected lcore 2 as core 0 on socket 0

EAL: Detected lcore 3 as core 0 on socket 0

EAL: Support maximum 128 logical core(s) by configuration.

EAL: Detected 4 lcore(s)
"

相关的外部接口及变量
---------------------
rte_config

lcore_config

eal_parse_sysfs_value

函数调用
---------
rte_eal_cpu_init

    cpu_detected

    cpu_core_id
    
    eal_cpu_socket_id

主要接口描述
------------
* cpu_detected(lcore_id)
    检测对应lcore_id是否存在
    读取/sys/devices/system/cpu/cpu%u/topology/core_id是否存在

* cpu_core_id(lcore_id)
    获取lcore_id对应core_id,既通过逻辑核心id获取物理核心id
    读取/sys/devices/system/cpu/cpu%u/topology/core_id得值，即物理核心id

* eal_cpu_socket_id 
    获取lcore_id对应得socket
    首先从/sys/devices/system/cpu/cpu%u/读取是否包含nodeX的目录，如果读取到则X即socketid
    若不包含，则从/sys/devices/system/cpu/cpu％u/topology/physical_package_id文件中获取,以替代socketid
     Note: physical package id != NUMA nmde, but we use it as a fallback for kernels which don't create a nodeY link

     如果获取的socketid大于RTE_MAX_NUMA_NODES，则根据RTE_EAL_ALLOW_INV_SOCKET_ID宏定义来觉得。当设置RTE_EAL_ALLOW_INV_SOCKET_ID时
      会lcore_config[lcore_id].socket_id = 0;否则退出程序，打印堆栈。
eal_parse_args
===============
 ::

 EAL common options:
  -c COREMASK         Hexadecimal bitmask of cores to run on
  -l CORELIST         List of cores to run on
                      The argument format is <c1>[-c2][,c3[-c4],...]
                      where c1, c2, etc are core indexes between 0 and 128
  --lcores COREMAP    Map lcore set to physical cpu set
                      The argument format is
                            '<lcores[@cpus]>[<,lcores[@cpus]>...]'
                      lcores and cpus list are grouped by '(' and ')'
                      Within the group, '-' is used for range separator,
                      ',' is used for single number separator.
                      '( )' can be omitted for single element group,
                      '@' can be omitted if cpus and lcores have the same value
  --master-lcore ID   Core ID that is used as master
  -n CHANNELS         Number of memory channels
  -m MB               Memory to allocate (see also --socket-mem)
  -r RANKS            Force number of memory ranks (don't detect)
  -b, --pci-blacklist Add a PCI device in black list.
                      Prevent EAL from using this PCI device. The argument
                      format is <domain:bus:devid.func>.
  -w, --pci-whitelist Add a PCI device in white list.
                      Only use the specified PCI devices. The argument format
                      is <[domain:]bus:devid.func>. This option can be present
                      several times (once per device).
                      [NOTE: PCI whitelist cannot be used with -b option]
  --vdev              Add a virtual device.
                      The argument format is <driver><id>[,key=val,...]
                      (ex: --vdev=eth_pcap0,iface=eth2).
  -d LIB.so|DIR       Add a driver or driver directory
                      (can be used multiple times)
  --vmware-tsc-map    Use VMware TSC map instead of native RDTSC
  --proc-type         Type of this process (primary|secondary|auto)
  --syslog            Set syslog facility
  --log-level         Set default log level
  -v                  Display version information on startup
  -h, --help          This help

    EAL options for DEBUG use only:
  --huge-unlink       Unlink hugepage files after init
  --no-huge           Use malloc instead of hugetlbfs
  --no-pci            Disable PCI
  --no-hpet           Disable HPET
  --no-shconf         No shared config (mmap'd files)

    EAL Linux options:
  --socket-mem        Memory to allocate on sockets (comma separated values)
  --huge-dir          Directory where hugetlbfs is mounted
  --file-prefix       Prefix for hugepage filenames
  --base-virtaddr     Base virtual address
  --create-uio-dev    Create /dev/uioX (usually done by hotplug)
  --vfio-intr         Interrupt mode for VFIO (legacy|msi|msix)
  --xen-dom0          Support running on Xen dom0 without hugetlbfs


相关的外部接口和变量
---------------------
函数调用
--------
    eal_reset_internal_config(&internal_config);//初始化默认参数
主要接口描述
------------
*   eal_parse_coremask:解析-c 参数，并会修改rte_config及lcore_config中lcore对应的计数、flag等
*   eal_parse_corelist:解析-l 与-c效果相同;可以同时添加-c -l,但是会取后面的那个选项的配置。
*   eal_parse_lcores :解析--lcore,重新设置lcore绑定的cpu. 

    -c指定的核心，必须都重新设定，该函数首先会lcore_config[idx].core_index = -1;将所有
    核心对应设置为无效。
    参考下面的注释，以“,”隔开。
    如1 表示1号lcore_id设置不变，还是对应1号核心
    7-8表示lcore_id7 8仍对应7 8核心
    1@2 表示将lcore_id1绑定到2号核心
    1@(2,3)表示将1号核心绑定到2 3核心
    (0,6) 表示0和6号核心为一个组？？
    注意:－表示范围
    
    /*
     * The format pattern: --lcores='<lcores[@cpus]>[<,lcores[@cpus]>...]'
     * lcores, cpus could be a single digit/range or a group.
     * '(' and ')' are necessary if it's a group.
     * If not supply '@cpus', the value of cpus uses the same as lcores.
     * e.g. '1,2@(5-7),(3-5)@(0,2),(0,6),7-8' means start 9 EAL thread as below
     *   lcore 0 runs on cpuset 0x41 (cpu 0,6)
     *   lcore 1 runs on cpuset 0x2 (cpu 1)
     *   lcore 2 runs on cpuset 0xe0 (cpu 5,6,7)
     *   lcore 3,4,5 runs on cpuset 0x5 (cpu 0,2)
     *   lcore 6 runs on cpuset 0x41 (cpu 0,6)
     *   lcore 7 runs on cpuset 0x80 (cpu 7)
     *   lcore 8 runs on cpuset 0x100 (cpu 8)
     */

*  rte_eal_devargs_add:解析-b -c --dev ,将调用该函数。
     --dev:添加虚拟驱动
     --w:  将只会加载-w指定的网卡，只通过setup.sh脚步配置的网卡时不会加载的。 通过查看变量rte_eth_devices得出的结论。
     --b: 指定网卡加入黑名单，即被指定网卡不会被加载。 
    
    该函数逻辑：创建rte_devargs-> 解析参数->将创建的rte_devargs挂在devargs_list链表上。
    rte_devargs结构体储存网卡设备类型（黑名单，白名单，虚拟驱动）->设备对应的设备的pci编号或驱动类类型（虚拟驱动有eth_pcap,if之类）
* eal_parse_proc_type
    默认程序时RTE_PROC_PRIMARY
* 其他
    其他参数大多存在来internal_config全局变量中

eal_hugepage_info_init 
========================

只有在未设置no_hugetlbfs并且未设置xen的支持且为主进程时，才会调用该函数。

填充internal_config.hugepage_info［］信息，该数组最大为4

函数执行流程: 

* 遍历/sys/kernel/mm/hugepages目录下所有以hugepages-开头的文件，但只能取前3个。

* 获取该大页的大小，如hugepages-2048kB则大页大小为2MB

* 获取大页路径,并使用flock设置写锁

* 晴空大页路径下的*map_*的文件，如果没有被其他dpdk进程运行

* 获取大页个数

相关的外部接口及变量
---------------------

函数调用
---------

rte_str_to_size 获取大页大小

get_hugepage_dir 获取大页的路径

clear_hugedir 清空大页相关文件如果没有被其他dpdk进程运行

get_num_hugepages 获取大页个数

主要接口描述
------------
* get_hugepage_dir: 
   :: 

     先调用get_default_hp_size获取默认页面大小
     读取 /proc/mounts |grep hugetlbfs ，如果在选项字段包含pagesize=字段，则获取该值为pagesize,并与入参比较，确定大页目录
      如果选项字段不包含pageseze=字段，则以默认页面大小与入参比较，确定大页目录。
      所以返回的目录会又随机型，大部分系统是这样返回的
      [root@vmware hugepages]# cat /proc/mounts |grep hugetlbfs
      hugetlbfs /dev/hugepages hugetlbfs rw,relatime 0 0
      nodev /mnt/huge hugetlbfs rw,relatime 0 0
      那么对此种配置，则会选取靠前面的挂载点作为大页默认目录
      另外，如果使用--huge-dir显示的设置internal_config.hugepage_dir,则会以此目录作为大页路径

* get_default_hp_size:获取大页默认大小，从cat /proc/meminfo | grep Hugepagesize中读取。

* get_num_hugepages: 获取大页个数，从/sys/kernel/mm/hugepages/hugepages-xxx/中获取，free_hugepages－resv_hugepages即为所求值

rte_config_init
=================
初始化rte_config.mem_config，并保证主从进程的虚拟地址相同


* 如果是主进程，则调用rte_eal_config_create，默认创建/var/run/.rte_config文件，调用mmap获取sizeof(struct rte_mem_config)大小的虚拟内存。并

   将共享内存的基址存到共享内存中，供子进程使用，从而保证主次进程映射的基址相同。
  参见rte_eal_config.h 中的struct rte_mem_config结构体

* 如果是从进程则会先获取先调用mmap,获取主进程设置的rte_config.mem_cfg_addr(主进程映射的地址空间)，
  从新调用mmap(使用祝进程的虚拟地址)，从而保证主从进程虚拟地址相同。
  注意:从进程将一直等待主进程(rte_eal_mcfg_complete完成mem配置)，才会从新调用rte_eal_config_reattach()
    rte_config_init(void)
    {
    	rte_config.process_type = internal_config.process_type;
    
    	switch (rte_config.process_type){
    	case RTE_PROC_PRIMARY:
    		rte_eal_config_create();
    		break;
    	case RTE_PROC_SECONDARY:
    		rte_eal_config_attach();
    		rte_eal_mcfg_wait_complete(rte_config.mem_config);
    		rte_eal_config_reattach();
    		break;
    	case RTE_PROC_AUTO:
    	case RTE_PROC_INVALID:
    		rte_panic("Invalid process type\n");
    	}
    }



相关的外部接口及变量
---------------------

rte_config

函数调用
---------

主要接口描述
------------

* rte_eal_config_create(主进程调用) 首先调用eal_runtime_config_path 获取rte_config的文件路径
  
  如果设置--no-shconf 则直接return
  
  调用ftruncate fcnt设置.rte_config文件大小，锁定文件等。
  
  调用mmap获取rte_mem_config大小的内存，并将共享内存地址存到共享内存rte_config.mem_cfg_addr中

* eal_runtime_config_path: 如果是root用户则会返回默认的/var/run/.rte_config(注意.rte_config 可以根据--file-prefix进行修改)

* rte_eal_config_attach(从进程调用) 首先调用eal_runtime_config_path 获取rte_config的文件路径
   
  如果设置--no-shconf 则直接return

  调用mmap获取内存内存基址,并将该地址存到rte_config.mem_config中。

rte_eal_mcfg_wait_complete:等待主进程rte_eal_mcfg_complete完成内存配置


* rte_eal_config_reattach(从进程调用) 
  
  读取rte_config.mem_cfg_addr(主进程存的虚拟地址)。并使用该地址从新调用mmap，从而保证进程间虚拟地址相同。


rte_eal_pci_init(todo)
====================
相关的外部接口及变量
---------------------

函数调用
---------

主要接口描述
------------

rte_eal_memory_init
====================

主进程调用rte_eal_hugepage_init，子进程调用rte_eal_hugepage_attach
相关的外部接口及变量
---------------------

函数调用
---------

主要接口描述
------------
rte_eal_hugepage_init
=======================

lib/librte_eal/linuxapp/eal/eal_memory.c

*  1. map N huge pages in separate files in hugetlbfs
*  2. find associated physical addr
*  3. find associated NUMA socket ID
*  4. sort all huge pages by physical address
*  5. remap these N huge pages in the correct order
*  6. unmap the first mapping
*  7. fill memsegs in configuration with contiguous zones
    这个时候可以正确设置num_pages了internal_config.hugepage_info[j].num_pages[socket]++;
    将大页内存信息存入/var/run/.rte_hugepage_info的共享内存
若干个页根据是否连续，是否同一个socket，是否相同页尺寸等，\
   分成最多RTE_MAX_MEMSEG(默认256)个内存段(memory segment)：

.. code-block:: c

 if (new_memseg) {

 j += 1;
 if (j == RTE_MAX_MEMSEG)
 break;

 mcfg->memseg[j].phys_addr = hugepage[i].physaddr;
 mcfg->memseg[j].addr = hugepage[i].final_va;
 mcfg->memseg[j].len = hugepage[i].size;
 mcfg->memseg[j].socket_id = hugepage[i].socket_id;
 mcfg->memseg[j].hugepage_sz = hugepage[i].size;
 }
 // continuation of previous memseg
 else
 mcfg->memseg[j].len += mcfg->memseg[j].hugepage_sz;

 hugepage[i].memseg_id = j;

 }

相关的外部接口及变量
---------------------

函数调用
---------

主要接口描述
------------
*  map_all_hugepages(struct hugepage_file *hugepg_tbl,struct hugepage_info *hpi, int orig) 
  
    循环hpi->num_pages[0]遍历，比如设置512个内存大页面，则会创建512个rtemap_xxx 个文件。
   
    eal_get_hugefile_path将返回rte_mapxxx文件名称，放到hugepg_tbl[i].filepath中。
    
    调用open mmap分配hugepage_sz大小的内存virtadd。
    
    vma_addr有讲究(其实这段是为第二次重新分配内存设计的逻辑)：
    当设置RTE_EAL_SINGLE_FILE_SEGMENTS时，将调用get_virtual_area获取虚拟地址。
    
    当为设置时的逻辑是：rtemap_0是通过mmap获取的，而 rtemap_1---rtemap_n是根据virtaddr逐步加hugepage_sz，
    
    然后作为参数传给mmap,但是当该虚拟地址被使用则会重新分配一个地址。不知道dpdk的用意？？。
    
    如果orig==1,则hugepg_tbl[i].orig_va = virtadd，否则hugepg_tbl[i].final_va = virtaddr;
    
    调用flock锁定rtemap_xxx文件
    
    vma_addr = (char *)vma_addr + hugepage_sz;

    第二次重新mmap的逻辑如下：
   
    首先从当前i处开始找物理连续的内存页个数n，然后调用get_virtual_area获取足够的虚拟地址。get_virtual_area将会尽最大努力获取到
   
    [1,n)个大小的虚拟地址空间，然后把vma_len付为获取到的最大虚拟内存块。
   
    另一个代码逻辑（我感觉这个逻辑没用）：如果该虚拟地址不够则会将vma_len设置成
   
    hugepage_sz, 然后会在努力从i+1处，继续找到n-1个连续内存块，然后继续调用get_virtual_area获取足够的虚拟内存块。
   
    vma_len设计也是有作者的自己的思想的，vma_len是由物理连续块个数及虚拟地址区域决定的，当无法获取足够大的虚拟内存区域时，直接将
   
    vma_len设置成一块，在vma_addr = (char *)vma_addr + hugepage_sz;vma_len -= hugepage_sz;执行的时候不会出错。同时下面的核心代码，也只有
   
    vma_len被减成0时，才需要在重新调用get_virtual_area获取最大虚拟内存块的。
   
    核心代码
  
.. code-block:: c

 for (j = i+1; j < hpi->num_pages[0] ; j++) {
 #ifdef RTE_ARCH_PPC_64
 /* The physical addresses are sorted in
 * descending order on PPC64 */
   if (hugepg_tbl[j].physaddr !=
   hugepg_tbl[j-1].physaddr - hugepage_sz)
   break;
   #else
   if (hugepg_tbl[j].physaddr !=
   hugepg_tbl[j-1].physaddr + hugepage_sz)
   break;
   #endif
   }
   num_pages = j - i;
   vma_len = num_pages * hugepage_sz;

 /* get the biggest virtual memory area up to
 * vma_len. If it fails, vma_addr is NULL, so
   * let the kernel provide the address. */
     vma_addr = get_virtual_area(&vma_len, hpi->hugepage_sz);
     if (vma_addr == NULL)
     vma_len = hugepage_sz;
     }


* find_physaddrs 获取所有共享内存的物理地址，其实都是调用rte_mem_virt2phy实现的。

  rte_mem_virt2phy 根据虚拟地址转换成物理地址。从/proc/self/pagemap读取相关page信息.总体思想是获取page,根据page加上页内偏移算出物理地址。
  
  具体参考：https://shanetully.com/2014/12/translating-virtual-addresses-to-physcial-addresses-in-user-space/

* find_numasocket 获取虚拟内存对应的socketid；从/proc/self/numa_maps读取出现huge或者internal_config.hugefile_prefix字符的行,类似

  “01e00000 prefer:0 file=/dev/hugepages/rtemap_15 huge dirty=1 N0=1" 其中01e00000是虚拟地址，NO表示：N代表numa,0代表是socketid等于0
* sort_by_physaddr 根据物理内存排序

* get_virtual_area(size_t *size, size_t hugepage_sz) 获取虚拟地址空间.有两点：1. 使用mmap分配size+hugepage_sz大小空间 2.如果分配不出来减去hugepage_sz

  在分配，直至分配出来为止。并修改size值，把他传给调用者。3,munmap掉刚分配出的内存。4.按照hugepage_sz大小对其，并返回对其后的地址（在调用mmap时故意多加来一个页面大小）

下面初始化就是该函数打印的，总共分512个大页，共5段连续内存块。

.. code-block:: c

EAL: Ask a virtual area of 0x200000 bytes
EAL: Virtual area found at 0x7ffff6c00000 (size = 0x200000)
EAL: Ask a virtual area of 0x3f800000 bytes
EAL: Virtual area found at 0x7fffb7200000 (size = 0x3f800000)
EAL: Ask a virtual area of 0x200000 bytes
EAL: Virtual area found at 0x7fffb6e00000 (size = 0x200000)
EAL: Ask a virtual area of 0x200000 bytes
EAL: Virtual area found at 0x7fffb6a00000 (size = 0x200000)
EAL: Ask a virtual area of 0x200000 bytes
EAL: Virtual area found at 0x7fffb6600000 (size = 0x200000)
EAL: Requesting 512 pages of size 2MB from socket 0

* unmap_all_hugepages_orig 调用munmap将第一次mmap的大页(hugepg_tbl[i].orig_va)释放掉。

* calc_num_pages_per_socket 计算每个socket的页面数，应该与--socket-mem有关。（我感觉会根据某种策略来选择保留的大页面。todo）

* unmap_unneeded_hugepages 释放不用的大页面内存。感觉这个函数与calc_num_pages_per_socket有很大关系。
  }

* unlink_hugepage_files 如果设置来－－huge-unlink，则会调用该函数，Unlink hugepage files after init。

* create_shared_memory copy_hugepages_to_shared_mem: 使用/var/run/.rte_hugepage_info 调用nmap创建共享内存，将大页信息纪录到共享内存中。



