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



bool Elem_IsValid(PolyModel* model, int ElemInput)
{
	bool isValid=false;
	if(ElemInput){
		int key = ElemInput % model->ELEMHASHSIZE; // key value = the remainder of the ID divided by the hash size
		ELEM* elem = NULL;
		
		if(key<0){
			key += model->ELEMHASHSIZE;
		}
		for(elem=model->ElemHash[key];elem;elem = elem->next){
			if(elem->Global_id==ElemInput){
				isValid=true;
			}
		}
	}
	else{
		printf("0 is not allowed for an Elem Id . \n");
	}
	
	return isValid;
}

bool Face_IsValid(PolyModel* model, int FaceInput)
{
	bool isValid=false;
	if(FaceInput){
		int key = FaceInput % model->FACEHASHSIZE; // key value = the remainder of the ID divided by the hash size
		FACE* face = NULL;
		
		if(key<0){
			key += model->FACEHASHSIZE;
		}
		for(face=model->FaceHash[key];face;face = face->next){
			if(face->Global_id==FaceInput){
				isValid=true;
			}
		}
	}
	else{
		printf("0 is not allowed for a Face Id . \n");
	}
	
	return isValid;
}

bool Node_IsValid(PolyModel* model, int NodeInput)
{
	bool isValid=false;
	if(NodeInput){
		int key = NodeInput % model->NODEHASHSIZE; // key value = the remainder of the ID divided by the hash size
		NODE* node = NULL;
		
		if(key<0){
			key += model->NODEHASHSIZE;
		}
		for(node=model->NodeHash[key];node;node = node->next){
			if(node->Global_id==NodeInput){
				isValid=true;
			}
		}
	}
	else{
		printf("0 is not allowed for a Node Id . \n");
	}
	
	return isValid;
}

bool Edge_IsValid(PolyModel* model, int* EdgeInput)
{
	int edgetail=EdgeInput[0];
  int edgehead=EdgeInput[1];
  int i,j,k;
  bool isValid=false;
  
  Edge Edgelist = Node_GetEdge(model, edgetail);
	
	for(i=0;i<Edgelist.cnt;i++){
    if((Edgelist.EdgeInfo[i][0]==edgetail&&Edgelist.EdgeInfo[i][1]==edgehead)||(Edgelist.EdgeInfo[i][1]==edgetail&&Edgelist.EdgeInfo[i][0]==edgehead)){
			isValid=true;
    }
  }

	return isValid;
}

bool Elem_IsBulk(PolyModel *model, int ElemInput){
	ELEM* ELEMInfo=Elem_GetELEMAtId(model, ElemInput);
	bool IsBulk;
	
	if(ELEMInfo->Element_Type==POLY_ELEMENT_POLY2D||ELEMInfo->Element_Type==POLY_ELEMENT_POLY3D){
		IsBulk=true;
	}
	else{
		IsBulk=false;
	}
	return IsBulk;
}


bool Elem_IsCohs(PolyModel *model, int ElemInput){
	ELEM* ELEMInfo=Elem_GetELEMAtId(model, ElemInput);
	bool IsCohs;
	
	if(ELEMInfo->Element_Type==POLY_ELEMENT_COH3POLY){
		IsCohs=true;
	}
	else{
		IsCohs=false;
	}
	return IsCohs;
}




NODE* Node_GetNODEAtId(PolyModel* model, int nodeid)
{
	NODE *node = model->NodeHash[nodeid % model->NODEHASHSIZE];

	while (node && node->Global_id != nodeid)
	{
		node = node->next;
	}
	return node;
}

ELEM* Elem_GetELEMAtId(PolyModel* model, int elemid)
{
	ELEM *elem = model->ElemHash[elemid % model->ELEMHASHSIZE];

	while (elem && elem->Global_id != elemid)
	{
		elem = elem->next;
	}
	return elem;
}

FACE* Face_GetFACEAtId(PolyModel* model, int faceid)
{
	FACE *face = model->FaceHash[faceid % model->FACEHASHSIZE];

	while (face && face->Global_id != faceid)
	{
		face = face->next;
	}
	return face;
}

int Elem_GetIdAtELEM(PolyModel* model, ELEM* elem)
{
	int t=elem->Global_id;
	return t;
}

int Face_GetIdAtFACE(PolyModel* model, FACE* face)
{
	int t=face->Global_id;
	return t;
}

int Node_GetIdAtNODE(PolyModel* model, NODE* node)
{
	int t=node->Global_id;
	return t;
}



bool Node_IsBoundary(PolyModel *model, int NodeInput){
	NODE* NODEInfo=Node_GetNODEAtId(model, NodeInput);
	bool Isbound=NODEInfo->isBoundary;
	
	return Isbound;
}

int Node_MakeBoundary(PolyModel *model, int NodeInput){
	int isvalid;
	NODE* NODEInfo=Node_GetNODEAtId(model, NodeInput);
	
	if(NODEInfo){
		NODEInfo->isBoundary=true;
		isvalid=1;
	}
	else{
		isvalid=0;
		printf("Error! Node _ Make boundary failed. \n");
	}
	
	return isvalid;
}




Face Elem_GetFace(PolyModel* model,int ElemInput){
  Face FaceReturn;
  FaceReturn.cnt=0;
  memset(FaceReturn.FaceInfo,0,sizeof(int)*sizeof(FaceReturn.FaceInfo)/sizeof(FaceReturn.FaceInfo[0]));
  
	ELEM* ElemCurr=Elem_GetELEMAtId(model,ElemInput);
  if (ElemCurr==NULL) FaceReturn.cnt=-1;
  else{
  	FaceReturn.cnt=ElemCurr->AdjEnt_cnt;
		memcpy(FaceReturn.FaceInfo,ElemCurr->AdjEnt_id,sizeof(int)*FaceReturn.cnt);
  }
	return FaceReturn;
}

Node Face_GetNode(PolyModel* model,int FaceInput){
  Node NodeReturn;
  NodeReturn.cnt=0;
  memset(NodeReturn.NodeInfo,0,sizeof(int)*sizeof(NodeReturn.NodeInfo)/sizeof(NodeReturn.NodeInfo[0]));
  
  FACE* FaceCurr=Face_GetFACEAtId(model,FaceInput);
  if (FaceCurr==NULL) NodeReturn.cnt=-1;
  else{
		NodeReturn.cnt=FaceCurr->AdjEnt_cnt;
		memcpy(NodeReturn.NodeInfo,FaceCurr->AdjEnt_id,sizeof(int)*NodeReturn.cnt);
  }
	return NodeReturn;
}

Elem Node_GetElem(PolyModel* model,int NodeInput){
  Elem ElemReturn;
  ElemReturn.cnt=0;
  memset(ElemReturn.ElemInfo,0,sizeof(int)*sizeof(ElemReturn.ElemInfo)/sizeof(ElemReturn.ElemInfo[0]));
  
  NODE* NodeCurr=Node_GetNODEAtId(model,NodeInput);
  if (NodeCurr==NULL) ElemReturn.cnt=-1;
  else{
  	int i;
  	for(i=0;i<NodeCurr->AdjEnt_cnt;i++){
			ElemReturn.ElemInfo[i]=NodeCurr->AdjEnt_id[i];
			ElemReturn.cnt++;
  	}
  }
	return ElemReturn;
}

Edge Face_GetEdge(PolyModel* model,int FaceInput){
  Edge EdgeReturn;
  EdgeReturn.cnt=0;
	memset(EdgeReturn.EdgeInfo,0,sizeof(int)*sizeof(EdgeReturn.EdgeInfo)/sizeof(EdgeReturn.EdgeInfo[0]));

	Node Nodelist=Face_GetNode(model,FaceInput);
  if (Nodelist.cnt==-1) EdgeReturn.cnt=-1;
  else{
    while(EdgeReturn.cnt<Nodelist.cnt){
      EdgeReturn.EdgeInfo[EdgeReturn.cnt][0]=Nodelist.NodeInfo[EdgeReturn.cnt];
      EdgeReturn.EdgeInfo[EdgeReturn.cnt][1]=Nodelist.NodeInfo[EdgeReturn.cnt+1];
      EdgeReturn.cnt++;
    }
    EdgeReturn.EdgeInfo[EdgeReturn.cnt-1][1]=Nodelist.NodeInfo[0];
  }
  return EdgeReturn;
}



Node Elem_GetNode(PolyModel* model,int ElemInput){
  Node NodeReturn;
	NodeReturn.cnt=0;
	memset(NodeReturn.NodeInfo,0,sizeof(int)*sizeof(NodeReturn.NodeInfo)/sizeof(NodeReturn.NodeInfo[0]));
	
	int i,j,k;
	int NodeCmpr;
	Face Facelist=Elem_GetFace(model,ElemInput);
	for(i=0;i<Facelist.cnt;i++){
    Node Nodelist=Face_GetNode(model,Facelist.FaceInfo[i]);
    for(j=0;j<Nodelist.cnt;j++){
    	NodeCmpr=0;
      for(k=0;k<NodeReturn.cnt;k++){
        if(Nodelist.NodeInfo[j]==NodeReturn.NodeInfo[k]){
          NodeCmpr=1;
          break;
        }
      }
      if(NodeCmpr==0){
        NodeReturn.NodeInfo[NodeReturn.cnt++]=Nodelist.NodeInfo[j];
      }
    }
  }
	return NodeReturn;
}

Edge Elem_GetEdge(PolyModel* model,int ElemInput){
  Edge EdgeReturn;
  EdgeReturn.cnt=0;
  memset(EdgeReturn.EdgeInfo,0,sizeof(int)*sizeof(EdgeReturn.EdgeInfo)/sizeof(EdgeReturn.EdgeInfo[0]));
	
	int i,j,k;
	int first,secon;
  int small,large;
	int flag;
	Face Facelist=Elem_GetFace(model,ElemInput);
	for(i=0;i<Facelist.cnt;i++){
    Edge Edgelist=Face_GetEdge(model,Facelist.FaceInfo[i]);
    for(j=0;j<Edgelist.cnt;j++){
    	first=Edgelist.EdgeInfo[j][0];
    	secon=Edgelist.EdgeInfo[j][1];
      if(first>secon){
      	small=secon;
      	large=first;
      }
      else{
      	small=first;
      	large=secon;
      }
      flag=0;
    	for(k=0;k<EdgeReturn.cnt;k++){
    		if(small==EdgeReturn.EdgeInfo[k][0]&&large==EdgeReturn.EdgeInfo[k][1]){
    			flag=1;
    			break;
    		}
    	}
    	if(flag==0){
    		EdgeReturn.EdgeInfo[EdgeReturn.cnt][0]=small;
      	EdgeReturn.EdgeInfo[EdgeReturn.cnt][1]=large;
      	EdgeReturn.cnt++;
    	}
		}
  }
  /**
		Each stored edge has a pair of nodes with the second node number 
		larger than the first node number
	*/
	return EdgeReturn;
}

Elem Face_GetElem(PolyModel* model,int FaceInput){
	Elem ElemReturn;
  ElemReturn.cnt=0;
  memset(ElemReturn.ElemInfo,0,sizeof(int)*sizeof(ElemReturn.ElemInfo)/sizeof(ElemReturn.ElemInfo[0]));

	if(model->dimen==2){
		FACE* FaceCurr=Face_GetFACEAtId(model,FaceInput);
		ElemReturn.cnt=1;
  	ElemReturn.ElemInfo[0]=FaceCurr->Global_id;
	}
	else if(model->dimen==3){
		Node Nodelist=Face_GetNode(model,FaceInput);
	//  printf("Node: ");
	//  for(int kk=0;kk<Nodelist.cnt;kk++){
	//  	printf("%d ",Nodelist.NodeInfo[kk]);
	//	}
	//	printf("\n");
		
	  int i,j,k,l;
	  /**
			For F[E], a necessary condition for an element to be the adjacent element to a given face
			is that it contains all nodes in that face. Hence we simply record the element list to the
			first node and delete those elements that do not appear in the adjacent element lists to 
			other nodes.
		*/
	  for(i=0;i<Nodelist.cnt;i++){
	    Elem Elemlist=Node_GetElem(model,Nodelist.NodeInfo[i]);
	    
	//    printf("Elem: ");
	//    for(int kk=0;kk<Elemlist.cnt;kk++){
	//  		printf("%d ",Elemlist.ElemInfo[kk]);
	//		}
	//		printf("\n");
			
			if(i==0){
				/**
					Here we stored all adjacent elements to the first node in the output list
				*/
				memcpy(ElemReturn.ElemInfo,Elemlist.ElemInfo,sizeof(int)*Elemlist.cnt);
				ElemReturn.cnt=Elemlist.cnt;
				
	//			printf("ElemReturn: ");
	//    	for(int kk=0;kk<ElemReturn.cnt;kk++){
	//  			printf("%d ",ElemReturn.ElemInfo[kk]);
	//			}
	//			printf("cnt=%d ",ElemReturn.cnt);
	//			printf("\n");
			}
			else{
				/**
					There is at least one element that owns the input face. So if the count of the return 
					list equals 1, then the loop can stop. 
				*/
				if(ElemReturn.cnt==1){
					break;
				}
				/**
					And then we compare these elements with adjacent elements to other nodes and Delete
					those elements that do not repeat in adjacency to other nodes. 
				*/
	      for(j=0;j<ElemReturn.cnt;j++){
	      	for(k=0;k<Elemlist.cnt;k++){
		        if(ElemReturn.ElemInfo[j]==Elemlist.ElemInfo[k]){
		          break;
		        }
		      }
		      if(k==Elemlist.cnt){
		      	/**
		      		ElemReturn.ElemInfo[j] is the element that is not adjacent to
		      	 	Nodelist.NodeInfo[i] , hence need to be deleted from ElemReturn . 
		      	 	We simply replace it with the last element in ElemReturn.ElemInfo[] . 
		      	*/
		        ElemReturn.ElemInfo[j]=ElemReturn.ElemInfo[ElemReturn.cnt-1];
		        ElemReturn.cnt--;
		        j--;
		        
	//	        printf("ElemReturn: ");
	//    			for(int kk=0;kk<ElemReturn.cnt;kk++){
	//  					printf("%d ",ElemReturn.ElemInfo[kk]);
	//					}
	//					printf("\n");
		      }
		    }
	    }
	  }
	}
  return ElemReturn;
}

Face Node_GetFace(PolyModel* model,int NodeInput){
	Face FaceReturn;
  FaceReturn.cnt=0;
  memset(FaceReturn.FaceInfo,0,sizeof(int)*sizeof(FaceReturn.FaceInfo)/sizeof(FaceReturn.FaceInfo[0]));
  
  int i,j,k,l;
  Elem Elemlist=Node_GetElem(model,NodeInput);
  for(i=0;i<Elemlist.cnt;i++){
    Face Facelist=Elem_GetFace(model,Elemlist.ElemInfo[i]);
    for(j=0;j<Facelist.cnt;j++){
      Node Nodelist=Face_GetNode(model,Facelist.FaceInfo[j]);
     	for(k=0;k<Nodelist.cnt;k++){
   			/**
    			Skip those faces that do not contain NodeInput
    		*/
   			if(Nodelist.NodeInfo[k]==NodeInput){
   				break;
   			}
   		}
    	if(k==Nodelist.cnt){
    		continue;
    	}
      else{
      	/**
					Find those faces that contains NodeInput . 
					And skip repeated ones . 
				*/
				for(l=0;l<FaceReturn.cnt;l++){
					if(Facelist.FaceInfo[j]==FaceReturn.FaceInfo[l]){
						break;
					}
				}
				if(l==FaceReturn.cnt){
        	FaceReturn.FaceInfo[FaceReturn.cnt++]=Facelist.FaceInfo[j];
        }
      }
    }
  }
  return FaceReturn;
}

Edge Node_GetEdge(PolyModel* model,int NodeInput){
	Edge EdgeReturn;
  EdgeReturn.cnt=0;
  memset(EdgeReturn.EdgeInfo,0,sizeof(int)*sizeof(EdgeReturn.EdgeInfo)/sizeof(EdgeReturn.EdgeInfo[0][0]));
  
  int i,j,k,l;
  int prev,next;
  Elem Elemlist=Node_GetElem(model,NodeInput);
  for(i=0;i<Elemlist.cnt;i++){
    Face Facelist=Elem_GetFace(model,Elemlist.ElemInfo[i]);
    for(j=0;j<Facelist.cnt;j++){
      Node Nodelist=Face_GetNode(model,Facelist.FaceInfo[j]);
     	for(k=0;k<Nodelist.cnt;k++){
   			if(Nodelist.NodeInfo[k]==NodeInput){
   				break;
   			}
   		}
 			/**
  			Skip those faces that do not contain NodeInput
  		*/
 			if(k==Nodelist.cnt){
 				continue;
 			}
 			/**
  			Record the edges
  		*/
			else if(k==0){
        prev=Nodelist.NodeInfo[Nodelist.cnt-1];
        next=Nodelist.NodeInfo[k+1];
      }
      else if(k==Nodelist.cnt-1){
        prev=Nodelist.NodeInfo[k-1];
        next=Nodelist.NodeInfo[0];
      }
      else{
        prev=Nodelist.NodeInfo[k-1];
        next=Nodelist.NodeInfo[k+1];
      }
      
 			/**
  			Check if the edges are repeated and store if not . 
  		*/
      for(l=0;l<EdgeReturn.cnt;l++){
        if(prev==EdgeReturn.EdgeInfo[l][1]){
        	break;
        }
      }
      if(l==EdgeReturn.cnt){
      	EdgeReturn.EdgeInfo[EdgeReturn.cnt][0]=NodeInput;
      	EdgeReturn.EdgeInfo[EdgeReturn.cnt][1]=prev;
      	EdgeReturn.cnt++;
      }
      for(l=0;l<EdgeReturn.cnt;l++){
        if(next==EdgeReturn.EdgeInfo[l][1]){
        	break;
        }
      }
      if(l==EdgeReturn.cnt){
      	EdgeReturn.EdgeInfo[EdgeReturn.cnt][0]=NodeInput;
      	EdgeReturn.EdgeInfo[EdgeReturn.cnt][1]=next;
      	EdgeReturn.cnt++;
      }
    }
  }
  return EdgeReturn;
}

Node Node_GetNode(PolyModel* model,int NodeInput){
	Node NodeReturn;
	NodeReturn.cnt=0;
	memset(NodeReturn.NodeInfo,0,sizeof(int)*sizeof(NodeReturn.NodeInfo)/sizeof(NodeReturn.NodeInfo[0]));

	int i,j,k;
  Elem Elemlist=Node_GetElem(model,NodeInput);
  for(i=0;i<Elemlist.cnt;i++){
  	Node Nodelist=Elem_GetNode(model,Elemlist.ElemInfo[i]);
  	for(j=0;j<Nodelist.cnt;j++){
  		if(Nodelist.NodeInfo[j]==NodeInput){
  			continue;
  		}
  		else{
  			for(k=0;k<NodeReturn.cnt;k++){
  				if(Nodelist.NodeInfo[j]==NodeReturn.NodeInfo[k]){
  					break;
  				}
  			}
  			if(k==NodeReturn.cnt){
  				NodeReturn.NodeInfo[NodeReturn.cnt++]=Nodelist.NodeInfo[j];
  			}
  		}
  	}
  }
	return NodeReturn;
}



Elem Edge_GetElem(PolyModel* model,int *EdgeInput){
  Elem ElemReturn;
  ElemReturn.cnt=0;
  memset(ElemReturn.ElemInfo,0,sizeof(int)*sizeof(ElemReturn.ElemInfo)/sizeof(ElemReturn.ElemInfo[0]));
  
  bool bo = Edge_IsValid(model, EdgeInput);
  if(bo==true){
	  Node Nodelist=Edge_GetNode(model,EdgeInput);
	  
	  int edgetail=Nodelist.NodeInfo[0];
	  int edgehead=Nodelist.NodeInfo[1];
		Elem Elemlisttail=Node_GetElem(model,edgetail);
		Elem Elemlisthead=Node_GetElem(model,edgehead);

	  int i,j;
	  if (Elemlisttail.cnt==-1||Elemlisthead.cnt==-1) ElemReturn.cnt=-1;
	  else{
	    for(i=0;i<Elemlisttail.cnt;i++){
	      for(j=0;j<Elemlisthead.cnt;j++){
	        if(Elemlisttail.ElemInfo[i]==Elemlisthead.ElemInfo[j]){
	          ElemReturn.ElemInfo[ElemReturn.cnt++]=Elemlisttail.ElemInfo[i];
	        }
	      }
	    }
	  }
	}
  return ElemReturn;
}

Face Edge_GetFace(PolyModel* model,int *EdgeInput){
  int edgetail=*EdgeInput;
  int edgehead=*(EdgeInput+1);
  
  Face FaceReturn;
  FaceReturn.cnt=0;
  memset(FaceReturn.FaceInfo,0,sizeof(int)*sizeof(FaceReturn.FaceInfo)/sizeof(FaceReturn.FaceInfo[0]));
	
	bool bo = Edge_IsValid(model, EdgeInput);
  if(bo==true){
	  int i,j,k,l;
	  int exist1, exist2;
	  Elem Elemlist=Edge_GetElem(model,EdgeInput);
	  for(i=0;i<Elemlist.cnt;i++){
	    Face Facelist=Elem_GetFace(model,Elemlist.ElemInfo[i]);
	    for(j=0;j<Facelist.cnt;j++){
	    	Node Nodelist=Face_GetNode(model,Facelist.FaceInfo[j]);
	      exist1=0;
	     	exist2=0;
	      for(k=0;k<Nodelist.cnt;k++){
	        if(Nodelist.NodeInfo[k]==edgetail){
	          exist1++;
	        }
	        if(Nodelist.NodeInfo[k]==edgehead){
	        	exist2++;
	        }
	      }
	      if(exist1&&exist2){
	      	/**
						If both nodes exist , then EdgeInput is contained in this face . 
						So we check whether it exists in output list and store it if not . 
					*/
					for(l=0;l<FaceReturn.cnt;l++){
						if(Facelist.FaceInfo[j]==FaceReturn.FaceInfo[l]){
							break;
						}
					}
					if(l==FaceReturn.cnt){
	        	FaceReturn.FaceInfo[FaceReturn.cnt++]=Facelist.FaceInfo[j];
	        }
	      }
	    }
	  }
	}
  return FaceReturn;
}

Edge Edge_GetEdge(PolyModel* model,int *EdgeInput){
	Edge EdgeReturn;
	EdgeReturn.cnt=0;
	memset(EdgeReturn.EdgeInfo,0,sizeof(int)*sizeof(EdgeReturn.EdgeInfo)/sizeof(EdgeReturn.EdgeInfo[0][0]));
	
	bool bo = Edge_IsValid(model, EdgeInput);
  if(bo==true){
	  int edgetail=*EdgeInput;
	  int edgehead=*(EdgeInput+1);
		Edge EdgesOfNodetail=Node_GetEdge(model,edgetail);
		Edge EdgesOfNodehead=Node_GetEdge(model,edgehead);
		
		int i,j;
	   for(i=0;i<EdgesOfNodetail.cnt;i++){
			 if(EdgesOfNodetail.EdgeInfo[i][1]==edgehead) continue;
		   EdgeReturn.EdgeInfo[EdgeReturn.cnt][0]=edgetail;
		   EdgeReturn.EdgeInfo[EdgeReturn.cnt][1]=EdgesOfNodetail.EdgeInfo[i][1];
		   EdgeReturn.cnt++;
	   }
	   for(j=0;j<EdgesOfNodehead.cnt;j++){
		   if(EdgesOfNodehead.EdgeInfo[j][1]==edgetail) continue;
		   EdgeReturn.EdgeInfo[EdgeReturn.cnt][0]=edgehead;
		   EdgeReturn.EdgeInfo[EdgeReturn.cnt][1]=EdgesOfNodehead.EdgeInfo[j][1];
		   EdgeReturn.cnt++;
	   }
   }
   return EdgeReturn;
 }
 
Node Edge_GetNode(PolyModel* model,int* EdgeInput){
  Node NodeReturn;
  NodeReturn.cnt=2;
  memset(NodeReturn.NodeInfo,0,sizeof(int)*sizeof(NodeReturn.NodeInfo)/sizeof(NodeReturn.NodeInfo[0]));
  
  bool bo = Edge_IsValid(model, EdgeInput);
  if(bo==true){
  	memcpy(NodeReturn.NodeInfo,EdgeInput,sizeof(int)*NodeReturn.cnt);
  }
  
  return NodeReturn;
}

Elem Elem_GetElem(PolyModel* model,int ElemInput){
	Elem ElemReturn;
  ElemReturn.cnt=0;
  memset(ElemReturn.ElemInfo,0,sizeof(int)*sizeof(ElemReturn.ElemInfo)/sizeof(ElemReturn.ElemInfo[0]));

	/**
		If we define adjacent element by faces like 3D , then it is trivial because element and face are the 
		same . 
	*/
	if(model->dimen==2){
		Edge Edgelist=Elem_GetEdge(model,ElemInput);
  
	  int i,j;
	  int EdgeCurr[2]={0};
	  for(i=0;i<Edgelist.cnt;i++){
	  	EdgeCurr[0]=Edgelist.EdgeInfo[i][0];
	  	EdgeCurr[1]=Edgelist.EdgeInfo[i][1];
	  	Elem Elemlist=Edge_GetElem(model,EdgeCurr);
	  	for(j=0;j<Elemlist.cnt;j++){
	  		if(Elemlist.ElemInfo[j]!=ElemInput){
	  			ElemReturn.ElemInfo[ElemReturn.cnt++]=Elemlist.ElemInfo[j];
	  		}
	  	}
	  }
	}
	
	else if(model->dimen==3){
		Face Facelist=Elem_GetFace(model,ElemInput);
	  int i;
	  for(i=0;i<Facelist.cnt;i++){
	    Elem Elemlist=Face_GetElem(model,Facelist.FaceInfo[i]);
			if(Elemlist.cnt!=1){
				if(Elemlist.ElemInfo[0]==ElemInput){
					ElemReturn.ElemInfo[ElemReturn.cnt++]=Elemlist.ElemInfo[1];
				}
				else{
					ElemReturn.ElemInfo[ElemReturn.cnt++]=Elemlist.ElemInfo[0];
				}
			}
		}
	}
  return ElemReturn;
}

Face Face_GetFace(PolyModel* model,int FaceInput){
	Face FaceReturn;
  FaceReturn.cnt=0;
  memset(FaceReturn.FaceInfo,0,sizeof(int)*sizeof(FaceReturn.FaceInfo)/sizeof(FaceReturn.FaceInfo[0]));
  Edge Edgelist=Face_GetEdge(model,FaceInput);
  
  int i,j;
  int EdgeCurr[2]={0};
  for(i=0;i<Edgelist.cnt;i++){
  	EdgeCurr[0]=Edgelist.EdgeInfo[i][0];
  	EdgeCurr[1]=Edgelist.EdgeInfo[i][1];
  	Face Facelist=Edge_GetFace(model,EdgeCurr);
  	for(j=0;j<Facelist.cnt;j++){
  		if(Facelist.FaceInfo[j]!=FaceInput){
  			FaceReturn.FaceInfo[FaceReturn.cnt++]=Facelist.FaceInfo[j];
  		}
  	}
  }
  return FaceReturn;
}







char *getDateTime(){
	static char nowtime[20];
	time_t rawtime;
	struct tm *ltime;
	time(&rawtime);
	ltime = localtime(&rawtime);
	strftime(nowtime,20,"%Y-%m-%d %H:%M:%S",ltime);
	return nowtime;
}

int nextPrime(int n){
    int i;
    int Curr=n;

    while(true){
        Curr++;
        for(i=2; i<=Curr; i++){
            if(i==Curr){
                return i;
            }
            if(Curr%i==0){
                break;
            }
        }
    }
}


