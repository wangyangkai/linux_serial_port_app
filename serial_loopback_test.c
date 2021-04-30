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

extern int serial_port_init(int argc, char *argv[]);

static int fd_serial_port;
static pthread_t tid_producer, tid_consumer;

static pthread_mutex_t thread_mutex;
static pthread_cond_t condc,condp;
static int condition = 0;

#define STR_LEN 1024
static unsigned int count = 0;
static char send_buf[STR_LEN];

void *thread_consumer(void *arg)
{
	int ret;
	char recv_buf[STR_LEN];
	int recv_len;
	int i;

	printf("%s() start thread\n", __func__);

	while(1) {
		pthread_mutex_lock(&thread_mutex);

		while (condition == 0)
			pthread_cond_wait(&condc, &thread_mutex);

		memset(recv_buf, 0, sizeof(recv_buf));
		recv_len = 0;
		ret = serial_port_receive(fd_serial_port, recv_buf, STR_LEN, &recv_len, 5000);

		if (recv_len >= 0) {
			if ((count % 100) == 0) {
				printf("rx_len:%d<-", recv_len);
				dumptxt_oneline(recv_buf, recv_len);
				printf("\n");
			}

			if (memcmp(send_buf, recv_buf, STR_LEN) != 0) {
				printf("mem cmp error, count:%d\n", count);

				printf("tx_len:%d->", STR_LEN);
				dumptxt_oneline(send_buf, STR_LEN);
				printf("\n");

				printf("rx_len:%d<-", recv_len);
				dumptxt_oneline(recv_buf, recv_len);
				printf("\n");

				exit(1);
			}
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

		count++;

		condition = 0;

		if (condition == 0)
			pthread_cond_signal(&condp);

		pthread_mutex_unlock(&thread_mutex);
	}

    return ((void *)0);
}

void rand_number_init(void)
{
	srand(time(NULL));
}

int rand_number(int from, int to)
{
	/* [from, to] select a rand number  */
	return (rand() % (from - to + 1)) + from;
}

char rand_visible_character(void)
{
	return (char)rand_number('!', '`');
}

void *thread_producer(void *arg)
{
	int ret;
	int i;

	printf("%s() start thread\n", __func__);
	
	rand_number_init();

	while(1) {
		pthread_mutex_lock(&thread_mutex);

		while (condition == 1)
			pthread_cond_wait(&condp, &thread_mutex);

		memset(send_buf, 0, sizeof(send_buf));
		for (i = 0; i < STR_LEN; i++)
			send_buf[i] = rand_visible_character();

		if ((count % 100) == 0) {
			printf("%s() count:%d\n", __func__, count);
			printf("tx_len:%d->", STR_LEN);
			dumptxt_oneline(send_buf, STR_LEN);
			printf("\n");
		}

		ret = serial_port_send(fd_serial_port, send_buf, STR_LEN);
		if (ret < 0) {
				printf("%s() error, serial_port_send() ret:%d, errno:%d,%s\n", 
					__func__, ret, errno, strerror(errno));
		}

		condition = 1;

		if (condition == 1)
			pthread_cond_signal(&condc);

		pthread_mutex_unlock(&thread_mutex);		
	}

	return ((void *)0);  
}

int main(int argc,char *argv[])
{
	fd_serial_port = serial_port_init(argc, argv);
	if (fd_serial_port < 0) {
		printf("%s() error, serial_port_init() ret:%d\n", __func__, fd_serial_port);
		return fd_serial_port;
	}

	pthread_mutex_init(&thread_mutex, 0);

	pthread_cond_init(&condc, 0);
	pthread_cond_init(&condp, 0);
	
	pthread_create(&tid_producer, NULL, thread_producer, NULL);
	usleep(10 * 1000);
	pthread_create(&tid_consumer, NULL, thread_consumer, NULL);

	pthread_join(tid_producer, 0);
	pthread_join(tid_consumer, 0);

	pthread_cond_destroy(&condc);
	pthread_cond_destroy(&condp);
	pthread_mutex_destroy(&thread_mutex);

	return 0;
}
