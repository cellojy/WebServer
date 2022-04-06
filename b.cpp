#include<sys/socket.h>
#include<sys/un.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<signal.h>
#include<unistd.h>
#include<stdlib.h>
#include<assert.h>
#include<stdio.h>
#include<string.h>
int main(int argc,char* argv[])
{
    const char* ip= argv[1];
	int port=atoi(argv[2]);
//	int backlog=atoi(argv[3]);
	int sock=socket(PF_INET,SOCK_STREAM,0);
	assert(sock>=0);
	
	sockaddr_in address;
	bzero(&address,sizeof(address));
	address.sin_family=AF_INET;
	inet_pton(AF_INET,ip,&address.sin_addr);
	address.sin_port=htons(port);
    socklen_t len=sizeof(address);
   // int res=bind(sock,(sockaddr*)&addr,len);
    int pp= connect(sock,(sockaddr*)&address,len);
    if(pp<0) {printf("fail!");return 0;}
    char buf[100];
    while(1)
    {
        scanf("%s",buf);
        if(buf[0]=='q') break;
        send(sock,buf,strlen(buf),0);
        send(sock,"123",3,MSG_OOB);
    }
    close(sock);

}
