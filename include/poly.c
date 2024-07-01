#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h> 
#include "typedefines.h"
#include "data_structure.h"
#include "perf_test.h"
#include "poly.h"

enum{
	ELEMENT_POLY2D = 1,
	ELEMENT_POLY3D = 0,
	ELEMENT_POLY3D_COH
};

PolyModel* polyModel_Create(int dim, int NOE, int NOF, int NON, double HashSizeDensity){
	PolyModel *model=(PolyModel *)malloc(sizeof(PolyModel));
	model->dimen=dim;
	model->NumberOfElems=0;
	model->NumberOfFaces=0;
	model->NumberOfEdges=0;
	model->NumberOfNodes=0;
	model->IdInfo.ELEMIDMAX=0;
	model->IdInfo.NODEIDMAX=0;
	model->IdInfo.FACEIDMAX=0;

	/**
		Initialize hash tables and Set the buckets of hash tables to NULL 
	*/
	model->ELEMHASHSIZE=nextPrime((int)(NOE/HashSizeDensity));
	model->FACEHASHSIZE=nextPrime((int)(NOF/HashSizeDensity));
	model->NODEHASHSIZE=nextPrime((int)(NON/HashSizeDensity));
	
	model->ElemHash=(ELEM**)malloc(sizeof(ELEM*)*model->ELEMHASHSIZE);
	model->FaceHash=(FACE**)malloc(sizeof(FACE*)*model->FACEHASHSIZE);
	model->NodeHash=(NODE**)malloc(sizeof(NODE*)*model->NODEHASHSIZE);
	
	int i;
	for(i=0;i<model->ELEMHASHSIZE;i++){
		model->ElemHash[i]=NULL;
	}
	for(i=0;i<model->FACEHASHSIZE;i++){
		model->FaceHash[i]=NULL;
	}
	for(i=0;i<model->NODEHASHSIZE;i++){
		model->NodeHash[i]=NULL;
	}
	return model;
}

void polyModel_SetFileInfo(PolyModel* model, FileInfo FileInfo){
	memset(model->FileInfo.NOE,0,strlen(model->FileInfo.NOE));
	memset(model->FileInfo.FileE,0,strlen(model->FileInfo.FileE));
	memset(model->FileInfo.FileF,0,strlen(model->FileInfo.FileF));
	memset(model->FileInfo.FileN,0,strlen(model->FileInfo.FileN));
	memset(model->FileInfo.FileO,0,strlen(model->FileInfo.FileO));
	
	strcpy(model->FileInfo.NOE, FileInfo.NOE);
	strcpy(model->FileInfo.FileE, FileInfo.FileE);
	strcpy(model->FileInfo.FileF, FileInfo.FileF);
	strcpy(model->FileInfo.FileN, FileInfo.FileN);
	strcpy(model->FileInfo.FileO, FileInfo.FileO);
	model->FileInfo.Dim = FileInfo.Dim;
	model->FileInfo.NoE = FileInfo.NoE;
	model->FileInfo.NoF = FileInfo.NoF;
	model->FileInfo.NoN = FileInfo.NoN;
}


void polyModel_InitializeNE(PolyModel* model){
	int i;
	
	/**
		Obtain and record adjacent element info to nodetable
	*/	
	int elemitr;
	PolyElemItr* eitr = polyModel_CreateElemItr(model);
	for(polyElemItr_Begin(eitr); polyElemItr_IsValid(eitr); polyElemItr_Next(eitr)){
		PolyElem polyelem = polyElemItr_GetCurr (eitr);
		elemitr = polyElem_GetId(model, polyelem);
		
		Node Nodelist = Elem_GetNode(model,elemitr);
 		for(i = 0; i < Nodelist.cnt; i++){
 			NODE* NODECurr = Node_GetNODEAtId(model,Nodelist.NodeInfo[i]);
 			if(NODECurr->AdjEnt_cnt==0){
 				NODECurr->AdjEnt_cnt++;
 				NODECurr->AdjEnt_id=(int*)malloc(sizeof(int));
 				NODECurr->AdjEnt_id[0]=elemitr;
 			}
 			else{
 				NODECurr->AdjEnt_cnt++;
 				NODECurr->AdjEnt_id=(int*)realloc(NODECurr->AdjEnt_id,sizeof(int)*NODECurr->AdjEnt_cnt);
 				NODECurr->AdjEnt_id[NODECurr->AdjEnt_cnt-1]=elemitr;
 			}
 		}
	}
	polyElemItr_Destroy(eitr);
	
	
	/**
		Check whether the node is on the boundary (2D).
		Algorithm : find all edges adjacent to the given node and see whether any of them
		is on the boundary . We say the edge is on the boundary if it has only one adjacent 
		element. 
	*/		
	if(model->dimen == 2){
		int t, id;
		PolyNodeItr* nitr = polyModel_CreateNodeItr(model);
		for(polyNodeItr_Begin(nitr); polyNodeItr_IsValid(nitr); polyNodeItr_Next(nitr)){	
			PolyEdge tmp;
			PolyNode polynode = polyNodeItr_GetCurr (nitr);
			int no = polyNode_GetNEdges (model, polynode);
			for (i=0; i<no; i++){
				tmp = polyNode_GetEdge(model, polynode, i);
				t = polyEdge_GetNElems(model, tmp);
				if(t==1){
					id = polyNode_GetId(model, polynode);
					NODE* nodedata = Node_GetNODEAtId(model, id);
					nodedata->isBoundary=1;
				}
			}
		}
		polyNodeItr_Destroy(nitr);
	}
	/**
		Check whether the node is on the boundary (3D).
		Algorithm : Use Face_GetElem to determine boundary faces. 
		If a face is on the boundary, then all its nodes are on the boundary
	*/		
	else if (model->dimen == 3)
	{
		int t, id;
		PolyFaceItr* fitr = polyModel_CreateFaceItr(model);
		for(polyFaceItr_Begin(fitr); polyFaceItr_IsValid(fitr); polyFaceItr_Next(fitr)){	
			PolyNode tmp;
			PolyFace polyface = polyFaceItr_GetCurr (fitr);
			int no = polyFace_GetNElems (model, polyface);
			if(no==1){
				int nod = polyFace_GetNNodes (model, polyface);
				for (i=0; i<nod; i++){
					tmp = polyFace_GetNode(model, polyface, i);
					id = polyNode_GetId(model, tmp);
					NODE* nodedata = Node_GetNODEAtId(model, id);
					nodedata->isBoundary=1;
				}
			}
		}
		polyFaceItr_Destroy(fitr);
	}

}



void polyModel_Destroy(PolyModel* model){
	free(model->ElemHash);
	free(model->FaceHash);
	free(model->NodeHash);
	free(model);
	
	return;
}


int polyModel_GetELEMIDMAX(PolyModel* model){
	return model->IdInfo.ELEMIDMAX;
}

int polyModel_GetFACEIDMAX(PolyModel* model){
	return model->IdInfo.FACEIDMAX;
}

int polyModel_GetNODEIDMAX(PolyModel* model){
	return model->IdInfo.NODEIDMAX;
}



bool polyModel_InsertNode(PolyModel* model, NODE* newnode)
{
	bool bo = false;
	if( Node_IsValid(model, newnode->Global_id) ){
		printf("Error! Node %d already exists . Node insertion aborted . \n", newnode->Global_id);
	}
	else{
		int key = newnode->Global_id % model->NODEHASHSIZE; // key value = the remainder of the ID divided by the hash size
		newnode->next = model->NodeHash[key]; // The next on the new node points to the address that the hash's pointer points to
		model->NodeHash[key] = newnode; // The hash's pointer points to the new node
		model->NumberOfNodes++;
		if(newnode->Global_id>model->IdInfo.NODEIDMAX){
			model->IdInfo.NODEIDMAX=newnode->Global_id;
		}
		bo = true;
	}
	return bo;
}

void polyModel_RemNode(PolyModel* model, int nid)
{
	int key = nid % model->NODEHASHSIZE; // key value = the remainder of the ID divided by the hash size
	NODE *pre, *cur;

	pre = NULL;
	cur = model->NodeHash[key];
	while (cur && cur->Global_id != nid)
	{
		pre = pre->next;
		cur = cur->next;
	}

	if (cur == NULL)
	{
		printf("##ERROR## The node id cannot be found! (\"polyModel_RemNode\" function)\n\n");
		exit(1);
	}

	if (pre == NULL) // delete the node that hash's pointer points to
	{
		model->NodeHash[key] = cur->next;
		cur->next = NULL;
	}
	else // the other cases
	{
		pre->next = cur->next;
		cur->next = NULL;
	}

	free(cur);
	free(pre);
}

void polyModel_PrintAllNodeData(PolyModel* model)
{
	printf("********** NODE DATA **********\n");
	NODE* node = NULL;
	int i,j;
	int cnt=0;
	
	for (i = 0; i <= model->NODEHASHSIZE && cnt<model->NumberOfNodes; i++)
	{
		node = model->NodeHash[i];

		while (node)
		{
			printf("id : %d, x : %.3lf y : %.3lf z : %.3lf\n", node->Global_id, node->x, node->y, node->z);
			printf("elem_id : ");
			for (j = 0; j < node->AdjEnt_cnt; j++)
			{
				printf("%d ", node->AdjEnt_id[j]);
			}
			printf("\t\t\t IsBoundary : %d", node->isBoundary);
			printf("\n");
			cnt++;
			node = node->next;
		}
	}
	free(node);
}

bool polyModel_InsertFace(PolyModel* model, FACE* newface)
{
	bool bo = false;
	if( Face_IsValid(model, newface->Global_id) ){
		printf("Error! Face %d already exists . Face insertion aborted . \n", newface->Global_id);
	}
	else{
		int key = newface->Global_id % model->FACEHASHSIZE; // key value = the remainder of the ID divided by the hash size
		newface->next = model->FaceHash[key]; // The next on the new face points to the address that the hash's pointer points to
		model->FaceHash[key] = newface; // The hash's pointer points to the new face
		model->NumberOfFaces++;
		if(newface->Global_id>model->IdInfo.FACEIDMAX){
			model->IdInfo.FACEIDMAX=newface->Global_id;
		}
		bo = true;
	}
	return bo;
}

void polyModel_RemFace(PolyModel* model, int face_id)
{
	int key = face_id % model->FACEHASHSIZE; // key value = the remainder of the ID divided by the hash size

	FACE* pre = NULL;
	FACE* cur = model->FaceHash[key];

	while (cur && cur->Global_id != face_id)
	{
		pre = pre->next;
		cur = cur->next;
	}

	if (cur == NULL)
	{
		printf("##ERROR## The face id cannot be found! (\"polyModel_RemFace\" function)\n\n");
		exit(1);
	}

	if (pre == NULL) // delete the node that hash's pointer points to
	{
		model->FaceHash[key] = cur->next;
		cur->next = NULL;
	}
	else // the other cases
	{
		pre->next = cur->next;
		cur->next = NULL;
	}

	free(pre);
	free(cur);
}

void polyModel_PrintAllFaceData(PolyModel* model)
{
	printf("********** FACE DATA **********\n");
	FACE *face = NULL;
	int* node_id;
	int i, j;
	int cnt=0;
	
	for (i = 0; i < model->FACEHASHSIZE && cnt<=model->NumberOfFaces; i++)
	{
		face = model->FaceHash[i];

		while (face)
		{
			printf("id : %d, ", face->Global_id);
			cnt++;
			
			node_id = face->AdjEnt_id;
			printf("node_id : ");
			for (j = 0; j < face->AdjEnt_cnt; j++)
			{
				printf("%d ", node_id[j]);
			}
			printf("\n");

			face = face->next;
		}
	}

	free(face);
}

bool polyModel_InsertElem(PolyModel* model, ELEM *newelem)
{
	bool bo = false;
	if( Elem_IsValid(model, newelem->Global_id) ){
		printf("Error! Elem %d already exists . Elem insertion aborted . \n", newelem->Global_id);
	}
	else{
		int key = newelem->Global_id % model->ELEMHASHSIZE; // key value = the remainder of the ID divided by the hash size
		newelem->next = model->ElemHash[key]; // The next on the new elem points to the address that the hash's pointer points to
		model->ElemHash[key] = newelem; // The hash's pointer points to the new elem
		model->NumberOfElems++;
		if(newelem->Global_id>model->IdInfo.ELEMIDMAX){
			model->IdInfo.ELEMIDMAX=newelem->Global_id;
		}
		bo = true;
	}
	return bo;
}

void polyModel_RemElem(PolyModel* model, int elem_id)
{
	int key = elem_id % model->ELEMHASHSIZE; // key value = the remainder of the ID divided by the hash size

	ELEM* pre = NULL;
	ELEM* cur = model->ElemHash[key];

	while (cur && cur->Global_id != elem_id)
	{
		pre = pre->next;
		cur = cur->next;
	}

	if (cur == NULL)
	{
		printf("##ERROR## The element id cannot be found! (\"polyModel_RemElem\" function)\n\n");
		exit(1);
	}

	if (pre == NULL) // delete the node that hash's pointer points to
	{
		model->ElemHash[key] = cur->next;
		cur->next = NULL;
	}
	else // the other cases
	{
		pre->next = cur->next;
		cur->next = NULL;
	}

	free(pre);
	free(cur);
}

void polyModel_PrintAllElemData(PolyModel* model)
{
	printf("********** ELEMENT DATA **********\n");
	ELEM* elem = NULL;
	int *node_id;
	int *face_id;
	int i, j;
	int cnt=0;
	
	for (i = 0; i < model->ELEMHASHSIZE && cnt<=model->NumberOfElems; i++)
	{
		elem = model->ElemHash[i];

		while (elem)
		{
			printf("id : %d, ", elem->Global_id);
			cnt++;

			printf("face_id : ");
			for (j = 0; j < elem->AdjEnt_cnt; j++)
			{
				printf("%d ", elem->AdjEnt_id[j]);
			}
			printf("\n");
			elem = elem->next;
		}
	}
	free(elem);
}


void polyModel_PrintAllEdgeData(PolyModel* model)
{
	printf("********** EDGE DATA **********\n");
	PolyEdgeItr* editr = polyModel_CreateEdgeItr(model);
	int i;
	int cnt=1;
	
	for (polyEdgeItr_Begin(editr); polyEdgeItr_IsValid(editr); polyEdgeItr_Next(editr)){	
		PolyNode tmp;
		PolyEdge Edge = polyEdgeItr_GetCurr (editr);
		printf("Edge %d :", cnt);
		int no = polyEdge_GetNNodes (model, Edge);
		for (i=0; i<no; i++){
			tmp = polyEdge_GetNode (model, Edge, i);
			int t=polyNode_GetId(model, tmp);
			printf("%d ", t);
		}
		printf("\n");
		cnt++;
	}
	
	polyEdgeItr_Destroy(editr);
}



int polyElem_GetId(PolyModel *model, PolyElem PolyElemInput){
	int t=PolyElemInput.id;
	return t;
}

PolyElem polyElem_GetElemAtId(PolyModel *model, int IdInput){
	PolyElem t;
	t.id=IdInput;
	return t;
}

bool polyElem_IsValid(PolyModel *model, PolyElem PolyElemInput){
	int Id = polyElem_GetId(model, PolyElemInput);
	
	bool bo = Elem_IsValid(model, Id);
	
	return bo;
}

void polyElem_Copy(PolyModel *model, PolyElem PolyElem1, PolyElem PolyElem2){
	PolyElem1.id=PolyElem2.id;
}

bool polyElem_Compare(PolyModel *model, PolyElem PolyElem1, PolyElem PolyElem2){
	if(PolyElem1.id == PolyElem2.id){
		return true;
	}
	else{
		return false;
	}
}


int polyFace_GetId(PolyModel *model, PolyFace PolyFaceInput){
	int t=PolyFaceInput.id;
	return t;
}

PolyFace polyFace_GetFaceAtId(PolyModel *model, int IdInput){
	PolyFace t;
	t.id=IdInput;
	return t;
}

bool polyFace_IsValid(PolyModel *model, PolyFace PolyFaceInput){
	int Id = polyFace_GetId(model, PolyFaceInput);
	
	bool bo = Face_IsValid(model, Id);
	
	return bo;
}

void polyFace_Copy(PolyModel *model, PolyFace PolyFace1, PolyFace PolyFace2){
	PolyFace1.id=PolyFace2.id;
}

bool polyFace_Compare(PolyModel *model, PolyFace PolyFace1, PolyFace PolyFace2){
	if(PolyFace1.id == PolyFace2.id){
		return true;
	}
	else{
		return false;
	}
}


int polyNode_GetId(PolyModel *model, PolyNode PolyNodeInput){
	int t=PolyNodeInput.id;
	
	return t;
}

PolyNode polyNode_GetNodeAtId(PolyModel *model, int IdInput){
	PolyNode t;
	
	NODE* node=Node_GetNODEAtId(model, IdInput);
	t.id=node->Global_id;
	t.x=node->x;
	t.y=node->y;
	
	return t;
}

bool polyNode_IsValid(PolyModel *model, PolyNode PolyNodeInput){
	int Id = polyNode_GetId(model, PolyNodeInput);
	
	bool bo = Node_IsValid(model, Id);
	
	return bo;
}

bool polyNode_IsBoundary(PolyModel *model, PolyNode PolyNodeInput){
	int Id = polyNode_GetId(model, PolyNodeInput);
	
	bool bo = Node_IsBoundary(model, Id);
	
	return bo;
}

void polyNode_Copy(PolyModel *model, PolyNode PolyNode1, PolyNode PolyNode2){
	PolyNode1.id=PolyNode2.id;
}

bool polyNode_Compare(PolyModel *model, PolyNode PolyNode1, PolyNode PolyNode2){
	if(PolyNode1.id == PolyNode2.id){
		return true;
	}
	else{
		return false;
	}
}

PolyEdge polyEdge_SetEdge(PolyModel* model, int node1, int node2){
	PolyEdge polyedge;
	polyedge.EdgeTail=node1;
	polyedge.EdgeHead=node2;
	
	return polyedge;
}

bool polyEdge_IsValid(PolyModel *model, PolyEdge PolyEdgeInput){
	int Edgepair[2] = {PolyEdgeInput.EdgeTail,PolyEdgeInput.EdgeHead};
	
	bool bo = Edge_IsValid(model, Edgepair);
	
	return bo;
}

void polyEdge_Copy(PolyModel *model, PolyEdge PolyEdge1, PolyEdge PolyEdge2){
	PolyEdge1.EdgeTail=PolyEdge2.EdgeTail;
	PolyEdge1.EdgeHead=PolyEdge2.EdgeHead;
}

bool polyEdge_Compare(PolyModel *model, PolyEdge PolyEdge1, PolyEdge PolyEdge2){
	if(PolyEdge1.EdgeTail == PolyEdge2.EdgeTail&&PolyEdge1.EdgeHead == PolyEdge2.EdgeHead){
		return true;
	}
	else{
		return false;
	}
}


int polyModel_GetNNodes(PolyModel* model)
{
	return model->NumberOfNodes;
}

int polyModel_GetNFaces(PolyModel* model)
{
	return model->NumberOfFaces;
}

int polyModel_GetNElems(PolyModel* model)
{
	return model->NumberOfElems;
}


// Algorithmsforiterators

PolyElemItr* polyModel_CreateElemItr(PolyModel* model){
	PolyElemItr* polyelemitr=(PolyElemItr*)malloc(sizeof(PolyElemItr));

	polyelemitr->Itr_Elem_BucId=0;
	polyelemitr->Itr_Elem_Ptr=NULL;
	polyelemitr->Model=model;
	return polyelemitr;
}

void polyElemItr_Destroy (PolyElemItr* Polyelemitr){
	free(Polyelemitr);
}

void polyElemItr_Begin(PolyElemItr* Polyelemitr){
	for(Polyelemitr->Itr_Elem_BucId=0;Polyelemitr->Model->ElemHash[Polyelemitr->Itr_Elem_BucId]==NULL;Polyelemitr->Itr_Elem_BucId++) 
		;
	Polyelemitr->Itr_Elem_Ptr=Polyelemitr->Model->ElemHash[Polyelemitr->Itr_Elem_BucId];
	Polyelemitr->Itr_Elem_Cnt=1;
}

bool polyElemItr_IsValid(PolyElemItr* Polyelemitr){
	if(Polyelemitr->Itr_Elem_Cnt<=Polyelemitr->Model->NumberOfElems){
		return true;
	}
	else{
		return false;
	}
}

void polyElemItr_Next(PolyElemItr* Polyelemitr){
	if(Polyelemitr->Itr_Elem_Ptr->next!=NULL){
		Polyelemitr->Itr_Elem_Ptr=Polyelemitr->Itr_Elem_Ptr->next;
	}
	else{
		Polyelemitr->Itr_Elem_BucId++;
		while(Polyelemitr->Model->ElemHash[Polyelemitr->Itr_Elem_BucId]==NULL){
			if (Polyelemitr->Itr_Elem_BucId == Polyelemitr->Model->ELEMHASHSIZE) break;
			Polyelemitr->Itr_Elem_BucId++;
		}
		Polyelemitr->Itr_Elem_Ptr=Polyelemitr->Model->ElemHash[Polyelemitr->Itr_Elem_BucId];
	}
	Polyelemitr->Itr_Elem_Cnt++;
}

PolyElem polyElemItr_GetCurr(PolyElemItr* Polyelemitr){
	ELEM* Curr=Polyelemitr->Itr_Elem_Ptr;
	PolyElem elem;
	elem.id=Elem_GetIdAtELEM(Polyelemitr->Model, Curr);
	
	return elem;
}






PolyFaceItr* polyModel_CreateFaceItr(PolyModel* model){
	PolyFaceItr* polyfaceitr=(PolyFaceItr*)malloc(sizeof(PolyFaceItr));

	polyfaceitr->Itr_Face_BucId=0;
	polyfaceitr->Itr_Face_Ptr=NULL;
	polyfaceitr->Model=model;
	return polyfaceitr;
}

void polyFaceItr_Destroy (PolyFaceItr* Polyfaceitr){
	free(Polyfaceitr);
}

void polyFaceItr_Begin(PolyFaceItr* Polyfaceitr){
	for(Polyfaceitr->Itr_Face_BucId=0;Polyfaceitr->Model->FaceHash[Polyfaceitr->Itr_Face_BucId]==NULL;Polyfaceitr->Itr_Face_BucId++) 
		;
	Polyfaceitr->Itr_Face_Ptr=Polyfaceitr->Model->FaceHash[Polyfaceitr->Itr_Face_BucId];
	Polyfaceitr->Itr_Face_Cnt=1;
}

bool polyFaceItr_IsValid(PolyFaceItr* Polyfaceitr){
	if(Polyfaceitr->Itr_Face_Cnt<=Polyfaceitr->Model->NumberOfFaces){
		return true;
	}
	else{
		return false;
	}
}

void polyFaceItr_Next(PolyFaceItr* Polyfaceitr){
	if(Polyfaceitr->Itr_Face_Ptr->next!=NULL){
		Polyfaceitr->Itr_Face_Ptr=Polyfaceitr->Itr_Face_Ptr->next;
	}
	else{
		Polyfaceitr->Itr_Face_BucId++;
		while(Polyfaceitr->Model->FaceHash[Polyfaceitr->Itr_Face_BucId]==NULL){
			if (Polyfaceitr->Itr_Face_BucId == Polyfaceitr->Model->FACEHASHSIZE) break;
			Polyfaceitr->Itr_Face_BucId++;
		}
		Polyfaceitr->Itr_Face_Ptr=Polyfaceitr->Model->FaceHash[Polyfaceitr->Itr_Face_BucId];
	}
	Polyfaceitr->Itr_Face_Cnt++;
}

PolyFace polyFaceItr_GetCurr(PolyFaceItr* Polyfaceitr){
	FACE* Curr=Polyfaceitr->Itr_Face_Ptr;
	PolyFace face;
	face.id=Face_GetIdAtFACE(Polyfaceitr->Model, Curr);
	
	return face;
}






PolyNodeItr* polyModel_CreateNodeItr(PolyModel* model){
	PolyNodeItr* polynodeitr=(PolyNodeItr*)malloc(sizeof(PolyNodeItr));
	polynodeitr->Itr_Node_BucId=0;
	polynodeitr->Itr_Node_Ptr=NULL;
	polynodeitr->Model=model;
	return polynodeitr;
}

void polyNodeItr_Destroy(PolyNodeItr* Polynodeitr){
	free(Polynodeitr);
	
	return;
}

void polyNodeItr_Begin(PolyNodeItr* Polynodeitr){
	for(Polynodeitr->Itr_Node_BucId=0;Polynodeitr->Model->NodeHash[Polynodeitr->Itr_Node_BucId]==NULL;Polynodeitr->Itr_Node_BucId++) 
		;
	Polynodeitr->Itr_Node_Ptr=Polynodeitr->Model->NodeHash[Polynodeitr->Itr_Node_BucId];
	Polynodeitr->Itr_Node_Cnt=1;
}

bool polyNodeItr_IsValid(PolyNodeItr* Polynodeitr){
	if(Polynodeitr->Itr_Node_Cnt<=Polynodeitr->Model->NumberOfNodes){
		return true;
	}
	else{
		return false;
	}
}

void polyNodeItr_Next(PolyNodeItr* Polynodeitr){
	if(Polynodeitr->Itr_Node_Ptr->next!=NULL){
		Polynodeitr->Itr_Node_Ptr=Polynodeitr->Itr_Node_Ptr->next;
	}
	else{
		Polynodeitr->Itr_Node_BucId++;
		while(Polynodeitr->Model->NodeHash[Polynodeitr->Itr_Node_BucId]==NULL){
			if (Polynodeitr->Itr_Node_BucId == Polynodeitr->Model->NODEHASHSIZE) break;
			Polynodeitr->Itr_Node_BucId++;
		}
		Polynodeitr->Itr_Node_Ptr=Polynodeitr->Model->NodeHash[Polynodeitr->Itr_Node_BucId];
	}
	Polynodeitr->Itr_Node_Cnt++;
}

PolyNode polyNodeItr_GetCurr(PolyNodeItr* Polynodeitr){
	NODE* Curr=Polynodeitr->Itr_Node_Ptr;
	PolyNode node;
	node.id=Node_GetIdAtNODE(Polynodeitr->Model, Curr);
	node.x=Curr->x;
	node.y=Curr->y;
	
	return node;
}




PolyEdgeItr* polyModel_CreateEdgeItr(PolyModel* model){
	PolyEdgeItr* polyEdgeitr=(PolyEdgeItr*)malloc(sizeof(PolyEdgeItr));
	
	polyEdgeitr->Itr_FirstNodeItr=polyModel_CreateNodeItr(model);
	memset(polyEdgeitr->Itr_SecondNode_Id,0,sizeof(int)*sizeof(polyEdgeitr->Itr_SecondNode_Id)/sizeof(polyEdgeitr->Itr_SecondNode_Id[0]));
	polyEdgeitr->AdjEnt_cnt=0;
	polyEdgeitr->Curr_SecondNode_IdId=0;
	polyEdgeitr->Itr_Edge_Cnt=0;
	polyEdgeitr->Model=model;
	
	return polyEdgeitr;
}

void polyEdgeItr_Destroy(PolyEdgeItr* polyEdgeitr){
	polyNodeItr_Destroy(polyEdgeitr->Itr_FirstNodeItr);
	free(polyEdgeitr);
	
	return;
}

void polyEdgeItr_Begin(PolyEdgeItr* polyEdgeitr){
	polyNodeItr_Begin(polyEdgeitr->Itr_FirstNodeItr);
	PolyNode polynode=polyNodeItr_GetCurr(polyEdgeitr->Itr_FirstNodeItr);
	Edge Edgelist=Node_GetEdge(polyEdgeitr->Model,polynode.id);
	
	polyEdgeitr->AdjEnt_cnt=Edgelist.cnt;
	memset(polyEdgeitr->Itr_SecondNode_Id,0,sizeof(int)*sizeof(polyEdgeitr->Itr_SecondNode_Id)/sizeof(polyEdgeitr->Itr_SecondNode_Id[0]));

	int i;
	for(i=0;i<Edgelist.cnt;i++){
		polyEdgeitr->Itr_SecondNode_Id[i]=Edgelist.EdgeInfo[i][1];
	}
	polyEdgeitr->Curr_SecondNode_IdId=0;
	if(polyEdgeitr->Itr_SecondNode_Id[polyEdgeitr->Curr_SecondNode_IdId]<polynode.id){
		polyEdgeItr_Next(polyEdgeitr);
	}
	
	polyEdgeitr->Itr_Edge_Cnt=1;
	
	return;
}

bool polyEdgeItr_IsValid(PolyEdgeItr* polyEdgeitr){
	bool bo=polyNodeItr_IsValid(polyEdgeitr->Itr_FirstNodeItr);
	
	return bo;
}

void polyEdgeItr_Next(PolyEdgeItr* polyEdgeitr){
	PolyNode polynode=polyNodeItr_GetCurr(polyEdgeitr->Itr_FirstNodeItr);
	bool bo;
	
	do{
		if(polyEdgeitr->Curr_SecondNode_IdId+1<polyEdgeitr->AdjEnt_cnt){
			polyEdgeitr->Curr_SecondNode_IdId++;
		}
		else if(polyEdgeitr->Curr_SecondNode_IdId+1==polyEdgeitr->AdjEnt_cnt){
			polyNodeItr_Next(polyEdgeitr->Itr_FirstNodeItr);
			bo=polyNodeItr_IsValid(polyEdgeitr->Itr_FirstNodeItr);
			if(bo==false){
				break;
			}
			polynode=polyNodeItr_GetCurr(polyEdgeitr->Itr_FirstNodeItr);
			Edge Edgelist=Node_GetEdge(polyEdgeitr->Model,polynode.id);
			
			polyEdgeitr->AdjEnt_cnt=Edgelist.cnt;
			memset(polyEdgeitr->Itr_SecondNode_Id,0,sizeof(int)*sizeof(polyEdgeitr->Itr_SecondNode_Id)/sizeof(polyEdgeitr->Itr_SecondNode_Id[0]));

			int i;
			for(i=0;i<Edgelist.cnt;i++){
				polyEdgeitr->Itr_SecondNode_Id[i]=Edgelist.EdgeInfo[i][1];
			}
			polyEdgeitr->Curr_SecondNode_IdId=0;
		}
	}
	while(polyEdgeitr->Itr_SecondNode_Id[polyEdgeitr->Curr_SecondNode_IdId]<polynode.id);
	
	if(bo==true){
			polyEdgeitr->Itr_Edge_Cnt++;
	}
}

PolyEdge polyEdgeItr_GetCurr(PolyEdgeItr* polyEdgeitr){
	PolyNode polynode=polyNodeItr_GetCurr(polyEdgeitr->Itr_FirstNodeItr);
	PolyEdge Edge;
	
	Edge.EdgeTail=polynode.id;
	Edge.EdgeHead=polyEdgeitr->Itr_SecondNode_Id[polyEdgeitr->Curr_SecondNode_IdId];
	
	return Edge;
}


/**
	This part is the user-oriented functions for retrieval of adjacent information , which call
	those inner functions that returns a list of adjacent Info to the given entity .
		
	The first one : polyxxxx_GetNxxxxs
		Returns the number of adjacent Info . 
		Each time when polyxxxx_GetNxxxxs is called , 
		the adjacent Info is stored to PolyModel.CurrInfo
		
	The second one : polyxxxx_Getxxxx
		Returns one entity out of the adjaceny Info .
		parameter : The numbering in the Info list . 
		Each time when polyxxxx_Getxxxx is called , it first determine whether PolyModel.CurrInfo
		currently stores the required Info . If not , it executes polyxxxx_GetNxxxxs and returns
		the answer . 
*/
int polyElem_GetNElems(PolyModel* model, PolyElem PolyElemInput){
	int ElemInput=polyElem_GetId(model, PolyElemInput);
	model->CurrInfo.Type=1;
	polyElem_Copy(model,model->CurrInfo.CurrInfoPolyElem,PolyElemInput);
	model->CurrInfo.CurrInfoElem=Elem_GetElem(model, ElemInput);
	return model->CurrInfo.CurrInfoElem.cnt;
}
PolyElem polyElem_GetElem(PolyModel* model, PolyElem PolyElemInput, int Pos){
	if(model->CurrInfo.Type!=1||polyElem_Compare(model,model->CurrInfo.CurrInfoPolyElem,PolyElemInput)){
		polyElem_GetNElems(model, PolyElemInput);
	}
	int temp=model->CurrInfo.CurrInfoElem.ElemInfo[Pos];
	PolyElem t=polyElem_GetElemAtId(model, temp);
	return t;
}

int polyElem_GetNFaces(PolyModel* model, PolyElem PolyElemInput){
	int ElemInput=polyElem_GetId(model, PolyElemInput);
	model->CurrInfo.Type=2;
	polyElem_Copy(model,model->CurrInfo.CurrInfoPolyElem,PolyElemInput);
	model->CurrInfo.CurrInfoFace=Elem_GetFace(model, ElemInput);
	return model->CurrInfo.CurrInfoFace.cnt;
}
PolyFace polyElem_GetFace(PolyModel* model, PolyElem PolyElemInput, int Pos){
	if(model->CurrInfo.Type!=2||polyElem_Compare(model,model->CurrInfo.CurrInfoPolyElem,PolyElemInput)){
		polyElem_GetNFaces(model, PolyElemInput);
	}
	int temp=model->CurrInfo.CurrInfoFace.FaceInfo[Pos];
	PolyFace t=polyFace_GetFaceAtId(model, temp);
	return t;
}

int polyElem_GetNEdges(PolyModel* model, PolyElem PolyElemInput){
	int ElemInput=polyElem_GetId(model, PolyElemInput);
	model->CurrInfo.Type=3;
	polyElem_Copy(model,model->CurrInfo.CurrInfoPolyElem,PolyElemInput);
	model->CurrInfo.CurrInfoEdge=Elem_GetEdge(model, ElemInput);
	return model->CurrInfo.CurrInfoEdge.cnt;
}
PolyEdge polyElem_GetEdge(PolyModel* model, PolyElem PolyElemInput, int Pos){
	if(model->CurrInfo.Type!=3||polyElem_Compare(model,model->CurrInfo.CurrInfoPolyElem,PolyElemInput)){
		polyElem_GetNEdges(model, PolyElemInput);
	}
	PolyEdge t;
	t.EdgeTail=model->CurrInfo.CurrInfoEdge.EdgeInfo[Pos][0];
	t.EdgeHead=model->CurrInfo.CurrInfoEdge.EdgeInfo[Pos][1];
	return t;
}

int polyElem_GetNNodes(PolyModel* model, PolyElem PolyElemInput){
	int ElemInput=polyElem_GetId(model, PolyElemInput);
	model->CurrInfo.Type=4;
	polyElem_Copy(model,model->CurrInfo.CurrInfoPolyElem,PolyElemInput);
	model->CurrInfo.CurrInfoNode=Elem_GetNode(model, ElemInput);
	return model->CurrInfo.CurrInfoNode.cnt;
}
PolyNode polyElem_GetNode(PolyModel* model, PolyElem PolyElemInput, int Pos){
	if(model->CurrInfo.Type!=4||polyElem_Compare(model,model->CurrInfo.CurrInfoPolyElem,PolyElemInput)){
		polyElem_GetNNodes(model, PolyElemInput);
	}
	int temp=model->CurrInfo.CurrInfoNode.NodeInfo[Pos];
	PolyNode t=polyNode_GetNodeAtId(model, temp);
	return t;
}

int polyFace_GetNElems(PolyModel* model, PolyFace PolyFaceInput){
	int FaceInput=polyFace_GetId(model, PolyFaceInput);
	model->CurrInfo.Type=5;
	polyFace_Copy(model,model->CurrInfo.CurrInfoPolyFace,PolyFaceInput);
	model->CurrInfo.CurrInfoElem=Face_GetElem(model, FaceInput);
	return model->CurrInfo.CurrInfoElem.cnt;
}
PolyElem polyFace_GetElem(PolyModel* model, PolyFace PolyFaceInput, int Pos){
	if(model->CurrInfo.Type!=5||polyFace_Compare(model,model->CurrInfo.CurrInfoPolyFace,PolyFaceInput)){
		polyFace_GetNElems(model, PolyFaceInput);
	}
	int temp=model->CurrInfo.CurrInfoElem.ElemInfo[Pos];
	PolyElem t=polyElem_GetElemAtId(model, temp);
	return t;
}

int polyFace_GetNFaces(PolyModel* model, PolyFace PolyFaceInput){
	int FaceInput=polyFace_GetId(model, PolyFaceInput);
	model->CurrInfo.Type=6;
	polyFace_Copy(model,model->CurrInfo.CurrInfoPolyFace,PolyFaceInput);
	model->CurrInfo.CurrInfoFace=Face_GetFace(model, FaceInput);
	return model->CurrInfo.CurrInfoFace.cnt;
}
PolyFace polyFace_GetFace(PolyModel* model, PolyFace PolyFaceInput, int Pos){
	if(model->CurrInfo.Type!=6||polyFace_Compare(model,model->CurrInfo.CurrInfoPolyFace,PolyFaceInput)){
		polyFace_GetNFaces(model, PolyFaceInput);
	}
	int temp=model->CurrInfo.CurrInfoFace.FaceInfo[Pos];
	PolyFace t=polyFace_GetFaceAtId(model, temp);
	return t;
}

int polyFace_GetNEdges(PolyModel* model, PolyFace PolyFaceInput){
	int FaceInput=polyFace_GetId(model, PolyFaceInput);
	model->CurrInfo.Type=7;
	polyFace_Copy(model,model->CurrInfo.CurrInfoPolyFace,PolyFaceInput);
	model->CurrInfo.CurrInfoEdge=Face_GetEdge(model, FaceInput);
	return model->CurrInfo.CurrInfoEdge.cnt;
}
PolyEdge polyFace_GetEdge(PolyModel* model, PolyFace PolyFaceInput, int Pos){
	if(model->CurrInfo.Type!=7||polyFace_Compare(model,model->CurrInfo.CurrInfoPolyFace,PolyFaceInput)){
		polyFace_GetNEdges(model, PolyFaceInput);
	}
	PolyEdge t;
	t.EdgeTail=model->CurrInfo.CurrInfoEdge.EdgeInfo[Pos][0];
	t.EdgeHead=model->CurrInfo.CurrInfoEdge.EdgeInfo[Pos][1];
	return t;
}

int polyFace_GetNNodes(PolyModel* model, PolyFace PolyFaceInput){
	int FaceInput=polyFace_GetId(model, PolyFaceInput);
	model->CurrInfo.Type=8;
	polyFace_Copy(model,model->CurrInfo.CurrInfoPolyFace,PolyFaceInput);
	model->CurrInfo.CurrInfoNode=Face_GetNode(model, FaceInput);
	return model->CurrInfo.CurrInfoNode.cnt;
}
PolyNode polyFace_GetNode(PolyModel* model, PolyFace PolyFaceInput, int Pos){
	if(model->CurrInfo.Type!=8||polyFace_Compare(model,model->CurrInfo.CurrInfoPolyFace,PolyFaceInput)){
		polyFace_GetNNodes(model, PolyFaceInput);
	}
	int temp=model->CurrInfo.CurrInfoNode.NodeInfo[Pos];
	PolyNode t=polyNode_GetNodeAtId(model, temp);
	return t;
}

int polyEdge_GetNElems(PolyModel* model, PolyEdge PolyEdgeInput){
	int EdgeInput[2]={PolyEdgeInput.EdgeTail,PolyEdgeInput.EdgeHead};
	model->CurrInfo.Type=9;
	polyEdge_Copy(model,model->CurrInfo.CurrInfoPolyEdge,PolyEdgeInput);
	model->CurrInfo.CurrInfoElem=Edge_GetElem(model, EdgeInput);
	return model->CurrInfo.CurrInfoElem.cnt;
}
PolyElem polyEdge_GetElem(PolyModel* model, PolyEdge PolyEdgeInput, int Pos){
	if(model->CurrInfo.Type!=9||polyEdge_Compare(model,model->CurrInfo.CurrInfoPolyEdge,PolyEdgeInput)){
		polyEdge_GetNElems(model, PolyEdgeInput);
	}
	int temp=model->CurrInfo.CurrInfoElem.ElemInfo[Pos];
	PolyElem t=polyElem_GetElemAtId(model, temp);
	return t;
}

int polyEdge_GetNFaces(PolyModel* model, PolyEdge PolyEdgeInput){
	int EdgeInput[2]={PolyEdgeInput.EdgeTail,PolyEdgeInput.EdgeHead};
	model->CurrInfo.Type=10;
	polyEdge_Copy(model,model->CurrInfo.CurrInfoPolyEdge,PolyEdgeInput);
	model->CurrInfo.CurrInfoFace=Edge_GetFace(model, EdgeInput);
	return model->CurrInfo.CurrInfoFace.cnt;
}
PolyFace polyEdge_GetFace(PolyModel* model, PolyEdge PolyEdgeInput, int Pos){
	if(model->CurrInfo.Type!=10||polyEdge_Compare(model,model->CurrInfo.CurrInfoPolyEdge,PolyEdgeInput)){
		polyEdge_GetNFaces(model, PolyEdgeInput);
	}
	int temp=model->CurrInfo.CurrInfoFace.FaceInfo[Pos];
	PolyFace t=polyFace_GetFaceAtId(model, temp);
	return t;
}

int polyEdge_GetNEdges(PolyModel* model, PolyEdge PolyEdgeInput){
	int EdgeInput[2]={PolyEdgeInput.EdgeTail,PolyEdgeInput.EdgeHead};
	model->CurrInfo.Type=11;
	polyEdge_Copy(model,model->CurrInfo.CurrInfoPolyEdge,PolyEdgeInput);
	model->CurrInfo.CurrInfoEdge=Edge_GetEdge(model, EdgeInput);
	return model->CurrInfo.CurrInfoEdge.cnt;
}
PolyEdge polyEdge_GetEdge(PolyModel* model, PolyEdge PolyEdgeInput, int Pos){
	if(model->CurrInfo.Type!=11||polyEdge_Compare(model,model->CurrInfo.CurrInfoPolyEdge,PolyEdgeInput)){
		polyEdge_GetNEdges(model, PolyEdgeInput);
	}
	PolyEdge t;
	t.EdgeTail=model->CurrInfo.CurrInfoEdge.EdgeInfo[Pos][0];
	t.EdgeHead=model->CurrInfo.CurrInfoEdge.EdgeInfo[Pos][1];
	return t;
}

int polyEdge_GetNNodes(PolyModel* model, PolyEdge PolyEdgeInput){
	int EdgeInput[2]={PolyEdgeInput.EdgeTail,PolyEdgeInput.EdgeHead};
	model->CurrInfo.Type=12;
	model->CurrInfo.CurrInfoPolyEdge.EdgeTail=PolyEdgeInput.EdgeTail;
	model->CurrInfo.CurrInfoPolyEdge.EdgeHead=PolyEdgeInput.EdgeHead;
	polyEdge_Copy(model,model->CurrInfo.CurrInfoPolyEdge,PolyEdgeInput);
	model->CurrInfo.CurrInfoNode=Edge_GetNode(model, EdgeInput);
	return model->CurrInfo.CurrInfoNode.cnt;
}
PolyNode polyEdge_GetNode(PolyModel* model, PolyEdge PolyEdgeInput, int Pos){
	if(model->CurrInfo.Type!=12||polyEdge_Compare(model,model->CurrInfo.CurrInfoPolyEdge,PolyEdgeInput)){
		polyEdge_GetNNodes(model, PolyEdgeInput);
	}
	int temp=model->CurrInfo.CurrInfoNode.NodeInfo[Pos];
	PolyNode t=polyNode_GetNodeAtId(model, temp);
	return t;
}

int polyNode_GetNElems(PolyModel* model, PolyNode PolyNodeInput){
	int NodeInput=polyNode_GetId(model, PolyNodeInput);
	model->CurrInfo.Type=13;
	polyNode_Copy(model,model->CurrInfo.CurrInfoPolyNode,PolyNodeInput);
	model->CurrInfo.CurrInfoElem=Node_GetElem(model, NodeInput);
	return model->CurrInfo.CurrInfoElem.cnt;
}
PolyElem polyNode_GetElem(PolyModel* model, PolyNode PolyNodeInput, int Pos){
	if(model->CurrInfo.Type!=13||polyNode_Compare(model,model->CurrInfo.CurrInfoPolyNode,PolyNodeInput)){
		polyNode_GetNElems(model, PolyNodeInput);
	}
	int temp=model->CurrInfo.CurrInfoElem.ElemInfo[Pos];
	PolyElem t=polyElem_GetElemAtId(model, temp);
	return t;
}

int polyNode_GetNFaces(PolyModel* model, PolyNode PolyNodeInput){
	int NodeInput=polyNode_GetId(model, PolyNodeInput);
	model->CurrInfo.Type=14;
	polyNode_Copy(model,model->CurrInfo.CurrInfoPolyNode,PolyNodeInput);
	model->CurrInfo.CurrInfoFace=Node_GetFace(model, NodeInput);
	return model->CurrInfo.CurrInfoFace.cnt;
}
PolyFace polyNode_GetFace(PolyModel* model, PolyNode PolyNodeInput, int Pos){
	if(model->CurrInfo.Type!=14||polyNode_Compare(model,model->CurrInfo.CurrInfoPolyNode,PolyNodeInput)){
		polyNode_GetNFaces(model, PolyNodeInput);
	}
	int temp=model->CurrInfo.CurrInfoFace.FaceInfo[Pos];
	PolyFace t=polyFace_GetFaceAtId(model, temp);
	return t;
}

int polyNode_GetNEdges(PolyModel* model, PolyNode PolyNodeInput){
	int NodeInput=polyNode_GetId(model, PolyNodeInput);
	model->CurrInfo.Type=15;
	polyNode_Copy(model,model->CurrInfo.CurrInfoPolyNode,PolyNodeInput);
	model->CurrInfo.CurrInfoEdge=Node_GetEdge(model, NodeInput);
	return model->CurrInfo.CurrInfoEdge.cnt;
}
PolyEdge polyNode_GetEdge(PolyModel* model, PolyNode PolyNodeInput, int Pos){
	if(model->CurrInfo.Type!=15||polyNode_Compare(model,model->CurrInfo.CurrInfoPolyNode,PolyNodeInput)){
		polyNode_GetNEdges(model, PolyNodeInput);
	}
	PolyEdge t;
	t.EdgeTail=model->CurrInfo.CurrInfoEdge.EdgeInfo[Pos][0];
	t.EdgeHead=model->CurrInfo.CurrInfoEdge.EdgeInfo[Pos][1];
	return t;
}

int polyNode_GetNNodes(PolyModel* model, PolyNode PolyNodeInput){
	int NodeInput=polyNode_GetId(model, PolyNodeInput);
	model->CurrInfo.Type=16;
	polyNode_Copy(model,model->CurrInfo.CurrInfoPolyNode,PolyNodeInput);
	model->CurrInfo.CurrInfoNode=Node_GetNode(model, NodeInput);
	return model->CurrInfo.CurrInfoNode.cnt;
}
PolyNode polyNode_GetNode(PolyModel* model, PolyNode PolyNodeInput, int Pos){
	if(model->CurrInfo.Type!=16||polyNode_Compare(model,model->CurrInfo.CurrInfoPolyNode,PolyNodeInput)){
		polyNode_GetNNodes(model, PolyNodeInput);
	}
	int temp=model->CurrInfo.CurrInfoNode.NodeInfo[Pos];
	PolyNode t=polyNode_GetNodeAtId(model, temp);
	return t;
}






void polyElem_SetAttrib(PolyModel* model, PolyElem Polyelem, void* data){
	int id = polyElem_GetId(model, Polyelem);
	ELEM* elem = Elem_GetELEMAtId(model, id);
	
	elem->Attrib = data;
	
	return;
}

void polyFace_SetAttrib(PolyModel* model, PolyFace Polyface, void* data){
	int id = polyFace_GetId(model, Polyface);
	FACE* face = Face_GetFACEAtId(model, id);
	
	face->Attrib = data;
	
	return;
}

void polyNode_SetAttrib(PolyModel* model, PolyNode Polynode, void* data){
	int id = polyNode_GetId(model, Polynode);
	NODE* node = Node_GetNODEAtId(model, id);
	
	node->Attrib = data;
	
	return;
}

void* polyElem_GetAttrib(PolyModel* model, PolyElem Polyelem){
	int id = polyElem_GetId(model, Polyelem);
	ELEM* elem = Elem_GetELEMAtId(model, id);
	
	void* data = elem->Attrib;

	return data;
}

void* polyFace_GetAttrib(PolyModel* model, PolyFace Polyface){
	int id = polyFace_GetId(model, Polyface);
	FACE* face = Face_GetFACEAtId(model, id);
	
	void* data = face->Attrib;
	
	return data;
}

void* polyNode_GetAttrib(PolyModel* model, PolyNode Polynode){
	int id = polyNode_GetId(model, Polynode);
	NODE* node = Node_GetNODEAtId(model, id);
	
	void* data = node->Attrib;
	
	return data;
}

void polyElem_DeleteAttrib(PolyModel* model, PolyElem Polyelem){
	int id = polyElem_GetId(model, Polyelem);
	ELEM* elem = Elem_GetELEMAtId(model, id);
	
	free(elem->Attrib);
	
	return;
}

void polyFace_DeleteAttrib(PolyModel* model, PolyFace Polyface){
	int id = polyFace_GetId(model, Polyface);
	FACE* face = Face_GetFACEAtId(model, id);
	
	free(face->Attrib);
	
	return;
}

void polyNode_DeleteAttrib(PolyModel* model, PolyNode Polynode){
	int id = polyNode_GetId(model, Polynode);
	NODE* node = Node_GetNODEAtId(model, id);
	
	free(node->Attrib);

	return;
}