/* serial_port_init.c
 *
 * Author: Yangkai Wang 
 * wang_yangkai@163.com
 *
 * Coding in 2014/12/25
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

extern int serial_port_init(int argc, char *argv[])
{
	int ret;
	int fd;
	int baud_rate, databits, stopbits, flow_control;
	char parity;

	if (argc != 3 ) {
		printf("%s() parameter error\n", __func__);
		printf("%s [/dev/tty*] [\"115200 8 n 1 f0\"]\n", argv[0]);
		printf("example:\n%s /dev/ttySAC2 \"115200 8 n 1 f0\"\n", argv[0]);
		return -1;
	}

	ret = sscanf(argv[2], "%d %d %c %d f%d", &baud_rate, &databits, &parity, &stopbits, &flow_control);
	if (ret != 4)
		printf("%s() parameter error\n", __func__);

	printf("%s() tty device file:%s, baud rate:%d, databits:%d, stopbits:%d, parity:%c, flow_control:%d\n",
		__func__, argv[1], baud_rate, databits, stopbits, parity, flow_control);

	fd = serial_port_open(argv[1]);
	if (fd < 0) {
		printf("%s() error, serial port open, ret:%d, errno:%d,%s\n", __func__, fd, errno, strerror(errno));
		return fd;
	} else {
		printf("%s() open %s success, fd:%d\n", __func__, argv[1], fd);
	}

	ret = serial_port_set_ctrlflag(fd, baud_rate, databits, stopbits, parity, flow_control);
	if (ret != 0) {
		printf("%s() configure %s serial port fail, ret:%d\n", __func__, argv[1], ret);
		return ret;
	} else {
		printf("%s() configure %s serial port success\n", __func__, argv[1]);
	}

	return fd;
}
