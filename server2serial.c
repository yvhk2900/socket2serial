#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "serial_module.h"

#define FALSE 0
#define TRUE 1

int serial_fd;
volatile int STOP=FALSE; 
char serial_buf[255];
char scan_buf[255];
int serial_res;

int init_serial(void){
	serial_fd = openPort(0);
	if(serial_fd < 0)
	{
		printf("serial open error.\n");
		exit(0);
	}
	//fd，115200， 
	setPara(serial_fd,6,8,1,0);
}

int main(int argc, char *argv[])
{
	init_serial();

	int server_sockfd;//服务器端套接字
	int client_sockfd;//客户端套接字
	int len;
	struct sockaddr_in my_addr;   //服务器网络地址结构体
	struct sockaddr_in remote_addr; //客户端网络地址结构体
	int sin_size;
	char buf[BUFSIZ];  //数据传送的缓冲区
	memset(&my_addr,0,sizeof(my_addr)); //数据初始化--清零
	my_addr.sin_family=AF_INET; //设置为IP通信
	my_addr.sin_addr.s_addr=INADDR_ANY;//服务器IP地址--允许连接到所有本地地址上
	my_addr.sin_port=htons(8000); //服务器端口号
	
	/*创建服务器端套接字--IPv4协议，面向连接通信，TCP协议*/
	if((server_sockfd=socket(PF_INET,SOCK_STREAM,0))<0)
	{  
		perror("socket");
		return 1;
	}
 
        /*将套接字绑定到服务器的网络地址上*/
	if (bind(server_sockfd,(struct sockaddr *)&my_addr,sizeof(struct sockaddr))<0)
	{
		perror("bind");
		return 1;
	}

	//----------------------------working------------------------------------------------------------
	while(1){	
		printf("waiting...\n");
		/*监听连接请求--监听队列长度为5*/
		listen(server_sockfd,5);
		sin_size=sizeof(struct sockaddr_in);
		/*等待客户端连接请求到达*/
		if((client_sockfd=accept(server_sockfd,(struct sockaddr *)&remote_addr,&sin_size))<0)
		{
			perror("accept");
			return 1;
		}
		printf("accept client %s\n",inet_ntoa(remote_addr.sin_addr));
		len=send(client_sockfd,"Welcome to my server\n",21,0);//发送欢迎信息
		
		/*接收客户端的数据并将其发送给客户端--recv返回接收到的字节数，send返回发送的字节数*/
		while((len=recv(client_sockfd,buf,BUFSIZ,0))>0)
		{
			buf[len]='\0';
			printf("%s\n",buf);
			serial_res = nwrite(serial_fd,buf,strlen(buf));//通过串口发送
		}
	}
	close(client_sockfd);
	close(server_sockfd);
        return 0;
}
