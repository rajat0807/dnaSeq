#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "suffixtries.h"

#define ALPHABETSIZE 4
#define LIMIT 10
#define BUFFERLENGTH 8

static treenode *root;
static char *text;
static int textsize;
static int numberofnodes;
static treenode **sidelinksvector;
static int loadedfromfile;
static int maxdepth = LIMIT;

static unsigned char fileiobuffer;
static int fileiobuffersize;
static unsigned char bitmask;
const unsigned char masklast = (unsigned char)1;
const unsigned char maskfirst = (unsigned char)128;
const unsigned char zero = (unsigned char)0;
const unsigned char one = (unsigned char)1;
treenode * getSuffixTreeRoot(){
	return root;
}
treenode ** getSideLinksVector(){
	return sidelinksvector;
}
void initializeSideLinksVector(int limit){
	int i,n;
	n=(limit+1);
	sidelinksvector=(treenode **)malloc(n*sizeof(treenode *));
	for(i=0;i<n;i++) sidelinksvector[i]=NULL;
}
int getCharCode(char c){
	if(c=='A') return 0;
	if(c=='C') return 1;
	if(c=='G') return 2;
	if(c=='T') return 3;
	if(c=='a') return 0;
	if(c=='c') return 1;
	if(c=='g') return 2;
	if(c=='t') return 3;
	return -1;
}
char getCodeChar(unsigned char c){
	if(((int)c)==3) return 'T';
	if(((int)c)==2) return 'G';
	if(((int)c)==1) return 'C';
	return 'A';
}
treenode * newNode(char label,int depth){
	treenode *newnode=(treenode *)malloc(sizeof(treenode));
	newnode->label=label;
	newnode->count=1;
	newnode->depth=depth;
	newnode->suffixlink=NULL;
	newnode->branches=(treenode **)calloc((ALPHABETSIZE),sizeof(treenode *));
	newnode->numbranches=0;
	newnode->sidelink=sidelinksvector[depth];
	sidelinksvector[depth]=newnode;
	numberofnodes++;
	return newnode;
}
treenode * addBranch(treenode *node, char label){
	int pos=getCharCode(label);
	treenode *newbranch;
	if(pos==-1) return NULL;
	newbranch=newNode(label,(node->depth)+1);
	node->branches[pos]=newbranch;
	node->numbranches++;
	return newbranch;
}
treenode * getBranchOfChar(treenode *node, char c){
	int pos=getCharCode(c);
	if(pos==-1) return NULL;
	return (node->branches[pos]);
}
treenode * getNthBranch(treenode *node, int n){
	int i,k;
	treenode *branch;
	k=-1;
	for(i=0;i<ALPHABETSIZE;i++){
		branch=node->branches[i];
		if(branch!=NULL) k++;
		if(k==n) return branch;
	}
	return NULL;
}
void updatePreviousCounts(treenode *node){
	treenode *current;
	current=node;
	while(current!=root){
		current->count++;
		current=current->suffixlink;
	}
}
treenode * buildTree(char *txt, int limit, int *stepsoutput){
	treenode *nextstart,*current,*new,*previousnew,*found;
	int i,n,steps;
	char ti;
	n=(int)strlen(txt);
	text=txt;
	textsize=n;
	maxdepth=limit;
	steps=0;
	numberofnodes=0;
	loadedfromfile=0;
	initializeSideLinksVector(limit);
	root=newNode('#',0);
	root->count=n;
	nextstart=root;
	for(i=0;i<n;i++){
		ti=text[i];
		current=nextstart;
		nextstart=NULL;
		previousnew=NULL;
		found=NULL;
		new=NULL;
		while(1){
			steps++;
			if((current->depth)==limit)
				current=current->suffixlink;
			found=getBranchOfChar(current,ti);
			if(found!=NULL){
				found->count++;
				new=found;
			}
			else
				new=addBranch(current,ti);
			if(nextstart==NULL)
				nextstart=new;
			if(previousnew!=NULL)
				previousnew->suffixlink=new;
			if(found!=NULL){
				updatePreviousCounts(found->suffixlink);
				break;
			}
			if(current==root){
				new->suffixlink=root;
				break;
			}
			previousnew=new;
			current=current->suffixlink;
		}
	}
	if(stepsoutput!=NULL) (*stepsoutput)=steps;
	return root;
}
int getSubStringCount(treenode *node, char *substring){
	treenode *currentnode=NULL;
	int i,n=(int)strlen(substring);
	currentnode=node;
	for(i=0;i<n;i++){
		currentnode=getBranchOfChar(currentnode,substring[i]);
		if(currentnode==NULL) return 0;
	}
	return (currentnode->count);
}
void getBackCountsVector(treenode *startnode, int *countsvector){
	int depth;
	treenode *currentnode;
	depth=startnode->depth;
	currentnode=startnode;
	while(currentnode!=root){
		countsvector[depth]=currentnode->count;
		depth--;
		currentnode=currentnode->suffixlink;
	}
}
treenode *followInTree(char *string, int depth){
	int i;
	treenode *currentnode;
	currentnode=root;
	for(i=0;i<depth;i++){
		currentnode=getBranchOfChar(currentnode,string[i]);
		if(currentnode==NULL) return NULL;
	}
	return currentnode;
}
int getCountByFollowInTree(char *string, int depth){
	treenode *node;
	node=followInTree(string,depth);
	if(node==NULL) return 0;
	return node->count;
}
treenode *getCountsVector(int pos, int *countsvector, treenode *start){
	int i;
	treenode *startnode,*currentnode;
	if(start!=NULL) startnode=start;
	else startnode=followInTree((char *)(text+pos),maxdepth);
	currentnode=startnode;
	for(i=1;i<=maxdepth;i++){
		countsvector[i]=(currentnode->count);
		currentnode=currentnode->suffixlink;
	}
	if((pos+maxdepth+1)>=textsize) return NULL;
	return getBranchOfChar(startnode->suffixlink,(char)(text[pos+maxdepth+1]));
}
void printSuffixTree(treenode *node,int pos){
	int i=0;
	int n=node->numbranches;
	printf("[%d]",n);
	for(i=0;i<pos;i++)	printf("\t");
		printf("%c (%d)",node->label,node->count);
	printf("\n");
	for(i=0;i<n;i++){
		if((node->branches[i])!=NULL) printSuffixTree(node->branches[i],pos+1);
	}
}
char *countToString(int n){
	int nn;
	char *countstring, *tmp;
	nn=(n%1000);
	countstring=(char *)malloc(8*sizeof(char));
	tmp=(char *)malloc(4*sizeof(char));
	strcpy(countstring,"(");
	if(nn<100) strcat(countstring,"0");
	if(nn<10) strcat(countstring,"0");
	sprintf(tmp,"%d",nn);
	strcat(countstring,tmp);
	strcat(countstring,")");
	return countstring;
}
void drawSuffixTree(FILE *output, treenode *node, int totalnodes, int nodenumber, char *linestart, char *countslinestart){
	int i,j,n;
	char *newlinestart, *newcountslinestart, *countstring;
	n=node->numbranches;
	if(nodenumber!=1)
		fprintf(output,"%s|\n%s",linestart,linestart);
	if(nodenumber==totalnodes && nodenumber!=1) fprintf(output,"\'--");
	else fprintf(output,"|--");
	fprintf(output,"%c",node->label);
	
	newcountslinestart=(char *)malloc(((int)strlen(countslinestart)+7+1)*sizeof(char));
	strcpy(newcountslinestart,countslinestart);
	if(nodenumber!=totalnodes) strcat(newcountslinestart,"|");
	else strcat(newcountslinestart," ");
	countstring=countToString(node->count);
	strcat(newcountslinestart,countstring);
	strcat(newcountslinestart," ");
	free(countstring);

	if(n==0){
		fprintf(output,"\n");
		fprintf(output,"%s\n",newcountslinestart);
	}
	else{
		fprintf(output,"-->");
		newlinestart=(char *)malloc(((int)strlen(linestart)+7+1)*sizeof(char));
		strcpy(newlinestart,linestart);
		if(nodenumber!=totalnodes) strcat(newlinestart,"|      ");
		else strcat(newlinestart,"       ");
		i=1;
		for(j=0;j<ALPHABETSIZE;j++){
			if((node->branches[j])!=NULL){
				if(i==1) drawSuffixTree(output,node->branches[j],n,1,newlinestart,newcountslinestart);
				else drawSuffixTree(output,node->branches[j],n,i,newlinestart,newlinestart);
				i++;
			}
		}
	}
}
int printTreeToFile(treenode *node, char * filename){
	FILE *file;
	file=fopen(filename,"w");
	if(file==NULL) return 0;
	drawSuffixTree(file,node,1,1,"","");
	if(fclose(file)!=0) return 0;
	return 1;
}
int logtwo(int x){
	int twopower = 1;
	int i = 1;
	if(x==1) return 1;
	while(twopower<=x){
		twopower = twopower << 1;
		i++;
	}
	return (i-1);
}
void writeNodeBits(FILE *file, int numbranches, char label, int count){
	char mode;
	unsigned char data,tempdata;
	unsigned int countdata;
	int i,n,nzeros;
	mode='N';
	while(1){
		if(mode=='c'){
			countdata = countdata >> 1;
			data=(unsigned char)countdata;
			tempdata =  data & masklast;
			fileiobuffer = fileiobuffer | tempdata;
			i++;
			if(tempdata==zero) nzeros++;
			else nzeros=0;
			if(i==n) mode='F';
			if(nzeros==3) mode='Z';
		}
		else if(mode=='C'){
			n=logtwo(count);
			i=1;
			nzeros=0;
			countdata=(unsigned int)count;
			data=(unsigned char)countdata;
			tempdata =  data & masklast;
			fileiobuffer = fileiobuffer | tempdata;
			mode='c';
			if(tempdata==zero) nzeros++;
			if(n==1) mode='F';
		}
		else if(mode=='N'){
			i=1;
			data=(unsigned char)numbranches;
			tempdata = data & masklast;
			fileiobuffer = fileiobuffer | tempdata;
			mode='n';
		}
		else if(mode=='n'){
			data = data >> 1;
			tempdata = data & masklast;
			fileiobuffer = fileiobuffer | tempdata;
			i++;
			if(i==3) mode='L';
		}
		else if(mode=='L'){
			data=(unsigned char)getCharCode(label);
			tempdata = data & masklast;
			fileiobuffer = fileiobuffer | tempdata;
			mode='l';
		}
		else if(mode=='l'){
			data = data >> 1;
			tempdata = data & masklast;
			fileiobuffer = fileiobuffer | tempdata;
			mode='C';
		}
		else if(mode=='F'){
			i=1;
			mode='f';
		}
		else if(mode=='f'){
			if(i==4) break;
			i++;
		}
		else if(mode=='Z'){
			fileiobuffer = fileiobuffer | one;
			nzeros=0;
			mode='c';
			if(i==n) mode='F';
		}
		fileiobuffersize++;
		if(fileiobuffersize==BUFFERLENGTH){
			fwrite(&fileiobuffer,1,1,file);
			fileiobuffer=(unsigned char)0;
			fileiobuffersize=0;
		}
		fileiobuffer = fileiobuffer << 1;	
	}
}


void freeTreeNode(treenode *node){
	int i;
	for(i=0;i<ALPHABETSIZE;i++)
		if((node->branches[i])!=NULL){
			freeTreeNode(node->branches[i]);
			node->numbranches--;
		}
	free(node->branches);
	free(node);
	node=NULL;
}



int getNumberOfTreeNodes(treenode *node){
	return numberofnodes;
}


int getTreeSize(treenode *node){
	return numberofnodes*sizeof(treenode);
}


