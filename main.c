#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h> 
#include "include/typedefines.h"
#include "include/data_structure.h"
#include "include/perf_test.h"
#include "include/poly.h"
#include "include/cohesive.h"
#include "include/cohesive3D.h"


#define EXCHANGE(num1, num2)  { num1 = num1 ^ num2;num2 = num1 ^ num2;num1 = num1 ^ num2;}

void buddleSort(int num[],int count)
{
    for (int i = 0; i < count - 1; i++) {

        for (int j = 0; j < count - i - 1; j++) {

            if (num[j] > num[j + 1]) EXCHANGE(num[j], num[j + 1]);
        }
    }
}


FileInfo InitFileInput();
void ReadData(PolyModel*, FileInfo);
void ReadElem(PolyModel*);
void ReadFace(PolyModel*);  
void ReadNode(PolyModel*); 

void Cohesive3D_Runtime (PolyModel *model, double runtime[]);
void PrintOut(PolyModel*);
void PrintOutSeq(PolyModel*);

void InsertCohesiveWithFile(PolyModel*,FILE *);

int main()
{
	FileInfo FileInfo = InitFileInput();
	PolyModel* model=polyModel_Create(FileInfo.Dim, FileInfo.NoE, FileInfo.NoF, FileInfo.NoN, 1);
	ReadData(model, FileInfo);
	polyModel_InitializeNE(model);
	
	
  PolyEdgeItr *editr = polyModel_CreateEdgeItr (model);

	for (polyEdgeItr_Begin(editr); polyEdgeItr_IsValid(editr); polyEdgeItr_Next(editr))
	{
	    PolyEdge edge = polyEdgeItr_GetCurr (editr);
//	    int t = polyEdge_GetNNodes(model, edge);
	    PolyNode node1 = polyEdge_GetNode (model, edge, 0);
	    PolyNode node2 = polyEdge_GetNode (model, edge, 1);
	    
	    printf("nid1 %d nid2 %d\n", polyNode_GetId(model, node1), polyNode_GetId(model, node2));
	}
//	FILE  *fpr;
//	fpr = fopen("/home/yu112358/Programs/All_Results/Time_Coh3D_240427.txt","a+");
//	
//	int i,j,k;
//	double runtime[20], total_runtime[20] = {0,};
//	struct rusage rusage_mem;
//	long mem_start, mem_end;
//	
//	
//	char *nowtime = getDateTime();
//	printf("%s\n",nowtime);
//	fprintf(fpr,"%s\n",nowtime);
//	
//	getrusage(RUSAGE_SELF, &rusage_mem);
//	mem_start = (float)rusage_mem.ru_maxrss/1E3;
//	printf("Current memory used at START: %ld MB %ld %ld kb \n", mem_start, rusage_mem.ru_idrss, rusage_mem.ru_isrss);
//	fprintf(fpr,"Current memory used at START: %ld MB %ld %ld kb \n", mem_start, rusage_mem.ru_idrss, rusage_mem.ru_isrss);
//	
//	int nelem = polyModel_GetNElems(model);
//	fprintf(fpr,"NoE : %d \n", nelem);
//
//	FILE * fpcsv = fopen("/home/yu112358/Programs/All_Results/Time_Coh3D_240427.csv", "a");  
//	fprintf(fpcsv,"%s\n",nowtime);
//	
//	int id;
//	int NumberOfTests=5;
//	
//	for (i=0; i<NumberOfTests; i++)
//	{
//		memset(runtime,0,sizeof(int)*20);
//		
//		Cohesive3D_Runtime (model, runtime);
//	  for (j=0; j<20; j++){
//	  	total_runtime[j]+= runtime[j];
//	    fprintf(fpr,"%d : %e\n", j, runtime[j]); 
//  	}
//	}
//	fprintf(fpcsv,"NoE : %d \n", nelem);
//	for (j=0; j<20; j++){
//	    fprintf(fpcsv, "%e\n", total_runtime[j]/NumberOfTests);
//  	}
//	
//	getrusage(RUSAGE_SELF, &rusage_mem);
//	mem_start = (float)rusage_mem.ru_maxrss/1E3;
//	printf("Current memory used at testend: %ld MB %ld %ld kb \n", mem_start, rusage_mem.ru_idrss, rusage_mem.ru_isrss);
//	fprintf(fpr,"Current memory used at testend: %ld MB %ld %ld kb \n", mem_start, rusage_mem.ru_idrss, rusage_mem.ru_isrss);
//
//	fclose(fpcsv);
	
	
	polyModel_Destroy (model);
	
//	getrusage(RUSAGE_SELF, &rusage_mem);
//	mem_start = (float)rusage_mem.ru_maxrss/1E3;
//	printf("Current memory used at codeend: %ld MB %ld %ld kb \n", mem_start, rusage_mem.ru_idrss, rusage_mem.ru_isrss);
//	fprintf(fpr,"Current memory used at codeend: %ld MB %ld %ld kb \n", mem_start, rusage_mem.ru_idrss, rusage_mem.ru_isrss);
//	
	return 0;
}


void Cohesive3D_Runtime (PolyModel *model, double runtime[])
{	
	struct timeval startTime, endTime;
	long long int t = 0;
	int i,j;
	double Time;
	int cnt = 0;
	int threshold = 0;
	int nface = polyModel_GetNFaces(model);
	PolyFaceItr* fitr = polyModel_CreateFaceItr(model);

	gettimeofday(&startTime, NULL);
	
	j=0;
	for (polyFaceItr_Begin(fitr); polyFaceItr_IsValid(fitr); polyFaceItr_Next(fitr)){	
		threshold = (int)((j+1)*nface/20);
//		printf("threshold : %d\n", threshold); 
		PolyFace face = polyFaceItr_GetCurr (fitr);
		int no = polyFace_GetId(model, face);
		Cohesive3D_InsertCohesiveAtFace_Auto(model,no);
		cnt++;
//		printf("cnt : %d\n", cnt); 
		if(cnt == threshold){
			gettimeofday(&endTime, NULL);
			t = (endTime.tv_sec - startTime.tv_sec) * 1000000 + (endTime.tv_usec - startTime.tv_usec);
			runtime[j] = 1.0 * t / 1000000; //1,000,000 converts time from usec to sec
//			printf("%d : %e\n", j, runtime[j]); 
			j++;
		}
		if(j==19) break;
	}
	polyFaceItr_Destroy(fitr);
}



void InsertCohesiveWithFile(PolyModel *model, FILE *fpr){
	char ch[400] = {0};
	int edgetail=0, edgehead=0;
	int reflist[11] = {0};
	int cnt = 0;
	while( fgets(ch, 400, fpr)!=NULL ){
//		if(cnt==19) break;
		
		printf("%s",ch);

    int i,j=0,n=0;
    for(i=0;ch[i]!='\0';i++){
      if(ch[i]>='0'&&ch[i]<='9'){
        n=n*10;
        n=n+(ch[i]-'0');
      }
      else{
    		reflist[j++]=n;
    		n=0;
      }
    }
		
		Cohesive3D_InsertCohesiveAtFace_Test(model, reflist);
		
//		Node Nodel=Face_GetNode(model, 20);
//		printf("Face 20: ");
//		for(i=0;i<Nodel.cnt;i++){
//			printf("%d ",Nodel.NodeInfo[i]);
//		}
//		printf("\n");
		cnt++;
	}
}




FileInfo InitFileInput(){
	FileInfo FileInfo;
	FileInfo.Dim=3;
	
	FILE *fp1, *fp2;
	memset(FileInfo.FileE,0,strlen(FileInfo.FileE));
	strcpy(FileInfo.FileE,"/home/yu112358/Programs/testElem3D13.txt");
	fp1 = fopen(FileInfo.FileE,"r+");
	if(fp1==NULL){
  	printf("Open E file failed\n");
  }
  
	memset(FileInfo.FileF,0,strlen(FileInfo.FileF));
	strcpy(FileInfo.FileF,"/home/yu112358/Programs/testFace3D13.txt");
	
	char strE[20], strF[20], strN[20];
		fscanf(fp1, "%s %s %s\n", strE, strF, strN); 
		FileInfo.NoE = atoi(strE);
		FileInfo.NoF = atoi(strF);
		FileInfo.NoN = atoi(strN);
	
	printf("%d %d %d %d\n", FileInfo.Dim, FileInfo.NoE, FileInfo.NoF, FileInfo.NoN);


	return FileInfo;
}

/**
	Face to Node in 2D and in 3D are consistent in our definition of the data structure . 
	And we state the Num. of Elements , Num. of Faces and Num. of Nodes first in FN file . 
	After we read the first line of this file , we update 
*/
void ReadData(PolyModel *model, FileInfo FileInfo)
{
	polyModel_SetFileInfo(model, FileInfo);
	
	ReadFace(model);
	ReadElem(model);
	ReadNode(model);
	
  return;
}

void ReadFace(PolyModel *model)
{
	FILE *fp;
	int face_id;
	int cnt;
	if (!(fp = fopen(model->FileInfo.FileF, "r+")))
	{
		printf("##Error## \"%s\" file cannot be opened!\n\n", model->FileInfo.FileF);
		exit(1);
	}
	
	char strA[20], strB[20];
	if(model->dimen==2){
		//The first line has to be ignored in 2D case. That is why we have to include 
		//this if-statement here.
		fscanf(fp, "%s %s\n", strA, strB); 
	}
	
	for (cnt = 1; cnt <= model->FileInfo.NoF; cnt++)
	{
		char ch[400] = {0};
		
		int temp_node_id[30]={0}; 
    fgets(ch, 400, fp);
    //printf("%s",ch);

    int i,j=0,n=0;
    for(i=0;ch[i]!='\0';i++){
      if(ch[i]>='0'&&ch[i]<='9'){
        n=n*10;
        n=n+(ch[i]-'0');
      }
      else{
      	if(j==0) face_id=n;
        if(j) temp_node_id[j-1]=n; //Remove face id when scanning .txt file
        j++;
        n=0;
      }
    }
    int size=0;
    while(temp_node_id[size++]!=0) ;
		size--;
		
		FACE *newface = (FACE*)malloc(sizeof(FACE));
		newface->Global_id = face_id;
		newface->AdjEnt_id = (int*)malloc(sizeof(int)*size); // malloc for list of face id of the faceent
		memcpy(newface->AdjEnt_id,temp_node_id,sizeof(int)*size);
		newface->AdjEnt_cnt = size; // Save the node size
		polyModel_InsertFace(model, newface); // Add new face to "FaceHash"
	}
	if(fp) fclose(fp);
	return;
}

void ReadElem(PolyModel *model)
{
	FILE *fp;
	int elem_id;
	int i,j;
	int cnt;
	if (!(fp = fopen(model->FileInfo.FileE, "r+")))
	{
		printf("##Error## \"%s\" file cannot be opened!\n\n", model->FileInfo.FileE);
		exit(1);
	}
	char str1[20], str2[20], str3[20];
	if(model->dimen==2){
		fscanf(fp, "%s %s\n", str1, str2);
		for (i = 1; i <= model->FileInfo.NoE; i++)
		{
			ELEM *newelem = (ELEM*)malloc(sizeof(ELEM));
			
			char ch[400] = {0};
	    fgets(ch, 400, fp);
	    //printf("%s",ch);

	    int i,j=0,n=0;
	    for(i=0;ch[i]!='\0';i++){
	      if(ch[i]>='0'&&ch[i]<='9'){
	        n=n*10;
	        n=n+(ch[i]-'0');
	      }
	      else{
	      	if(j==0) elem_id=n;
	        if(j) break; //Remove element id when scanning .txt file
	        j++;
	        n=0;
	      }
	    }

			newelem->Global_id = elem_id;
			newelem->Element_Type = POLY_ELEMENT_POLY2D;
			newelem->AdjEnt_id = (int*)malloc(sizeof(int)); // malloc for list of face id of the element
			newelem->AdjEnt_id[0] = elem_id;
			newelem->AdjEnt_cnt = 1; // Save the face size
			polyModel_InsertElem(model, newelem); // Add new element to "ElemHash"
		}
	}
		
	else if(model->dimen==3){
		fscanf(fp, "%s %s %s\n", str1, str2, str3);
		for (i = 1; i <= model->FileInfo.NoE; i++)
		{
			char ch[400] = {0};
			
			int temp_adj_id[100]; 
	    fgets(ch, 400, fp);
	    //printf("%s",ch);

	    int i,j=0,n=0;
	    for(i=0;ch[i]!='\0';i++){
	      if(ch[i]>='0'&&ch[i]<='9'){
	        n=n*10;
	        n=n+(ch[i]-'0');
	      }
	      else{
	      	if(j==0) elem_id=n;
	        if(j) temp_adj_id[j-1]=n; //Remove element id when scanning .txt file
	        j++;
	        n=0;
	      }
	    }
	    int size=0;
	    while(temp_adj_id[size++]!=0) ;
			size--;
			
			ELEM *newelem = (ELEM*)malloc(sizeof(ELEM));
			newelem->Global_id = elem_id;
			newelem->Element_Type = POLY_ELEMENT_POLY3D;
			newelem->AdjEnt_id = (int*)malloc(sizeof(int)*size); // malloc for list of face id of the element
			//newelem->faceNormal = (int*)malloc(sizeof(int)*size); // malloc for list of face normal vector : KP modification
			memcpy(newelem->AdjEnt_id,temp_adj_id,sizeof(int)*size);
			newelem->AdjEnt_cnt = size; // Save the face size
			polyModel_InsertElem(model, newelem); // Add new element to "ElemHash"
		}
	}
	if(fp) fclose(fp);
	return;
}

void ReadNode(PolyModel* model)
{
	int i,j;
	int cnt;
	for (i = 1; i <= model->FileInfo.NoN; i++)
	{
    NODE *newnode = (NODE*)malloc(sizeof(NODE));
		newnode->Global_id = i;
		newnode->x = 0;
		newnode->y = 0;
		newnode->z = 0;
		newnode->AdjEnt_cnt = 0;
		newnode->AdjEnt_id=NULL;
		newnode->isBoundary=0;
		polyModel_InsertNode(model, newnode);
		
	}

	return;
}


void PrintOutSeq(PolyModel* model){
	FILE *fprresultEN, *fprresultNE, *fprresultFN;
	fprresultEN = fopen("/home/yu112358/Programs/CompareTopSPolyCohesive3D/Test0/ResultPolyEN.txt","a+");
	fprresultNE = fopen("/home/yu112358/Programs/CompareTopSPolyCohesive3D/Test0/ResultPolyNE.txt","a+");
	FACE *face = NULL;
	int* node_id;
	int i,j;
	int cnt=0;

	int nelem = polyModel_GetNElems(model);

	for (j=1; j<=nelem; j++){	
		int en[10]={0,};
		Node Nodelist = Elem_GetNode(model, j);
		fprintf(fprresultEN,"%d ",j);
		for (i=0; i<Nodelist.cnt; i++){
			en[i]=Nodelist.NodeInfo[i];
		}
		buddleSort(en,Nodelist.cnt);
		for (i=0; i<Nodelist.cnt; i++)
		{
			fprintf(fprresultEN, "%d ", en[i]);
		}
		fprintf(fprresultEN,"\n");
	}
	fprintf(fprresultEN,"\n");
	if(fprresultEN) fclose(fprresultEN);
	
	
	int nnode = polyModel_GetNNodes(model);
	cnt=0;
	for (j=1; j<=nnode || cnt<nnode; j++){	
		int ne[100]={0,};
		Elem Elemlist = Node_GetElem(model, j);
		fprintf(fprresultNE,"%d ",j);
		if(Elemlist.cnt>0) cnt++;
		for (i=0; i<Elemlist.cnt; i++){
			ne[i]=Elemlist.ElemInfo[i];
		}
		buddleSort(ne,Elemlist.cnt);
		for (i=0; i<Elemlist.cnt; i++)
		{
			fprintf(fprresultNE, "%d ", ne[i]);
		}
		fprintf(fprresultNE,"\n");
	}
	fprintf(fprresultNE,"\n");
	if(fprresultNE) fclose(fprresultNE);
	
	
	
	return;
}
//
void PrintOut(PolyModel* model){
	FILE *fprresultEN, *fprresultNE, *fprresultFN;
	fprresultEN = fopen("/home/yu112358/Programs/CompareTopSPolyCohesive3D/Test0/ResultPolyEN.txt","a+");
	fprresultNE = fopen("/home/yu112358/Programs/CompareTopSPolyCohesive3D/Test0/ResultPolyNE.txt","a+");
	fprresultFN = fopen("/home/yu112358/Programs/CompareTopSPolyCohesive3D/Test0/ResultPolyFN.txt","a+");
	FACE *face = NULL;
	int* node_id;
	int i,j;
	int cnt=0;

	PolyElemItr* eitr = polyModel_CreateElemItr(model);

	for (polyElemItr_Begin(eitr); polyElemItr_IsValid(eitr); polyElemItr_Next(eitr)){	
		int en[10]={0,};
		PolyNode tmp;
		PolyElem elem = polyElemItr_GetCurr (eitr);
		fprintf(fprresultEN,"%d ",elem.id);
		int nelnode = polyElem_GetNNodes (model, elem);
		for (i=0; i<nelnode; i++){
			tmp = polyElem_GetNode (model, elem, i);
			en[i]=polyNode_GetId(model, tmp);
		}
		buddleSort(en,nelnode);
		for (i=0; i<nelnode; i++)
		{
			fprintf(fprresultEN, "%d ", en[i]);
		}
		fprintf(fprresultEN,"\n");
	}
	fprintf(fprresultEN,"\n");
	polyElemItr_Destroy(eitr);
	if(fprresultEN) fclose(fprresultEN);
	
	
	PolyNodeItr* nitr = polyModel_CreateNodeItr(model);

	for (polyNodeItr_Begin(nitr); polyNodeItr_IsValid(nitr); polyNodeItr_Next(nitr)){	
		int ne[100]={0,};
		PolyElem tmp;
		PolyNode Node = polyNodeItr_GetCurr (nitr);
		fprintf(fprresultNE,"%d ",Node.id);
		int no = polyNode_GetNElems (model, Node);
		for (i=0; i<no; i++){
			tmp = polyNode_GetElem (model, Node, i);
			ne[i]=polyElem_GetId(model, tmp);
			
		}
		buddleSort(ne,no);
		for (i=0; i<no; i++)
		{
			fprintf(fprresultNE,"%d ",ne[i]);
		}
		fprintf(fprresultNE,"\n");
	}

	fprintf(fprresultNE,"\n");
	polyNodeItr_Destroy(nitr);
	if(fprresultNE) fclose(fprresultNE);
	
	PolyFaceItr* fitr = polyModel_CreateFaceItr(model);

	for (polyFaceItr_Begin(fitr); polyFaceItr_IsValid(fitr); polyFaceItr_Next(fitr)){	
		PolyNode tmp;
		PolyFace Face = polyFaceItr_GetCurr (fitr);
		fprintf(fprresultFN,"%d ",Face.id);
		int no = polyFace_GetNNodes (model, Face);
		for (i=0; i<no; i++){
			tmp = polyFace_GetNode (model, Face, i);
			int t=polyNode_GetId(model, tmp);
			fprintf(fprresultFN,"%d ",t);
		}
		fprintf(fprresultFN,"\n");
	}

	fprintf(fprresultFN,"\n");
	polyFaceItr_Destroy(fitr);

	if(fprresultFN) fclose(fprresultFN);
		
	FILE *fprresultNF;
	fprresultNF = fopen("/home/yu112358/Programs/CompareTopSPolyCohesive3D/Test0/ResultPolyNF.txt","a+");
		
	nitr = polyModel_CreateNodeItr(model);

	for (polyNodeItr_Begin(nitr); polyNodeItr_IsValid(nitr); polyNodeItr_Next(nitr)){	
		PolyFace tmp;
		PolyNode Node = polyNodeItr_GetCurr (nitr);
		fprintf(fprresultNF,"%d ",Node.id);
		int no = polyNode_GetNFaces (model, Node);
		for (i=0; i<no; i++){
			tmp = polyNode_GetFace (model, Node, i);
			int t=polyFace_GetId(model, tmp);
			fprintf(fprresultNF,"%d ",t);
		}
		fprintf(fprresultNF,"\n");
	}

	fprintf(fprresultNF,"\n");
	polyNodeItr_Destroy(nitr);
	if(fprresultNF) fclose(fprresultNF);
		
		
		
		
	FILE *fprresultEF;
	fprresultEF = fopen("/home/yu112358/Programs/CompareTopSPolyCohesive3D/Test0/ResultPolyEF.txt","a+");
		
	eitr = polyModel_CreateElemItr(model);

	for (polyElemItr_Begin(eitr); polyElemItr_IsValid(eitr); polyElemItr_Next(eitr)){	
		PolyFace tmp;
		PolyElem Elem = polyElemItr_GetCurr (eitr);
		fprintf(fprresultEF,"%d ",Elem.id);
		int no = polyElem_GetNFaces (model, Elem);
		for (i=0; i<no; i++){
			tmp = polyElem_GetFace (model, Elem, i);
			int t=polyFace_GetId(model, tmp);
			fprintf(fprresultEF,"%d ",t);
		}
		fprintf(fprresultEF,"\n");
	}

	fprintf(fprresultEF,"\n");
	polyElemItr_Destroy(eitr);
	if(fprresultEF) fclose(fprresultEF);
	return;
}
