
#include "server.h"
#include "client.h"
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#include "sys.h"
#include "crc.h"
#include "plc_main.h"
#include <sys/socket.h>
#include <sys/msg.h>
#include <arpa/inet.h>
#include <stdlib.h>


int wait_flag = 0;
char modbus_sockt_state[MAX_CLIENT_CNT];
int modbus_client_sockptr[MAX_CLIENT_CNT];
unsigned int modbus_sockt_timer[MAX_CLIENT_CNT];
MyData clent_data_temp[MAX_CLIENT_CNT];
int g_comm_qmegid[MAX_CLIENT_CNT];

PARA_PLC Para_Plc;
PARA_PLC *pPara_plc = (PARA_PLC *)&Para_Plc;

int myprintbuf(int len, unsigned char *buf)
{
	int i = 0;
	printf("\nbuflen=%d\n", len);
	for (i = 0; i < len; i++)
		printf("0x%x ", buf[i]);
	printf("\n");
	return 0;
}
int AnalysModbus1(unsigned char *datain, unsigned short len, unsigned char *dataout)
{
	return 0;
}
void *_clientSend_thread(void *arg) // 25
{
	int id_thread = (int)arg;

	int ret_value = 0;
	msgClient pmsg;
	MyData pcsdata;
	int waittime = 0;
	int id_frame;

	printf("PCS[%d] Modbus_clientSend_thread  is Starting!\n", id_thread);

	key_t key = 0;
	g_comm_qmegid[id_thread] = os_create_msgqueue(&key, 1);

	// unsigned char code_fun[] = {0x03, 0x06, 0x10};
	// unsigned char errid_fun[] = {0x83, 0x86, 0x90};

	while (modbus_sockt_state[id_thread] == STATUS_OFF)
	{
		usleep(10000);
	}

	wait_flag = 0;

	// printf("modbus_sockt_state[id_thread] == STATUS_ON\n") ;
	while (modbus_sockt_state[id_thread] == STATUS_ON) //
	{
		ret_value = os_rev_msgqueue(g_comm_qmegid[id_thread], &pmsg, sizeof(msgClient), 0, 100);
		if (ret_value >= 0)
		{
	// 		memcpy((char *)&pcsdata, pmsg.data, sizeof(MyData));
	// 		id_frame = pcsdata.buf[0] * 256 + pcsdata.buf[1];

	// 		if ((id_frame != 0xffff && (g_num_frame - 1) == id_frame) || (id_frame == 0xffff && g_num_frame == 1))
	// 		{
	// 			printf("recv form pcs!!!!!g_num_frame=%d  id_frame=%d\n", g_num_frame, id_frame);
	// //			AnalysModbus(id_thread, pcsdata.buf, pcsdata.len);
	// 		}
	// 		else
	// 			printf("检查是否发生丢包现象！！！！！g_num_frame=%d  id_frame=%d\n", g_num_frame, id_frame);
			wait_flag = 0;
			continue;
		}
		else if (wait_flag == 1)
		{
			waittime++;
			if (waittime == 1000)
			{
				wait_flag = 0;
				waittime = 0;
			}
			continue;
		}
		if (wait_flag == 0)
			continue;

	//	RunAccordingtoStatus(id_thread);
		// usleep(100);
	}
	return NULL;
}

static int recvFrame(int fd, int qid, MyData *recvbuf)
{
	int readlen;

	// int index = 0, length = 0;
	//  int i;
	msgClient msg;
	// MyData *precv = (MyData *)&msg.data;
	readlen = recv(fd, recvbuf->buf, MAX_FLAME_LEN, 0);
	//		readlen = recv(fd, (recvbuf.buf + recvbuf.len),
	//				(MAX_BUF_SIZE - recvbuf.len), 0);
	//		printf("*****  F:%s L:%d recv readlen=%d\n", __FUNCTION__, __LINE__,	readlen);
	if (readlen < 0)
	{
		printf("连接断开或异常\r\n");
		return -1;
	}
	else if (readlen == 0)
		return 1;

	printf("收到一包数据 wait_flag=%d", wait_flag);
	recvbuf->len = readlen;
	myprintbuf(readlen, recvbuf->buf);
	msg.msgtype = 1;
	memcpy((char *)&msg.data, recvbuf->buf, readlen);
	sleep(1);
	if (msgsnd(qid, &msg, sizeof(msgClient), IPC_NOWAIT) != -1)
	{

		printf("succ succ succ succ !!!!!!!"); //连接主站的网络参数I
		return 0;
	}
	else
	{
		return 1;
	}

	// for(i=0;i<readlen;i++)
	// 	printf("0x%2x ",recvbuf->buf[i]);
	// printf("\n");
}
void *_clientRecv_thread(void *arg) // 25
{
	int id_thread = (int)arg;
	int fd = -1;
	fd_set maxFd;
	struct timeval tv;
	MyData recvbuf;
	int i = 0, jj = 0;
	int ret;

loop:
	while (1)
	{
		if (modbus_sockt_state[id_thread] == STATUS_ON)
		{
			break;
		}
		usleep(10);
	}
	printf("客户端连接服务器成功！！！！\n");

	while (1)
	{
		fd = modbus_client_sockptr[id_thread];
		if (fd == -1)
			break;
		FD_ZERO(&maxFd);
		FD_SET(fd, &maxFd);
		tv.tv_sec = 0;
		//    tv.tv_usec = 50000;
		tv.tv_usec = 200000;
		ret = select(fd + 1, &maxFd, NULL, NULL, &tv);
		if (ret < 0)
		{

			printf("网络有问题！！！！！！！！！！！！");
			break;
		}
		else if (ret == 0)
		{
			jj++;

			if (jj > 1000)
			{
				printf("暂时没有数据传入！！！！未接收到数据次数=%d！！！！！！！！！！！！！！！！\r\n", jj);
				jj = 0;

				//				break;
			}
			continue;
		}
		else
		{

			jj = 0;

			// printf("貌似收到数据！！！！！！！！！！！！");
			if (FD_ISSET(fd, &maxFd))
			{
				ret = recvFrame(fd, g_comm_qmegid[id_thread], &recvbuf);
				printf("recvFrame返回值:%d\n", ret);
				if (ret == -1)
				{
					i++;

					if (i > 30)
					{
						printf("接收不成功！！！！！！！！！！！！！！！！i=%d\r\n", i);
						break;
					}
					else
						continue;
				}
				else if (ret == 1)
				{
					//                 i++;

					// if(i>30)
					// {
					// 	printf("接收数据长度为0！！！！！！！！！！！！！！！！\r\n");

					// 	i=0;

					// }
					continue;
				}
				else
				{
					i = 0;
					printf("接收成功！！！！！！！！！！！！！！！！wait_flag=%d modbus_sockt_state[id_thread]=%d\r\n", wait_flag, modbus_sockt_state[id_thread]);
				}
			}
			else
			{
				printf("未知错误////////////////////////////////r/n");
				break;
			}
		}
	}
	close(modbus_client_sockptr[id_thread]);
	modbus_sockt_state[id_thread] = STATUS_OFF;
	printf("网络断开，等待客户端重连！！！！");
	goto loop;
}