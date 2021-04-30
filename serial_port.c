/* serial_port.c
 *
 * Author: Yangkai Wang 
 * wang_yangkai@163.com
 *
 * Coding in 2014/12/25
 */

#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>

extern int serial_port_open(char *dev_path)
{
	int fd;

	fd = open(dev_path, O_RDWR | O_NOCTTY | O_NDELAY);
	if (fd < 0) {
		printf("%s() error, open() ret:%d, errno:%d,%s\n", __func__, fd, errno, strerror(errno));
		return fd;
	}

	return fd;
}

extern int serial_port_close(int fd)
{
	int ret;

	ret = close(fd);
	if (ret < 0)
		printf("%s() error, close() ret:%d, errno:%d,%s\n", __func__, ret, errno, strerror(errno));

	return ret;
}

extern int serial_port_send(int fd, char *send_data, int send_len)
{
	int ret;
	int remain = send_len;

	while (1) {
		ret = write(fd, send_data + (send_len - remain), remain);
		if (ret > 0)
			remain -= ret;

		if (remain == 0)
			return send_len - remain;
		else
			usleep(100 * 1000);

		if (ret < 0) {
			printf("%s() error, write() ret:%d, errno:%d,%s\n", __func__, ret, errno, strerror(errno));

			if (remain != 0)
				printf("%s() error, remain:%d\n", __func__, remain);
			return ret;
		}
	}

    return ret;
}

extern int serial_port_receive(int fd, char *recv_data, int max_len, int *recv_len, int wait_time_ms)
{
	int ret;
	int remain;

	remain = max_len;

	while (1) {
		ret = read(fd, recv_data + max_len - remain, remain);
		if (ret > 0) {
			remain -= ret;
			if (remain == 0) {
				*recv_len = max_len;
				return 0;
 			} else {
 				continue ;
			}
		} else if (ret < 0) {
			if (errno == EAGAIN) {
				/*printf("%s() error:EAGAIN, ret:%d\n", __func__, ret);*/
			} else {
				*recv_len = max_len - remain;
				printf("%s() error, read() ret:%d, errno:%d,%s\n", __func__, ret, errno, strerror(errno));
				return ret;
			}
        	}

		if (wait_time_ms == 0) {
			*recv_len = max_len - remain;
			/*printf("%s() time out, ret:%d\n", __func__, ret);
			return -1;*/
			return ret;
		}
		usleep(1000);
		wait_time_ms--;

		/*if ((wait_time_ms % 1000) == 0) {
			printf("%s() wait_time_ms:%d\n", __func__, wait_time_ms);
		}*/
	}
}

extern int serial_port_set_ctrlflag(int fd, int baud_rate, int databits, int stopbits, char parity, int flow_control)
{
	int ret;
	struct termios opt;

	printf("%s() setting baud_rate:%d, databits:%dbit, stopbits:%dbit, parity:%c, flow_control:%d\n", 
		__func__, baud_rate, databits, stopbits, parity, flow_control);

	ret = tcgetattr(fd, &opt);
	if (ret != 0)
		return ret;
	
	switch (baud_rate) {
	case 1200:
		cfsetispeed(&opt, B1200);
		cfsetospeed(&opt, B1200);
		break;
	case 2400:
		cfsetispeed(&opt, B2400);
		cfsetospeed(&opt, B2400);
		break;
	case 4800:
		cfsetispeed(&opt, B4800);
		cfsetospeed(&opt, B4800);
		break;
	case 9600:
		cfsetispeed(&opt, B9600);
		cfsetospeed(&opt, B9600);
		break;
	case 19200:
		cfsetispeed(&opt, B19200);
		cfsetospeed(&opt, B19200);
		break;
	case 38400:
		cfsetispeed(&opt, B38400);
		cfsetospeed(&opt, B38400);
		break;
	case 57600:
		cfsetispeed(&opt, B57600);
		cfsetospeed(&opt, B57600);
		break;
	case 115200:
		cfsetispeed(&opt, B115200);
		cfsetospeed(&opt, B115200);
		break;
	case 921600:
		cfsetispeed(&opt, B921600);
		cfsetospeed(&opt, B921600);
		break;
	case 1500000:
		cfsetispeed(&opt, B1500000);
		cfsetospeed(&opt, B1500000);
	case 2000000:
		cfsetispeed(&opt, B2000000);
		cfsetospeed(&opt, B2000000);
		break;
	case 3000000:
		cfsetispeed(&opt, B3000000);
		cfsetospeed(&opt, B3000000);
		break;
	case 4000000:
		cfsetispeed(&opt, B4000000);
		cfsetospeed(&opt, B4000000);
		break;
	default:
		printf("%s() error, app does not support baud_rate\n", __func__);
		return -1;
	}

	/*8N1*/
	opt.c_cflag &= ~CSIZE; /* Mask the character size bits */
	switch (databits) {
	case 7:
		opt.c_cflag |= CS7;
		break;
	case 8:
		opt.c_cflag |= CS8;
		break;
	default:
		printf("%s() error, databits no support\n", __func__);
		return -1;
	}

	/*set stop bits*/
	switch (stopbits) {
	case 1:
		opt.c_cflag &= ~CSTOPB;
		break;
	case 2:
		opt.c_cflag |= CSTOPB;
		break;
	default:
		printf("%s() error, stopbits no support\n", __func__);
		return -1;
	}

	/*set parity*/
	switch (parity){
	case 'n':
	case 'N':
		opt.c_cflag &= ~PARENB; /* Clear parity enable */
		opt.c_iflag &= ~INPCK; /* Enable parity checking */
		printf("%s() no parity\n", __func__);
		break;
	case 'e':
	case 'E':
		opt.c_cflag |= PARENB; /* Enable parity */
		opt.c_cflag &= ~PARODD; /* Set even checking*/  
		opt.c_iflag |= INPCK; /* Disnable parity checking */
		printf("%s() even parity\n", __func__);
		break;
	case 'o':
	case 'O':
		opt.c_cflag |= PARENB; /* Enable parity */
		opt.c_cflag |= PARODD; /* Set odd checking*/ 
		opt.c_iflag |= INPCK; /* Disnable parity checking */
		printf("%s() odd parity\n", __func__);
		break;
	default:
		printf("%s() error, parity no support\n", __func__);
		break;
	}

	/* Set input parity option */
	if (parity != 'n')
		opt.c_iflag |= INPCK;

	opt.c_cc[VTIME] = 150; /*15 seconds*/
	opt.c_cc[VMIN] = 0;

	if (flow_control == 0) {
		opt.c_cflag &= ~CRTSCTS; /*disable hardware flow control*/
		printf("%s() disable hardware flow control\n", __func__);
	} else if (flow_control == 1) {
		opt.c_cflag |= CRTSCTS; /*enable hardware flow control*/
		printf("%s() enable hardware flow control\n", __func__);
	} else {
		opt.c_cflag &= ~CRTSCTS; /*disable hardware flow control*/
		printf("%s() disable hardware flow control\n", __func__);		
	}
	opt.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);/*raw input*/
	opt.c_oflag  &= ~OPOST; /*raw output*/

	tcflush(fd, TCIFLUSH); /* Update the options and do it NOW */

	ret = tcsetattr(fd, TCSANOW, &opt);
	if (ret != 0) {
		printf("%s() failed\n", __func__);
        	return ret;
	}

	return 0;
}
