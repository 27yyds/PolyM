#ifndef __COHESIVE_H__
#define __COHESIVE_H__


typedef struct RandEdgeList      
{
	int RandEdgeListCnt;
	int* RandEdgeListPtr;
}RandEdgeList;


/*
		In 2D, we say that a face is cohesive if it belongs to a cohesive element
*/
bool Cohesive2D_Face_IsCohesive(PolyModel *model, int FaceInput);

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
                      
    
    Cohesive2D_EdgeNode_GetFace()
				Input : e1 = [ n2 n1 ]
				Output : f1
				
		Cohesive2D_FaceEdge_GetNode()
				Input : f1 , e1 = [ n2 n1 ]
				Output : n4 (another node connected with n2 in f1)		
    
    Cohesive2D_FaceEdge_GetFace()
				Input : f1 , e6 = [ n2 n4 ]
				Output : f2 (face as the neighbor of f1 at e6)		
				
				
    Firstly, notice the criteria that n2 and n6 are already marked as "on the boundary", while n1 is not .
    So for inserting C1, we go to the case that the edge has one and only one node on the boundary . And 
    n2 is exactly the node to be duplicated . 
    
    Then, let n2 be NodeDupl and n1 be Node2 as the input of the function . The edge is defined as 
     { n2 , n1 } = e1.  
	*/   


/*
	Input : 	FaceInput is one of the faces that is adjacent to the input edge . 
						NodeDupl is the node to be duplicated . 
						NodePrev is the other node of the input edge . 
	Output : 	Another edge containing NodeDupl in FaceInput . 
	Algo	:		Find another node from FACE TABLE
*/
int Cohesive2D_FaceEdge_GetNode(PolyModel *model, int FaceInput, int NodeDupl, int NodePrev);

int Cohesive2D_FaceEdge_GetFace(PolyModel *model, int FaceInput, int NodeDupl, int NodePrev);

Face Cohesive2D_GatherFace(PolyModel *model, int NodeDupl, int NodeRef, int Direction);

/*
	In 2D element and face are different representations of the same entity, and they 
	share a same id . Hence we create face and elem at the same time . 
*/
bool Cohesive2D_CreateFace(PolyModel *model, int Node1, int Node2, int NewFaceId);

bool Cohesive2D_CreateNode(PolyModel* model, double x, double y, double z, bool isBoundary, int id);

void Cohesive2D_UpdateNE(PolyModel *model, int NodeUpdate, Face Facelist);

bool Cohesive2D_UpdateNode(PolyModel *model, int NodeUpdate, int Node2, int NodeOb, int CohElem, int ElemUpdate, int ElemLock, int NewNodeid, int ref);

int Cohesive2D_Edge_GetAnchorFace(PolyModel *model, int* EdgeInput);

bool polyModel_InsertCohesiveAtEdge_Manual(PolyModel *model, PolyEdge PolyEdgeInput);

bool polyModel_InsertCohesiveAtEdge_Auto(PolyModel *model, PolyEdge PolyEdgeInput);

bool polyModel_InsertCohesiveAtEdge_Test(PolyModel *model, int* reflist);

void Cohesive_Insert(PolyModel *model, int Node1, int Node2, int RefElem1, int RefElem2, int Node3, int Node4);

RandEdgeList Cohesive_RandomEdge(PolyModel *model, int Percentage);

RandEdgeList Cohesive_AllEdge(PolyModel *model);




/*
	Algorithm :
	
			1. Fix a node 
			2. Use Node_GetAllElem to find all bulk elements and cohesive elements that contain this node .
			3. Use Node_GetEdge to find all edges containing the given node . Then use Edge_GetAllElem to
			find all elements containing these edges . 
			4. Compare the elements from step 2 and 3 . 
			
	Explaination :
			The result from step 2 is copied directly from NE table . 
			The result from step 3 is retrieved from firstly finding all edges and then refering to all elements . 
			We only consider bulk elements when getting these edges . 
			
			So it is nontrivial to compare step 2 and 3 . 
			
			The update on node indices "seperate" those elements that are not connected . That is why we only 
			compare element indices when checking the consistency . 
*/
bool Cohesive2D_CheckConsistencyOnNode(PolyModel *model, int NodeInput);

#endif