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
#include <sys/select.h>

#define UNIXSTR_PATH "/tmp/s.unixstr"
#define MAXSOCKS 32
#define MAXLINE 254

void printerror(char *msg){
	printf("[error] %s\n",msg);
}

void exiterror(char *msg){
	printerror(msg);
	exit(-1);
}

int main(void){
	int sockfd,newsockfd,clilen,childpid,servlen;
	struct sockaddr_un cli_addr,serv_addr;
	if((sockfd=socket(AF_UNIX,SOCK_STREAM,0))<0) exiterror("server: can't open stream socket");
	unlink(UNIXSTR_PATH);
	bzero((char *)&serv_addr,sizeof(serv_addr));
	serv_addr.sun_family=AF_UNIX;
	strcpy(serv_addr.sun_path,UNIXSTR_PATH);
	servlen=strlen(serv_addr.sun_path)+sizeof(serv_addr.sun_family);
	if(bind(sockfd,(struct sockaddr *)&serv_addr,servlen)<0) exiterror("server: can't bind local address");
	listen(sockfd,5);

	fd_set testmask,mask;
	FD_ZERO(&testmask);
	FD_SET(sockfd,&testmask);

	while(1){

		mask=testmask;
		select(MAXSOCKS,&mask,0,0,0);
		if(FD_ISSET(sockfd,&mask)){
			clilen=sizeof(cli_addr);
			newsockfd=accept(sockfd,(struct sockaddr*)&cli_addr,&clilen);
			printf("%d\n",newsockfd);
			close(newsockfd);
		}

/*
		clilen=sizeof(cli_addr);
		newsockfd=accept(sockfd,(struct sockaddr *)&cli_addr,&clilen);
		if(newsockfd<0) printerror("server: accept error");
		if((childpid=fork())<0) printerror("server: fork error");
    		else if(childpid==0){
			close(sockfd);

			int n;
			char line[MAXLINE];
			while(1){
				n=read(newsockfd,line,MAXLINE);
				if(n==0) break;
				else if(n<0) printerror("str_echo: readline error");
				printf("[client@%d](%d): %s\n",newsockfd,n,line);
				if(write(newsockfd,line,n)!=n) printerror("str_echo: writen error");
			}

			exit(0);
		}
		close(newsockfd);
*/
	}

}
