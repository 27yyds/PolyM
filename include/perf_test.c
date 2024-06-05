#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h> 
#include "typedefines.h"
#include "data_structure.h"
#include "perf_test.h"
#include "poly.h"


void ALLTEST(PolyModel *model,int NumberOfTests){
	NumberOfTests=1;
	AllElem_Test(model,NumberOfTests);
	AllFace_Test(model,NumberOfTests);
	AllNode_Test(model,NumberOfTests);
	AllEdge_Test(model,NumberOfTests);

	EF_Test(model,NumberOfTests);
	FN_Test(model,NumberOfTests);
	NE_Test(model,NumberOfTests);
	Fe_Test(model,NumberOfTests);
	
	EN_Test(model,NumberOfTests);
	Ee_Test(model,NumberOfTests);

	FE_Test(model,NumberOfTests);
	NF_Test(model,NumberOfTests);
	Ne_Test(model,NumberOfTests);
	NN_Test(model,NumberOfTests);

	eE_Test(model,NumberOfTests);
	eF_Test(model,NumberOfTests);
	ee_Test(model,NumberOfTests);
	eN_Test(model,NumberOfTests);

	EE_Test(model,NumberOfTests);
	FF_Test(model,NumberOfTests);
}


void ALLPRINT(PolyModel *model,int NumberOfTests){
	NumberOfTests=1;
	AllElem_Print(model,NumberOfTests);
	AllFace_Print(model,NumberOfTests);
	AllNode_Print(model,NumberOfTests);
	AllEdge_Print(model,NumberOfTests);

	EF_Print(model,NumberOfTests);
	FN_Print(model,NumberOfTests);
	NE_Print(model,NumberOfTests);
	Fe_Print(model,NumberOfTests);
	
	EN_Print(model,NumberOfTests);
	Ee_Print(model,NumberOfTests);

	FE_Print(model,NumberOfTests);
	NF_Print(model,NumberOfTests);
	Ne_Print(model,NumberOfTests);
	NN_Print(model,NumberOfTests);

	eE_Print(model,NumberOfTests);
	eF_Print(model,NumberOfTests);
	ee_Print(model,NumberOfTests);
	eN_Print(model,NumberOfTests);

	EE_Print(model,NumberOfTests);
	FF_Print(model,NumberOfTests);
}


TimeList ALLTIME(PolyModel *model,int NumberOfTests){
	TimeList timelist;

	timelist.timeEF=EF_Time(model,NumberOfTests);
	timelist.timeFN=FN_Time(model,NumberOfTests);
	timelist.timeNE=NE_Time(model,NumberOfTests);
	timelist.timeFe=Fe_Time(model,NumberOfTests);
	timelist.timeEN=EN_Time(model,NumberOfTests);
	timelist.timeEe=Ee_Time(model,NumberOfTests);
	timelist.timeFE=FE_Time(model,NumberOfTests);
	timelist.timeNF=NF_Time(model,NumberOfTests);
	timelist.timeNe=Ne_Time(model,NumberOfTests);
	timelist.timeNN=NN_Time(model,NumberOfTests);
	timelist.timeeE=eE_Time(model,NumberOfTests);
	timelist.timeeF=eF_Time(model,NumberOfTests);
	timelist.timeee=ee_Time(model,NumberOfTests);
	timelist.timeeN=eN_Time(model,NumberOfTests);
	timelist.timeEE=EE_Time(model,NumberOfTests);
	timelist.timeFF=FF_Time(model,NumberOfTests);
	
	timelist.timeAE=AllElem_Time(model,NumberOfTests);
	timelist.timeAF=AllFace_Time(model,NumberOfTests);
	timelist.timeAN=AllNode_Time(model,NumberOfTests);
	timelist.timeAe=AllEdge_Time(model,NumberOfTests);
	return timelist;
}



void EE_Test(PolyModel *model,int NumberOfTests){
	int i,j;
	PolyElemItr* eitr = polyModel_CreateElemItr(model);

	for(j=0;j<NumberOfTests;j++){
		for (polyElemItr_Begin(eitr); polyElemItr_IsValid(eitr); polyElemItr_Next(eitr)){	

			PolyElem tmp;
			PolyElem elem = polyElemItr_GetCurr (eitr);
			printf("PolyElem: %d, ",elem.id);
			int no = polyElem_GetNElems (model, elem);
			printf("EE: %d, ",no);
			for (i=0; i<no; i++){
				tmp = polyElem_GetElem (model, elem, i);
				int t=polyElem_GetId(model, tmp);
				printf("%d ",t);
			}
			printf("\n");
		}
	}
	printf("\n");
	polyElemItr_Destroy(eitr);
	
	return;
}

void EF_Test(PolyModel *model,int NumberOfTests){
	int i,j;
	PolyElemItr* eitr = polyModel_CreateElemItr(model);
	
	for(j=0;j<NumberOfTests;j++){
		for (polyElemItr_Begin(eitr); polyElemItr_IsValid(eitr); polyElemItr_Next(eitr)){	

			PolyFace tmp;
			PolyElem elem = polyElemItr_GetCurr (eitr);
			printf("PolyElem: %d, ",elem.id);
			int no = polyElem_GetNFaces (model, elem);
			printf("EF: %d, ",no);
			for (i=0; i<no; i++){
				tmp = polyElem_GetFace (model, elem, i);
				int t=polyFace_GetId(model, tmp);
				printf("%d ",t);
			}
			printf("\n");
		}
	}
	printf("\n");
	polyElemItr_Destroy(eitr);
	
	return;
}

void Ee_Test(PolyModel *model,int NumberOfTests){
	int i,j;
	PolyElemItr* eitr = polyModel_CreateElemItr(model);
 	
	for(j=0;j<NumberOfTests;j++){
		for (polyElemItr_Begin(eitr); polyElemItr_IsValid(eitr); polyElemItr_Next(eitr)){	
			PolyEdge tmp;
			PolyElem elem = polyElemItr_GetCurr (eitr);
			printf("PolyElem: %d, ",elem.id);
			int no = polyElem_GetNEdges (model, elem);
			printf("Ee: %d, ",no);
			for (i=0; i<no; i++){
				tmp = polyElem_GetEdge (model, elem, i);
				
				printf("%d-%d ",tmp.EdgeTail,tmp.EdgeHead);
			}
			printf("\n");
		}
	}
	printf("\n");
	polyElemItr_Destroy(eitr);

	return;
}

void EN_Test(PolyModel *model,int NumberOfTests){
	int i,j;
	PolyElemItr* eitr = polyModel_CreateElemItr(model);

	for(j=0;j<NumberOfTests;j++){
		for (polyElemItr_Begin(eitr); polyElemItr_IsValid(eitr); polyElemItr_Next(eitr)){	

			PolyNode tmp;
			PolyElem elem = polyElemItr_GetCurr (eitr);
			printf("PolyElem: %d, ",elem.id);
			int nelnode = polyElem_GetNNodes (model, elem);
			printf("EN: %d, ",nelnode);
			for (i=0; i<nelnode; i++){
				tmp = polyElem_GetNode (model, elem, i);
				int t=polyNode_GetId(model, tmp);
				printf("%d ",t);
			}
			printf("\n");
		}
	}
	printf("\n");
	polyElemItr_Destroy(eitr);
	
	return;
}

void FE_Test(PolyModel *model,int NumberOfTests){
	int i,j;
	PolyFaceItr* fitr = polyModel_CreateFaceItr(model);

	for(j=0;j<NumberOfTests;j++){
		for (polyFaceItr_Begin(fitr); polyFaceItr_IsValid(fitr); polyFaceItr_Next(fitr)){	

			PolyElem tmp;
			PolyFace face = polyFaceItr_GetCurr (fitr);
			printf("PolyFace: %d, ",face.id);
			int no = polyFace_GetNElems (model, face);
			printf("FE: %d, ",no);
			for (i=0; i<no; i++){
				tmp = polyFace_GetElem (model, face, i);
				int t=polyElem_GetId(model, tmp);
				printf("%d ",t);
			}
			printf("\n");
		}
	}
	printf("\n");
	polyFaceItr_Destroy(fitr);
	
	return;
}

void FF_Test(PolyModel *model,int NumberOfTests){
	int i,j;
	PolyFaceItr* fitr = polyModel_CreateFaceItr(model);

	for(j=0;j<NumberOfTests;j++){
		for (polyFaceItr_Begin(fitr); polyFaceItr_IsValid(fitr); polyFaceItr_Next(fitr)){	

			PolyFace tmp;
			PolyFace face = polyFaceItr_GetCurr (fitr);
			printf("PolyFace: %d, ",face.id);
			int no = polyFace_GetNFaces (model, face);
			printf("FF: %d, ",no);
			for (i=0; i<no; i++){
				tmp = polyFace_GetFace (model, face, i);
				int t=polyFace_GetId(model, tmp);
				printf("%d ",t);
			}
			printf("\n");
		}
	}
	printf("\n");
	polyFaceItr_Destroy(fitr);
	
	return;
}

void Fe_Test(PolyModel *model,int NumberOfTests){
	int i,j;
	PolyFaceItr* fitr = polyModel_CreateFaceItr(model);
 	
	for(j=0;j<NumberOfTests;j++){
		for (polyFaceItr_Begin(fitr); polyFaceItr_IsValid(fitr); polyFaceItr_Next(fitr)){	
			PolyEdge tmp;
			PolyFace face = polyFaceItr_GetCurr (fitr);
			printf("PolyFace: %d, ",face.id);
			int no = polyFace_GetNEdges (model, face);
			printf("Fe: %d, ",no);
			for (i=0; i<no; i++){
				tmp = polyFace_GetEdge (model, face, i);
				
				printf("%d-%d ",tmp.EdgeTail,tmp.EdgeHead);
			}
			printf("\n");
		}
	}
	printf("\n");
	polyFaceItr_Destroy(fitr);

	return;
}

void FN_Test(PolyModel *model,int NumberOfTests){
	int i,j;
	PolyFaceItr* fitr = polyModel_CreateFaceItr(model);

	for(j=0;j<NumberOfTests;j++){
		for (polyFaceItr_Begin(fitr); polyFaceItr_IsValid(fitr); polyFaceItr_Next(fitr)){	

			PolyNode tmp;
			PolyFace face = polyFaceItr_GetCurr (fitr);
			printf("PolyFace: %d, ",face.id);
			int no = polyFace_GetNNodes (model, face);
			printf("FN: %d, ",no);
			for (i=0; i<no; i++){
				tmp = polyFace_GetNode (model, face, i);
				int t=polyNode_GetId(model, tmp);
				printf("%d ",t);
			}
			printf("\n");
		}
	}
	printf("\n");
	polyFaceItr_Destroy(fitr);
	
	return;
}

void eE_Test(PolyModel *model,int NumberOfTests){
	int i,j;
	PolyEdgeItr* editr = polyModel_CreateEdgeItr(model);

	for(j=0;j<NumberOfTests;j++){
		for (polyEdgeItr_Begin(editr); polyEdgeItr_IsValid(editr); polyEdgeItr_Next(editr)){	
			PolyElem tmp;
			PolyEdge Edge = polyEdgeItr_GetCurr (editr);
			printf("PolyEdge: %d - %d, ",Edge.EdgeTail,Edge.EdgeHead);
			int no = polyEdge_GetNElems (model, Edge);
			printf("eE: %d, ",no);
			for (i=0; i<no; i++){
				tmp = polyEdge_GetElem (model, Edge, i);
				int t=polyElem_GetId(model, tmp);
				printf("%d ",t);
			}
			printf("\n");
		}
	}
	printf("\n");
	polyEdgeItr_Destroy(editr);

	return;
}

void eF_Test(PolyModel *model,int NumberOfTests){
	int i,j;
	PolyEdgeItr* editr = polyModel_CreateEdgeItr(model);

	for(j=0;j<NumberOfTests;j++){
		for (polyEdgeItr_Begin(editr); polyEdgeItr_IsValid(editr); polyEdgeItr_Next(editr)){

			PolyFace tmp;
			PolyEdge edge = polyEdgeItr_GetCurr (editr);
			printf("PolyEdge: %d - %d, ",edge.EdgeTail,edge.EdgeHead);
			int no = polyEdge_GetNFaces (model, edge);
			printf("eF: %d, ",no);
			for (i=0; i<no; i++){
				tmp = polyEdge_GetFace (model, edge, i);
				int t=polyFace_GetId(model, tmp);
				printf("%d ",t);
			}
			printf("\n");
		}
	}
	printf("\n");
	polyEdgeItr_Destroy(editr);
	
	return;
}

void ee_Test(PolyModel *model,int NumberOfTests){
	int i,j;
	PolyEdgeItr* editr = polyModel_CreateEdgeItr(model);

	for(j=0;j<NumberOfTests;j++){
		for (polyEdgeItr_Begin(editr); polyEdgeItr_IsValid(editr); polyEdgeItr_Next(editr)){	
			PolyEdge tmp;
			PolyEdge Edge = polyEdgeItr_GetCurr (editr);
			printf("PolyEdge: %d - %d, ",Edge.EdgeTail,Edge.EdgeHead);
			int no = polyEdge_GetNEdges (model, Edge);
			printf("ee: %d, ",no);
			for (i=0; i<no; i++){
				tmp = polyEdge_GetEdge (model, Edge, i);
				printf("%d-%d ",tmp.EdgeTail,tmp.EdgeHead);
			}
			printf("\n");
		}
	}
	printf("\n");
	polyEdgeItr_Destroy(editr);

	return;
}

void eN_Test(PolyModel *model,int NumberOfTests){
	int i,j;
	PolyEdgeItr* editr = polyModel_CreateEdgeItr(model);

	for(j=0;j<NumberOfTests;j++){
		for (polyEdgeItr_Begin(editr); polyEdgeItr_IsValid(editr); polyEdgeItr_Next(editr)){	
			PolyNode tmp;
			PolyEdge Edge = polyEdgeItr_GetCurr (editr);
			printf("PolyEdge: %d - %d, ",Edge.EdgeTail,Edge.EdgeHead);
			int no = polyEdge_GetNNodes(model, Edge);
			printf("eN: %d, ",no);
			for (i=0; i<no; i++){
				tmp = polyEdge_GetNode(model, Edge, i);
				int t=polyNode_GetId(model, tmp);
				printf("%d ",t);
			}
			printf("\n");
		}
	}
	printf("\n");
	polyEdgeItr_Destroy(editr);

	return;
}

void NE_Test(PolyModel *model,int NumberOfTests){
	int i,j;
	PolyNodeItr* nitr = polyModel_CreateNodeItr(model);

	for(j=0;j<NumberOfTests;j++){
		for (polyNodeItr_Begin(nitr); polyNodeItr_IsValid(nitr); polyNodeItr_Next(nitr)){	
			PolyElem tmp;
			PolyNode Node = polyNodeItr_GetCurr (nitr);
			printf("PolyNode: %d, ",Node.id);
			int no = polyNode_GetNElems (model, Node);
			printf("NE: %d, ",no);
			for (i=0; i<no; i++){
				tmp = polyNode_GetElem (model, Node, i);
				int t=polyElem_GetId(model, tmp);
				printf("%d ",t);
			}
			printf("\n");
		}
	}
	printf("\n");
	polyNodeItr_Destroy(nitr);
	
	return;
}

void NF_Test(PolyModel *model,int NumberOfTests){
	int i,j;
	PolyNodeItr* nitr = polyModel_CreateNodeItr(model);
	
	for(j=0;j<NumberOfTests;j++){
		for (polyNodeItr_Begin(nitr); polyNodeItr_IsValid(nitr); polyNodeItr_Next(nitr)){	

			PolyFace tmp;
			PolyNode node = polyNodeItr_GetCurr (nitr);
			printf("PolyNode: %d, ",node.id);
			int no = polyNode_GetNFaces (model, node);
			printf("NF: %d, ",no);
			for (i=0; i<no; i++){
				tmp = polyNode_GetFace (model, node, i);
				int t=polyFace_GetId(model, tmp);
				printf("%d ",t);
			}
			printf("\n");
		}
	}
	printf("\n");
	polyNodeItr_Destroy(nitr);
	
	return;
}

void Ne_Test(PolyModel *model,int NumberOfTests){
	int i,j;
	PolyNodeItr* nitr = polyModel_CreateNodeItr(model);
 	
	for(j=0;j<NumberOfTests;j++){
		for (polyNodeItr_Begin(nitr); polyNodeItr_IsValid(nitr); polyNodeItr_Next(nitr)){	
			PolyEdge tmp;
			PolyNode elem = polyNodeItr_GetCurr (nitr);
			printf("PolyNode: %d, ",elem.id);
			int no = polyNode_GetNEdges (model, elem);
			printf("Ne: %d, ",no);
			for (i=0; i<no; i++){
				tmp = polyNode_GetEdge (model, elem, i);
				
				printf("%d-%d ",tmp.EdgeTail,tmp.EdgeHead);
			}
			printf("\n");
		}
	}
	printf("\n");
	polyNodeItr_Destroy(nitr);
	
	return;
}

void NN_Test(PolyModel *model,int NumberOfTests){
	int i,j;
	PolyNodeItr* nitr = polyModel_CreateNodeItr(model);

	for(j=0;j<NumberOfTests;j++){
		for (polyNodeItr_Begin(nitr); polyNodeItr_IsValid(nitr); polyNodeItr_Next(nitr)){	
			PolyNode tmp;
			PolyNode Node = polyNodeItr_GetCurr (nitr);
			printf("PolyNode: %d, ",Node.id);
			int no = polyNode_GetNNodes (model, Node);
			printf("NN: %d, ",no);
			for (i=0; i<no; i++){
				tmp = polyNode_GetNode (model, Node, i);
				int t=polyNode_GetId(model, tmp);
				printf("%d ",t);
			}
			printf("\n");
		}
	}
	printf("\n");
	polyNodeItr_Destroy(nitr);

	return;
}




void AllElem_Test(PolyModel *model,int NumberOfTests){
	int i,j;
	PolyElemItr* eitr = polyModel_CreateElemItr(model);
	
	for(j=0;j<NumberOfTests;j++){
		for (polyElemItr_Begin(eitr); polyElemItr_IsValid(eitr); polyElemItr_Next(eitr)){	

			PolyElem elem = polyElemItr_GetCurr (eitr);
			printf("PolyElem: %d, ",elem.id);
			printf("\n");
		}
	}
	printf("\n");
	polyElemItr_Destroy(eitr);
	
	return;
}

void AllFace_Test(PolyModel *model,int NumberOfTests){
	int i,j;
	PolyFaceItr* fitr = polyModel_CreateFaceItr(model);

	for(j=0;j<NumberOfTests;j++){
		for (polyFaceItr_Begin(fitr); polyFaceItr_IsValid(fitr); polyFaceItr_Next(fitr)){	
			PolyFace face = polyFaceItr_GetCurr (fitr);
			printf("PolyFace: %d, ",face.id);
			printf("\n");
		}
	}
	printf("\n");
	polyFaceItr_Destroy(fitr);
	
	return;
}

void AllNode_Test(PolyModel *model,int NumberOfTests){
	int i,j;
	PolyNodeItr* nitr = polyModel_CreateNodeItr(model);
 	
	for(j=0;j<NumberOfTests;j++){
		for (polyNodeItr_Begin(nitr); polyNodeItr_IsValid(nitr); polyNodeItr_Next(nitr)){	
			PolyNode elem = polyNodeItr_GetCurr (nitr);
			printf("PolyNode: %d, ",elem.id);
			printf("\n");
		}
	}
	printf("\n");
	polyNodeItr_Destroy(nitr);
	
	return;
}

void AllEdge_Test(PolyModel *model,int NumberOfTests){
	int i,j;
	PolyEdgeItr* editr = polyModel_CreateEdgeItr(model);

	for(j=0;j<NumberOfTests;j++){
		for (polyEdgeItr_Begin(editr); polyEdgeItr_IsValid(editr); polyEdgeItr_Next(editr)){	
			PolyNode tmp;
			PolyEdge Edge = polyEdgeItr_GetCurr (editr);
			int no = polyEdge_GetNNodes (model, Edge);
			for (i=0; i<no; i++){
				tmp = polyEdge_GetNode (model, Edge, i);
				int t=polyNode_GetId(model, tmp);
			}
		}
	}
	polyEdgeItr_Destroy(editr);

	return;
}









void EE_Print(PolyModel *model,int NumberOfTests){
	int i,j;
	PolyElemItr* eitr = polyModel_CreateElemItr(model);
 	FILE *fppr;      
 	fppr = fopen(model->FileInfo.FileO,"a+");
	fprintf(fppr,"EE\n");

	for(j=0;j<NumberOfTests;j++){
		for (polyElemItr_Begin(eitr); polyElemItr_IsValid(eitr); polyElemItr_Next(eitr)){	

			PolyElem tmp;
			PolyElem elem = polyElemItr_GetCurr (eitr);
			fprintf(fppr,"PolyElem: %d, ",elem.id);
			int no = polyElem_GetNElems (model, elem);
			fprintf(fppr,"EE: %d, ",no);
			for (i=0; i<no; i++){
				tmp = polyElem_GetElem (model, elem, i);
				int t=polyElem_GetId(model, tmp);
				fprintf(fppr,"%d ",t);
			}
			fprintf(fppr,"\n");
		}
	}
	fprintf(fppr,"\n");
	polyElemItr_Destroy(eitr);
	fclose(fppr);
	return;
}

void EF_Print(PolyModel *model,int NumberOfTests){
	int i,j;
	PolyElemItr* eitr = polyModel_CreateElemItr(model);
 	FILE *fppr;      
 	fppr = fopen(model->FileInfo.FileO,"a+");
 	fprintf(fppr,"EF\n");
 	
	for(j=0;j<NumberOfTests;j++){
		for (polyElemItr_Begin(eitr); polyElemItr_IsValid(eitr); polyElemItr_Next(eitr)){	

			PolyFace tmp;
			PolyElem elem = polyElemItr_GetCurr (eitr);
			fprintf(fppr,"PolyElem: %d, ",elem.id);
			int no = polyElem_GetNFaces (model, elem);
			fprintf(fppr,"EF: %d, ",no);
			for (i=0; i<no; i++){
				tmp = polyElem_GetFace (model, elem, i);
				int t=polyFace_GetId(model, tmp);
				fprintf(fppr,"%d ",t);
			}
			fprintf(fppr,"\n");
		}
	}
	fprintf(fppr,"\n");
	polyElemItr_Destroy(eitr);
	fclose(fppr);
	return;
}

void Ee_Print(PolyModel *model,int NumberOfTests){
	int i,j;
	PolyElemItr* eitr = polyModel_CreateElemItr(model);
 	FILE *fppr;      
 	fppr = fopen(model->FileInfo.FileO,"a+");
 	fprintf(fppr,"Ee\n");
 	
	for(j=0;j<NumberOfTests;j++){
		for (polyElemItr_Begin(eitr); polyElemItr_IsValid(eitr); polyElemItr_Next(eitr)){	
			PolyEdge tmp;
			PolyElem elem = polyElemItr_GetCurr (eitr);
			fprintf(fppr,"PolyElem: %d, ",elem.id);
			int no = polyElem_GetNEdges (model, elem);
			fprintf(fppr,"Ee: %d, ",no);
			for (i=0; i<no; i++){
				tmp = polyElem_GetEdge (model, elem, i);
				
				fprintf(fppr,"%d-%d ",tmp.EdgeTail,tmp.EdgeHead);
			}
			fprintf(fppr,"\n");
		}
	}
	fprintf(fppr,"\n");
	polyElemItr_Destroy(eitr);

	fclose(fppr);
	return;
}

void EN_Print(PolyModel *model,int NumberOfTests){
	int i,j;
	PolyElemItr* eitr = polyModel_CreateElemItr(model);
 	FILE *fppr;      
 	fppr = fopen(model->FileInfo.FileO,"a+");
	fprintf(fppr,"EN\n");
	
	for(j=0;j<NumberOfTests;j++){
		for (polyElemItr_Begin(eitr); polyElemItr_IsValid(eitr); polyElemItr_Next(eitr)){	

			PolyNode tmp;
			PolyElem elem = polyElemItr_GetCurr (eitr);
			fprintf(fppr,"PolyElem: %d, ",elem.id);
			int nelnode = polyElem_GetNNodes (model, elem);
			fprintf(fppr,"EN: %d, ",nelnode);
			for (i=0; i<nelnode; i++){
				tmp = polyElem_GetNode (model, elem, i);
				int t=polyNode_GetId(model, tmp);
				fprintf(fppr,"%d ",t);
			}
			fprintf(fppr,"\n");
		}
	}
	fprintf(fppr,"\n");
	polyElemItr_Destroy(eitr);
	
	fclose(fppr);
	return;
}

void FE_Print(PolyModel *model,int NumberOfTests){
	int i,j;
	PolyFaceItr* fitr = polyModel_CreateFaceItr(model);
 	FILE *fppr;      
 	fppr = fopen(model->FileInfo.FileO,"a+");
 	fprintf(fppr,"FE\n");

	for(j=0;j<NumberOfTests;j++){
		for (polyFaceItr_Begin(fitr); polyFaceItr_IsValid(fitr); polyFaceItr_Next(fitr)){	

			PolyElem tmp;
			PolyFace face = polyFaceItr_GetCurr (fitr);
			fprintf(fppr,"PolyFace: %d, ",face.id);
			int no = polyFace_GetNElems (model, face);
			fprintf(fppr,"FE: %d, ",no);
			for (i=0; i<no; i++){
				tmp = polyFace_GetElem (model, face, i);
				int t=polyElem_GetId(model, tmp);
				fprintf(fppr,"%d ",t);
			}
			fprintf(fppr,"\n");
		}
	}
	fprintf(fppr,"\n");
	polyFaceItr_Destroy(fitr);
	
	fclose(fppr);
	return;
}

void FF_Print(PolyModel *model,int NumberOfTests){
	int i,j;
	PolyFaceItr* fitr = polyModel_CreateFaceItr(model);
 	FILE *fppr;      
 	fppr = fopen(model->FileInfo.FileO,"a+");
 	fprintf(fppr,"FF\n");

	for(j=0;j<NumberOfTests;j++){
		for (polyFaceItr_Begin(fitr); polyFaceItr_IsValid(fitr); polyFaceItr_Next(fitr)){	

			PolyFace tmp;
			PolyFace face = polyFaceItr_GetCurr (fitr);
			fprintf(fppr,"PolyFace: %d, ",face.id);
			int no = polyFace_GetNFaces (model, face);
			fprintf(fppr,"FF: %d, ",no);
			for (i=0; i<no; i++){
				tmp = polyFace_GetFace (model, face, i);
				int t=polyFace_GetId(model, tmp);
				fprintf(fppr,"%d ",t);
			}
			fprintf(fppr,"\n");
		}
	}
	fprintf(fppr,"\n");
	polyFaceItr_Destroy(fitr);
	
	fclose(fppr);
	return;
}

void Fe_Print(PolyModel *model,int NumberOfTests){
	int i,j;
	PolyFaceItr* fitr = polyModel_CreateFaceItr(model);
 	FILE *fppr;      
 	fppr = fopen(model->FileInfo.FileO,"a+");
 	fprintf(fppr,"Fe\n");
 	
	for(j=0;j<NumberOfTests;j++){
		for (polyFaceItr_Begin(fitr); polyFaceItr_IsValid(fitr); polyFaceItr_Next(fitr)){	
			PolyEdge tmp;
			PolyFace face = polyFaceItr_GetCurr (fitr);
			fprintf(fppr,"PolyFace: %d, ",face.id);
			int no = polyFace_GetNEdges (model, face);
			fprintf(fppr,"Fe: %d, ",no);
			for (i=0; i<no; i++){
				tmp = polyFace_GetEdge (model, face, i);
				
				fprintf(fppr,"%d-%d ",tmp.EdgeTail,tmp.EdgeHead);
			}
			fprintf(fppr,"\n");
		}
	}
	fprintf(fppr,"\n");
	polyFaceItr_Destroy(fitr);

	fclose(fppr);
	return;
}

void FN_Print(PolyModel *model,int NumberOfTests){
	int i,j;
	PolyFaceItr* fitr = polyModel_CreateFaceItr(model);
 	FILE *fppr;      
 	fppr = fopen(model->FileInfo.FileO,"a+");
 	fprintf(fppr,"FN\n");

	for(j=0;j<NumberOfTests;j++){
		for (polyFaceItr_Begin(fitr); polyFaceItr_IsValid(fitr); polyFaceItr_Next(fitr)){	

			PolyNode tmp;
			PolyFace face = polyFaceItr_GetCurr (fitr);
			fprintf(fppr,"PolyFace: %d, ",face.id);
			int no = polyFace_GetNNodes (model, face);
			fprintf(fppr,"FN: %d, ",no);
			for (i=0; i<no; i++){
				tmp = polyFace_GetNode (model, face, i);
				int t=polyNode_GetId(model, tmp);
				fprintf(fppr,"%d ",t);
			}
			fprintf(fppr,"\n");
		}
	}
	fprintf(fppr,"\n");
	polyFaceItr_Destroy(fitr);
	
	fclose(fppr);
	return;
}

void eE_Print(PolyModel *model,int NumberOfTests){
	int i,j;
	PolyEdgeItr* editr = polyModel_CreateEdgeItr(model);
 	FILE *fppr;      
 	fppr = fopen(model->FileInfo.FileO,"a+");
	fprintf(fppr,"eE\n"); 	

	for(j=0;j<NumberOfTests;j++){
		for (polyEdgeItr_Begin(editr); polyEdgeItr_IsValid(editr); polyEdgeItr_Next(editr)){	
			PolyElem tmp;
			PolyEdge Edge = polyEdgeItr_GetCurr (editr);
			fprintf(fppr,"PolyEdge: %d - %d, ",Edge.EdgeTail,Edge.EdgeHead);
			int no = polyEdge_GetNElems (model, Edge);
			fprintf(fppr,"eE: %d, ",no);
			for (i=0; i<no; i++){
				tmp = polyEdge_GetElem (model, Edge, i);
				int t=polyElem_GetId(model, tmp);
				fprintf(fppr,"%d ",t);
			}
			fprintf(fppr,"\n");
		}
	}
	fprintf(fppr,"\n");
	polyEdgeItr_Destroy(editr);

	fclose(fppr);
	return;
}

void eF_Print(PolyModel *model,int NumberOfTests){
	int i,j;
	PolyEdgeItr* editr = polyModel_CreateEdgeItr(model);
 	FILE *fppr;      
 	fppr = fopen(model->FileInfo.FileO,"a+");
	fprintf(fppr,"eF\n"); 	

	for(j=0;j<NumberOfTests;j++){
		for (polyEdgeItr_Begin(editr); polyEdgeItr_IsValid(editr); polyEdgeItr_Next(editr)){

			PolyFace tmp;
			PolyEdge edge = polyEdgeItr_GetCurr (editr);
			fprintf(fppr,"PolyEdge: %d - %d, ",edge.EdgeTail,edge.EdgeHead);
			int no = polyEdge_GetNFaces (model, edge);
			fprintf(fppr,"eF: %d, ",no);
			for (i=0; i<no; i++){
				tmp = polyEdge_GetFace (model, edge, i);
				int t=polyFace_GetId(model, tmp);
				fprintf(fppr,"%d ",t);
			}
			fprintf(fppr,"\n");
		}
	}
	fprintf(fppr,"\n");
	polyEdgeItr_Destroy(editr);
	
	fclose(fppr);
	return;
}

void ee_Print(PolyModel *model,int NumberOfTests){
	int i,j;
	PolyEdgeItr* editr = polyModel_CreateEdgeItr(model);
 	FILE *fppr;      
 	fppr = fopen(model->FileInfo.FileO,"a+");
	fprintf(fppr,"ee\n"); 	

	for(j=0;j<NumberOfTests;j++){
		for (polyEdgeItr_Begin(editr); polyEdgeItr_IsValid(editr); polyEdgeItr_Next(editr)){	
			PolyEdge tmp;
			PolyEdge Edge = polyEdgeItr_GetCurr (editr);
			fprintf(fppr,"PolyEdge: %d - %d, ",Edge.EdgeTail,Edge.EdgeHead);
			int no = polyEdge_GetNEdges (model, Edge);
			fprintf(fppr,"ee: %d, ",no);
			for (i=0; i<no; i++){
				tmp = polyEdge_GetEdge (model, Edge, i);
				fprintf(fppr,"%d-%d ",tmp.EdgeTail,tmp.EdgeHead);
			}
			fprintf(fppr,"\n");
		}
	}
	fprintf(fppr,"\n");
	polyEdgeItr_Destroy(editr);

	fclose(fppr);
	return;
}

void eN_Print(PolyModel *model,int NumberOfTests){
	int i,j;
	PolyEdgeItr* editr = polyModel_CreateEdgeItr(model);
 	FILE *fppr;      
 	fppr = fopen(model->FileInfo.FileO,"a+");
	fprintf(fppr,"eN\n"); 	

	for(j=0;j<NumberOfTests;j++){
		for (polyEdgeItr_Begin(editr); polyEdgeItr_IsValid(editr); polyEdgeItr_Next(editr)){	
			PolyNode tmp;
			PolyEdge Edge = polyEdgeItr_GetCurr (editr);
			fprintf(fppr,"PolyEdge: %d - %d, ",Edge.EdgeTail,Edge.EdgeHead);
			int no = polyEdge_GetNNodes(model, Edge);
			fprintf(fppr,"eN: %d, ",no);
			for (i=0; i<no; i++){
				tmp = polyEdge_GetNode(model, Edge, i);
				int t=polyNode_GetId(model, tmp);
				fprintf(fppr,"%d ",t);
			}
			fprintf(fppr,"\n");
		}
	}
	fprintf(fppr,"\n");
	polyEdgeItr_Destroy(editr);

	fclose(fppr);
	return;
}

void NE_Print(PolyModel *model,int NumberOfTests){
	int i,j;
	PolyNodeItr* nitr = polyModel_CreateNodeItr(model);
 	FILE *fppr;      
 	fppr = fopen(model->FileInfo.FileO,"a+");
	fprintf(fppr,"NE\n"); 	

	for(j=0;j<NumberOfTests;j++){
		for (polyNodeItr_Begin(nitr); polyNodeItr_IsValid(nitr); polyNodeItr_Next(nitr)){	
			PolyElem tmp;
			PolyNode Node = polyNodeItr_GetCurr (nitr);
			fprintf(fppr,"PolyNode: %d, ",Node.id);
			int no = polyNode_GetNElems (model, Node);
			fprintf(fppr,"NE: %d, ",no);
			for (i=0; i<no; i++){
				tmp = polyNode_GetElem (model, Node, i);
				int t=polyElem_GetId(model, tmp);
				fprintf(fppr,"%d ",t);
			}
			fprintf(fppr,"\n");
		}
	}
	fprintf(fppr,"\n");
	polyNodeItr_Destroy(nitr);
	
	fclose(fppr);
	return;
}

void NF_Print(PolyModel *model,int NumberOfTests){
	int i,j;
	PolyNodeItr* nitr = polyModel_CreateNodeItr(model);
 	FILE *fppr;      
 	fppr = fopen(model->FileInfo.FileO,"a+");
	fprintf(fppr,"NF\n"); 	
	
	for(j=0;j<NumberOfTests;j++){
		for (polyNodeItr_Begin(nitr); polyNodeItr_IsValid(nitr); polyNodeItr_Next(nitr)){	

			PolyFace tmp;
			PolyNode node = polyNodeItr_GetCurr (nitr);
			fprintf(fppr,"PolyNode: %d, ",node.id);
			int no = polyNode_GetNFaces (model, node);
			fprintf(fppr,"NF: %d, ",no);
			for (i=0; i<no; i++){
				tmp = polyNode_GetFace (model, node, i);
				int t=polyFace_GetId(model, tmp);
				fprintf(fppr,"%d ",t);
			}
			fprintf(fppr,"\n");
		}
	}
	fprintf(fppr,"\n");
	polyNodeItr_Destroy(nitr);
	
	fclose(fppr);
	return;
}

void Ne_Print(PolyModel *model,int NumberOfTests){
	int i,j;
	PolyNodeItr* nitr = polyModel_CreateNodeItr(model);
 	FILE *fppr;      
 	fppr = fopen(model->FileInfo.FileO,"a+");
	fprintf(fppr,"Ne\n"); 	
 	
	for(j=0;j<NumberOfTests;j++){
		for (polyNodeItr_Begin(nitr); polyNodeItr_IsValid(nitr); polyNodeItr_Next(nitr)){	
			PolyEdge tmp;
			PolyNode elem = polyNodeItr_GetCurr (nitr);
			fprintf(fppr,"PolyNode: %d, ",elem.id);
			int no = polyNode_GetNEdges (model, elem);
			fprintf(fppr,"Ne: %d, ",no);
			for (i=0; i<no; i++){
				tmp = polyNode_GetEdge (model, elem, i);
				
				fprintf(fppr,"%d-%d ",tmp.EdgeTail,tmp.EdgeHead);
			}
			fprintf(fppr,"\n");
		}
	}
	fprintf(fppr,"\n");
	polyNodeItr_Destroy(nitr);
	
	fclose(fppr);
	return;
}

void NN_Print(PolyModel *model,int NumberOfTests){
	int i,j;
	PolyNodeItr* nitr = polyModel_CreateNodeItr(model);
 	FILE *fppr;      
 	fppr = fopen(model->FileInfo.FileO,"a+");
	fprintf(fppr,"NN\n"); 	

	for(j=0;j<NumberOfTests;j++){
		for (polyNodeItr_Begin(nitr); polyNodeItr_IsValid(nitr); polyNodeItr_Next(nitr)){	
			PolyNode tmp;
			PolyNode Node = polyNodeItr_GetCurr (nitr);
			fprintf(fppr,"PolyNode: %d, ",Node.id);
			int no = polyNode_GetNNodes (model, Node);
			fprintf(fppr,"NN: %d, ",no);
			for (i=0; i<no; i++){
				tmp = polyNode_GetNode (model, Node, i);
				int t=polyNode_GetId(model, tmp);
				fprintf(fppr,"%d ",t);
			}
			fprintf(fppr,"\n");
		}
	}
	fprintf(fppr,"\n");
	polyNodeItr_Destroy(nitr);

	fclose(fppr);
	return;
}




void AllElem_Print(PolyModel *model,int NumberOfTests){
 	FILE *fppr;      
 	fppr = fopen(model->FileInfo.FileO,"a+");
 	
	fprintf(fppr,"********** ELEMENT DATA **********\n");
	ELEM* elem = NULL;
	int *node_id;
	int *face_id;
	int i,j;
	int cnt=0;
	
	for (i = 0; i < model->ELEMHASHSIZE && cnt<=model->NumberOfElems; i++)
	{
		elem = model->ElemHash[i];

		while (elem)
		{
			fprintf(fppr,"id : %d, ", elem->Global_id);
			cnt++;

			fprintf(fppr,"face_id : ");
			for (j = 0; j < elem->AdjEnt_cnt; j++)
			{
				fprintf(fppr,"%d ", elem->AdjEnt_id[j]);
			}
			fprintf(fppr,"\n");
			elem = elem->next;
		}
	}
	free(elem);
	fclose(fppr);
	return;
}

void AllFace_Print(PolyModel *model,int NumberOfTests){
 	FILE *fppr;      
 	fppr = fopen(model->FileInfo.FileO,"a+");
 	fprintf(fppr,"********** FACE DATA **********\n");
	FACE *face = NULL;
	int* node_id;
	int i,j;
	int cnt=0;
	
	for (i = 0; i < model->FACEHASHSIZE && cnt<=model->NumberOfFaces; i++)
	{
		face = model->FaceHash[i];

		while (face)
		{
			fprintf(fppr,"id : %d, ", face->Global_id);
			cnt++;
			
			node_id = face->AdjEnt_id;
			fprintf(fppr,"node_id : ");
			for (j = 0; j < face->AdjEnt_cnt; j++)
			{
				fprintf(fppr,"%d ", node_id[j]);
			}
			fprintf(fppr,"\n");

			face = face->next;
		}
	}

	free(face);
	fclose(fppr);
	return;
}

void AllNode_Print(PolyModel *model,int NumberOfTests){
 	FILE *fppr;      
 	fppr = fopen(model->FileInfo.FileO,"a+");
 	
 	fprintf(fppr,"********** NODE DATA **********\n");
	NODE* node = NULL;
	int i,j;
	int cnt=0;
	
	for (i = 0; i <= model->NODEHASHSIZE && cnt<model->NumberOfNodes; i++)
	{
		node = model->NodeHash[i];

		while (node)
		{
			fprintf(fppr,"id : %d, x : %.3lf y : %.3lf z : %.3lf\n", node->Global_id, node->x, node->y, node->z);
			fprintf(fppr,"elem_id : ");
			for (j = 0; j < node->AdjEnt_cnt; j++)
			{
				fprintf(fppr,"%d ", node->AdjEnt_id[j]);
			}
			fprintf(fppr,"\n");
			cnt++;
			node = node->next;
		}
	}
	free(node);
	fclose(fppr);
	return;
}

void AllEdge_Print(PolyModel *model,int NumberOfTests){
	int i,j;
	PolyEdgeItr* editr = polyModel_CreateEdgeItr(model);
 	FILE *fppr;      
 	fppr = fopen(model->FileInfo.FileO,"a+");
 	
 	fprintf(fppr,"********** EDGE DATA **********\n");
	
	for(j=0;j<NumberOfTests;j++){
		int cnt=1;
		for (polyEdgeItr_Begin(editr); polyEdgeItr_IsValid(editr); polyEdgeItr_Next(editr)){	
			PolyNode tmp;
			PolyEdge Edge = polyEdgeItr_GetCurr (editr);
			fprintf(fppr,"Edge %d : ", cnt);
			int no = polyEdge_GetNNodes (model, Edge);
			for (i=0; i<no; i++){
				tmp = polyEdge_GetNode (model, Edge, i);
				int t=polyNode_GetId(model, tmp);
				fprintf(fppr,"%d ", t);
			}
			fprintf(fppr,"\n");
			cnt++;
		}
	}
	polyEdgeItr_Destroy(editr);
	fclose(fppr);
	return;
}











double EE_Time(PolyModel *model,int NumberOfTests){
	struct timeval startTime, endTime;
	long long int t = 0;
	int i,j;
	double Time;
	PolyElemItr* eitr = polyModel_CreateElemItr(model);

	gettimeofday(&startTime, NULL);

	for(j=0;j<NumberOfTests;j++){
		for (polyElemItr_Begin(eitr); polyElemItr_IsValid(eitr); polyElemItr_Next(eitr)){	

			PolyElem tmp;
			PolyElem elem = polyElemItr_GetCurr (eitr);
			int no = polyElem_GetNElems (model, elem);
			for (i=0; i<no; i++){
				tmp = polyElem_GetElem (model, elem, i);
				int t=polyElem_GetId(model, tmp);
			}
		}
	}
	polyElemItr_Destroy(eitr);
	
	gettimeofday(&endTime, NULL);
	t = (endTime.tv_sec - startTime.tv_sec) * 1000000 + (endTime.tv_usec - startTime.tv_usec);
	Time=1.0 * t / 1000000/NumberOfTests/model->NumberOfElems;//1,000,000 converts time from usec to sec

	return Time;
}

double EF_Time(PolyModel *model,int NumberOfTests){
	struct timeval startTime, endTime;
	long long int t = 0;
	int i,j;
	double Time;
	PolyElemItr* eitr = polyModel_CreateElemItr(model);

	gettimeofday(&startTime, NULL);
	
	for(j=0;j<NumberOfTests;j++){
		for (polyElemItr_Begin(eitr); polyElemItr_IsValid(eitr); polyElemItr_Next(eitr)){	

			PolyFace tmp;
			PolyElem elem = polyElemItr_GetCurr (eitr);
			int no = polyElem_GetNFaces (model, elem);
			for (i=0; i<no; i++){
				tmp = polyElem_GetFace (model, elem, i);
				int t=polyFace_GetId(model, tmp);
			}
		}
	}
	polyElemItr_Destroy(eitr);
	
	gettimeofday(&endTime, NULL);
	t = (endTime.tv_sec - startTime.tv_sec) * 1000000 + (endTime.tv_usec - startTime.tv_usec);
	Time=1.0 * t / 1000000/NumberOfTests/model->NumberOfElems;//1,000,000 converts time from usec to sec

	return Time;
}

double Ee_Time(PolyModel *model,int NumberOfTests){
	struct timeval startTime, endTime;
	long long int t = 0;
	int i,j;
	double Time;
	PolyElemItr* eitr = polyModel_CreateElemItr(model);
 	
 	gettimeofday(&startTime, NULL);
 	
	for(j=0;j<NumberOfTests;j++){
		for (polyElemItr_Begin(eitr); polyElemItr_IsValid(eitr); polyElemItr_Next(eitr)){	
			PolyEdge tmp;
			PolyElem elem = polyElemItr_GetCurr (eitr);
			int no = polyElem_GetNEdges (model, elem);
			for (i=0; i<no; i++){
				tmp = polyElem_GetEdge (model, elem, i);				
			}
		}
	}
	polyElemItr_Destroy(eitr);

	gettimeofday(&endTime, NULL);
	t = (endTime.tv_sec - startTime.tv_sec) * 1000000 + (endTime.tv_usec - startTime.tv_usec);
	Time=1.0 * t / 1000000/NumberOfTests/model->NumberOfElems;//1,000,000 converts time from usec to sec

	return Time;
}

double EN_Time(PolyModel *model,int NumberOfTests){
	struct timeval startTime, endTime;
	long long int t = 0;
	int i,j;
	double Time;
	PolyElemItr* eitr = polyModel_CreateElemItr(model);

	gettimeofday(&startTime, NULL);

	for(j=0;j<NumberOfTests;j++){
		for (polyElemItr_Begin(eitr); polyElemItr_IsValid(eitr); polyElemItr_Next(eitr)){	

			PolyNode tmp;
			PolyElem elem = polyElemItr_GetCurr (eitr);
			int nelnode = polyElem_GetNNodes (model, elem);
			for (i=0; i<nelnode; i++){
				tmp = polyElem_GetNode (model, elem, i);
				int t=polyNode_GetId(model, tmp);
			}
		}
	}
	polyElemItr_Destroy(eitr);
	
	gettimeofday(&endTime, NULL);
	t = (endTime.tv_sec - startTime.tv_sec) * 1000000 + (endTime.tv_usec - startTime.tv_usec);
	Time=1.0 * t / 1000000/NumberOfTests/model->NumberOfElems;//1,000,000 converts time from usec to sec

	return Time;
}

double FE_Time(PolyModel *model,int NumberOfTests){
	struct timeval startTime, endTime;
	long long int t = 0;
	int i,j;
	double Time;
	PolyFaceItr* fitr = polyModel_CreateFaceItr(model);

	gettimeofday(&startTime, NULL);

	for(j=0;j<NumberOfTests;j++){
		for (polyFaceItr_Begin(fitr); polyFaceItr_IsValid(fitr); polyFaceItr_Next(fitr)){	

			PolyElem tmp;
			PolyFace face = polyFaceItr_GetCurr (fitr);
			int no = polyFace_GetNElems (model, face);
			for (i=0; i<no; i++){
				tmp = polyFace_GetElem (model, face, i);
				int t=polyElem_GetId(model, tmp);
			}
		}
	}
	polyFaceItr_Destroy(fitr);
	
	gettimeofday(&endTime, NULL);
	t = (endTime.tv_sec - startTime.tv_sec) * 1000000 + (endTime.tv_usec - startTime.tv_usec);
	Time=1.0 * t / 1000000/NumberOfTests/model->NumberOfFaces;//1,000,000 converts time from usec to sec

	return Time;
}

double FF_Time(PolyModel *model,int NumberOfTests){
	struct timeval startTime, endTime;
	long long int t = 0;
	int i,j;
	double Time;
	PolyFaceItr* fitr = polyModel_CreateFaceItr(model);
	
	gettimeofday(&startTime, NULL);
	
	for(j=0;j<NumberOfTests;j++){
		for (polyFaceItr_Begin(fitr); polyFaceItr_IsValid(fitr); polyFaceItr_Next(fitr)){	

			PolyFace tmp;
			PolyFace face = polyFaceItr_GetCurr (fitr);
			int no = polyFace_GetNFaces (model, face);
			for (i=0; i<no; i++){
				tmp = polyFace_GetFace (model, face, i);
				int t=polyFace_GetId(model, tmp);
			}
		}
	}
	polyFaceItr_Destroy(fitr);
	
	gettimeofday(&endTime, NULL);
	t = (endTime.tv_sec - startTime.tv_sec) * 1000000 + (endTime.tv_usec - startTime.tv_usec);
	Time=1.0 * t / 1000000/NumberOfTests/model->NumberOfFaces;//1,000,000 converts time from usec to sec

	return Time;
}

double Fe_Time(PolyModel *model,int NumberOfTests){
	struct timeval startTime, endTime;
	long long int t = 0;
	int i,j;
	double Time;
	PolyFaceItr* fitr = polyModel_CreateFaceItr(model);
 		
	gettimeofday(&startTime, NULL);
	
	for(j=0;j<NumberOfTests;j++){
		for (polyFaceItr_Begin(fitr); polyFaceItr_IsValid(fitr); polyFaceItr_Next(fitr)){	
			PolyEdge tmp;
			PolyFace face = polyFaceItr_GetCurr (fitr);
			int no = polyFace_GetNEdges (model, face);
			for (i=0; i<no; i++){
				tmp = polyFace_GetEdge (model, face, i);
			}
		}
	}
	polyFaceItr_Destroy(fitr);

	gettimeofday(&endTime, NULL);
	t = (endTime.tv_sec - startTime.tv_sec) * 1000000 + (endTime.tv_usec - startTime.tv_usec);
	Time=1.0 * t / 1000000/NumberOfTests/model->NumberOfFaces;//1,000,000 converts time from usec to sec

	return Time;
}

double FN_Time(PolyModel *model,int NumberOfTests){
	struct timeval startTime, endTime;
	long long int t = 0;
	int i,j;
	double Time;
	PolyFaceItr* fitr = polyModel_CreateFaceItr(model);
	
	gettimeofday(&startTime, NULL);
	
	for(j=0;j<NumberOfTests;j++){
		for (polyFaceItr_Begin(fitr); polyFaceItr_IsValid(fitr); polyFaceItr_Next(fitr)){	

			PolyNode tmp;
			PolyFace face = polyFaceItr_GetCurr (fitr);
			int no = polyFace_GetNNodes (model, face);
			for (i=0; i<no; i++){
				tmp = polyFace_GetNode (model, face, i);
				int t=polyNode_GetId(model, tmp);
			}
		}
	}
	polyFaceItr_Destroy(fitr);
	
	gettimeofday(&endTime, NULL);
	t = (endTime.tv_sec - startTime.tv_sec) * 1000000 + (endTime.tv_usec - startTime.tv_usec);
	Time=1.0 * t / 1000000/NumberOfTests/model->NumberOfFaces;//1,000,000 converts time from usec to sec

	return Time;
}

double eE_Time(PolyModel *model,int NumberOfTests){
	struct timeval startTime, endTime;
	long long int t = 0;
	int i,j;
	double Time;
	PolyEdgeItr* editr = polyModel_CreateEdgeItr(model);
	
	gettimeofday(&startTime, NULL);
	
	for(j=0;j<NumberOfTests;j++){
		for (polyEdgeItr_Begin(editr); polyEdgeItr_IsValid(editr); polyEdgeItr_Next(editr)){	
			PolyElem tmp;
			PolyEdge Edge = polyEdgeItr_GetCurr (editr);
			int no = polyEdge_GetNElems (model, Edge);
			for (i=0; i<no; i++){
				tmp = polyEdge_GetElem (model, Edge, i);
				int t=polyElem_GetId(model, tmp);
			}
		}
	}
	polyEdgeItr_Destroy(editr);

	gettimeofday(&endTime, NULL);
	t = (endTime.tv_sec - startTime.tv_sec) * 1000000 + (endTime.tv_usec - startTime.tv_usec);
	Time=1.0 * t / 1000000/NumberOfTests/editr->Itr_Edge_Cnt;//1,000,000 converts time from usec to sec

	return Time;
}

double eF_Time(PolyModel *model,int NumberOfTests){
	struct timeval startTime, endTime;
	long long int t = 0;
	int i,j;
	double Time;
	PolyEdgeItr* editr = polyModel_CreateEdgeItr(model);
	
	gettimeofday(&startTime, NULL);
	
	for(j=0;j<NumberOfTests;j++){
		for (polyEdgeItr_Begin(editr); polyEdgeItr_IsValid(editr); polyEdgeItr_Next(editr)){

			PolyFace tmp;
			PolyEdge edge = polyEdgeItr_GetCurr (editr);
			int no = polyEdge_GetNFaces (model, edge);
			for (i=0; i<no; i++){
				tmp = polyEdge_GetFace (model, edge, i);
				int t=polyFace_GetId(model, tmp);
			}
		}
	}
	polyEdgeItr_Destroy(editr);
	
	gettimeofday(&endTime, NULL);
	t = (endTime.tv_sec - startTime.tv_sec) * 1000000 + (endTime.tv_usec - startTime.tv_usec);
	Time=1.0 * t / 1000000/NumberOfTests/editr->Itr_Edge_Cnt;//1,000,000 converts time from usec to sec

	return Time;
}

double ee_Time(PolyModel *model,int NumberOfTests){
	struct timeval startTime, endTime;
	long long int t = 0;
	int i,j;
	double Time;
	PolyEdgeItr* editr = polyModel_CreateEdgeItr(model);
	
	gettimeofday(&startTime, NULL);
	
	for(j=0;j<NumberOfTests;j++){
		for (polyEdgeItr_Begin(editr); polyEdgeItr_IsValid(editr); polyEdgeItr_Next(editr)){	
			PolyEdge tmp;
			PolyEdge Edge = polyEdgeItr_GetCurr (editr);
			int no = polyEdge_GetNEdges (model, Edge);
			for (i=0; i<no; i++){
				tmp = polyEdge_GetEdge (model, Edge, i);
			}
		}
	}
	polyEdgeItr_Destroy(editr);

	gettimeofday(&endTime, NULL);
	t = (endTime.tv_sec - startTime.tv_sec) * 1000000 + (endTime.tv_usec - startTime.tv_usec);
	Time=1.0 * t / 1000000/NumberOfTests/editr->Itr_Edge_Cnt;//1,000,000 converts time from usec to sec

	return Time;
}

double eN_Time(PolyModel *model,int NumberOfTests){
	struct timeval startTime, endTime;
	long long int t = 0;
	int i,j;
	double Time;
	PolyEdgeItr* editr = polyModel_CreateEdgeItr(model);
	
	gettimeofday(&startTime, NULL);
	
	for(j=0;j<NumberOfTests;j++){
		for (polyEdgeItr_Begin(editr); polyEdgeItr_IsValid(editr); polyEdgeItr_Next(editr)){	
			PolyNode tmp;
			PolyEdge Edge = polyEdgeItr_GetCurr (editr);
			int no = polyEdge_GetNNodes(model, Edge);
			for (i=0; i<no; i++){
				tmp = polyEdge_GetNode(model, Edge, i);
				int t=polyNode_GetId(model, tmp);
			}
		}
	}
	polyEdgeItr_Destroy(editr);

	gettimeofday(&endTime, NULL);
	t = (endTime.tv_sec - startTime.tv_sec) * 1000000 + (endTime.tv_usec - startTime.tv_usec);
	Time=1.0 * t / 1000000/NumberOfTests/editr->Itr_Edge_Cnt;//1,000,000 converts time from usec to sec

	return Time;
}

double NE_Time(PolyModel *model,int NumberOfTests){
	struct timeval startTime, endTime;
	long long int t = 0;
	int i,j;
	double Time;
	PolyNodeItr* nitr = polyModel_CreateNodeItr(model);
	
	gettimeofday(&startTime, NULL);
	
	for(j=0;j<NumberOfTests;j++){
		for (polyNodeItr_Begin(nitr); polyNodeItr_IsValid(nitr); polyNodeItr_Next(nitr)){	
			PolyElem tmp;
			PolyNode Node = polyNodeItr_GetCurr (nitr);
			int no = polyNode_GetNElems (model, Node);
			for (i=0; i<no; i++){
				tmp = polyNode_GetElem (model, Node, i);
				int t=polyElem_GetId(model, tmp);
			}
		}
	}
	polyNodeItr_Destroy(nitr);
	
	gettimeofday(&endTime, NULL);
	t = (endTime.tv_sec - startTime.tv_sec) * 1000000 + (endTime.tv_usec - startTime.tv_usec);
	Time=1.0 * t / 1000000/NumberOfTests/model->NumberOfNodes;//1,000,000 converts time from usec to sec

	return Time;
}

double NF_Time(PolyModel *model,int NumberOfTests){
	struct timeval startTime, endTime;
	long long int t = 0;
	int i,j;
	double Time;
	PolyNodeItr* nitr = polyModel_CreateNodeItr(model);
		
	gettimeofday(&startTime, NULL);
	
	for(j=0;j<NumberOfTests;j++){
		for (polyNodeItr_Begin(nitr); polyNodeItr_IsValid(nitr); polyNodeItr_Next(nitr)){	

			PolyFace tmp;
			PolyNode node = polyNodeItr_GetCurr (nitr);
			int no = polyNode_GetNFaces (model, node);
			for (i=0; i<no; i++){
				tmp = polyNode_GetFace (model, node, i);
				int t=polyFace_GetId(model, tmp);
			}
		}
	}
	polyNodeItr_Destroy(nitr);
	
	gettimeofday(&endTime, NULL);
	t = (endTime.tv_sec - startTime.tv_sec) * 1000000 + (endTime.tv_usec - startTime.tv_usec);
	Time=1.0 * t / 1000000/NumberOfTests/model->NumberOfNodes;//1,000,000 converts time from usec to sec

	return Time;
}

double Ne_Time(PolyModel *model,int NumberOfTests){
	struct timeval startTime, endTime;
	long long int t = 0;
	int i,j;
	double Time;
	PolyNodeItr* nitr = polyModel_CreateNodeItr(model);
 		
	gettimeofday(&startTime, NULL);
	
	for(j=0;j<NumberOfTests;j++){
		for (polyNodeItr_Begin(nitr); polyNodeItr_IsValid(nitr); polyNodeItr_Next(nitr)){	
			PolyEdge tmp;
			PolyNode elem = polyNodeItr_GetCurr (nitr);
			int no = polyNode_GetNEdges (model, elem);
			for (i=0; i<no; i++){
				tmp = polyNode_GetEdge (model, elem, i);
			}
		}
	}
	polyNodeItr_Destroy(nitr);
	
	gettimeofday(&endTime, NULL);
	t = (endTime.tv_sec - startTime.tv_sec) * 1000000 + (endTime.tv_usec - startTime.tv_usec);
	Time=1.0 * t / 1000000/NumberOfTests/model->NumberOfNodes;//1,000,000 converts time from usec to sec

	return Time;
}

double NN_Time(PolyModel *model,int NumberOfTests){
	struct timeval startTime, endTime;
	long long int t = 0;
	int i,j;
	double Time;
	PolyNodeItr* nitr = polyModel_CreateNodeItr(model);
	
	gettimeofday(&startTime, NULL);
	
	for(j=0;j<NumberOfTests;j++){
		for (polyNodeItr_Begin(nitr); polyNodeItr_IsValid(nitr); polyNodeItr_Next(nitr)){	
			PolyNode tmp;
			PolyNode Node = polyNodeItr_GetCurr (nitr);
			int no = polyNode_GetNNodes (model, Node);
			for (i=0; i<no; i++){
				tmp = polyNode_GetNode (model, Node, i);
				int t=polyNode_GetId(model, tmp);
			}
		}
	}
	polyNodeItr_Destroy(nitr);

	gettimeofday(&endTime, NULL);
	t = (endTime.tv_sec - startTime.tv_sec) * 1000000 + (endTime.tv_usec - startTime.tv_usec);
	Time=1.0 * t / 1000000/NumberOfTests/model->NumberOfNodes;//1,000,000 converts time from usec to sec

	return Time;
}



double AllElem_Time(PolyModel *model,int NumberOfTests){
	struct timeval startTime, endTime;
	long long int t = 0;
	int i,j;
	double Time;
	PolyElemItr* eitr = polyModel_CreateElemItr(model);

	gettimeofday(&startTime, NULL);
	
	for(j=0;j<NumberOfTests;j++){
		for (polyElemItr_Begin(eitr); polyElemItr_IsValid(eitr); polyElemItr_Next(eitr)){	

			PolyElem elem = polyElemItr_GetCurr (eitr);
//			printf("PolyElem: %d, ",elem.id);
//			printf("\n");
		}
	}
//	printf("\n"s);
	polyElemItr_Destroy(eitr);
	
	
	gettimeofday(&endTime, NULL);
	t = (endTime.tv_sec - startTime.tv_sec) * 1000000 + (endTime.tv_usec - startTime.tv_usec);
	Time=1.0 * t / 1000000/NumberOfTests;//1,000,000 converts time from usec to sec

	return Time;
}

double AllFace_Time(PolyModel *model,int NumberOfTests){
	struct timeval startTime, endTime;
	long long int t = 0;
	int i,j;
	double Time;
	PolyFaceItr* fitr = polyModel_CreateFaceItr(model);
	
	gettimeofday(&startTime, NULL);
	
	for(j=0;j<NumberOfTests;j++){
		for (polyFaceItr_Begin(fitr); polyFaceItr_IsValid(fitr); polyFaceItr_Next(fitr)){	
			PolyFace face = polyFaceItr_GetCurr (fitr);
//			printf("PolyFace: %d, ",face.id);
//			printf("\n");
		}
	}
//	printf("\n");
	polyFaceItr_Destroy(fitr);
	
	gettimeofday(&endTime, NULL);
	t = (endTime.tv_sec - startTime.tv_sec) * 1000000 + (endTime.tv_usec - startTime.tv_usec);
	Time=1.0 * t /1000000/NumberOfTests;

	return Time;
}

double AllNode_Time(PolyModel *model,int NumberOfTests){
	struct timeval startTime, endTime;
	long long int t = 0;
	int i,j;
	double Time;
	PolyNodeItr* nitr = polyModel_CreateNodeItr(model);
 	
 	gettimeofday(&startTime, NULL);
 	
	for(j=0;j<NumberOfTests;j++){
		for (polyNodeItr_Begin(nitr); polyNodeItr_IsValid(nitr); polyNodeItr_Next(nitr)){	
			PolyNode elem = polyNodeItr_GetCurr (nitr);
//			printf("PolyNode: %d, ",elem.id);
//			printf("\n");
		}
	}
//	printf("\n");
	polyNodeItr_Destroy(nitr);
	
	gettimeofday(&endTime, NULL);
	t = (endTime.tv_sec - startTime.tv_sec) * 1000000 + (endTime.tv_usec - startTime.tv_usec);
	Time=1.0 * t /1000000/NumberOfTests;

	return Time;
}

double AllEdge_Time(PolyModel *model,int NumberOfTests){
	struct timeval startTime, endTime;
	long long int t = 0;
	int i,j;
	double Time;
	PolyEdgeItr* editr = polyModel_CreateEdgeItr(model);
	
	gettimeofday(&startTime, NULL);
	
	for(j=0;j<NumberOfTests;j++){
		for (polyEdgeItr_Begin(editr); polyEdgeItr_IsValid(editr); polyEdgeItr_Next(editr)){	
			PolyNode tmp;
			PolyEdge Edge = polyEdgeItr_GetCurr (editr);
			int no = polyEdge_GetNNodes (model, Edge);
			for (i=0; i<no; i++){
				tmp = polyEdge_GetNode(model, Edge, i);
				int t=polyNode_GetId(model, tmp);
			}
		}
	}
	polyEdgeItr_Destroy(editr);

	gettimeofday(&endTime, NULL);
	t = (endTime.tv_sec - startTime.tv_sec) * 1000000 + (endTime.tv_usec - startTime.tv_usec);
	Time=1.0 * t /1000000/NumberOfTests;//1,000,000 converts time from usec to sec

	return Time;
}


void Attribute_Test(PolyModel *model){
	int i,j;
	PolyNodeItr* nitr = polyModel_CreateNodeItr(model);

	for (polyNodeItr_Begin(nitr); polyNodeItr_IsValid(nitr); polyNodeItr_Next(nitr)){	
		PolyElem tmp;
		PolyNode Node = polyNodeItr_GetCurr (nitr);
		printf("PolyNode: %d, ",Node.id);
		PolyElem* point = (PolyElem*)malloc(sizeof(PolyElem));
		point->id = 23333;

		polyNode_SetAttrib(model, Node, point);

		PolyElem* data = polyNode_GetAttrib(model, Node);
		printf("data->id: %d\n",data->id);
//			polyElem_DeleteAttrib(PolyModel*, PolyElem);
	}
	polyNodeItr_Destroy(nitr);
	
	return;
}

double cal_cpuoccupy (cpu_occupy_t *o, cpu_occupy_t *n)
{
    double od, nd;
    double id, sd;
    double cpu_use ;
 
    od = (double) (o->user + o->nice + o->system +o->idle+o->softirq+o->iowait+o->irq);//第一次(用户+优先级+系统+空闲)的时间再赋给od
    nd = (double) (n->user + n->nice + n->system +n->idle+n->softirq+n->iowait+n->irq);//第二次(用户+优先级+系统+空闲)的时间再赋给od
 
    id = (double) (n->idle);    //用户第一次和第二次的时间之差再赋给id
    sd = (double) (o->idle) ;    //系统第一次和第二次的时间之差再赋给sd
    if((nd-od) != 0)
        cpu_use =100.0 - ((id-sd))/(nd-od)*100.00; //((用户+系统)乖100)除(第一次和第二次的时间差)再赋给g_cpu_used
    else 
        cpu_use = 0;
    return cpu_use;
}
 
void get_cpuoccupy (cpu_occupy_t *cpust)
{
    FILE *fd;
    int n;
    char buff[256];
    cpu_occupy_t *cpu_occupy;
    cpu_occupy=cpust;
 
    fd = fopen ("/proc/stat", "r");
    if(fd == NULL)
    {
            perror("fopen:");
            exit (0);
    }
    fgets (buff, sizeof(buff), fd);
 
    sscanf (buff, "%s %u %u %u %u %u %u %u", cpu_occupy->name, &cpu_occupy->user, &cpu_occupy->nice,&cpu_occupy->system, &cpu_occupy->idle ,&cpu_occupy->iowait,&cpu_occupy->irq,&cpu_occupy->softirq);
 
    fclose(fd);
}
 
double get_sysCpuUsage()
{
    cpu_occupy_t cpu_stat1;
    cpu_occupy_t cpu_stat2;
    double cpu;
    get_cpuoccupy((cpu_occupy_t *)&cpu_stat1);
    sleep(2);
    //第二次获取cpu使用情况
    get_cpuoccupy((cpu_occupy_t *)&cpu_stat2);
 
    //计算cpu使用率
    cpu = cal_cpuoccupy ((cpu_occupy_t *)&cpu_stat1, (cpu_occupy_t *)&cpu_stat2);
 
    return cpu;
}
