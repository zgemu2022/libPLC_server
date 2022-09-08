#ifndef _PLC_MAIN_H_
#define _PLC_MAIN_H_
typedef struct
{
	int conn_num;
	char  server_ip[64];

	unsigned short server_port;
} PARA_PLC; //从主控传到plc模块的结构1
#endif