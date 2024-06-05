#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include "typedefines.h"
#include "data_structure.h"
#include "perf_test.h"
#include "cohesive3D.h"
#include "poly.h"

bool Cohesive3D_Elem_IsCohesive(PolyModel *model, int ElemInput){
	bool Iscoh = 0;
	ELEM* ELEMInfo=Elem_GetELEMAtId(model, ElemInput);
	int t =ELEMInfo->Element_Type;
	if(t!=0 && t!=1){
		Iscoh = 1;
	}
	return Iscoh;
}

bool Cohesive3D_Face_IsBarrier(PolyModel *model, int FaceInput){
	bool IsBarr=0;
	int i;
	
	Elem Elem = Face_GetElem(model,FaceInput);
	if(Elem.cnt == 1){
		IsBarr=1;
	}
	else if(Elem.cnt == 2){
		for(i=0;i<Elem.cnt;i++){
			if(Elem_IsBulk(model, Elem.ElemInfo[0])!=Elem_IsBulk(model, Elem.ElemInfo[1])){
				IsBarr=1;
			}
			else{
				// printf("Error! Check Cohesive3D_Face_IsBarrier \n");
			}
		}
	}
	else{
		// printf("Error! A face has at most 2 adjacent elements. Check Cohesive3D_Face_IsBarrier \n");
	}
	return IsBarr;
}

bool Cohesive3D_ElemElem_IsAdjacent(PolyModel *model, int ElemInput1, int ElemInput2, int FaceInput){
	bool isAdj = false;
	int i,j;
	
	Face Face1 = Elem_GetFace(model, ElemInput1);
	Face Face2 = Elem_GetFace(model, ElemInput2);
	
	for(i=0;i<Face1.cnt;i++){
		for(j=0;j<Face2.cnt;j++){
			if(Face1.FaceInfo[i]==Face2.FaceInfo[j] && Face1.FaceInfo[i]!=FaceInput){
				isAdj = true;
				break;
			}
		}
		if(isAdj == true){
			break;
		}
	}
	
	return isAdj;
}

Elem Cohesive3D_Node_GetBulkElem(PolyModel *model, int NodeInput){
	Elem ElemReturn;
  ElemReturn.cnt=0;
  memset(ElemReturn.ElemInfo,0,sizeof(int)*sizeof(ElemReturn.ElemInfo)/sizeof(ElemReturn.ElemInfo[0]));
  
  NODE* NodeCurr=Node_GetNODEAtId(model,NodeInput);
  if (NodeCurr==NULL) ElemReturn.cnt=-1;
  else{
  	int i;
  	for(i=0;i<NodeCurr->AdjEnt_cnt;i++){
  		if(Elem_IsBulk(model, NodeCurr->AdjEnt_id[i])){
  			ElemReturn.ElemInfo[ElemReturn.cnt++]=NodeCurr->AdjEnt_id[i];
  		}
  	}
  }
	return ElemReturn;
}

Elem Cohesive3D_Node_GetCohsElem(PolyModel *model, int NodeInput){
	Elem ElemReturn;
  ElemReturn.cnt=0;
  memset(ElemReturn.ElemInfo,0,sizeof(int)*sizeof(ElemReturn.ElemInfo)/sizeof(ElemReturn.ElemInfo[0]));
  
  NODE* NodeCurr=Node_GetNODEAtId(model,NodeInput);
  if (NodeCurr==NULL) ElemReturn.cnt=-1;
  else{
  	int i;
  	for(i=0;i<NodeCurr->AdjEnt_cnt;i++){
  		if(Elem_IsCohs(model, NodeCurr->AdjEnt_id[i])){
  			ElemReturn.ElemInfo[ElemReturn.cnt++]=NodeCurr->AdjEnt_id[i];
  		}
  	}
  }
	return ElemReturn;
}

void Cohesive3D_ElemElem_DFS(PolyModel *model, int ElemInput1, int ElemInput2, Elem ElemBulkToNodeCurr, int FaceInput, int* visited){
	int i,j;
	if(visited[1] == 1){
		return;
	}
	for(i=0;i<ElemBulkToNodeCurr.cnt;i++){
		if(ElemInput1 == ElemBulkToNodeCurr.ElemInfo[i]){
			visited[i] = 1;
		}
	}
	// printf("%d ", ElemInput1);
	if(Cohesive3D_ElemElem_IsAdjacent(model, ElemInput1, ElemInput2, FaceInput)){
		// printf("%d ", ElemInput2);
		visited[1] = 1;
		return;
	}
	for (j=0;j<ElemBulkToNodeCurr.cnt;j++){
		if(visited[1] == 1){
			break;
		}
		if (!visited[j] && Cohesive3D_ElemElem_IsAdjacent(model, ElemInput1, ElemBulkToNodeCurr.ElemInfo[j], FaceInput)) {
		    Cohesive3D_ElemElem_DFS(model, ElemBulkToNodeCurr.ElemInfo[j], ElemInput2, ElemBulkToNodeCurr, FaceInput,visited);
		}
	}
}


NodeCheck Cohesive3D_FaceNode_GatherElem(PolyModel *model, int NodeInput, int FaceInput, int Elem1, int Elem2){
/*
	This function is dependent on the legality of int NodeInput, int FaceInput, int Elem1, int Elem2 : 
			1. FaceInput must be a valid face for insertion of cohesive element on it . 
			2. NodeInput must be a member of FaceInput . 
			3. Elem1 and Elem2 must be two bulk elements that is adjacent to FaceInput . 
			
	Algorithm : 
			1. Find all bulk elements around NodeInput and store them in ElemBulkToNodeCurr. 
			2. Start from Elem1 to find a path to Elem2 through elements in set ElemBulkToNodeCurr (using DFS). 
			3. If the path exist, stop . 
			
	Analysis : 
			The function stops either when it finds Elem2 or when it gets all bulk elements on the same side with
			Elem1 . So it is favorable to return the set of elements that DFS has already captured, let's denote 
			it by ElemReturn . If the last member of ElemReturn is Elem2, then we don't need to duplicate node .
			Otherwise , we have to duplicate it . 
*/

	NodeCheck NodeCheckReturn;
  NodeCheckReturn.Node_Id=0;
  NodeCheckReturn.ElemHome_cnt=0;
  NodeCheckReturn.ElemAway_cnt=0;
  memset(NodeCheckReturn.ElemHome_Id,0,sizeof(NodeCheckReturn.ElemHome_Id));
	memset(NodeCheckReturn.ElemAway_Id,0,sizeof(NodeCheckReturn.ElemAway_Id));
	
	NodeCheckReturn.Node_Id=NodeInput;
	
	int i,j;
	/*
		Put Elem1 and Elem2 at the first two buckets of ElemBulkToNodeCurr for convenience
	*/
	Elem ElemBulkToNodeCurr = Cohesive3D_Node_GetBulkElem(model, NodeInput);
	
//	// printf("Elements from ElemBulkToNodeCurr: ");
//	for(i=0;i<ElemBulkToNodeCurr.cnt;i++){
//		// printf("%d ",ElemBulkToNodeCurr.ElemInfo[i]);
//	}
//	// printf("\n");
	
	for(i=0;i<ElemBulkToNodeCurr.cnt;i++){
		if(ElemBulkToNodeCurr.ElemInfo[i]==Elem1){
			swap(&ElemBulkToNodeCurr.ElemInfo[0],&ElemBulkToNodeCurr.ElemInfo[i]);
		}
		if(ElemBulkToNodeCurr.ElemInfo[i]==Elem2){
			swap(&ElemBulkToNodeCurr.ElemInfo[1],&ElemBulkToNodeCurr.ElemInfo[i]);
		}
	}
	
//	// printf("Elements from ElemBulkToNodeCurr: ");
//	for(i=0;i<ElemBulkToNodeCurr.cnt;i++){
//		// printf("%d ",ElemBulkToNodeCurr.ElemInfo[i]);
//	}
//	// printf("\n");
	
	int visited[100]={0,};
	
	// printf("Elements from DFS: ");
	
	Cohesive3D_ElemElem_DFS(model, Elem1, Elem2, ElemBulkToNodeCurr, FaceInput, visited);
	
	// printf("\n");
	
	if(visited[1] == 1){
		// printf("Node %d pass \n", NodeInput);
	}
	else{
		// printf("Node %d need to be duplicated. \n", NodeInput);
		for(i=0;i<ElemBulkToNodeCurr.cnt;i++){
			if(visited[i]==1){
				NodeCheckReturn.ElemHome_Id[NodeCheckReturn.ElemHome_cnt++]=ElemBulkToNodeCurr.ElemInfo[i];
			}
			else{
				NodeCheckReturn.ElemAway_Id[NodeCheckReturn.ElemAway_cnt++]=ElemBulkToNodeCurr.ElemInfo[i];
			}
		}
//		// printf("NodeCheckReturn.ElemAway_cnt = %d", NodeCheckReturn.ElemAway_cnt);
	}
	return NodeCheckReturn;
}

bool Cohesive3D_Face_CreateElem(PolyModel *model, int FaceInput, int NewElemId, int NewFaceId, int RefElem){
	bool bo=false;
	int i;
	
	if(Elem_IsValid(model, NewElemId)){
		// printf("Error! Insertion of elem failure : the elem with Id %d already exists. \n", NewElemId);
		bo = false;
	}
	else if(Face_IsValid(model, NewFaceId)){
		// printf("Error! Insertion of face failure : the face with Id %d already exists. \n", NewFaceId);
		bo = false;
	}
	else{
		/* 
			Create cohesive element in ElemHash
		*/
		ELEM *newelem = (ELEM*)malloc(sizeof(ELEM));
		newelem->Global_id = NewElemId;
		newelem->Element_Type = POLY_ELEMENT_COH3POLY;
		newelem->AdjEnt_id = (int*)malloc(sizeof(int)*2);
		newelem->AdjEnt_id[0] = FaceInput;
		newelem->AdjEnt_id[1] = NewFaceId;
		newelem->AdjEnt_cnt = 2; 
		polyModel_InsertElem(model, newelem); 
		
		/* 
			Create new face in FaceHash
		*/
		Node Nodelist = Face_GetNode(model, FaceInput);
		FACE *newface = (FACE*)malloc(sizeof(FACE));
		newface->Global_id = NewFaceId;
		newface->AdjEnt_id = (int*)malloc(sizeof(int)*Nodelist.cnt);
		for(i=0;i<Nodelist.cnt;i++){
			newface->AdjEnt_id[i]=Nodelist.NodeInfo[i];
		}
		newface->AdjEnt_cnt = Nodelist.cnt;
		polyModel_InsertFace(model, newface);

		/* 
			One of the adjacent elements to FaceInput should inherit the new face . 
			So we Update the face Id from FaceInput to NewFaceId in RefElem . 
		*/
		ELEM* ELEM = Elem_GetELEMAtId(model, RefElem);
		for(i=0;i<ELEM->AdjEnt_cnt;i++){
			if(ELEM->AdjEnt_id[i]==FaceInput){
				ELEM->AdjEnt_id[i]=NewFaceId;
			}
		}
		
		/* 
			And of course we have to put the newly inserted cohesive element into the adjacent 
			information of all those nodes in FaceInput . 
		*/
		int nodeCurrr;
		for(i=0;i<Nodelist.cnt;i++){
			nodeCurrr = Nodelist.NodeInfo[i];
			NODE* NODECurr = Node_GetNODEAtId(model, nodeCurrr);
			NODECurr->AdjEnt_cnt++;
			NODECurr->AdjEnt_id=(int*)realloc(NODECurr->AdjEnt_id,sizeof(int)*NODECurr->AdjEnt_cnt);
			NODECurr->AdjEnt_id[NODECurr->AdjEnt_cnt-1]=NewElemId;
		}
		
		bo = true;
	}
	
	return bo;
}

bool Cohesive3D_CreateNode(PolyModel* model, double x, double y, double z, bool isBoundary, int id){
	bool bo;
	if(Node_IsValid(model,id)==true){
		// printf("Error! Insertion of node failure : the node with id %d already exists. \n", id);
		bo = false;
	}
	else{
		NODE *newnode = (NODE*)malloc(sizeof(NODE));
	
		newnode->Global_id = id;
		newnode->x = x;
		newnode->y = y;
		newnode->z = z;
		newnode->AdjEnt_cnt = 1;
		newnode->AdjEnt_id = (int*)malloc(sizeof(int));
		newnode->AdjEnt_id[0] = 0;
		newnode->isBoundary=isBoundary;
		polyModel_InsertNode(model, newnode);
		bo = true;
	}
	
	return bo;
}


bool Cohesive3D_UpdateNode(PolyModel *model, int NodeCurr, int newnodeid, int Coh, Elem ElemUpdate, Elem ElemLocked){
	/*
		NodeCurr is the current node we are updating . newnodeid is the newly created node which is a 
		duplicate of NodeCurr
		
		All the faces which belongs to members of ElemUpdate and contains NodeCurr need to be updated . 
		
		After it , we delete the elements of ElemUpdate from the adjacency of NodeCurr . 
		We put the elements of ElemUpdate as well as the cohesive element Coh to the adjacency list of 
		newnodeid
	*/
	int i,j,k;
	
	for(i=0;i<ElemUpdate.cnt;i++){
		Face Facelist = Elem_GetFace(model,ElemUpdate.ElemInfo[i]);
		for(j=0;j<Facelist.cnt;j++){
			FACE* FACECurr=Face_GetFACEAtId(model,Facelist.FaceInfo[j]);
			for(k=0;k<FACECurr->AdjEnt_cnt;k++){
				if(FACECurr->AdjEnt_id[k]==NodeCurr){
					FACECurr->AdjEnt_id[k]=newnodeid;
				}
			}
		}
	}
		
	NODE* NODECurr=Node_GetNODEAtId(model,NodeCurr);
	
	// printf("Node info for %d before : ",NodeCurr);
	for(i=0;i<NODECurr->AdjEnt_cnt;i++){
		// printf("%d ", NODECurr->AdjEnt_id[i]);
	}	
	// printf("\n");
	
	for(j=0;j<ElemUpdate.cnt;j++){
		for(i=0;i<NODECurr->AdjEnt_cnt;i++){
			if(NODECurr->AdjEnt_id[i]==ElemUpdate.ElemInfo[j]){
				/*
					Move the last entry of NODECurr->AdjEnt_id[] to NODECurr->AdjEnt_id[i] . 
				*/
				NODECurr->AdjEnt_cnt--;
				NODECurr->AdjEnt_id[i]=NODECurr->AdjEnt_id[NODECurr->AdjEnt_cnt];
				NODECurr->AdjEnt_id[NODECurr->AdjEnt_cnt]=0;
				i--;
			}
		}	
	}	
	NODECurr->AdjEnt_id=(int*)realloc(NODECurr->AdjEnt_id,sizeof(int)*NODECurr->AdjEnt_cnt);
	
	
	// printf("Node info for %d after first : ",NodeCurr);
	for(i=0;i<NODECurr->AdjEnt_cnt;i++){
		// printf("%d ", NODECurr->AdjEnt_id[i]);
	}	
	// printf("\n");
	
	/*
		Analysis : 
				For all cohesive elements adjacent to NodeCurr ( set 0 ), some of them only belong to ElemUpdate community 
				( set 1 ) , some of them only belong to ElemLocked community ( set 2 ) , and some of them belong to both 
				communities ( set 3 ) . Set 1 to 3 is a *partition* of set 0 , and we want to find two sets 
						CohsToDeleteInOldNode = set 1 ( which is the complement of set 2 union set 3 ) ;
						CohsToAppendToNewNode = set 1 union set 3 .
		Algorithm : 
				1. 	Find all cohesive elements containing NodeCurr ( the old node ) and store in AllCohs and CohsToDeleteInOldNode
						Initialize empty CohsToAppendToNewNode . 
				2. 	Find the element elA that is adjacent to any of the element in ElemUpdate. (this is exactly set 1 
						union set 3 ) And store it to CohsToAppendToNewNode
				3. 	Find the element elB that is adjacent to any of the element in ElemLocked. (this is exactly set 2 
						union set 3 ) And delete it from CohsToDeleteInOldNode
	*/
	Elem AllCohs = Cohesive3D_Node_GetCohsElem(model, NodeCurr);
	bool isFriend;
	Elem CohsToDeleteInOldNode;
	CohsToDeleteInOldNode.cnt=AllCohs.cnt;
  memcpy(CohsToDeleteInOldNode.ElemInfo,AllCohs.ElemInfo,sizeof(int)*AllCohs.cnt);
	for(i=0;i<CohsToDeleteInOldNode.cnt;i++){
		isFriend = false;
		for(j=0;j<ElemLocked.cnt;j++){
			if(Cohesive3D_ElemElem_IsAdjacent(model, CohsToDeleteInOldNode.ElemInfo[i], ElemLocked.ElemInfo[j], 0)){
				isFriend = true;
			}
		}
		if(isFriend){
			CohsToDeleteInOldNode.cnt--;
			CohsToDeleteInOldNode.ElemInfo[i]=CohsToDeleteInOldNode.ElemInfo[CohsToDeleteInOldNode.cnt];
			i--;
		}
	}
	
	// printf("CohesToDeleteInOldNode : ");
	for(i=0;i<CohsToDeleteInOldNode.cnt;i++){
		// printf("%d ", CohsToDeleteInOldNode.ElemInfo[i]);
	}	
	// printf("\n");
	
	for(j=0;j<CohsToDeleteInOldNode.cnt;j++){
		for(i=0;i<NODECurr->AdjEnt_cnt;i++){
			if(NODECurr->AdjEnt_id[i]==CohsToDeleteInOldNode.ElemInfo[j]){
				/*
					Move the last entry of NODECurr->AdjEnt_id[] to NODECurr->AdjEnt_id[i] . 
				*/
				NODECurr->AdjEnt_cnt--;
				NODECurr->AdjEnt_id[i]=NODECurr->AdjEnt_id[NODECurr->AdjEnt_cnt];
				NODECurr->AdjEnt_id[NODECurr->AdjEnt_cnt]=0;
				i--;
			}
		}	
	}	
	NODECurr->AdjEnt_id=(int*)realloc(NODECurr->AdjEnt_id,sizeof(int)*NODECurr->AdjEnt_cnt);
	
	
	// printf("Node info for %d after second : ",NodeCurr);
	for(i=0;i<NODECurr->AdjEnt_cnt;i++){
		// printf("%d ", NODECurr->AdjEnt_id[i]);
	}	
	// printf("\n");
	
	
	Elem CohsToAppendToNewNode;
	CohsToAppendToNewNode.cnt=0;
  memset(CohsToAppendToNewNode.ElemInfo,0,sizeof(int)*sizeof(CohsToAppendToNewNode.ElemInfo)/sizeof(CohsToAppendToNewNode.ElemInfo[0]));
  for(i=0;i<AllCohs.cnt;i++){
		isFriend = false;
		for(j=0;j<ElemUpdate.cnt;j++){
			if(Cohesive3D_ElemElem_IsAdjacent(model, AllCohs.ElemInfo[i], ElemUpdate.ElemInfo[j], 0)){
				isFriend = true;
				break;
			}
		}
		if(isFriend){
			CohsToAppendToNewNode.ElemInfo[CohsToAppendToNewNode.cnt++]=AllCohs.ElemInfo[i];
		}
	}
	
	NODE* NODENew=Node_GetNODEAtId(model,newnodeid);
	NODENew->AdjEnt_cnt=ElemUpdate.cnt+CohsToAppendToNewNode.cnt;
	NODENew->AdjEnt_id=(int*)realloc(NODENew->AdjEnt_id,sizeof(int)*NODENew->AdjEnt_cnt);
	for(i=0;i<ElemUpdate.cnt;i++){
		NODENew->AdjEnt_id[i]=ElemUpdate.ElemInfo[i];
	}	
	for(i=0;i<CohsToAppendToNewNode.cnt;i++){
		NODENew->AdjEnt_id[i+ElemUpdate.cnt]=CohsToAppendToNewNode.ElemInfo[i];
	}	
	
	NODENew->x=NODECurr->x;
	NODENew->y=NODECurr->y;
	NODENew->z=NODECurr->z;
	return 1;
}



bool Cohesive3D_InsertCohesiveAtFace_Manual(PolyModel *model, int FaceInput){
	int i,j,k;
	char string[20] = {0,}; //for user's input during execution of this function
	
	bool bo_facevalid = Face_IsValid(model, FaceInput);
	if(!bo_facevalid){
		printf("Error! Face %d is not a valid one. Please input another one. \n", FaceInput);
	}
	else{
		Elem Elemlist = Face_GetElem(model,FaceInput);
		if(Elemlist.cnt == 1){
			 printf("Error! It is illegal to insert cohesive element at a boundary Face %d. Please input another one. \n", FaceInput);

		}
		else if(Elemlist.cnt != 2){
			 printf("Error! Face %d has neither 1 or 2 adjacent elements. Check Cohesive3D_InsertCohesiveAtFace_Manual\n", FaceInput);
		}
		else if(Elemlist.cnt == 2){
			if(!Elem_IsBulk(model, Elemlist.ElemInfo[0]) || !Elem_IsBulk(model, Elemlist.ElemInfo[1])){
				 printf("Error! Face %d is a barrier or is illegal. Please input another one. \n", FaceInput);
			}
			else if(Elem_IsBulk(model, Elemlist.ElemInfo[0]) && Elem_IsBulk(model, Elemlist.ElemInfo[1])){
				 printf("Face %d is valid for insertion . \n", FaceInput);
				int Elem1 = Elemlist.ElemInfo[0];
				int Elem2 = Elemlist.ElemInfo[1];

				NodeDupl_List NodeDuplList;
				NodeDuplList.Node_cnt=0;
				for(i=0;i<=NodeDupl_ListSize;i++){
					NodeDuplList.NodeDupl_ListInfo[i].Node_Id=0;
				 	NodeDuplList.NodeDupl_ListInfo[i].ElemHome_cnt=0;
				  NodeDuplList.NodeDupl_ListInfo[i].ElemAway_cnt=0;
				  memset(NodeDuplList.NodeDupl_ListInfo[i].ElemHome_Id,0,sizeof(NodeDuplList.NodeDupl_ListInfo[i].ElemHome_Id));
					memset(NodeDuplList.NodeDupl_ListInfo[i].ElemAway_Id,0,sizeof(NodeDuplList.NodeDupl_ListInfo[i].ElemAway_Id));
				}
			  
				Node Nodelist = Face_GetNode(model,FaceInput);
				
				for(i=0;i<Nodelist.cnt;i++){
					int NodeCurr = Nodelist.NodeInfo[i];
					NodeCheck NodeCheckCurr = Cohesive3D_FaceNode_GatherElem(model, NodeCurr, FaceInput, Elem1, Elem2);
					if(NodeCheckCurr.ElemAway_cnt!=0){  /* which means we have to store this node information */
						NodeDuplList.NodeDupl_ListInfo[NodeDuplList.Node_cnt].Node_Id=NodeCheckCurr.Node_Id;
						NodeDuplList.NodeDupl_ListInfo[NodeDuplList.Node_cnt].ElemHome_cnt=NodeCheckCurr.ElemHome_cnt;
						memcpy(NodeDuplList.NodeDupl_ListInfo[NodeDuplList.Node_cnt].ElemHome_Id,NodeCheckCurr.ElemHome_Id,sizeof(int)*NodeCheckCurr.ElemHome_cnt);
						NodeDuplList.NodeDupl_ListInfo[NodeDuplList.Node_cnt].ElemAway_cnt=NodeCheckCurr.ElemAway_cnt;
						memcpy(NodeDuplList.NodeDupl_ListInfo[NodeDuplList.Node_cnt].ElemAway_Id,NodeCheckCurr.ElemAway_Id,sizeof(int)*NodeCheckCurr.ElemAway_cnt);
						NodeDuplList.Node_cnt++;
					}	
				}
				
				/* 
					Now we have the list of nodes that are needed to be duplicated .  We First start from replicating the faces then 
					continue with duplicating the nodes . 
				*/
				
				/*
					Replication of FaceInput .
				*/
				bool isElemInserted = false;
				int newelemid, newfaceid;
				do{
					 printf("Face %d is valid for insertion . Current ELEMIDMAX is %d . \n", FaceInput, polyModel_GetELEMIDMAX(model));
					 printf("Please input a preferred Id for new cohesive element ( Enter any letter to select automatically ) : \n");
					scanf("%s", &string);
					if( (string[0] >= 'a' && string[0] <= 'z') || (string[0] >= 'A' && string[0] <= 'Z') ){
						newelemid = polyModel_GetELEMIDMAX(model)+1;
					}
					else{
						newelemid = atoi(string);
					}
					memset(string, '\0', sizeof(char)*20);
					 printf("Current FACEIDMAX is %d . \n", polyModel_GetFACEIDMAX(model));
					 printf("Please input a preferred Id for new face ( Enter any letter to select automatically ) : \n");
					scanf("%s", &string);
					if( (string[0] >= 'a' && string[0] <= 'z') || (string[0] >= 'A' && string[0] <= 'Z') ){
						newfaceid = polyModel_GetFACEIDMAX(model)+1;
					}
					else{
						newfaceid = atoi(string);
					}
				
					isElemInserted = Cohesive3D_Face_CreateElem(model, FaceInput, newelemid, newfaceid, Elem1);
				}while(!isElemInserted);
				/*
					Now we duplicate the nodes one by one . In this step , duplication of a node ( say , N_prev ), 
					will not affect the duplication of the next one ( say , N_next ) . 
					This is because when we duplicate N_prev , we only update the index of N_prev to N_prev_prime 
					in some of the elements , and renew NE table . None of the connectivities related to N_next
					or its ElemHome and ElemAway is changed . 
				*/
				int elemhome=0, elemaway=0;
				int elem_update=0;
				int elemlocation=0;
				for(i=0;i<NodeDuplList.Node_cnt;i++){
					
					int NodeDuplCurr = NodeDuplList.NodeDupl_ListInfo[i].Node_Id;
					printf("Node %d need to be duplicated. The reference elements for ElemHome are : \n Elem Ids : ", NodeDuplCurr);
					
					for(j=0;j<NodeDuplList.NodeDupl_ListInfo[i].ElemHome_cnt;j++){
						elemhome=NodeDuplList.NodeDupl_ListInfo[i].ElemHome_Id[j];
						printf("%d ", elemhome);
					}
					printf("\n");
					
					printf("The reference elements for ElemAway are : \n Elem Ids : ");
					for(j=0;j<NodeDuplList.NodeDupl_ListInfo[i].ElemAway_cnt;j++){
						elemaway=NodeDuplList.NodeDupl_ListInfo[i].ElemAway_Id[j];
						printf("%d ", elemaway);
					}
					printf("\n");
					
					do{
						// printf("Please select an element to insert a new node ( Enter any letter to select automatically ) : \n");
						memset(string, '\0', sizeof(char)*20);
						scanf("%s", &string);
						if( (string[0] >= 'a' && string[0] <= 'z') || (string[0] >= 'A' && string[0] <= 'Z') ){
							elem_update = Elem1;
						}
						else{
							elem_update = atoi(string);
						}
						
						/*
								elemlocation indicates which group to receive the new node Id . 
								case 1 : elemlocation == 1 ElemHome receives 
								case 2 : elemlocation == 2 ElemAway receives 
						*/

						for(j=0;j<NodeDuplList.NodeDupl_ListInfo[i].ElemHome_cnt;j++){
							if(elem_update == NodeDuplList.NodeDupl_ListInfo[i].ElemHome_Id[j]){
								elemlocation=1;
								break;
							}
						}
						if(j==NodeDuplList.NodeDupl_ListInfo[i].ElemHome_cnt){
							elemlocation=0;
						}
						if(elemlocation!=1){
							for(j=0;j<NodeDuplList.NodeDupl_ListInfo[i].ElemAway_cnt;j++){
								if(elem_update == NodeDuplList.NodeDupl_ListInfo[i].ElemAway_Id[j]){
									elemlocation=2;
								}
							}
						}
						if(!elemlocation){
							printf("Error! Please input an available element Id. \n");
						}
					}while(!elemlocation);
					
//					// printf("elemlocation =%d \n", elemlocation);
					
					Elem ElemUpdate, ElemLocked;
					ElemUpdate.cnt=0;
  				memset(ElemUpdate.ElemInfo,0,sizeof(ElemUpdate.ElemInfo));
  				ElemLocked.cnt=0;
  				memset(ElemLocked.ElemInfo,0,sizeof(ElemLocked.ElemInfo));
  				
  				if(elemlocation==1){
  					ElemUpdate.cnt=NodeDuplList.NodeDupl_ListInfo[i].ElemHome_cnt;
  					memcpy(ElemUpdate.ElemInfo,NodeDuplList.NodeDupl_ListInfo[i].ElemHome_Id,sizeof(int)*ElemUpdate.cnt);
  				  ElemLocked.cnt=NodeDuplList.NodeDupl_ListInfo[i].ElemAway_cnt;
  					memcpy(ElemLocked.ElemInfo,NodeDuplList.NodeDupl_ListInfo[i].ElemAway_Id,sizeof(int)*ElemLocked.cnt);
  				}
  				else if(elemlocation==2){
  					ElemUpdate.cnt=NodeDuplList.NodeDupl_ListInfo[i].ElemAway_cnt;
  					memcpy(ElemUpdate.ElemInfo,NodeDuplList.NodeDupl_ListInfo[i].ElemAway_Id,sizeof(int)*ElemUpdate.cnt);
  				  ElemLocked.cnt=NodeDuplList.NodeDupl_ListInfo[i].ElemHome_cnt;
  					memcpy(ElemLocked.ElemInfo,NodeDuplList.NodeDupl_ListInfo[i].ElemHome_Id,sizeof(int)*ElemLocked.cnt);
  				}
					
					printf("Node %d will be Duplicated in Elem %d and its neighbors. \n", NodeDuplCurr, elem_update);
					
//					printf("ElemUpdate : ");
//					for(j=0;j<ElemUpdate.cnt;j++){
//						// printf("%d ",ElemUpdate.ElemInfo[j]);
//					}
					// printf("\n");
//					// printf("ElemLocked : ");
//					for(j=0;j<ElemLocked.cnt;j++){
//						// printf("%d ",ElemLocked.ElemInfo[j]);
//					}
//					// printf("\n");
					
					bool nodeId_available_flag = false;
					int newnodeid;
					do{
						printf("Current NODEIDMAX is %d . \n", polyModel_GetNODEIDMAX(model));
						printf("Please input a preferred Id for new node: ( Enter any letter to select automatically ) : \n");
						memset(string, '\0', sizeof(char)*20);
						scanf("%s", &string);
						if( (string[0] >= 'a' && string[0] <= 'z') || (string[0] >= 'A' && string[0] <= 'Z') ){
							newnodeid = polyModel_GetNODEIDMAX(model)+1;
						}
						else{
							newnodeid = atoi(string);
						}
						nodeId_available_flag = Cohesive3D_CreateNode(model, 0, 0, 0, 1, newnodeid);
						if( !nodeId_available_flag ){
							printf("Error! Please input an available node Id. \n");
						}
					}while( !nodeId_available_flag );

					Cohesive3D_UpdateNode(model, NodeDuplCurr, newnodeid, newelemid, ElemUpdate, ElemLocked);

					printf("\n ********** Node %d Duplicated . New Node %d in Elem %d . ********** \n\n", NodeDuplCurr, newnodeid, ElemUpdate.ElemInfo[0]);
				}
			}
		}
	}
}


int Cohesive3D_InsertCohesiveAtFace_Auto(PolyModel *model, int FaceInput){
	int i,j,k;
//	char string[20] = {0,}; //for user's input during execution of this function
	
	bool bo_facevalid = Face_IsValid(model, FaceInput);
	if(!bo_facevalid){
//		printf("Error! Face %d is not a valid one. Please input another one. \n", FaceInput);
	}
	else{
		Elem Elemlist = Face_GetElem(model,FaceInput);
		if(Elemlist.cnt == 1){
//			 printf("Error! It is illegal to insert cohesive element at a boundary Face %d. Please input another one. \n", FaceInput);

		}
		else if(Elemlist.cnt != 2){
//			 printf("Error! Face %d has neither 1 or 2 adjacent elements. Check Cohesive3D_InsertCohesiveAtFace_Manual\n", FaceInput);
		}
		else if(Elemlist.cnt == 2){
			if(!Elem_IsBulk(model, Elemlist.ElemInfo[0]) || !Elem_IsBulk(model, Elemlist.ElemInfo[1])){
//				 printf("Error! Face %d is a barrier or is illegal. Please input another one. \n", FaceInput);
			}
			else if(Elem_IsBulk(model, Elemlist.ElemInfo[0]) && Elem_IsBulk(model, Elemlist.ElemInfo[1])){
//				 printf("Face %d is valid for insertion . \n", FaceInput);
				int Elem1 = Elemlist.ElemInfo[0];
				int Elem2 = Elemlist.ElemInfo[1];

				NodeDupl_List NodeDuplList;
				NodeDuplList.Node_cnt=0;
				for(i=0;i<=NodeDupl_ListSize;i++){
					NodeDuplList.NodeDupl_ListInfo[i].Node_Id=0;
				 	NodeDuplList.NodeDupl_ListInfo[i].ElemHome_cnt=0;
				  NodeDuplList.NodeDupl_ListInfo[i].ElemAway_cnt=0;
				  memset(NodeDuplList.NodeDupl_ListInfo[i].ElemHome_Id,0,sizeof(NodeDuplList.NodeDupl_ListInfo[i].ElemHome_Id));
					memset(NodeDuplList.NodeDupl_ListInfo[i].ElemAway_Id,0,sizeof(NodeDuplList.NodeDupl_ListInfo[i].ElemAway_Id));
				}
			  
				Node Nodelist = Face_GetNode(model,FaceInput);
				
				for(i=0;i<Nodelist.cnt;i++){
					int NodeCurr = Nodelist.NodeInfo[i];
					NodeCheck NodeCheckCurr = Cohesive3D_FaceNode_GatherElem(model, NodeCurr, FaceInput, Elem1, Elem2);
					if(NodeCheckCurr.ElemAway_cnt!=0){  /* which means we have to store this node information */
						NodeDuplList.NodeDupl_ListInfo[NodeDuplList.Node_cnt].Node_Id=NodeCheckCurr.Node_Id;
						NodeDuplList.NodeDupl_ListInfo[NodeDuplList.Node_cnt].ElemHome_cnt=NodeCheckCurr.ElemHome_cnt;
						memcpy(NodeDuplList.NodeDupl_ListInfo[NodeDuplList.Node_cnt].ElemHome_Id,NodeCheckCurr.ElemHome_Id,sizeof(int)*NodeCheckCurr.ElemHome_cnt);
						NodeDuplList.NodeDupl_ListInfo[NodeDuplList.Node_cnt].ElemAway_cnt=NodeCheckCurr.ElemAway_cnt;
						memcpy(NodeDuplList.NodeDupl_ListInfo[NodeDuplList.Node_cnt].ElemAway_Id,NodeCheckCurr.ElemAway_Id,sizeof(int)*NodeCheckCurr.ElemAway_cnt);
						NodeDuplList.Node_cnt++;
					}	
				}
				
				/* 
					Now we have the list of nodes that are needed to be duplicated .  We First start from replicating the faces then 
					continue with duplicating the nodes . 
				*/
				
				/*
					Replication of FaceInput .
				*/
				int newelemid, newfaceid;
				newelemid = polyModel_GetELEMIDMAX(model)+1;
				newfaceid = polyModel_GetFACEIDMAX(model)+1;
				Cohesive3D_Face_CreateElem(model, FaceInput, newelemid, newfaceid, Elem1);

				/*
					Now we duplicate the nodes one by one . In this step , duplication of a node ( say , N_prev ), 
					will not affect the duplication of the next one ( say , N_next ) . 
					This is because when we duplicate N_prev , we only update the index of N_prev to N_prev_prime 
					in some of the elements , and renew NE table . None of the connectivities related to N_next
					or its ElemHome and ElemAway is changed . 
				*/
				int elemhome=0, elemaway=0;
				int elem_update=0;
				int elemlocation=0;
				for(i=0;i<NodeDuplList.Node_cnt;i++){
					int NodeDuplCurr = NodeDuplList.NodeDupl_ListInfo[i].Node_Id;
					elem_update = Elem1;
					/*
							Elem 1 is always in ElemHome
					*/
					Elem ElemUpdate, ElemLocked;
					ElemUpdate.cnt=0;
  				memset(ElemUpdate.ElemInfo,0,sizeof(ElemUpdate.ElemInfo));
  				ElemLocked.cnt=0;
  				memset(ElemLocked.ElemInfo,0,sizeof(ElemLocked.ElemInfo));
  				

					ElemUpdate.cnt=NodeDuplList.NodeDupl_ListInfo[i].ElemHome_cnt;
					memcpy(ElemUpdate.ElemInfo,NodeDuplList.NodeDupl_ListInfo[i].ElemHome_Id,sizeof(int)*ElemUpdate.cnt);
				  ElemLocked.cnt=NodeDuplList.NodeDupl_ListInfo[i].ElemAway_cnt;
					memcpy(ElemLocked.ElemInfo,NodeDuplList.NodeDupl_ListInfo[i].ElemAway_Id,sizeof(int)*ElemLocked.cnt);
					

					int newnodeid = polyModel_GetNODEIDMAX(model)+1;
					Cohesive3D_CreateNode(model, 0, 0, 0, 1, newnodeid);
					Cohesive3D_UpdateNode(model, NodeDuplCurr, newnodeid, newelemid, ElemUpdate, ElemLocked);

//					printf("\n ********** Node %d Duplicated . New Node %d in Elem %d . ********** \n\n", NodeDuplCurr, newnodeid, ElemUpdate.ElemInfo[0]);
				
				return newelemid;
				}
			}
		}
	}
}


bool Cohesive3D_InsertCohesiveAtFace_Test(PolyModel *model, int* reflist){
	/*
		Structure of reflist : 
		[Node1 Node2 Node3 Refelem1 Refelem2 Refelem3 CohNode1 CohNode2 CohNode3 CohNode4 CohNode5 CohNode6]

		Refelem corresponds to the newly inserted CohNode
	*/
	int FaceInput = Test_Nodes_GetFace(model, reflist[0], reflist[1], reflist[2]);
	
	
	int i,j,k;
	char string[20] = {0,}; //for user's input during execution of this function
	
	bool bo_facevalid = Face_IsValid(model, FaceInput);
	if(!bo_facevalid){
//		// printf("Error! Face %d is not a valid one. Please input another one. \n", FaceInput);
	}
	else{
		Elem Elemlist = Face_GetElem(model,FaceInput);
		if(Elemlist.cnt == 1){
//			// printf("Error! It is illegal to insert cohesive element at a boundary Face %d. Please input another one. \n", FaceInput);

		}
		else if(Elemlist.cnt != 2){
//			// printf("Error! Face %d has neither 1 or 2 adjacent elements. Check Cohesive3D_InsertCohesiveAtFace_Manual\n", FaceInput);
		}
		else if(Elemlist.cnt == 2){
			if(!Elem_IsBulk(model, Elemlist.ElemInfo[0]) || !Elem_IsBulk(model, Elemlist.ElemInfo[1])){
//				// printf("Error! Face %d is a barrier or is illegal. Please input another one. \n", FaceInput);
			}
			else if(Elem_IsBulk(model, Elemlist.ElemInfo[0]) && Elem_IsBulk(model, Elemlist.ElemInfo[1])){
//				// printf("Face %d is valid for insertion . \n", FaceInput);
				int Elem1 = Elemlist.ElemInfo[0];
				int Elem2 = Elemlist.ElemInfo[1];

				NodeDupl_List NodeDuplList;
				NodeDuplList.Node_cnt=0;
				for(i=0;i<=NodeDupl_ListSize;i++){
					NodeDuplList.NodeDupl_ListInfo[i].Node_Id=0;
				 	NodeDuplList.NodeDupl_ListInfo[i].ElemHome_cnt=0;
				  NodeDuplList.NodeDupl_ListInfo[i].ElemAway_cnt=0;
				  memset(NodeDuplList.NodeDupl_ListInfo[i].ElemHome_Id,0,sizeof(NodeDuplList.NodeDupl_ListInfo[i].ElemHome_Id));
					memset(NodeDuplList.NodeDupl_ListInfo[i].ElemAway_Id,0,sizeof(NodeDuplList.NodeDupl_ListInfo[i].ElemAway_Id));
				}
			  
				Node Nodelist = Face_GetNode(model,FaceInput);
				
				for(i=0;i<Nodelist.cnt;i++){
					int NodeCurr = Nodelist.NodeInfo[i];
					NodeCheck NodeCheckCurr = Cohesive3D_FaceNode_GatherElem(model, NodeCurr, FaceInput, Elem1, Elem2);
					if(NodeCheckCurr.ElemAway_cnt!=0){  /* which means we have to store this node information */
						NodeDuplList.NodeDupl_ListInfo[NodeDuplList.Node_cnt].Node_Id=NodeCheckCurr.Node_Id;
						NodeDuplList.NodeDupl_ListInfo[NodeDuplList.Node_cnt].ElemHome_cnt=NodeCheckCurr.ElemHome_cnt;
						memcpy(NodeDuplList.NodeDupl_ListInfo[NodeDuplList.Node_cnt].ElemHome_Id,NodeCheckCurr.ElemHome_Id,sizeof(int)*NodeCheckCurr.ElemHome_cnt);
						NodeDuplList.NodeDupl_ListInfo[NodeDuplList.Node_cnt].ElemAway_cnt=NodeCheckCurr.ElemAway_cnt;
						memcpy(NodeDuplList.NodeDupl_ListInfo[NodeDuplList.Node_cnt].ElemAway_Id,NodeCheckCurr.ElemAway_Id,sizeof(int)*NodeCheckCurr.ElemAway_cnt);
						NodeDuplList.Node_cnt++;
					}	
				}
				
				/* 
					Now we have the list of nodes that are needed to be duplicated .  We First start from replicating the faces then 
					continue with duplicating the nodes . 
				*/
				
				/*
					Replication of FaceInput .
				*/
				bool isElemInserted = false;
				int newelemid, newfaceid;
				newelemid = polyModel_GetELEMIDMAX(model)+1;
				newfaceid = polyModel_GetFACEIDMAX(model)+1;
				isElemInserted = Cohesive3D_Face_CreateElem(model, FaceInput, newelemid, newfaceid, Elem1);
				
				/*
					Now we duplicate the nodes one by one . In this step , duplication of a node ( say , N_prev ), 
					will not affect the duplication of the next one ( say , N_next ) . 
					This is because when we duplicate N_prev , we only update the index of N_prev to N_prev_prime 
					in some of the elements , and renew NE table . None of the connectivities related to N_next
					or its ElemHome and ElemAway is changed . 
				*/
				int elemhome=0, elemaway=0;
				int elem_update=0;
				int elemlocation=0;
				for(i=0;i<NodeDuplList.Node_cnt;i++){
					
					int NodeDuplCurr = NodeDuplList.NodeDupl_ListInfo[i].Node_Id;
//					// printf("Node %d need to be duplicated. The reference elements for ElemHome are : \n Elem Ids : ", NodeDuplCurr);
					
//					for(j=0;j<NodeDuplList.NodeDupl_ListInfo[i].ElemHome_cnt;j++){
//						elemhome=NodeDuplList.NodeDupl_ListInfo[i].ElemHome_Id[j];
//						// printf("%d ", elemhome);
//					}
//					// printf("\n");
					
//					// printf("The reference elements for ElemAway are : \n Elem Ids : ");
//					for(j=0;j<NodeDuplList.NodeDupl_ListInfo[i].ElemAway_cnt;j++){
//						elemaway=NodeDuplList.NodeDupl_ListInfo[i].ElemAway_Id[j];
//						// printf("%d ", elemaway);
//					}
//					// printf("\n");
					
					int newnodeid;
					for(j=0;j<3;j++){
						if(NodeDuplCurr==reflist[6+j]||NodeDuplCurr==reflist[9+j]){
							elem_update = reflist[3+j];
							if(reflist[6+j]<reflist[9+j]){
								newnodeid=reflist[9+j];
							}
							else{
								newnodeid=reflist[6+j];
							}
						}
					}
					/*
							elemlocation indicates which group to receive the new node Id . 
							case 1 : elemlocation == 1 ElemHome receives 
							case 2 : elemlocation == 2 ElemAway receives 
					*/

					for(j=0;j<NodeDuplList.NodeDupl_ListInfo[i].ElemHome_cnt;j++){
						if(elem_update == NodeDuplList.NodeDupl_ListInfo[i].ElemHome_Id[j]){
							elemlocation=1;
							break;
						}
					}
					if(j==NodeDuplList.NodeDupl_ListInfo[i].ElemHome_cnt){
						elemlocation=0;
					}
					if(elemlocation!=1){
						for(j=0;j<NodeDuplList.NodeDupl_ListInfo[i].ElemAway_cnt;j++){
							if(elem_update == NodeDuplList.NodeDupl_ListInfo[i].ElemAway_Id[j]){
								elemlocation=2;
							}
						}
					}
					
//					// printf("elemlocation =%d \n", elemlocation);
					
					Elem ElemUpdate, ElemLocked;
					ElemUpdate.cnt=0;
  				memset(ElemUpdate.ElemInfo,0,sizeof(ElemUpdate.ElemInfo));
  				ElemLocked.cnt=0;
  				memset(ElemLocked.ElemInfo,0,sizeof(ElemLocked.ElemInfo));
  				
  				if(elemlocation==1){
  					ElemUpdate.cnt=NodeDuplList.NodeDupl_ListInfo[i].ElemHome_cnt;
  					memcpy(ElemUpdate.ElemInfo,NodeDuplList.NodeDupl_ListInfo[i].ElemHome_Id,sizeof(int)*ElemUpdate.cnt);
  				  ElemLocked.cnt=NodeDuplList.NodeDupl_ListInfo[i].ElemAway_cnt;
  					memcpy(ElemLocked.ElemInfo,NodeDuplList.NodeDupl_ListInfo[i].ElemAway_Id,sizeof(int)*ElemLocked.cnt);
  				}
  				else if(elemlocation==2){
  					ElemUpdate.cnt=NodeDuplList.NodeDupl_ListInfo[i].ElemAway_cnt;
  					memcpy(ElemUpdate.ElemInfo,NodeDuplList.NodeDupl_ListInfo[i].ElemAway_Id,sizeof(int)*ElemUpdate.cnt);
  				  ElemLocked.cnt=NodeDuplList.NodeDupl_ListInfo[i].ElemHome_cnt;
  					memcpy(ElemLocked.ElemInfo,NodeDuplList.NodeDupl_ListInfo[i].ElemHome_Id,sizeof(int)*ElemLocked.cnt);
  				}
					
					// printf("Node %d will be Duplicated in Elem %d and its neighbors. \n", NodeDuplCurr, elem_update);
					
					// printf("ElemUpdate : ");
					for(j=0;j<ElemUpdate.cnt;j++){
						// printf("%d ",ElemUpdate.ElemInfo[j]);
					}
					// printf("\n");
//					// printf("ElemLocked : ");
//					for(j=0;j<ElemLocked.cnt;j++){
//						// printf("%d ",ElemLocked.ElemInfo[j]);
//					}
//					// printf("\n");
					
					bool nodeId_available_flag = false;

					nodeId_available_flag = Cohesive3D_CreateNode(model, 0, 0, 0, 1, newnodeid);
					if( !nodeId_available_flag ){
						// printf("Error! Please input an available node Id. \n");
					}

					Cohesive3D_UpdateNode(model, NodeDuplCurr, newnodeid, newelemid, ElemUpdate, ElemLocked);

					// printf("\n ********** Node %d Duplicated . New Node %d in Elem %d . ********** \n\n", NodeDuplCurr, newnodeid, ElemUpdate.ElemInfo[0]);
				}
			}
		}
	}
}




int Test_Nodes_GetFace(PolyModel* model, int Node1, int Node2, int Node3){
	Face Face1 = Node_GetFace(model,Node1);
	Face Face2 = Node_GetFace(model,Node2);
	Face Face3 = Node_GetFace(model,Node3);
	
	int i,j,k;
	int face = 0;
	
	for(i=0;i<Face1.cnt&&!face;i++){
		for(j=0;j<Face2.cnt&&!face;j++){
			for(k=0;k<Face3.cnt&&!face;k++){
				if(Face1.FaceInfo[i]==Face2.FaceInfo[j] && Face1.FaceInfo[i]==Face3.FaceInfo[k]){
					face = Face1.FaceInfo[i];
				}
			}
		}
	}
	return face;
}



void swap(int* a, int* b)
{
	int tmp = *a;
	*a = *b;
	*b = tmp;
}