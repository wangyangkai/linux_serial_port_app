/* serial_communication.c
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
#include <pthread.h>
#include <semaphore.h>

#define BUFFER_SIZE 8192

extern int serial_port_init(int argc, char *argv[]);

static int fd_serial_port;
static pthread_t tid_send, tid_receive;
/*static pthread_mutex_t printf_mutex;*/

void *thread_receive(void *arg)
{
	int ret;
	unsigned char recv_buf[BUFFER_SIZE];
	int recv_len;
	int i;

	printf("%s() start thread\n", __func__);

	while(1) {
		recv_len = 0;
		ret = serial_port_receive(fd_serial_port, recv_buf, sizeof(recv_buf), &recv_len, 500);

		if (recv_len > 0) {
				printf("rx:%d<-", recv_len);
				dumptxt(recv_buf, recv_len);
				printf("\n");
		}
		if (recv_len == 0) {
			/* do nothing */
		}

		if (ret < 0) {
			if (errno == EAGAIN) {
				/*printf("%s() error, serial_port_receive() ret:%d, errno:%d,%s\n", 
					__func__, ret, errno, strerror(errno));*/
			} else {
				printf("%s() error, serial_port_receive() ret:%d, errno:%d,%s\n", 
					__func__, ret, errno, strerror(errno));
				exit(1);
			}
		}
	}

	return ((void *)0);
}

void *thread_send(void *arg)
{
	int ret;
	int send_len;
	char send_buf[BUFFER_SIZE] = {0};

	printf("%s() start thread\n", __func__);

	while(1) {
		send_len = 0;

		while((send_buf[send_len++] = getchar()) != '\n');

		printf("tx:%d->", send_len);
		dumptxt(send_buf, send_len);
		printf("\n");

		ret = serial_port_send(fd_serial_port, send_buf, send_len);
		if (ret < 0)
			printf("%s() error, serial_port_send() ret:%d, errno:%d,%s\n", 
				__func__, ret, errno, strerror(errno));

		memset(send_buf, 0, send_len);
	}

    return ((void *)0);  
}

int main(int argc, char *argv[])
{
	fd_serial_port = serial_port_init(argc, argv);
	if (fd_serial_port < 0) {
		printf("%s() error, serial_port_init() ret:%d\n", __func__, fd_serial_port);
		return fd_serial_port;
	}

	pthread_create(&tid_send, NULL, thread_send, NULL);
	pthread_create(&tid_receive, NULL, thread_receive, NULL);

	pthread_join(tid_send, 0);
	pthread_join(tid_receive, 0);

	return 0;
}
