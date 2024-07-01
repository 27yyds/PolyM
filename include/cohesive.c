#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include "typedefines.h"
#include "data_structure.h"
#include "perf_test.h"
#include "cohesive.h"
#include "poly.h"

bool Cohesive2D_Face_IsCohesive(PolyModel *model, int FaceInput){
	FACE* FACEInfo=Face_GetFACEAtId(model, FaceInput);
	bool Iscoh=FACEInfo->isCohesive;
	
	return Iscoh;
}

          
int Cohesive2D_FaceEdge_GetNode(PolyModel *model, int FaceInput, int NodeDupl, int NodePrev){
	int k;
	int prev,next;
	
	Node Nodelist=Face_GetNode(model,FaceInput);
	for(k=0;k<Nodelist.cnt;k++){
		if(Nodelist.NodeInfo[k]==NodeDupl){
			break;
		}
	}
	/**
		Record the edges
	*/
	if(k==0){
    prev=Nodelist.NodeInfo[Nodelist.cnt-1];
    next=Nodelist.NodeInfo[1];
  }
  else if(k==Nodelist.cnt-1){
    prev=Nodelist.NodeInfo[Nodelist.cnt-2];
    next=Nodelist.NodeInfo[0];
  }
  else{
    prev=Nodelist.NodeInfo[k-1];
    next=Nodelist.NodeInfo[k+1];
  }

	int Nodereturn;
  if(prev!=NodePrev&&next!=NodePrev){
  	printf("Error! The input edge is not contained in the input element.\n");
		Nodereturn=0;
  }
  else{
  	if(prev==NodePrev){
  		Nodereturn=next;
  	}
  	else{
  		Nodereturn=prev;
  	}
  }
  return Nodereturn;
}

int Cohesive2D_FaceEdge_GetFace(PolyModel *model, int FaceInput, int NodeDupl, int NodePrev){
	int Facereturn;
	
	int EdgeInput[2]={NodeDupl, NodePrev};
	Face Facelist=Edge_GetFace(model,EdgeInput);
	
	if(Facelist.cnt==1){
		Facereturn = 0;
	}
	else if(Facelist.cnt==2){
		Facereturn = Facelist.FaceInfo[(Facelist.FaceInfo[0]==FaceInput)];
	}
	else if(Facelist.cnt==3){
		int i;
		for(i=0;i<Facelist.cnt;i++){
			if(Cohesive2D_Face_IsCohesive(model, Facelist.FaceInfo[i])){
				Facereturn = Facelist.FaceInfo[i];
			}
		}
	}
	return Facereturn;
}


Face Cohesive2D_GatherFace(PolyModel *model, int NodeDupl, int Node2, int FaceCurr){
	/*
		
                 
                   n3-----------------------n4       
                  /  \                     /  \               
                 /    \                   /    \             
                /      \                 /      \           
               /        \      f2       /        \         
         e2 / /          \             /          \           
           V /          ^ \         ^ /            \          
  		      /				f3   \ \       / /			f1	    \          
      		 /			       e7 \    e6 /			           \                   
          /           		   \     /           		    \     /    
         /             			  \   /             			 \   /   
        /                      \ /         -->e1        \ /     
       n5-----------------------n2-----------C1----------n1    
        \        <--e8         / \         <--e2        / \     
         \                  ^ /   \                    /   \    
          \         f4     / /     \         f6       /     \            
           \             e4 /       \                /          
            \              /         \              /               
             \            C2          \            /                
              \          /	/		f5     \          /				          
               \        /	 V		        \        /			            
         		    \      /  e3        		 \      /                    
          			 \    /            			  \    /                     
                  \  /                     \  /                       
      -------------n6-----------------------n7               
                  /  \ 
                 /    \
                      
    
    Suppose we already have a cohesive element C2 in this small hexagon . And our aim is to insert another
    cohesive element C1 at the edge bounded by n1 and n2 . 
    
    Firstly, notice the criteria that n2 and n6 are already marked as "on the boundary", while n1 is not .
    So for inserting C1, we go to the case that the edge has one and only one node on the boundary . And 
    n2 is exactly the node to be duplicated . 
    
    Then, let n2 be NodeDupl and n1 be Node2 as the input of the function . The edge is defined as 
     { n2 , n1 } = e1.  
                                                                        	
    Select one of the faces containing e1, namely f1 in this case, as the start point. 
	*/             
	
	/*
		Start to iterate from f1 . 
		Use f1 and e1 to find e6 , check whether n4 is on the boundary . 
	*/

	Face FaceReturn;
	FaceReturn.cnt=0;
	memset(FaceReturn.FaceInfo,0,sizeof(int)*sizeof(FaceReturn.FaceInfo)/sizeof(FaceReturn.FaceInfo[0]));
	
	int Nodecurr = Node2;
	FaceReturn.FaceInfo[FaceReturn.cnt++]=FaceCurr;
	do{
		Nodecurr=Cohesive2D_FaceEdge_GetNode(model,FaceCurr,NodeDupl,Nodecurr);
//		printf("FaceCurr = %d, NodeDupl = %d, Nodecurr = %d\n",FaceCurr,NodeDupl,Nodecurr);
		FaceCurr=Cohesive2D_FaceEdge_GetFace(model,FaceCurr,NodeDupl,Nodecurr);
//		printf("FaceCurr = %d, NodeDupl = %d, Nodecurr = %d\n",FaceCurr,NodeDupl,Nodecurr);
		if(FaceCurr!=0){
			FaceReturn.FaceInfo[FaceReturn.cnt++]=FaceCurr;
		}
		else{
			break;
		}
	}while(!Cohesive2D_Face_IsCohesive(model,FaceCurr));
//	printf("GatherFace end\n");
	
	return FaceReturn;
}

void Cohesive2D_UpdateNE(PolyModel *model, int NodeUpdate, Face Facelist){
		NODE* NODECurr;
		
		NODECurr = Node_GetNODEAtId(model,NodeUpdate);
		NODECurr->AdjEnt_cnt=Facelist.cnt;
 		NODECurr->AdjEnt_id=(int*)realloc(NODECurr->AdjEnt_id,sizeof(int)*Facelist.cnt);
 		memset(NODECurr->AdjEnt_id,0,sizeof(int)*Facelist.cnt);
 		memcpy(NODECurr->AdjEnt_id,Facelist.FaceInfo,sizeof(int)*Facelist.cnt);
 		
 		return;
}

bool Cohesive2D_CreateFace(PolyModel *model, int Node1, int Node2, int NewFaceId){
	bool bo;
	if(Face_IsValid(model, NewFaceId)){
		printf("Error! Insertion of face failure : the face with Id %d already exists. \n", NewFaceId);
		bo = false;
	}
	else{
		FACE *newface = (FACE*)malloc(sizeof(FACE));
		
		newface->Global_id = NewFaceId;
		newface->isCohesive= true;
		newface->AdjEnt_id = (int*)malloc(sizeof(int)*4); // malloc for list of face id of the faceent
		newface->AdjEnt_id[0]=Node1;
		newface->AdjEnt_id[1]=Node2;
		newface->AdjEnt_id[2]=Node2;
		newface->AdjEnt_id[3]=Node1;
		newface->AdjEnt_cnt = 4; // Save the node size
		polyModel_InsertFace(model, newface); // Add new face to "FaceHash"
		
		/*
			Create element with the same id as face .
		*/
		ELEM *newelem = (ELEM*)malloc(sizeof(ELEM));
		
		newelem->Global_id = newface->Global_id;
		newelem->Element_Type = POLY_ELEMENT_COH2E2;
		newelem->AdjEnt_id = (int*)malloc(sizeof(int)); // malloc for list of face id of the element
		newelem->AdjEnt_id[0] = newface->Global_id;
		newelem->AdjEnt_cnt = 1; // Save the face size
		polyModel_InsertElem(model, newelem); // Add new element to "ElemHash"
		bo = true;
	}
	
	return bo;
}

bool Cohesive2D_CreateNode(PolyModel* model, double x, double y, double z, bool isBoundary, int id){
	bool bo;
	if(Node_IsValid(model,id)==true){
		printf("Error! Insertion of node failure : the node with id %d already exists. \n", id);
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

bool Cohesive2D_UpdateNode(PolyModel *model, int NodeUpdate, int Node2, int NodeOb, int CohElem, int ElemUpdate, int ElemLock, int NewNodeid, int ref){
	/*
		This function , Cohesive2D_UpdateNode , need to be called twice to update connectivity for both
		nodes of the input from Cohesive_Insert . 
		
		When the first node , say Node1 , is updated , there is one circumstance that Node1 is duplicated 
		into another node , say Node7 . 
		
		So this function returns the newly duplicated node , Node7 as a reference for the 2nd time when this
		function is called . Specifically , Node7 is input as NodeOb for the 2nd call of this function . 
		If the node is not duplicated , then it simply returns NodeUpdate . 
	*/
	int i,j,k;
	bool isbdd=Node_IsBoundary(model,NodeUpdate);
	if(isbdd==false){
		Node_MakeBoundary(model,NodeUpdate);
		Face FaceList=Node_GetFace(model,NodeUpdate);
		FaceList.FaceInfo[FaceList.cnt++]=CohElem;
		Cohesive2D_UpdateNE(model, NodeUpdate, FaceList);
	}
	else{
		int NodeDupl = NodeUpdate;
		Face FaceHome, FaceAway;
		if(Node2 == NodeOb){
			FaceHome = Cohesive2D_GatherFace(model,NodeDupl,Node2,ElemUpdate);
			FaceAway = Cohesive2D_GatherFace(model,NodeDupl,Node2,ElemLock);
		}
		else{
			int EdgeDup[2] = {NodeDupl, NodeOb};
			Elem Elemlist = Edge_GetElem(model, EdgeDup);
			if(Elemlist.ElemInfo[0] == ElemUpdate){
				FaceHome = Cohesive2D_GatherFace(model,NodeDupl,NodeOb,ElemUpdate);
				FaceAway = Cohesive2D_GatherFace(model,NodeDupl,Node2,ElemLock);
			}
			else{
				FaceHome = Cohesive2D_GatherFace(model,NodeDupl,Node2,ElemUpdate);
				FaceAway = Cohesive2D_GatherFace(model,NodeDupl,NodeOb,ElemLock);
			}
		}
		
		FaceHome.FaceInfo[FaceHome.cnt++]=CohElem;
		FaceAway.FaceInfo[FaceAway.cnt++]=CohElem;

		
		bool switchflag=true;
		for(k=0;k<FaceHome.cnt;k++){
			if(FaceHome.FaceInfo[k]==ref){
				switchflag=false;
			}
		}
		
		if(switchflag==true){
			Face FaceTemp;
			FaceTemp.cnt=FaceHome.cnt;
			for(k=0;k<FaceHome.cnt;k++){
				FaceTemp.FaceInfo[k]=FaceHome.FaceInfo[k];
			}
			
			FaceHome.cnt=FaceAway.cnt;
			for(k=0;k<FaceAway.cnt;k++){
				FaceHome.FaceInfo[k]=FaceAway.FaceInfo[k];
			}
			
			FaceAway.cnt=FaceTemp.cnt;
			for(k=0;k<FaceTemp.cnt;k++){
				FaceAway.FaceInfo[k]=FaceTemp.FaceInfo[k];
			}
		}
		
//		printf("FaceAway = ");
//		for(k=0;k<FaceAway.cnt;k++){
//			printf("%d ",FaceAway.FaceInfo[k]);
//		}
//		printf("\n");
//		printf("FaceHome = ");
//		for(k=0;k<FaceHome.cnt;k++){
//			printf("%d ",FaceHome.FaceInfo[k]);
//		}
//		printf("\n");
		
		
		Cohesive2D_CreateNode(model, 0, 0, 0, true, NewNodeid);
		FACE* FACEcurr;
		for(i=0;i<FaceHome.cnt;i++){
			if(Cohesive2D_Face_IsCohesive(model,FaceHome.FaceInfo[i])==true){
				FACEcurr=Face_GetFACEAtId(model,FaceHome.FaceInfo[i]);
				for(j=0;j<3;j+=2){
					if(NodeDupl==FACEcurr->AdjEnt_id[j]||NodeDupl==FACEcurr->AdjEnt_id[j+1]){
						/*
							Use Node_cohesive_flag to determine whether the node is to be updated or not
							Use position_flag to store the position of the node
						*/
						int position_flag = j + !(NodeDupl==FACEcurr->AdjEnt_id[j]);
//						printf(" NodeDupl = %d , position_flag = %d \n",NodeDupl,position_flag);
						int Edgeref[2] = {FACEcurr->AdjEnt_id[j], FACEcurr->AdjEnt_id[j+1]};
//						printf(" Edgeref = ( %d , %d ) \n",Edgeref[0],Edgeref[1]);
						int anchorface = Cohesive2D_Edge_GetAnchorFace(model, Edgeref);
//						printf(" Anchor Face = %d \n",anchorface);
						bool Node_cohesive_flag = false;
						for(k=0;k<FaceHome.cnt;k++){
							if(anchorface==FaceHome.FaceInfo[k]){
								Node_cohesive_flag = true;
							}
						}
//						printf(" Node_cohesive_flag = %d \n",Node_cohesive_flag);
						if(Node_cohesive_flag){
							FACEcurr->AdjEnt_id[position_flag]=NewNodeid;
						}
					}
				}
			}
		}
		
		for(i=0;i<FaceHome.cnt;i++){
			if(Cohesive2D_Face_IsCohesive(model,FaceHome.FaceInfo[i])==false){
				FACEcurr=Face_GetFACEAtId(model,FaceHome.FaceInfo[i]);
				for(j=0;j<FACEcurr->AdjEnt_cnt;j++){
					if(FACEcurr->AdjEnt_id[j]==NodeDupl){
						FACEcurr->AdjEnt_id[j]=NewNodeid;
					}
				}
			}
		}
		
		Cohesive2D_UpdateNE(model, NodeDupl, FaceAway);
		Cohesive2D_UpdateNE(model, NewNodeid, FaceHome);
	}
	
	return 0;
}




//bool polyModel_InsertCohesiveAtEdge_Manual(PolyModel *model, PolyEdge PolyEdgeInput){
//	int i;
//	char string[20] = {0,};
//	bool bo_edgevalid = polyEdge_IsValid(model, PolyEdgeInput);
//	if(!bo_edgevalid){
//		printf("Error! Edge ( %d , %d ) is not a valid edge. Please input another one. \n", PolyEdgeInput.EdgeTail, PolyEdgeInput.EdgeHead);
//	}
//	else{
//		int nelem = polyEdge_GetNElems(model, PolyEdgeInput);
//		if(nelem == 1){
//			printf("Error! Edge ( %d , %d ) is a boundary edge. Please input another one. \n", PolyEdgeInput.EdgeTail, PolyEdgeInput.EdgeHead);
//		}
//		else if(nelem > 2){
//			/*
//				If there are more than 2 adjacent elements of the input edge , either there is a cohesive element among them
//				or an error exists . 
//			*/
//			for(i=0;i<nelem;i++){
//				PolyElem polyelemcurr = polyEdge_GetElem(model, PolyEdgeInput, i);
//				int elemcurr_id = polyElem_GetId(model, polyelemcurr);
//				if(!Elem_IsBulk(model, elemcurr_id)){
//					printf("Error! Edge ( %d , %d ) is already inserted. ( Check Elem %d ). Please input another one. \n", PolyEdgeInput.EdgeTail, PolyEdgeInput.EdgeHead,elemcurr_id);
//				}
//			}
//		}
//		else if(nelem == 2){
//			int newfaceid;
//			printf("Edge ( %d , %d ) is valid for insertion . Current ELEMIDMAX is %d . \n", PolyEdgeInput.EdgeTail, PolyEdgeInput.EdgeHead, polyModel_GetELEMIDMAX(model));
//			printf("Please input a preferred Id for new cohesive element ( Enter any letter to select automatically ) : \n");
//			scanf("%s", &string);
//			if( (string[0] >= 'a' && string[0] <= 'z') || (string[0] >= 'A' && string[0] <= 'Z') ){
//				newfaceid = polyModel_GetFACEIDMAX(model)+1;
//			}
//			else{
//				newfaceid = atoi(string);
//			}
//			bool bo_insertface = Cohesive2D_CreateFace(model, PolyEdgeInput.EdgeTail, PolyEdgeInput.EdgeHead, newfaceid);
//			if(bo_insertface){
//				printf("\n ********** New cohesive element inserted . Elem Id is %d . ********** \n\n", newfaceid);
//				PolyElem polyelem[2] = {polyEdge_GetElem(model, PolyEdgeInput, 0), polyEdge_GetElem(model, PolyEdgeInput, 1)};
//				int elem_id[2] = {polyElem_GetId(model, polyelem[0]),polyElem_GetId(model, polyelem[1])};
//				
//				PolyNode polynode[2] = {polyEdge_GetNode(model, PolyEdgeInput, 0), polyEdge_GetNode(model, PolyEdgeInput, 1)};
//				int node_id[2] = {polyNode_GetId(model, polynode[0]),polyNode_GetId(model, polynode[1])};
//
//				int elem_update[2]={0,};
//				int elem_lock[2]={0,};
//				int newnodeid[2]={0,};
//				bool bo_dupl_node[2]={false,};
//				int nodecurr, noderef, nodeob;
//				
//				for(i=0;i<2;i++){
//					if(i==0){
//						nodecurr=node_id[0];
//						noderef=node_id[1];
//						nodeob=node_id[1];
//					}
//					else{
//						nodecurr=node_id[1];
//						noderef=node_id[0];
//						nodeob=node_id[0];
//						if(bo_dupl_node[0]){
//							nodeob=newnodeid[0];
//						} 
//					}
//					
//					bo_dupl_node[i] = polyNode_IsBoundary(model, polynode[i]);
//					if(!bo_dupl_node[i]){
//						Cohesive2D_UpdateNode(model, nodecurr, noderef, nodeob, newfaceid, 0, 0, 0);
//					}
//					else{
//						bool elemId_available_flag = false;
//						do{
//							printf("Node %d need to be duplicated. The reference elements are Elem %d and Elem %d . \n", nodecurr, elem_id[0], elem_id[1]);
//							printf("Please select an element to insert a new node ( Enter any letter to select automatically ) : \n");
//							scanf("%s", &string);
//							if( (string[0] >= 'a' && string[0] <= 'z') || (string[0] >= 'A' && string[0] <= 'Z') ){
//								elem_update[i] = elem_id[0];
//							}
//							else{
//								elem_update[i] = atoi(string);
//							}
//							elemId_available_flag = (elem_id[0] == elem_update[i])||(elem_id[1] == elem_update[i]);
//							if( !elemId_available_flag ){
//								printf("Error! Please input an available element Id. \n");
//							}
//						}while( !elemId_available_flag );
//						elem_lock[i] = elem_id[ (elem_id[0] == elem_update[i]) ];
//						
//						bool nodeId_available_flag = false;
//						do{
//							printf("Current NODEIDMAX is %d . \n", polyModel_GetNODEIDMAX(model));
//							printf("Please input a preferred Id for new node: ( Enter any letter to select automatically ) : \n");
//							scanf("%s", &string);
//							if( (string[0] >= 'a' && string[0] <= 'z') || (string[0] >= 'A' && string[0] <= 'Z') ){
//								newnodeid[i] = polyModel_GetNODEIDMAX(model)+1;
//							}
//							else{
//								newnodeid[i] = atoi(string);
//							}
//							nodeId_available_flag = !Node_IsValid(model, newnodeid[i]);
//							if( !elemId_available_flag ){
//								printf("Error! Please input an available node Id. \n");
//							}
//						}while( !nodeId_available_flag );
//					
//						Cohesive2D_UpdateNode(model, nodecurr, noderef, nodeob, newfaceid, elem_update[i], elem_lock[i], newnodeid[i]);
//						printf("\n ********** Node %d Duplicated . New Node %d in Elem %d . ********** \n\n", nodecurr, newnodeid[i], elem_update[i]);
//					}	
//				}
//			}
//		}
//	}
//}



//bool polyModel_InsertCohesiveAtEdge_Auto(PolyModel *model, PolyEdge PolyEdgeInput){
//	int i;
//	bool bo_edgevalid = polyEdge_IsValid(model, PolyEdgeInput);
//	if(!bo_edgevalid){
//		printf("Error! Edge ( %d , %d ) is not a valid edge. Please input another one. \n", PolyEdgeInput.EdgeTail, PolyEdgeInput.EdgeHead);
//	}
//	else{
//		int nelem = polyEdge_GetNElems(model, PolyEdgeInput);
//		if(nelem == 1){
//			printf("Error! Edge ( %d , %d ) is a boundary edge. Please input another one. \n", PolyEdgeInput.EdgeTail, PolyEdgeInput.EdgeHead);
//		}
//		else if(nelem > 2){
//			/*
//				If there are more than 2 adjacent elements of the input edge , either there is a cohesive element among them
//				or an error exists . 
//			*/
//			for(i=0;i<nelem;i++){
//				PolyElem polyelemcurr = polyEdge_GetElem(model, PolyEdgeInput, i);
//				int elemcurr_id = polyElem_GetId(model, polyelemcurr);
//				if(!Elem_IsBulk(model, elemcurr_id)){
//					printf("Error! Edge ( %d , %d ) is already inserted. ( Check Elem %d ). Please input another one. \n", PolyEdgeInput.EdgeTail, PolyEdgeInput.EdgeHead,elemcurr_id);
//				}
//			}
//		}
//		else if(nelem == 2){
//			int newfaceid = polyModel_GetELEMIDMAX(model) + 1 ;
//			printf(" ********** Edge ( %d , %d ) is valid for insertion . ", PolyEdgeInput.EdgeTail, PolyEdgeInput.EdgeHead);
//			bool bo_insertface = Cohesive2D_CreateFace(model, PolyEdgeInput.EdgeTail, PolyEdgeInput.EdgeHead, newfaceid);
//			if(bo_insertface){
//				printf(" New cohesive element inserted . Elem Id is %d . ********** \n", newfaceid);
//				PolyElem polyelem[2] = {polyEdge_GetElem(model, PolyEdgeInput, 0), polyEdge_GetElem(model, PolyEdgeInput, 1)};
//				int elem_id[2] = {polyElem_GetId(model, polyelem[0]),polyElem_GetId(model, polyelem[1])};
//				
//				PolyNode polynode[2] = {polyEdge_GetNode(model, PolyEdgeInput, 0), polyEdge_GetNode(model, PolyEdgeInput, 1)};
//				int node_id[2] = {polyNode_GetId(model, polynode[0]),polyNode_GetId(model, polynode[1])};
//
//				int elem_update[2]={0,};
//				int elem_lock[2]={0,};
//				int newnodeid[2]={0,};
//				bool bo_dupl_node[2]={false,};
//				int nodecurr, noderef, nodeob;
//				
//				for(i=0;i<2;i++){
//					if(i==0){
//						nodecurr=node_id[0];
//						noderef=node_id[1];
//						nodeob=node_id[1];
//					}
//					else{
//						nodecurr=node_id[1];
//						noderef=node_id[0];
//						nodeob=node_id[0];
//						if(bo_dupl_node[0]){
//							nodeob=newnodeid[0];
//						} 
//					}
//					
//					bo_dupl_node[i] = polyNode_IsBoundary(model, polynode[i]);
//					if(!bo_dupl_node[i]){
//						Cohesive2D_UpdateNode(model, nodecurr, noderef, nodeob, newfaceid, 0, 0, 0);
//					}
//					else{
//						elem_update[i] = elem_id[0];
//						elem_lock[i] = elem_id[1];
//						newnodeid[i] = polyModel_GetNODEIDMAX(model)+1;
//						
//						printf("nodecurr = %d, noderef = %d, nodeob = %d\n", nodecurr, noderef, nodeob);
//						Cohesive2D_UpdateNode(model, nodecurr, noderef, nodeob, newfaceid, elem_update[i], elem_lock[i], newnodeid[i]);
//						printf(" ********** Node %d Duplicated . New Node %d in Elem %d . ********** \n\n", nodecurr, newnodeid[i], elem_update[i]);
//					}	
//				}
//			}
//		}
//	}
//}


bool polyModel_InsertCohesiveAtEdge_Test(PolyModel *model, int* reflist){
	PolyEdge PolyEdgeInput;
	if(reflist[4]<reflist[6]){
		PolyEdgeInput.EdgeTail=reflist[4];
	}
	else{
		PolyEdgeInput.EdgeTail=reflist[6];
	}
	if(reflist[5]<reflist[7]){
		PolyEdgeInput.EdgeHead=reflist[5];
	}
	else{
		PolyEdgeInput.EdgeHead=reflist[7];
	}
	int i;
	bool bo_edgevalid = polyEdge_IsValid(model, PolyEdgeInput);
	if(!bo_edgevalid){
//		printf("Error! Edge ( %d , %d ) is not a valid edge. Please input another one. \n", PolyEdgeInput.EdgeTail, PolyEdgeInput.EdgeHead);
	}
	else{
		int nelem = polyEdge_GetNElems(model, PolyEdgeInput);
		if(nelem == 1){
//			printf("Error! Edge ( %d , %d ) is a boundary edge. Please input another one. \n", PolyEdgeInput.EdgeTail, PolyEdgeInput.EdgeHead);
		}
		else if(nelem > 2){
			/*
				If there are more than 2 adjacent elements of the input edge , either there is a cohesive element among them
				or an error exists . 
			*/
			for(i=0;i<nelem;i++){
				PolyElem polyelemcurr = polyEdge_GetElem(model, PolyEdgeInput, i);
				int elemcurr_id = polyElem_GetId(model, polyelemcurr);
				if(!Elem_IsBulk(model, elemcurr_id)){
//					printf("Error! Edge ( %d , %d ) is already inserted. ( Check Elem %d ). Please input another one. \n", PolyEdgeInput.EdgeTail, PolyEdgeInput.EdgeHead,elemcurr_id);
				}
			}
		}
		else if(nelem == 2){
			int newfaceid = polyModel_GetELEMIDMAX(model) + 1 ;
//			printf(" ********** Edge ( %d , %d ) is valid for insertion . ", PolyEdgeInput.EdgeTail, PolyEdgeInput.EdgeHead);
			bool bo_insertface = Cohesive2D_CreateFace(model, PolyEdgeInput.EdgeTail, PolyEdgeInput.EdgeHead, newfaceid);
			if(bo_insertface){
//				printf(" New cohesive element inserted . Elem Id is %d . ********** \n", newfaceid);
				PolyElem polyelem[2] = {polyEdge_GetElem(model, PolyEdgeInput, 0), polyEdge_GetElem(model, PolyEdgeInput, 1)};
				int elem_id[2] = {polyElem_GetId(model, polyelem[0]),polyElem_GetId(model, polyelem[1])};
				
				PolyNode polynode[2] = {polyEdge_GetNode(model, PolyEdgeInput, 0), polyEdge_GetNode(model, PolyEdgeInput, 1)};
				int node_id[2] = {polyNode_GetId(model, polynode[0]),polyNode_GetId(model, polynode[1])};

				int elem_update[2]={0,};
				int elem_lock[2]={0,};
				int newnodeid[2]={0,};
				if(reflist[4]<reflist[6]){
					newnodeid[0]=reflist[6];
				}
				else{
					newnodeid[0]=reflist[4];
				}
				if(reflist[5]<reflist[7]){
					newnodeid[1]=reflist[7];
				}
				else{
					newnodeid[1]=reflist[5];
				}
				bool bo_dupl_node[2]={false,};
				int nodecurr, noderef, nodeob;
				
				for(i=0;i<2;i++){
					if(i==0){
						nodecurr=node_id[0];
						noderef=node_id[1];
						nodeob=node_id[1];
					}
					else{
						nodecurr=node_id[1];
						noderef=node_id[0];
						nodeob=node_id[0];
						if(bo_dupl_node[0]){
							nodeob=newnodeid[0];
						} 
					}
					
					bo_dupl_node[i] = polyNode_IsBoundary(model, polynode[i]);
					if(!bo_dupl_node[i]){
						Cohesive2D_UpdateNode(model, nodecurr, noderef, nodeob, newfaceid, 0, 0, 0, 0);
					}
					else{
						
						
						
						elem_update[i] = elem_id[0];
						elem_lock[i] = elem_id[1];
//						printf("nodecurr = %d, noderef = %d, nodeob = %d\n", nodecurr, noderef, nodeob);
						Cohesive2D_UpdateNode(model, nodecurr, noderef, nodeob, newfaceid, elem_update[i], elem_lock[i], newnodeid[i], reflist[i+2]);
//						printf(" ********** Node %d Duplicated . New Node %d in Elem %d . ********** \n\n", nodecurr, newnodeid[i], reflist[i+2]);
					}	
				}
			}
		}
	}
}


RandEdgeList Cohesive_RandomEdge(PolyModel *model, int Percentage){
	
	RandEdgeList Randlist;
	int randnum;
	PolyEdge Edge;
	
	int threshold=model->NumberOfEdges*Percentage/100;
	Randlist.RandEdgeListPtr=(int*)malloc(sizeof(int)*(threshold*2+1));
	memset(Randlist.RandEdgeListPtr,0,sizeof(int)*(threshold*2+1));
	Randlist.RandEdgeListCnt=0;
	
	PolyEdgeItr* editr = polyModel_CreateEdgeItr(model);

	for (polyEdgeItr_Begin(editr); polyEdgeItr_IsValid(editr); polyEdgeItr_Next(editr)){	
		randnum = rand() % 100 + 1;
		if(randnum<=Percentage&&Randlist.RandEdgeListCnt<threshold){
			Edge = polyEdgeItr_GetCurr (editr);
			Randlist.RandEdgeListPtr[2*Randlist.RandEdgeListCnt]=Edge.EdgeTail;
			Randlist.RandEdgeListPtr[2*Randlist.RandEdgeListCnt+1]=Edge.EdgeHead;
			Randlist.RandEdgeListCnt++;
			
			printf("PolyEdge: %d - %d, ",Edge.EdgeTail,Edge.EdgeHead);
			printf("\n");
		}
	}
	polyEdgeItr_Destroy(editr);
	
	return Randlist;
}


int Cohesive2D_Edge_GetAnchorFace(PolyModel *model, int* EdgeInput){
  Face Facelist=Edge_GetFace(model,EdgeInput);
  int i,j;
  for(i=0;i<Facelist.cnt;i++){
  	if(!Cohesive2D_Face_IsCohesive(model,Facelist.FaceInfo[i])){
  		Edge Edgelist=Face_GetEdge(model,Facelist.FaceInfo[i]);
	    for(j=0;j<Edgelist.cnt;j++){
	      if(Edgelist.EdgeInfo[j][0]==EdgeInput[0]&&Edgelist.EdgeInfo[j][1]==EdgeInput[1]){
	        return Facelist.FaceInfo[i];
	      }
	    }
  	}
  }
}

RandEdgeList Cohesive_AllEdge(PolyModel *model){
	RandEdgeList Randlist;
	PolyEdge Edge;
	
	int i;
	int cnt=0;
	PolyEdgeItr* editr = polyModel_CreateEdgeItr(model);
	for (polyEdgeItr_Begin(editr); polyEdgeItr_IsValid(editr); polyEdgeItr_Next(editr)){	
		cnt++;
	}
	
	Randlist.RandEdgeListPtr=(int*)malloc(sizeof(int)*(cnt*2+1));
	memset(Randlist.RandEdgeListPtr,0,sizeof(int)*(cnt*2+1));
	Randlist.RandEdgeListCnt=0;

	for (polyEdgeItr_Begin(editr); polyEdgeItr_IsValid(editr); polyEdgeItr_Next(editr)){	
		Edge = polyEdgeItr_GetCurr (editr);
		Randlist.RandEdgeListPtr[2*Randlist.RandEdgeListCnt]=Edge.EdgeTail;
		Randlist.RandEdgeListPtr[2*Randlist.RandEdgeListCnt+1]=Edge.EdgeHead;
		Randlist.RandEdgeListCnt++;
	}
	polyEdgeItr_Destroy(editr);
	
	return Randlist;
}



bool Cohesive2D_CheckConsistencyOnNode(PolyModel *model, int NodeInput){
	Elem ElemList1 = Node_GetElem(model, NodeInput);
	
	Elem ElemList2;
  ElemList2.cnt=0;
  memset(ElemList2.ElemInfo,0,sizeof(int)*sizeof(ElemList2.ElemInfo)/sizeof(ElemList2.ElemInfo[0]));
  
	Edge EdgeList = Node_GetEdge(model, NodeInput);
	int i,j,k;
	int flag=0;
	for(i=0;i<EdgeList.cnt;i++){
		int Edge[2]={EdgeList.EdgeInfo[i][0],EdgeList.EdgeInfo[i][1]};
		Elem ElemListPrime = Edge_GetElem(model, Edge);
		for(j=0;j<ElemListPrime.cnt;j++){
			for(k=0;k<ElemList2.cnt;k++){
				if(ElemListPrime.ElemInfo[j]==ElemList2.ElemInfo[k]){
					flag=1;
				}
			}
			if(flag==0){
				ElemList2.ElemInfo[ElemList2.cnt++]=ElemListPrime.ElemInfo[j];
			}
		}
	}
	
	bool checkcons=false;
	if(ElemList1.cnt==ElemList2.cnt){
		checkcons=true;
	}
	
	return checkcons;
}

