Table of Contents
_________________

1 dpdk计算校验和功能
.. 1.1 运行命令
.. 1.2 计算 IP 头的校验和需要的几个步骤
.. 1.3 计算 TCP 头的校验和需要的几个步骤


1 dpdk计算校验和功能
====================

1.1 运行命令
~~~~~~~~~~~~
 仅供参考
 vm: ./build/l2fwd -c 3 -n 2 -- -p 3

13.31: ./build/l2fwd -c 1 -n 2 -- -p 1

1.2 计算 IP 头的校验和需要的几个步骤
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  ip头的各个字段填写正确。

  把ip头的 checksum 字段填写成0
  ipv4_hdr->hdr_checksum = 0;

  填写以下几个 mbuf 的结构体字段
  m->ol_flags |= PKT_TX_IPV4;
  m->ol_flags |= PKT_TX_IP_CKSUM;
  m->l2_len = sizeof(struct ether_hdr);
  m->l3_len =  sizeof(struct ipv4_hdr);

  这样，dpdk在发送这个mbuf中的数据包时，就会调用网卡计算ip的校验和了。
  
1.3 计算 TCP 头的校验和需要的几个步骤
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  网卡接收队列的 conf 选项中的标记需要清零static const struct
  rte_eth_txconf tx_conf = { .txq_flags = 0, };

  设置网卡的接收队列，最后一个参数就是以上提到的 tx_conf
  rte_eth_tx_queue_setup(portid, 0, nb_txd, rte_eth_dev_socket_id(portid), &tx_conf);

  tcp 头各个字段填写正确

  把 tcp 伪头部的校验和填写进 checksum 字段。
  关于tcp伪头的校验和计算可以参考rst例子程序中的get_psd_sum()函数
  ol_flags |= PKT_TX_TCP_CKSUM;
  ethertype =  _htons(ETHER_TYPE_IPv4);
  tcp_hdr->cksum = get_psd_sum((char *)(ipv4_hdr), ethertype, ol_flags);

  设置 mbuf 中 tcp校验和相关的字段值
  m->ol_flags |= PKT_TX_TCP_CKSUM;
  m->l4_len = (tcp_hdr->data_off & 0xf0) >> 2;

  这样，dpdk在发送这个mbuf中的数据包时，就会调用网卡计算tcp的校验和了。

