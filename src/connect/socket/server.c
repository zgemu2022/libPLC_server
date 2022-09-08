#include "server.h"
#include "client.h"
#include <pthread.h>
#include <sys/socket.h>
#include <string.h> 
#include  <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
struct sockaddr_in Server_Addr, Client_addr;
int modbus_sockptr;
void *Modbus_ServerConnectThread(void *arg)
{
	// int ret;
    
	// int flags;
	int size_add;
	int socket_optval , socket_optlen;

	
	Server_Addr.sin_family = AF_INET;  
	Server_Addr.sin_addr.s_addr = htonl(pPara_plc->server_ip);
	Server_Addr.sin_port = htons(pPara_plc->server_port);
	socket_optlen = sizeof(int);

	//creat socket
	modbus_sockptr = socket(AF_INET,SOCK_STREAM,0);	

	socket_optval = 1;
	//reuse addr
	setsockopt(modbus_sockptr , SOL_SOCKET , SO_REUSEADDR , &socket_optval, socket_optlen);

	int br;	
	int i;	

	for (i = 0; i < pPara_plc->conn_num; i++)
	{
		modbus_sockt_state[i] = STATUS_OFF;
		modbus_client_sockptr[i] = 0;
	}
	//bind
	do
	{
		 br = bind(modbus_sockptr, (struct sockaddr*)&Server_Addr, sizeof(Server_Addr));//
		 sleep(2);
	}while(br != 0);
	
	listen(modbus_sockptr, pPara_plc->conn_num+1);
	
	while (1)
	{
		for (i = 0; i < pPara_plc->conn_num; i++)
		{
			if (modbus_sockt_state[i] == STATUS_OFF)
			{				
				modbus_client_sockptr[i] = accept(modbus_sockptr,(struct sockaddr*)&Client_addr, (socklen_t*)&size_add);
				modbus_sockt_state[i] = STATUS_ON;
				printf("Modbus Client %d Come!\n", i);
			}
		}
	}
}	