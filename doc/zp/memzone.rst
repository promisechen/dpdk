
Memzone
=======

概述
----

memzone的示意图如下：

.. image:: _static/mem_struct.png

.. _memzone_init:

初始化
------

在EAL初始化过程中，先调用rte_eal_memory_init()进行了大页内存初始化，\
紧接着调用rte_eal_memzone_init()进行了memzone的初始化。

rte_eal_memzone_init (librte_eal/common/eal_common_memzone.c)

.. code-block:: c

    memseg = rte_eal_get_physmem_layout();

    rte_rwlock_write_lock(&mcfg->mlock);
    mcfg->memzone_cnt = 0;
    memset(mcfg->memzone, 0, sizeof(mcfg->memzone));
    rte_rwlock_write_unlock(&mcfg->mlock);

    return rte_eal_malloc_heap_init();

在rte_eal_malloc_heap_init中，把各个memory segment挂到malloc
heap链表中，以供之后使用。详见 :ref:`malloc_init`


内存分配
--------

核心函数是memzone_reserve_aligned_thread_unsafe(librte_eal/common\
/eal_common_memzone.c)。

内存分配通过malloc_heap_alloc完成，见 :ref:`malloc_heap_alloc` 。\
之后构造memzone结构体并返回。

.. code-block:: c

    void *mz_addr = malloc_heap_alloc(&mcfg->malloc_heaps[socket], NULL,
                requested_len, flags, align, bound);
    const struct malloc_elem *elem = malloc_elem_from_data(mz_addr);
    struct rte_memzone *mz = get_next_free_memzone();
    mcfg->memzone_cnt++;
    snprintf(mz->name, sizeof(mz->name), "%s", name);
    mz->phys_addr = rte_malloc_virt2phy(mz_addr);
    mz->addr = mz_addr;
    mz->len = (requested_len == 0 ? elem->size : requested_len);
    mz->hugepage_sz = elem->ms->hugepage_sz;
    mz->socket_id = elem->ms->socket_id;
    mz->flags = 0;
    mz->memseg_id = elem->ms - rte_eal_get_configuration()->mem_config->memseg;

    return mz;
    

内存释放
--------

rte_memzone_free(librte_eal/common/eal_common_memzone.c).

先求得memzone对应的idx，然后把该memzone结构体清0，最后释放堆内存。

.. code-block:: c

    idx = ((uintptr_t)mz - (uintptr_t)mcfg->memzone);
    idx = idx / sizeof(struct rte_memzone);
    addr = mcfg->memzone[idx].addr;

    memset(&mcfg->memzone[idx], 0, sizeof(mcfg->memzone[idx]));
    mcfg->memzone_cnt--;
    rte_free(addr);


参考
----

.. [dpdk_api_memzone] `memzone API <http://dpdk.org/doc/api/rte__memzone_8h.html>`_

