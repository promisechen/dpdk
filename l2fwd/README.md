#编译
make ;
修改Makefile 中的cflags 将-O3  改为-O0 -g进行调试
#配置
绑定对应端口

#运行
/bulid/l2fwd -c 3 -n 2 -- -p 3
