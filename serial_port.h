/* serial_port.h
 *
 * Author: Yangkai Wang 
 * wang_yangkai@163.com
 *
 * Coding in 2014/12/25
 */

#ifndef _SERIAL_PORT_H
#define _SERIAL_PORT_H

extern int serial_port_open(char *dev_path);
extern int serial_port_close(int fd);
extern int serial_port_send(int fd, char *send_data, int send_len);
extern int serial_port_receive(int fd, char *recv_data, int max_len, int *recv_len, int wait_time_ms);
extern int serial_port_set_ctrlflag(int fd, int baud, int databits, int stopbits, int parity);

#endif