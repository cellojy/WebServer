#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	char *ip = argv[1];
	int port = atoi(argv[2]);
	int listenfd = socket(AF_INET, SOCK_STREAM, 0);
	sockaddr_in address;
	bzero(&address, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_port = htons(port);
	inet_pton(AF_INET, ip, &address.sin_addr);
	int res = bind(listenfd, (sockaddr *)&address, sizeof(address));
	if (res == -1)
	{
		close(listenfd);
		puts("error 1");
		return 0;
	}
	res = listen(listenfd, 5);
	if (res == -1)
	{
		close(listenfd);
		puts("error 2");
		return 0;
	}
	socklen_t len = sizeof(address);

	//puts("11");
	int clientfd = accept(listenfd, (sockaddr *)&address, &len);
//	puts("1111");
	if (clientfd < 0)
	{
		close(listenfd);
		puts("error 3");
		return 0;
	}
	char buf[1024];
	fd_set read_fds;
	fd_set error_fds;
	FD_ZERO(&read_fds);
	FD_ZERO(&error_fds);
	while (1)
	{
		memset(buf, '\0', sizeof(buf));
		FD_SET(clientfd, &read_fds);
		FD_SET(clientfd, &error_fds);
	//	puts("55");
		res = select(clientfd + 1, &read_fds, NULL, &error_fds, NULL);
	//	puts("66");
		if (res < 0)
		{
			puts("error 4");
			return 0;
		}
		
		if (FD_ISSET(clientfd, &read_fds))
		{
		//	puts("11");
			res = recv(clientfd, buf, sizeof(buf) - 1, 0);
	//		puts("22");
			if (res <= 0)
			{
				break;
			}
			printf("%s\n", buf);
		}
	//	else puts("11111");
		if (FD_ISSET(clientfd, &error_fds))
		{

		//	puts("33");
			res = recv(clientfd, buf, sizeof(buf) - 1, MSG_OOB);
	//		puts("44");
			if (res <= 0)
				break;
			printf("%s\n", buf);
		}
	//	else puts("111111");
	}
	close(listenfd);
	close(clientfd);
	return 0;
}
