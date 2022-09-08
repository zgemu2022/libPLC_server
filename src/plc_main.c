#include "client.h"
#include "server.h"
#include "plc_main.h"
#include "sys.h"
#include <stdio.h>

static void CreatAllThread(int conn_num)
{
	pthread_t ThreadID;
	pthread_attr_t Thread_attr;
	int i;

	for (i = 0; i < conn_num; i++)
	{
		modbus_sockt_state[i]=STATUS_OFF;
		if (FAIL == CreateSettingThread(&ThreadID, &Thread_attr, (void *)_clientRecv_thread, (int *)i, 1, 1))
		{
			SYSERR_PRINTF("_clientRecv_thread CREATE ERR!\n");

			exit(1);
		}
		if (FAIL == CreateSettingThread(&ThreadID, &Thread_attr, (void *)_clientRecv_thread, (int *)i, 1, 1))
		{
			printf("_clientRecv_thread CREATE ERR!\n");
			exit(1);
		}

	}
	if (FAIL == CreateSettingThread(&ThreadID, &Thread_attr,(void *)Modbus_ServerConnectThread, NULL, 1, 1))
	{
		printf("MODBUS CONNECT THTREAD CREATE ERR!\n");
		
		exit(1);
	}
	printf("Threads CREATE success!\n");

	
}

int plc_main(void* para)
{
    memcpy((void*)pPara_plc,para,sizeof(PARA_PLC));
	CreatAllThread(pPara_plc->conn_num);
	printf("12345 ssdlh\n");
	return 0;
}