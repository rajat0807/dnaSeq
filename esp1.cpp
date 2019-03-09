#include "stdio.h"
#include <iostream>
#include "stdlib.h"
#include "string.h"
#include <vector>
#include "algorithm"
#include <time.h>
#include "sequencefile.h"

using namespace std;

#define MAXDESCSIZE 80
#define MAXDATASIZE 5000000

// Begins Suffix Arrays implementation
// O(n log n) - Manber and Myers algorithm
// Refer to "Suffix arrays: A new method for on-line txting searches",
// by Udi Manber and Gene Myers
 
//Usage:
// Fill txt with the characters of the txting.
// Call SuffixSort(n), where n is the length of the txting stored in txt.
// That's it!
 
//Output:
// SA = The suffix array. Contains the n suffixes of txt sorted in lexicographical order.
//       Each suffix is represented as a single integer (the SAition of txt where it starts).
// iSA = The inverse of the suffix array. iSA[i] = the index of the suffix txt[i..n)
//        in the SA array. (In other words, SA[i] = k <==> iSA[k] = i)
//        With this array, you can compare two suffixes in O(1): Suffix txt[i..n) is smaller
//        than txt[j..n) if and only if iSA[i] < iSA[j]
const int MAX = 100010000;
static char *filename = NULL;
static char *sequencetext = NULL;
static sequence *sequencefromfile = NULL;
static int sequencesize = 0;
FILE *fileout;
char txt[MAX]; //input
int iSA[MAX], SA[MAX]; //output
int cnt[MAX], next_[MAX]; //internal
bool bh[MAX], b2h[MAX];

struct childTabNode {
	int up, down, nextLIndex;
};
 
// Compares two suffixes according to their first characters
bool smaller_first_char(int a, int b){
  return txt[a] < txt[b];
}
 
void suffixSort(int n){
  //sort suffixes according to their first characters
  for (int i=0; i<n; ++i){
    SA[i] = i;
  }
  sort(SA, SA + n, smaller_first_char);
  //{SA contains the list of suffixes sorted by their first character}
 
  for (int i=0; i<n; ++i){
    bh[i] = i == 0 || txt[SA[i]] != txt[SA[i-1]];
    b2h[i] = false;
  }
 
  for (int h = 1; h < n; h <<= 1){
    //{bh[i] == false if the first h characters of SA[i-1] == the first h characters of SA[i]}
    int buckets = 0;
    for (int i=0, j; i < n; i = j){
      j = i + 1;
      while (j < n && !bh[j]) j++;
      next_[i] = j;
      buckets++;
    }
    if (buckets == n) break; // We are done! Lucky bastards!
    //{suffixes are separted in buckets containing txtings starting with the same h characters}
 
    for (int i = 0; i < n; i = next_[i]){
      cnt[i] = 0;
      for (int j = i; j < next_[i]; ++j){
        iSA[SA[j]] = i;
      }
    }
 
    cnt[iSA[n - h]]++;
    b2h[iSA[n - h]] = true;
    for (int i = 0; i < n; i = next_[i]){
      for (int j = i; j < next_[i]; ++j){
        int s = SA[j] - h;
        if (s >= 0){
          int head = iSA[s];
          iSA[s] = head + cnt[head]++;
          b2h[iSA[s]] = true;
        }
      }
      for (int j = i; j < next_[i]; ++j){
        int s = SA[j] - h;
        if (s >= 0 && b2h[iSA[s]]){
          for (int k = iSA[s]+1; !bh[k] && b2h[k]; k++) b2h[k] = false;
        }
      }
    }
    for (int i=0; i<n; ++i){
      SA[iSA[i]] = i;
      bh[i] |= b2h[i];
    }
  }
  for (int i=0; i<n; ++i){
    iSA[SA[i]] = i;
  }
}
// End of suffix array algorithm
 
 
// Begin of the O(n) longest common prefix algorithm
// Refer to "Linear-Time Longest-Common-Prefix Computation in Suffix
// Arrays and Its Applications" by Toru Kasai, Gunho Lee, Hiroki
// Arimura, Setsuo Arikawa, and Kunsoo Park.
int lcp[MAX];
// lcp[i] = length of the longest common prefix of suffix SA[i] and suffix SA[i-1]
// lcp[0] = 0
void getlcp(int n)
{
  for (int i=0; i<n; ++i) 
    iSA[SA[i]] = i;

  lcp[0] = 0;

  for (int i=0, h=0; i<n; ++i)
  {
    if (iSA[i] > 0)
    {
      int j = SA[iSA[i]-1];
      while (i + h < n && j + h < n && txt[i+h] == txt[j+h]) 
          h++;
      lcp[iSA[i]] = h;

      if (h > 0) 
        h--;
    }
  }


}




#include <stack>

struct lcp_interval {
	int lcpVal, lb, rb;
	vector <lcp_interval> intervalList;
};

void setIntervalDef(lcp_interval &l, int lcpVal = -1, int lb = -1, int rb = -1) {
	l.lcpVal = lcpVal; 
	l.lb = lb;
	l.rb = rb;
}

vector < lcp_interval > getChildIntervals(lcp_interval l, vector <childTabNode> childTab) {
	int i = l.lb;
	int j = l.rb;
	int i1,i2;
	vector <lcp_interval > childIntvl;

	if( childTab[j+1].up > i && childTab[j+1].up <= j)
		i1 = childTab[j+1].up;
	else
		i1 = childTab[i].down;



	lcp_interval t;
	setIntervalDef(t,-1, i, i1-1);
	childIntvl.push_back(t);


	while(childTab[i1].nextLIndex != 0) {
		i2 = childTab[i1].nextLIndex;
		setIntervalDef(t, -1, i1, i2-1);
		childIntvl.push_back(t);
		i1 = i2;
	}


	setIntervalDef(t,-1,i1,j);
	childIntvl.push_back(t);

	return childIntvl;

}


void lcp_intervals(vector<int> LCP, vector<int> ESA, vector < childTabNode > childTab) {
	stack<lcp_interval> S;
	lcp_interval l, lastInterval, def;
	setIntervalDef(lastInterval);
	setIntervalDef(l, 0, 0);
	int lb;
	int n  = (int)LCP.size();
	S.push(l);
	
	int c = 0;
	for(int i=1; i<=ESA.size(); i++) {
		// cout<<i<<" "<<S.size()<<endl;
		lb = i-1;
		int nextStart = 0, prevStart = -1;
		while(LCP[i-1] < S.top().lcpVal) {
			S.top().rb = i-1;
			lastInterval = S.top(); S.pop();

			int L = lastInterval.lb-1, R = lastInterval.rb-1;
			fprintf(fileout, "[%d-%d] %d\n", L, R, lastInterval.lcpVal);

			vector <int> lIndex;
			
			int prevEnd = -1, nextStart;

			if( lastInterval.intervalList.size() > 0 ) {

				for(int j = 0; j < lastInterval.intervalList.size(); j++) {

					nextStart = lastInterval.intervalList[j].lb - 1;
					for(int k = prevEnd+1; k < nextStart; k++) {
						lIndex.push_back(k);
					}


					// fprintf(fie, "%s\n", );
					fprintf(fileout, "%d %d\n", lastInterval.intervalList[j].lb-1, lastInterval.intervalList[j].rb-1);
					

					if(j!=0) {
						lIndex.push_back(lastInterval.intervalList[j].lb - 1);
					}

					prevEnd = lastInterval.intervalList[j].rb - 1;
					// cout<<"["<<lastInterval.intervalList[i].lb<<"-"<<lastInterval.intervalList[i].rb<<"]"<<endl;
				}

				nextStart = R+1;

				for(int k = prevEnd+1; k < nextStart; k++) {
					lIndex.push_back(k);
				}

				// for(int j = 0; j<lIndex.size(); j++) {
				// 	cout<<lIndex[j]<<" ";
				// }
				
				

			} else {
				int l = lastInterval.lb - 1;
				int r = lastInterval.rb - 1;


				for(int j=l+1; j<=r; j++) {
					lIndex.push_back(j);
				}

			}


			childTab[R+1].up = lIndex[0];
			childTab[L].down = lIndex[0];



			for(int j = 1; j<lIndex.size(); j++) {
				childTab[lIndex[j-1]].nextLIndex = lIndex[j];
			}



			lb = lastInterval.lb;
			if(LCP[i-1] <= S.top().lcpVal) {
				S.top().intervalList.push_back(lastInterval);
				setIntervalDef(lastInterval);
			}
		}
		if(LCP[i-1] > S.top().lcpVal) {
			lcp_interval intvl;
			setIntervalDef(intvl, LCP[i-1], lb);
			if( lastInterval.lb == -1 && lastInterval.rb == -1 ) {
				S.push(intvl);
			} else {
				intvl.intervalList.push_back(lastInterval);
				S.push(intvl);
				setIntervalDef(lastInterval);
			}
		}
	}

	vector <int> lIndex;
	// while(S.size()>0) {
	lastInterval = S.top();
	lastInterval.rb = n;
	S.pop();
	fprintf(fileout, "[%d-%d] %d\n", lastInterval.lb, lastInterval.rb-1, lastInterval.lcpVal);
	int prevEnd = -1, nextStart;
	int L = lastInterval.lb, R = lastInterval.rb - 1;
	for(int i = 0; i < lastInterval.intervalList.size(); i++) {
		nextStart = lastInterval.intervalList[i].lb - 1;
		for(int k = prevEnd+1; k < nextStart; k++) {
			lIndex.push_back(k);
		}

		lIndex.push_back(lastInterval.intervalList[i].lb - 1);
		prevEnd = lastInterval.intervalList[i].rb - 1;
		int l = lastInterval.intervalList[i].lb-1;
		int r = lastInterval.intervalList[i].rb-1;
		fprintf(fileout, "%d %d\n", l, r);
	}	
	nextStart = R+1;
	for(int k = prevEnd+1; k < nextStart; k++) {
		lIndex.push_back(k);
	}


	for(int i=0; i<lIndex.size(); i++) {
		cout<<lIndex[i]<<" ";
	}
	cout<<endl;

	// childTab[R+1].up = lIndex[0];
	// childTab[L].down = lIndex[0];

	for(int j = 1; j<lIndex.size(); j++) {
		childTab[lIndex[j-1]].nextLIndex = lIndex[j];		
	}


	for(int i = 0; i < childTab.size(); i++) {
		cout<<i<<" "<<childTab[i].up<<" "<<childTab[i].down<<" "<<childTab[i].nextLIndex<<endl;
	}

	
	setIntervalDef(l,-1,0,4);

	vector < lcp_interval > aa = getChildIntervals(l, childTab);

	for(int i=0; i<aa.size(); i++) {
		cout<<aa[i].lb-1<<" "<<aa[i].rb-1<<endl;
	}
	// }	

} 



char * getTimestamp(){
	time_t timevalue;
	time(&timevalue);
	return asctime(localtime(&timevalue));
}

char * parseStringArg(char **argslist, int argssize, char key){
	int i;
	for(i=0;i<argssize;i++)
		if(argslist[i][1]==key || ((char)((int)(argslist[i][1])+32))==key)
			if((int)strlen(argslist[i])>2) return (char *)(argslist[i]+2);
	return NULL;
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



// End of longest common prefixes algorithm
int main(int argc, char *argv[]){

	int len;

	char *description;

	filename=parseStringArg(argv,argc,'f');
	
	printf("Filename: '%s'\n",filename);
	
	printf("Loading the sequencefromfile...\n");

	sequencefromfile=loadSequence(filename);

	
	
	
	strcpy(txt, sequencefromfile->data);
	
	sequencesize=sequencefromfile->size;
	
	description=sequencefromfile->description;

	cout<<"Sequence - "<<sequencesize<<endl;

	suffixSort(sequencesize);
	getlcp(sequencesize);

	
	vector<int> LCP(lcp, lcp + sequencesize), ESA(SA, SA + sequencesize);
	vector< childTabNode > childTab(sequencesize+1);

	LCP.insert(LCP.end(), 0);

	ESA.insert(ESA.end(), sequencesize);

	// for(int i=0; i<LCP.size(); i++) {
	// 	cout<<LCP[i]<<" ";
	// }
	cout<<endl;



	cout<<"LCP complete."<<endl;
	
	fileout=fopen("ESAOutput.txt" ,"w");

	if(fileout==NULL) return -1;

	cout<<"Finding intervals"<<endl;

	lcp_intervals(LCP, ESA, childTab);

	cout<<"Done"<<endl;



	

	fclose(fileout);
   	return 0;
 }