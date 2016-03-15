log
====
rte_eal_cpu_init
=================
该函数主要是解析/sys/devices/system/cpu文件，获取物理及逻辑核心，并填充cpu_info信息。
主要设置了全局变量rte_config的lcore_count lcore_role,该字段是当前机器所有逻辑核心数。
    设置了全局变量lcore_config的core cpu_set相关信息
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
     Note: physical package id != NUMA node, but we use it as a fallback for kernels which don't create a nodeY link
      如果获取的socketid大于RTE_MAX_NUMA_NODES，则根据RTE_EAL_ALLOW_INV_SOCKET_ID宏定义来觉得。当设置RTE_EAL_ALLOW_INV_SOCKET_ID时
      会lcore_config[lcore_id].socket_id = 0;否则退出程序，打印堆栈。

