log
====
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

