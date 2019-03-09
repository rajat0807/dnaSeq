#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "sequencefile.h"
#include "suffixtries.h"

#define LIMIT 10
#define SIZE 50

static char type = '\0';
static char *filename = NULL;
static char *sequencetext = NULL;
static sequence *sequencefromfile = NULL;
static int sequencesize = 0;
static int lvalue = 0;
static int lmaxvalue = 0;
static int phivalue = 0;
static int position = 0;
static int positionwindow = 0;

static double *fvalueslist = NULL;
static double mteta = 0.0;
static double steta = 0.0;

static treenode *suffixtree = NULL;
static int *substringscount = NULL;
static double *xpointslist = NULL;
static double *ypointslist = NULL;
static int numberofpoints = 0;

static double *pvalueslist = NULL;
static double *zscoreslist = NULL;
static int *countslist = NULL;
static char **substringslist = NULL;

static double *averageslist = NULL;
static double *varianceslist = NULL;

struct EPVal {
    double val;
    int ind;
} a[SIZE+4];
// converte um double para um int
int d2i(double n){
	return (int)floor(n);
}

// devolve o m�imo de dois inteiros
int intmax(int x, int y){
	if(x>y) return x;
	return y;
}

double doublemax(double x, double y){
	if(x>y) return x;
	return y;
}

// devolve o m�imo de dois inteiros
int intmin(int x, int y){
	if(x<y) return x;
	return y;
}

char* substr(const char *src, int m, int n)
{
	int len = n - m;

	char *dest = (char*)malloc(sizeof(char) * (len + 1));

	for (int i = m; i < n && (*src != '\0'); i++)
	{
		*dest = *(src + i);
		dest++;
	}

	*dest = '\0';

	return dest - len;
}


// devolve uma mensagem de erro e sai do programa
void exitError(char *errordesc){
	printf("Error: %s\n",errordesc);
	exit(-1);
}

// liberta a mem�ia alocada pelas vari�eis usadas no c�culo dos perfis entr�icos
void freeVariables(){
	int i;
	if(xpointslist!=NULL) free(xpointslist);
	if(ypointslist!=NULL) free(ypointslist);
	if(substringscount!=NULL) free(substringscount);
	if(pvalueslist!=NULL) free(pvalueslist);
	if(zscoreslist!=NULL) free(zscoreslist);
	if(countslist!=NULL) free(countslist);
	if(substringslist!=NULL) {
		for(i=0;i<numberofpoints;i++) free(substringslist[i]);
		free(substringslist);
	}
}

// mostra a lista de pontos
void printPoints(){
	int i;
	for(i=0;i<numberofpoints;i++){
		printf("(%d,%f)\n",d2i(xpointslist[i]),ypointslist[i]);
	}
}

// salva os valores da m�ia e vari�cia para um ficheiro
void saveDataToFile(){
	int i,n;
	char *datafilename;
	FILE *fileout;
	if(averageslist==NULL || varianceslist==NULL) return;
	n=(int)strlen(filename);
	datafilename=(char *)malloc((n+6)*sizeof(char));
	strncpy(datafilename,filename,n);
	datafilename[n]='\0';
	strncat(datafilename,".data",5);
	datafilename[n+5]='\0';
	fileout=fopen(datafilename,"w");
	if(fileout==NULL) return;
	for(i=1;i<=LIMIT;i++) fprintf(fileout,"%f %f \n",averageslist[i],varianceslist[i]);
	fclose(fileout);
	free(datafilename);
}

// carrega os valores da m�ia e vari�cia a partir de um ficheiro
void loadDataFromFile(){
	int i,n;
	char *datafilename;
	FILE *filein;
	n=(int)strlen(filename);
	datafilename=(char *)malloc((n+6)*sizeof(char));
	strncpy(datafilename,filename,n);
	datafilename[n]='\0';
	strncat(datafilename,".data",5);
	datafilename[n+5]='\0';
	filein=fopen(datafilename,"r");
	if(filein==NULL) return;
	averageslist=(double *)calloc((LIMIT+1),sizeof(double));
	varianceslist=(double *)calloc((LIMIT+1),sizeof(double));
	for(i=1;i<=LIMIT;i++){
		fscanf(filein,"%lf %lf",&averageslist[i],&varianceslist[i]);
		//printf("%lf %lf\n",averageslist[i],varianceslist[i]);
	}
	fclose(filein);
	free(datafilename);
}


void saveDescriptionToFile(){
	int n;
	char *descfilename;
	FILE *fileout;
	if(sequencefromfile==NULL) return;
	n=(int)strlen(filename);
	descfilename=(char *)malloc((n+6)*sizeof(char));
	strncpy(descfilename,filename,n);
	descfilename[n]='\0';
	strncat(descfilename,".desc",5);
	descfilename[n+5]='\0';
	fileout=fopen(descfilename,"w");
	if(fileout==NULL) return;
	fprintf(fileout,"%s",sequencefromfile->description);
	fclose(fileout);
	free(descfilename);
}


void printBinary(unsigned int x){
	int i;
	unsigned int zero;
	unsigned int one;
	zero=(unsigned int)0;
	one=(unsigned int)1;
	for(i=(LIMIT-1);i>=0;i--){
		if( (( x >> i) & one ) == zero) printf("0");
		else printf("1");
	}
	printf("\n");
}

void savePointsToFile(int sort){
	int i,j;
	int *done;
	int k;
	double maxvalue;
	FILE *fileout;
	if((fileout=fopen("values.txt","w"))==NULL)
		return;
	/*
	for(i=0;i<numberofpoints;i++)
		fprintf(fileout,"%d %s %d %f %f %f\n",d2i(xpointslist[i]),substringslist[i],countslist[i],ypointslist[i],pvalueslist[i],zscoreslist[i]);
	*/
	if(sort>0){
		done=(int *)calloc(numberofpoints,sizeof(int));
		for(i=0;i<numberofpoints;i++){
			k=0;
			maxvalue=(double)(-sequencesize);
			for(j=0;j<numberofpoints;j++){
				if(done[j]==1) continue;
				if(ypointslist[j]>=maxvalue){
					maxvalue=ypointslist[j];
					k=j;
				}
			}
			done[k]=1;
			fprintf(fileout,"%d %s %d %f %.10f %f \n",d2i(xpointslist[k]),substringslist[k],countslist[k],ypointslist[k],pvalueslist[k],zscoreslist[k]);
		}
		free(done);
	}
	else{
		for(k=(numberofpoints-1);k>=2;k--){
			fprintf(fileout,"%d %s %d %f %.10f %f \n",d2i(xpointslist[k]),substringslist[k],countslist[k],ypointslist[k],pvalueslist[k],zscoreslist[k]);
		}
	}
	fclose(fileout);
}


char *getSubstring(int i, int l){
	char *substring;
	int length;
	int j=(i-l+1);
	length=l;
	substring=(char *)calloc((length+1),sizeof(char));
	if(i<1 || j<1 || length<1 || i>sequencesize) return substring;
	strncpy(substring,sequencetext+(j-1),length);
	substring[length]='\0';
	return substring;
}


int slowcount(int i, int j){
	char *substring;
	int length,count;
	int m,k;
	length=(j-i)+1;
	//printf("S[%d,%d]=",i,j);
	if(i<1 || j<1 || length<1 || j>sequencesize){
		//printf("\n");
		return 0;
	}
	substring=(char *)malloc((length+1)*sizeof(char));
	strncpy(substring,sequencetext+(i-1),length);
	substring[length]='\0';
	count=0;
	for(m=0;m<sequencesize;m++){
		for(k=0;k<length && (m+k)<sequencesize;k++){
			if(substring[k]!=sequencetext[(m+k)]) break;
			if(k==(length-1)) count++;
		}
	}
	
	return count;
}

int count(int i, int j){
	//char *substring;
	int length,count;
	length=(j-i)+1;
	//printf("S[%d,%d]=",i,j);
	if(i<1 || j<1 || length<1 || j>sequencesize){
		//printf("\n");
		return 0;
	}
	//substring=(char *)malloc((length+1)*sizeof(char));
	//strncpy(substring,sequencetext+(i-1),length);
	//substring[length]='\0';
	//count=getSubStringCount(suffixtree,substring);
	//printf("'%s'(%d)\n",substring,count);
	count=getCountByFollowInTree(sequencetext+(i-1),length);
	return count;
}

double f(double L, double phi, int i, int N){
	long double aux1,aux2;
	long double k;
	aux1 = 0.0;
	for( k = 1.0 ; k <= ( (long double) L ) ; k++ ){
		aux1 += pow( 4.0 , k ) * pow( (long double) phi , k ) * ( (long double) count( i - d2i(k) + 1 , i ) );
	}
	aux1 = 1.0 + ( 1.0 / ( (long double) N ) ) * aux1;
	aux2 = 0.0;
	for( k = 0.0 ; k <= ( (long double) L ) ; k++ ){
		aux2 += pow( (long double) phi , k );
	}
	return (double)( aux1 / aux2 );
}


double g(int L, int phi, int i, int N){
	return ( ( f((double)L,(double)phi,i,N) - mteta ) / steta );
}

double getSumFSquared(){
	int i,j;
	int pos;
	int iN=sequencesize;
	int iL=lvalue;
	int iPhi=phivalue;
	long double *fourphikvector;
	long double totalnumerator,numerator,denominator;
	int *countsvector;
	
	//int *totalcounts;
	//totalcounts=(int *)calloc((iL+1),sizeof(int));
	
	fourphikvector=(long double *)malloc((iL+1)*sizeof(long double));
	fourphikvector[0]=1;
	for(i=1;i<=iL;i++) fourphikvector[i] = (fourphikvector[i-1]) * ((long double)(4 * iPhi));
	//printf("4phikvector=%f\n",fourphikvector[iL]);
	
	countsvector=(int *)calloc((iL+1),sizeof(int));
	totalnumerator = 0.0;
	//for( i=0; i<(iN-iL+1); i++ ){
	for( i=(iL-1); i<iN; i++ ){
		numerator = 0.0;
		for(j=1;j<=iL;j++){
			//countsvector[j]=getCountByFollowInTree((char *)(sequencetext+i),j);
			pos=i-(j-1);
			countsvector[j]=getCountByFollowInTree((char *)(sequencetext+pos),j); // MUDAR PARA MAIS R�IDO!
			numerator += (fourphikvector[j]) * ((long double)(countsvector[j]));
			//if(i<3 || i==(iN-1)) printf("countsvector[%d,%d]=%d\n",i,j,countsvector[j]);
			//totalcounts[j]+=countsvector[j];
		}
		numerator = ((long double)(1.0)) + ( numerator / ((long double)iN) );
		numerator = numerator * numerator;
		totalnumerator += numerator;
	}
	
	//printf("totalcounts=%d\n",totalcounts[iL]);
	//printf("SumFSquared=%f\n",totalnumerator);
	denominator = ( pow( ((long double)iPhi) , ((long double)(iL + 1.0)) ) - ((long double)(1.0)) ) / ( (long double)(iPhi - 1.0) );
	//printf("denominator=%f\n",denominator);
	denominator = denominator * denominator;

	return ( totalnumerator / denominator );
}



void fastCalculateMS(){
	int i;
	int iN=sequencesize;
	int iL=lvalue;
	int iPhi=phivalue;
	double N=(double)sequencesize;
	double L=(double)lvalue;
	double Phi=(double)phivalue;
	//double mnumerator,mdenominator;
	//int *fourphikvector;
	//int *squaredcountsvector;
	long double lN=(long double)N;
	long double lPhi=(long double)Phi;
	long double mnumerator,mdenominator,entropy;
	long double *fourphikvector;
	long double *squaredcountsvector;
	int nodecount;
	treenode *currentnode;
	treenode **sidelinksvector;
	//squaredcountsvector=(int *)calloc((iL+1),sizeof(int));
	squaredcountsvector=(long double *)calloc((iL+1),sizeof(long double));
	sidelinksvector=getSideLinksVector();
	for(i=1;i<=iL;i++){
		currentnode=sidelinksvector[i];
		while(currentnode!=NULL){
			nodecount=(currentnode->count);
			squaredcountsvector[i]+=(long double)(nodecount*nodecount);
			currentnode=currentnode->sidelink;
		}
		//printf("squaredcountsvector[%Lf]=%d\n",i,squaredcountsvector[i]);
	}
	fourphikvector=(long double *)malloc((iL+1)*sizeof(long double));
	fourphikvector[0]=1.0;
	for(i=1;i<=iL;i++){
		fourphikvector[i] = (fourphikvector[i-1]) * ( (long double)(4 * iPhi) );
		//printf("fourphikvector[%d]=%Lf\n",i,fourphikvector[i]);
	}
	//mnumerator=0.0;
	//for(i=1;i<=iL;i++) mnumerator+=(double)((fourphikvector[i])*(squaredcountsvector[i]));
	//mnumerator = N + ( mnumerator / N );
	//mdenominator = ( pow( Phi , L + 1.0 ) - 1.0 ) / ( Phi - 1.0 );
	//mteta = ( mnumerator / ( N * mdenominator ) );
	mnumerator=0.0;
	for(i=1;i<=iL;i++){
		mnumerator+=(fourphikvector[i])*(squaredcountsvector[i]);
		//printf("(4phik*c^2)[%d]=%Lf\n",i,mnumerator);
	}
	
	mnumerator = lN + ( mnumerator / lN );
	//printf("mnumerator[10]=%Lf\n",mnumerator);
	mdenominator = (long double)( pow( lPhi , (long double)(L + 1.0) ) - (long double)(1.0) ) / ( (long double)(Phi - 1.0) );
	//printf("mdenominator[10]=%Lf\n",mdenominator);
	//printf("FSum=%f\n",(double)(mnumerator/mdenominator));
	mteta = (double)( mnumerator / ( lN * mdenominator ) );
	steta = 0.0;
	steta = getSumFSquared();
	//printf("SumFSquared=%f\n",steta);
	//steta = steta - ( 2.0 * mteta * (double)(mnumeratormdenominator) ) + ( N * mteta * mteta );
	steta = steta - ( mteta * mteta * N );
	steta = steta / ( N - 1.0 );
	steta = sqrt( steta );
	if(mteta<0 || steta<0) exitError("Possible overflow on normalization of f.");
}


char * parseStringArg(char **argslist, int argssize, char key){
	int i;
	for(i=0;i<argssize;i++)
		if(argslist[i][1]==key || ((char)((int)(argslist[i][1])+32))==key)
			if((int)strlen(argslist[i])>2) return (char *)(argslist[i]+2);
	return NULL;
}

char parseCharArg(char **argslist, int argssize, char key){
	int i;
	for(i=0;i<argssize;i++)
		if(argslist[i][1]==key || ((char)((int)(argslist[i][1])+32))==key)
			if((int)strlen(argslist[i])>2) return (char)(argslist[i][2]);
	return '\0';
}

int parseIntArg(char **argslist, int argssize, char key){
	int i;
	for(i=0;i<argssize;i++)
		if(argslist[i][1]==key || ((char)((int)(argslist[i][1])+32))==key)
			if((int)strlen(argslist[i])>2) return atoi(argslist[i]+2);
	return 0;
}

static int compareIt (const void * a, const void * b)
{
	struct EPVal *ia = (struct EPVal *)a;
	struct EPVal *ib = (struct EPVal *)b;

  if (ia->val > ib->val) return -1;
  else if (ia->val < ib->val) return 1;
  else return 0;  
}

int checkSubstring(char *text, int i1, int i2) {
	char *str1 = substr(text, i1 - lvalue + 1, i1 + 1);
	char *str2 = substr(text, i2 - lvalue + 1, i2 + 1);
	int cmp = ( strcmp(str1, str2) == 0 );
	return cmp;
}

int main(int argc, char *argv[]){

	int n,steps,limit,nbytes,nnodes,findmax;
	char *subsequence, *description;
	char *treefilename;
	char *motif;
	int loadedfromfile;

	
	filename=parseStringArg(argv,argc,'f');
	
	printf("Filename: '%s'\n",filename);
	
	printf("Loading the sequencefromfile...\n");

	sequencefromfile=loadSequence(filename);
	if(sequencefromfile==NULL) exitError("Invalid sequence file name.");
	
	printf("Load complete. \n");

	FILE *fileout;
	fileout=fopen( strcat ( substr(filename, 0, strlen(filename)-3) , "_output.txt"),"w");
	if(fileout==NULL) return -1;




	// for(int i=0; i<10; i++) {

		sequencetext=sequencefromfile->data;
		sequencesize=sequencefromfile->size;
		description=sequencefromfile->description;


		// lvalue=parseIntArg(argv,argc,'l');
		// if(sequencesize<lvalue) exitError("Sequence size too small.");
		//phivalue=parseIntArg(argv,argc,'p');

		lvalue = 7;
		phivalue=10;
		position=7;
		positionwindow = sequencesize;
		
		//if(positionwindow<1) positionwindow=1;
		//if(positionwindow>sequencesize) positionwindow=sequencesize;
		//if( ((position-positionwindow/2)<1) || ((position+positionwindow/2)>sequencesize) ) positionwindow=2*intmin(position,sequencesize-position);

		n=(int)strlen(filename);

		steps=0;
		limit=LIMIT;
		
		suffixtree=buildTree(sequencetext,limit,&steps);

		fastCalculateMS();
		saveDataToFile();
		
		fprintf(fileout, "M=%f\nS=%f\n\n",mteta,steta);
		
		// size_t len = (sequencesize+1)*sizeof(double);
		double maxDou = 0.0;
		// ypointslist=(double *)malloc(len);

	
		for(int j=1; j<sequencesize+1; j++) {
			double tt = g(lvalue,phivalue,j,sequencesize);
			
			if(j<=SIZE) {
				a[j-1].val = tt;
				a[j-1].ind = j;
			} else if( j > SIZE ) {
				double d = tt;

				int ii;

				if(d < a[SIZE-1].val) {
					continue;
				}


				int breakIt=0;
				for(ii=0; ii<SIZE; ii++) {
					if(a[ii].val == d) {
						if( checkSubstring(sequencetext, a[ii].ind, j) == 1 ) {
							breakIt = 1;
						}
					} else if(a[ii].val < d) {
						break;
					}
				}

				if(breakIt) {
					continue;
				}
				
				if(ii!=SIZE) {
					for(int i=SIZE-1; i>ii; i--) {
						a[i].val = a[i-1].val;
						a[i].ind = a[i-1].ind;
					}

					a[ii].val = d;
					a[ii].ind = j;
				}
			}

			if(j==SIZE) {
				qsort (a, SIZE, sizeof(struct EPVal), compareIt);

				for(int i = 0; i<SIZE; i++) {
					printf("%lf , %d\n", a[i].val, a[i].ind);
				}
				printf("\n");
			}
			
		}

		for(int i = 0; i<SIZE; i++) {
			fprintf(fileout, "%s\t%lf\n", substr(sequencetext, a[i].ind - lvalue + 1, a[i].ind + 1), a[i].val);
		}
		printf("\n");

	// }

	fclose(fileout);
	if(sequencefromfile!=NULL) freeSequence(sequencefromfile);
	freeTreeNode(suffixtree);
	freeVariables();

	
	free(subsequence);
	freeAlphabet();

	return 1;
}
