#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "sequencefile.h"

#define MAXDESCSIZE 80
#define MAXDATASIZE 5000000




sequence * newSequence(char *desc, char *data){
	sequence *new=(sequence *)malloc(sizeof(sequence));
	if(desc!=NULL){
		new->description=(char *)malloc(((int)strlen(desc)+1)*sizeof(char));
		strcpy(new->description,desc);
	}
	else new->description=NULL;
	if(data!=NULL){
		new->size=(long)strlen(data);
		new->data=(char *)malloc((new->size+1)*sizeof(char));
		strcpy(new->data,data);
	}
	else{
		new->size=0L;
		new->data=NULL;
	}
	return new;
}
char * getTimestamp(){
	time_t timevalue;
	time(&timevalue);
	return asctime(localtime(&timevalue));
}
sequence * loadSequence(char * path){
	FILE *fich;
	sequence * seq;
	char ch, *desc, *data;
	long k, datasize;
	long startpos, endpos;
	if((fich=fopen(path,"r"))==NULL)
		return NULL;
	desc=NULL;
	k=0;
	desc=(char *)malloc(MAXDESCSIZE*sizeof(char));
	if((ch=fgetc(fich))=='>'){
		while((ch=fgetc(fich))!='\n' && ch!='>' && k<MAXDESCSIZE){
			desc[k]=ch;
			k++;
		}
		desc[k]='\0';
	}
	else{
		strcpy(desc,"Sequence submitted on: ");
		strcat(desc,getTimestamp());
		desc[((int)strlen(desc)-1)]='\0';
		fseek(fich,0L,SEEK_SET);
	}
	startpos=ftell(fich);
	fseek(fich,0L,SEEK_END);
	endpos=ftell(fich);
	datasize=(endpos-startpos);
	fseek(fich,startpos,SEEK_SET);
	data=(char *)malloc((datasize+1)*sizeof(char));
	k=0;
	while((ch=fgetc(fich))!=EOF && ch!='\r')
		if(ch=='A' || ch=='C' || ch=='G' || ch=='T' || ch=='a' || ch=='c' || ch=='g' || ch=='t'){
			data[k]=ch;
			k++;
		}
	data[k]='\0';
	if(fclose(fich)!=0) return NULL;
	seq=(sequence *)malloc(sizeof(sequence));
	seq->description=desc;
	seq->data=data;
	seq->size=k;
	return seq;
}
void printSequence(sequence *seq){
	printf("> %s\n",seq->description);
	printf("> %d\n",seq->size);
	if((seq->size)<100) printf("> %s\n",seq->data);
	else printf("> ...\n");
}
void freeSequence(sequence *seq){
	free(seq->data);
	free(seq->description);
	free(seq);
}
int count_ = 0;
sequence * loadSequences(char * path){
	FILE *fich;
	sequence * seq;
	char ch, *desc, *data;
	long k, datasize;
	long startpos, endpos;
	if((fich=fopen(path,"r"))==NULL)
		return NULL;
	desc=NULL;
	k=0;
	seq=(sequence *)malloc(50*sizeof(sequence));
	desc=(char *)malloc(MAXDESCSIZE*sizeof(char));
	ch=fgetc(fich);
	do {
		k = 0;
		if(ch=='>'){

			while((ch=fgetc(fich))!='\n' && ch!='>' && k<MAXDESCSIZE){
				desc[k]=ch;
				k++;
			}
			desc[k]='\0';
			printf("%s\n", desc);
		}
		else{
			strcpy(desc,"Sequence submitted on: ");
			strcat(desc,getTimestamp());
			desc[((int)strlen(desc)-1)]='\0';
			fseek(fich,0L,SEEK_SET);
		}
		startpos=ftell(fich);
		fseek(fich,0L,SEEK_END);
		endpos=ftell(fich);
		datasize=(endpos-startpos);
		fseek(fich,startpos,SEEK_SET);
		data=(char *)malloc((datasize+1)*sizeof(char));
		k=0;
		while((ch=fgetc(fich))!=EOF && ch!='>' && ch!='\r')
			if(ch=='A' || ch=='C' || ch=='G' || ch=='T' || ch=='a' || ch=='c' || ch=='g' || ch=='t'){
				data[k]=ch;
				k++;
			}
		
		data[k]='\0';
		seq[count_].description=desc;
		seq[count_].data=data;
		seq[count_].size=k;
		count_++;


	} while(ch == '>');

	printf("Total sequences - %d\n", count_);
	
	if(fclose(fich)!=0) return NULL;
	
	return seq;
}



