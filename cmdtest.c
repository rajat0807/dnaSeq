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

#define TIMEOUT 5
#define TESTFILE "testfile.txt"

char *procname=NULL;
char *command=NULL;
char *listfile=NULL;
char *outputfile=NULL;
char *currentrequest=NULL;
char *email=NULL;
char *jobname=NULL;
int pid=-1;
int waiting=0;

//int myglobal;
//pthread_mutex_t mymutex = PTHREAD_MUTEX_INITIALIZER;
//pthread_mutex_init(&mymutex,NULL);
//pthread_mutex_lock(&mymutex);
//pthread_mutex_unlock(&mymutex);


void *timeoutkill(void *arg){
	int n=TIMEOUT;
	pthread_t tid=(pthread_t)arg;
	sleep(n);
	pthread_kill(tid,SIGINT);
	printf(":: Killed thread #%ld after %d seconds\n",tid,n);
	pthread_exit(0);
}

void countdown(int n){
	int i;
	//printf(">");fflush(stdout);
	for(i=n;i>=0;i--) {
		printf("> %2d\n",i);
		//printf(" %2d",i);fflush(stdout);
		if(i!=0) sleep(1);
	}
	//printf("\n");fflush(stdout);
}

void *functiontest(void *arg){
	int i,n;
	n=(int)arg;
	getchar();
	for(i=n;i>0;i--){
		printf("[%lu]: %i\n",pthread_self(),i);
		sleep(1);
	}
	printf("[%lu]: end\n",pthread_self());
	pthread_exit(0);
}

void threadstest(){
	pthread_t tid1, tid2;
	int tret1, tret2;
	tret1=pthread_create(&tid1,NULL,functiontest,(void *)3);
	tret2=pthread_create(&tid2,NULL,timeoutkill,(void *)tid1);
	pthread_join(tid1,NULL);
	pthread_kill(tid2,SIGINT);
}

void loop(){
	int i;
	double x;
	x=M_PI;
	for(i=0;i<1000000;i++) x=floor(x*sqrt(x));
}

void timevaltest(){
	struct timeval tim;
	gettimeofday(&tim, NULL);
	double t1=tim.tv_sec+(tim.tv_usec/1000000.0);
	loop();
	gettimeofday(&tim, NULL);
	double t2=tim.tv_sec+(tim.tv_usec/1000000.0);
	printf("%.6lf seconds elapsed\n", t2-t1);
}

void popentest(){
	FILE *cmdout;
	char *cmd="ls -l";
	char line[255];
	int n;
	if( ( cmdout=popen(cmd,"r") ) == NULL ) return;
	while(!feof(cmdout)){
		fgets(line,254,cmdout);
		n=(int)strlen(line);
		if(line[n-1]!='\n') line[n-1]='\n';
		printf("> %s",line);
	}
	/*
	char c;
	while( ( c=fgetc(cmdout) ) != EOF ){
		fputc(c,stdout);
		fflush(stdout);
	}
	*/
	pclose(cmdout);
}

void filesizetest(char *filename){
	FILE *file;
	struct stat filestatus;
	//unsigned long int filesize;
	if( (file=fopen(filename,"r")) == NULL ) return;
	//fseek(file,0,SEEK_END);
	//filesize=ftell(file);
	//printf("size('%s')=%lubytes\n",filename,filesize);
	fstat(fileno(file),&filestatus);
	printf("size('%s')=%lubytes\n",filename,filestatus.st_size);
	fclose(file);
}

void filelocktest(){
	FILE *file;
	int fd;
	if( (file=fopen(TESTFILE,"w")) == NULL ) return;
	fd=fileno(file);
	if( flock(fd,LOCK_EX) == -1 ) return;
	printf(":: [%d] LOCKed file [%d]'%s'",getpid(),fd,TESTFILE);getchar();
	flock(fd,LOCK_UN);
	printf(":: [%d] UNLOCKed file [%d] '%s'",getpid(),fd,TESTFILE);getchar();
	fclose(file);
}


/*****************/
/*****************/
/*****************/


int isAlreadyRunning(char *cmd){
	int n;
	FILE *cmdstream;
	char *cmdoutput;
	char *cmdline;
	n=(int)strlen(cmd);
	cmdline=(char *)malloc((n+26)*sizeof(char));
	strcpy(cmdline,"ps -o pid= --sort=pid -C ");
	strcat(cmdline,cmd);
	if( (cmdstream=popen(cmdline,"r")) == NULL ) return -1;
	cmdoutput=(char *)calloc(10,sizeof(char));
	fscanf(cmdstream,"%9s",cmdoutput);
	cmdoutput[9]='\0';
	pclose(cmdstream);
	n=(int)strlen(cmdoutput);
	//printf("[%d] '%s' > '%s'\n",n,cmdline,cmdoutput);
	free(cmdline);
	//return n;
	if(n==0){
		free(cmdoutput);
		return 0;
	}
	pid=atoi(cmdoutput);
	free(cmdoutput);
	if(getpid()==pid){
		printf(":: Process started with PID %d\n",pid);
		return 0;
	}
	printf(":: Process '%s' already running with PID %d\n",cmd,pid);
	return 1;
}

double getCpuUsage(char *cmd){
	int n;
	FILE *cmdstream;
	char *cmdoutput;
	char *cmdline;
	double usage;
	n=(int)strlen(cmd);
	cmdline=(char *)malloc((n+29)*sizeof(char));
	strcpy(cmdline,"ps -o pcpu= --sort=-pcpu -C ");
	strcat(cmdline,cmd);
	if( (cmdstream=popen(cmdline,"r")) == NULL ) return -1;
	cmdoutput=(char *)calloc(10,sizeof(char));
	fscanf(cmdstream,"%9s",cmdoutput);
	cmdoutput[9]='\0';
	pclose(cmdstream);
	n=(int)strlen(cmdoutput);
	//printf("[%d] '%s' > '%s'\n",n,cmdline,cmdoutput);
	usage=-100.0;
	if(n>2) usage=atof(cmdoutput);
	free(cmdline);
	free(cmdoutput);
	printf(":: Current CPU usage of '%s' is %.2f%%\n",cmd,usage);
	return usage;
}

void parseArguments(char **args, int count){
	int i,n;
	char *strptr;
	n=(int)strlen(args[0])-1;
	procname=(char *)calloc(n,sizeof(char));
	strcpy(procname,(char *)(args[0]+2));
	printf(":: Process name is '%s'\n",procname);
	listfile=(char *)calloc((n+5),sizeof(char));
	strcpy(listfile,procname);
	strcat(listfile,".list");
	printf(":: List file is '%s'\n",listfile);
	outputfile=(char *)calloc((n+7),sizeof(char));
	strcpy(outputfile,procname);
	strcat(outputfile,".output");
	printf(":: Output file is '%s'\n",outputfile);
	if(count<4) return;
	n=0;
	for(i=1;i<count;i++) n+=(int)strlen(args[i]);
	n+=(count-1);
	currentrequest=(char *)calloc(n,sizeof(char));
	n=0;
	strptr=currentrequest;
	for(i=1;i<count;i++){
		strcpy(strptr,args[i]);
		strptr+=(int)strlen(args[i]);
		if(i!=(count-1)) {
			strptr[0]=' ';
			strptr++;
		}
	}
	printf(":: Request is: '%s'\n",currentrequest);
	fflush(stdout);
}

void parseRequest(char *request){
	int i,n,k;
	n=(int)strlen(request);
	k=0;
	for(i=0;i<n;i++) if(request[i]==' ') k++;
	if(k<2) return;
	k=0;
	while(request[k]!=' ' && request[k]!='\0') k++;
	email=(char *)calloc((k+1),sizeof(char));
	for(i=0;i<k;i++) email[i]=request[i];
	k++;
	n=k;
	while(request[k]!=' ' && request[k]!='\0') k++;
	jobname=(char *)calloc((k-n+1),sizeof(char));
	for(i=0;i<(k-n);i++) jobname[i]=request[n+i];
	k++;
	n=k;
	while(request[k]!='\0') k++;
	command=(char *)calloc((k-n+1),sizeof(char));
	for(i=0;i<(k-n);i++) command[i]=request[n+i];
	//printf(":: Request arguments are: <'%s','%s','%s'>\n",email,jobname,command);
	printf(":: Request arguments are:\n");
	printf("   > e-mail : '%s'\n",email);
	printf("   > jobname: '%s'\n",jobname);
	printf("   > command: '%s'\n",command);
	fflush(stdout);
}

long filesize(FILE *file){
	struct stat filestatus;
	fstat(fileno(file),&filestatus);
	return (long)filestatus.st_size;
}

char *getTopRequest(char *filename){
	FILE *file;
	char *request;
	char c;
	int i,n;
	if( (file=fopen(filename,"r")) == NULL ) return NULL;
	n=0;
	while( ( c=fgetc(file) ) != EOF && c!='\n') n++;
	request=(char *)malloc((n+1)*sizeof(char));
	rewind(file);
	for(i=0;i<n;i++){
		c=fgetc(file);
		request[i]=c;
	}
	request[n]='\0';
	fclose(file);
	printf(":: Request '%s' fetched\n",request);
	return request;
}

void removeTopRequest(char *filename){
	FILE *file;
	fpos_t srcpos,dstpos;
	char c;
	long int n;
	if( (file=fopen(filename,"r+")) == NULL ) return;
	n=0;
	while( ( c=fgetc(file) ) != EOF && c!='\n') n++;
	fgetpos(file,&srcpos);
	rewind(file);
	fgetpos(file,&dstpos);
	fsetpos(file,&srcpos);
	n=0;
	while( (c=fgetc(file)) != EOF ){
		fgetpos(file,&srcpos);
		fsetpos(file,&dstpos);
		fputc(c,file);
		fgetpos(file,&dstpos);
		fsetpos(file,&srcpos);
		n++;
	}
	ftruncate(fileno(file),n);
	fclose(file);
	printf(":: Request removed from queue\n");
}

void addBottomRequest(char *filename, char *line){
	FILE *file;
	int i;
	char c;
	if(line==NULL) return;
	if( ((int)strlen(line))==0 ) return;
	if( (file=fopen(filename,"a+")) == NULL ) {printf(":: Error opening file '%s'\n",filename);return;}
	i=0;
	while( (c=line[i]) != '\n' && c != '\0' ){
		fputc(c,file);
		i++;
	}
	if(i>0) fputc('\n',file);
	fclose(file);
	printf(":: Request '%s' added to queue\n",line);
}

int countRequests(){
	FILE *file;
	char line[255];
	int i,n;
	if( (file=fopen(listfile,"r")) == NULL ) return 0;
	i=0;
	printf(":: Pending requests in '%s':\n",listfile);
	while( fgets(line,254,file)!=NULL ){
		i++;
		n=(int)strlen(line);
		if( n>0 && line[n-1]=='\n' ) line[n-1]='\0';
		printf("   [%d] '%s'\n",i,line);
	}
	printf(":: %d request(s) waiting\n",i);
	fclose(file);
	return i;
}

void executeCommand(char *cmd){
	FILE *cmdout;
	char line[255];
	char timestamp[20];
	char *cmdin;
	char *cmdadd=" 2>&1 | tee ";
	int n;
	time_t timenow;
	struct timeval hptime;
	double starttime, endtime;
	n=(int)(strlen(cmd)+strlen(cmdadd)+strlen(outputfile));
	cmdin=(char *)malloc((n+1)*sizeof(char));
	strcpy(cmdin,cmd);
	strcat(cmdin,cmdadd);
	strcat(cmdin,outputfile);
	cmdin[n]='\0';
	printf(":: Executing command '%s' ...\n",cmdin);
	gettimeofday(&hptime, NULL);
	starttime=hptime.tv_sec+(hptime.tv_usec/1000000.0);
	if( ( cmdout=popen(cmdin,"r") ) == NULL ) return;
	while(fgets(line,254,cmdout)!=NULL){
		time(&timenow);
		//strftime(timestamp,20,"%H:%M:%S %d-%m-%y",localtime(&timenow));
		strftime(timestamp,20,"%H:%M:%S",localtime(&timenow));
		n=(int)strlen(line);
		if(line[n-1]!='\n') line[n-1]='\n';
		printf("   |%s| %s",timestamp,line);
		fflush(stdout);
	}
	pclose(cmdout);
	gettimeofday(&hptime, NULL);
	endtime=hptime.tv_sec+(hptime.tv_usec/1000000.0);
	printf(":: Command running time was %.2lf seconds\n",(endtime-starttime));
	free(cmdin);
}

void readCommandOutput(){
	FILE *cmdout;
	char line[255];
	char *cmdin;
	char *cmd="tail -q --pid=";
	char *cmdadd=" -f ";
	char pidstr[20];
	int i,n;
	sprintf(pidstr,"%d",pid);
	n=(int)(strlen(cmd)+strlen(pidstr)+strlen(cmdadd)+strlen(outputfile));
	cmdin=(char *)malloc((n+1)*sizeof(char));
	strcpy(cmdin,cmd);
	strcat(cmdin,pidstr);
	strcat(cmdin,cmdadd);
	strcat(cmdin,outputfile);
	cmdin[n]='\0';
	printf(":: Reading output with '%s' ...\n",cmdin);
	if( ( cmdout=popen(cmdin,"r") ) == NULL ) return;
	i=1;
	while(fgets(line,254,cmdout)!=NULL){
		n=(int)strlen(line);
		if(line[n-1]!='\n') line[n-1]='\n';
		printf("   |%02d| %s",i++,line);
		fflush(stdout);
	}
	pclose(cmdout);
	printf(":: End of command output\n");
	free(cmdin);
}

void sendEmail(char *address, char *subject, char *bodyfile){
	FILE *cmdout;
	char line[255];
	char *cmd;
	char *cmdadd1="cat ";
	char *cmdadd2=" | mail -s \"[CMDTEST] "; // REMOVE
	char *cmdadd3="\" ";
	int n;
	printf(":: Sending e-mail '%s' to '%s'... ",subject,address);
	printf("disabled\n");return; // REMOVE
	n=(int)(strlen(cmdadd1)+strlen(bodyfile)+strlen(cmdadd2)+strlen(subject)+strlen(cmdadd3)+strlen(address));
	cmd=(char *)calloc((n+1),sizeof(char));
	strcpy(cmd,cmdadd1);
	strcat(cmd,bodyfile);
	strcat(cmd,cmdadd2);
	strcat(cmd,subject);
	strcat(cmd,cmdadd3);
	strcat(cmd,address);
	printf("\n[%s]\n",cmd);
	if( ( cmdout=popen(cmd,"r") ) == NULL ) {
		free(cmd);
		return;
	}
	fgets(line,254,cmdout);
	printf("[%s]",line);
	pclose(cmdout);
	printf(" done\n");
	free(cmd);
}

int main(int argc,char **argv){
	if(argc<4) {
		printf(":: Not enough arguments\n");
		printf(":: Usage: %s <email> <jobname> <command>\n",argv[0]);
		printf(":: Bye\n");
		exit(-1);
	}
	parseArguments(argv,argc);
	if(isAlreadyRunning(procname)) {
		getCpuUsage(procname);
		addBottomRequest(listfile,currentrequest);
		readCommandOutput();
	}
	else {
		addBottomRequest(listfile,currentrequest);
		while( (waiting=countRequests()) > 0 ){
			free(currentrequest);
			currentrequest=getTopRequest(listfile);
			parseRequest(currentrequest);
			executeCommand(command);
			sendEmail(email,jobname,outputfile);
			removeTopRequest(listfile);
			free(email);
			free(jobname);
			free(command);
		}
	}
	free(procname);
	free(listfile);
	free(outputfile);
	free(currentrequest);
	printf(":: Done.\n");
	exit(0);
}
