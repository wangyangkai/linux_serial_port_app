# linux_serial_port_app Makefile
#
# Author: Yangkai Wang 
# wang_yangkai@163.com
#
# Coding in 2014/12/25
#

CC:=/opt/toolchain/arm-cortex_a8-linux-gnueabi-4.9.3/bin/arm-cortex_a8-linux-gnueabi-gcc

COMMON_SRC:=serial_port_init.c serial_port.c common.c

all:
	$(CC) -o serial_loopback_test -lpthread serial_loopback_test.c $(COMMON_SRC)
	$(CC) -o serial_communication -lpthread serial_communication.c $(COMMON_SRC)
	$(CC) -o serial_loop_send -lpthread serial_loop_send.c $(COMMON_SRC)

clean:
	rm serial_loopback_test serial_communication serial_loop_send
