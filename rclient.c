#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/un.h>

#define UNIXSTR_PATH "/tmp/s.unixstr"
#define MAXLINE 254

void printerror(char *msg){
	printf("[error] %s\n",msg);
}

void exiterror(char *msg){
	printerror(msg);
	exit(-1);
}

int main(void){
	int sockfd, servlen;
	struct sockaddr_un serv_addr;
	if((sockfd=socket(AF_UNIX,SOCK_STREAM,0))<0) exiterror("client: can't open stream socket");
	bzero((char *)&serv_addr,sizeof(serv_addr));
	serv_addr.sun_family=AF_UNIX;
	strcpy(serv_addr.sun_path,UNIXSTR_PATH);
	servlen=strlen(serv_addr.sun_path)+sizeof(serv_addr.sun_family);
	if(connect(sockfd,(struct sockaddr *)&serv_addr,servlen)<0) exiterror("client: can't connect to server");
	
	int n;
	char sendline[MAXLINE+1], recvline[MAXLINE+1];
	while(fgets(sendline,MAXLINE,stdin)!=NULL){
		n=strlen(sendline);
		sendline[n-1]='\0';
		if(write(sockfd,sendline,n)!=n) printerror("str_cli: written error on socket");
		n=read(sockfd,recvline,MAXLINE);
		if(n<0) printerror("str_cli: readline error");
		recvline[n]='\0';
		printf("%s\n",recvline);
	}
	
	close(sockfd);
	exit(0);
}
