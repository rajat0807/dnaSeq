#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
int main(int argc,char **argv){
	int i,n;
	if(argc<2) exit(-1);
	n=atoi(argv[1]);
	for(i=n;i>=0;i--) {
		if(argc==3) printf("> '%s' %2d\n",argv[2],i);
		else printf("> %2d\n",i);
		fflush(stdout);
		if(i!=0) sleep(1);
	}
	exit(0);
}
