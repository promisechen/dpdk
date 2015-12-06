#杂记
1. 未绑定dpdk驱动时候，查看网卡是哪个cpu 
`cat /sys/class/net/eth0/device/local_cpulist`
2. 看哪个numa
`cat /sys/class/net/em2/device/numa_node`
 lscpu对应哪些cpu

3. DPDK默认是编译成静态库的，改成动态库
  只需要把config/common_linuxapp文件中CONFIG_RTE_BUILD_SHARED_LIB=n修改成CONFIG_RTE_BUILD_SHARED_LIB=y就行了。


4. 编译时，如果想使用 g++ 编译，可以指定 make 的 CC 参数：
    $ make CC=g++
    默认情况下，g++ 只认识 .cpp 扩展名，可以通过 CXX-suffix 指定自己的扩展名
    $ make CC=g++ CXX-suffix=cc
5. 查看网卡队列数
  ethtool -S eth0
  查看网卡是否支持checksum
  ethtool -k p8p1
  查看是哪张网卡，执行下面命令 eth0会不停闪烁
  ethtool -p eth0 10

6. 发送的时候,调用free和put
ixgbe_dev_tx_queue_setup注册->ixgbe_xmit_pkts-》A.ixgbe_xmit_pkts_simple-》tx_xmit_pkts-》ixgbe_tx_free_bufs->rte_mempool_put
B.ixgbe_xmit_pkts->rte_pktmbuf_free_seg  __rte_pktmbuf_prefree_seg->__rte_mbuf_raw_free->rte_mempool_put

7. 孤立核



   
 grubby --update-kernel=ALL --args="isolcpus=0,2,4,6,8,10,12,14,16,18,20,22,24,26,28,30,32,34,36,3811,13,15,17,19,31,33,35,37,39"
 
 grubby --update-kernel=ALL --remove-args="isolcpus"
 
 systemctl disable irqbalance.service
 
 systemctl stop irqbalance.service
 
 systemctl list-unit-files|grep irq
 
 isolcpus=1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39 hugepages=1024

   http://blog.csdn.net/maray/article/details/6123725
   关闭irqbalance（和多队列网卡绑定cpu有冲突） 
   service irqbalance stop 
   service irqbalance status   
   查看中断  mpstat -I SUM -P ALL 1 
8. 优化工具
    iostat mpstat numastat sar (yum install sysstat)
   top htop 
    cat /proc/interrupts
   ps -Leo pid,tid,args:30,psr,comm
##vtune
[命令行快速使用手册](https://software.intel.com/zh-cn/blogs/2010/11/10/amplxe-cl/)
amplxe-cl -collect hotspots ./bin
amplxe-cl -report hotspots -r r000hs 
