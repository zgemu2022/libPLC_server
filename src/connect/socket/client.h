#ifndef _CLIENT_H_
#define _CLIENT_H_
#include "plc_main.h"
#define MAX_FLAME_LEN 1024

#define MAX_CLIENT_CNT  6

typedef struct
 {
	  int msgtype;
	  char data[MAX_FLAME_LEN];
}msgClient;
typedef struct
{
	unsigned char  buf[MAX_FLAME_LEN];	//数据
	int len;	//buf的长度
}MyData;

typedef struct
{
	unsigned short frameId;//帧序号
	unsigned char recvbuf[MAX_FLAME_LEN];//收到的数据部分
	unsigned short lenrecv;//收到的数据长度

}ModTcp_Frame;




extern PARA_PLC *pPara_plc ;
extern char modbus_sockt_state[];
extern int modbus_client_sockptr[];
extern unsigned int modbus_sockt_timer[];
void *_clientRecv_thread(void *arg) ;
void *_clientSend_thread(void *arg) ;

#endif//_CLIENT_H_