#ifndef __COHESIVE3D_H__
#define __COHESIVE3D_H__

typedef struct NodeCheck
{
	int Node_Id;
	int ElemHome_cnt;
	int ElemHome_Id[50];
	int ElemAway_cnt;
	int ElemAway_Id[50];
}NodeCheck;

#define NodeDupl_ListSize 50

typedef struct NodeDupl_List
{
	int Node_cnt;
	NodeCheck NodeDupl_ListInfo[NodeDupl_ListSize];
}NodeDupl_List;


bool Cohesive3D_Elem_IsCohesive(PolyModel *model, int ElemInput);

bool Cohesive3D_Face_IsBarrier(PolyModel *model, int FaceInput);

bool Cohesive3D_ElemElem_IsAdjacent(PolyModel *model, int ElemInput1, int ElemInput2, int FaceInput);

Elem Cohesive3D_Node_GetBulkElem(PolyModel *model, int NodeInput);

Elem Cohesive3D_Node_GetCohsElem(PolyModel *model, int NodeInput);

void Cohesive3D_ElemElem_DFS(PolyModel *model, int ElemInput1, int ElemInput2, Elem ElemBulkToNodeCurr, int FaceInput, int* visited);

NodeCheck Cohesive3D_FaceNode_GatherElem(PolyModel *model, int NodeInput, int FaceInput, int Elem1, int Elem2);

bool Cohesive3D_Face_CreateElem(PolyModel *model, int FaceInput, int NewElemId, int NewFaceId, int RefElem);

bool Cohesive3D_CreateNode(PolyModel* model, double x, double y, double z, bool isBoundary, int id);

bool Cohesive3D_UpdateNode(PolyModel *model, int NodeCurr, int newnodeid, int Coh, Elem ElemUpdate, Elem ElemLocked);

bool Cohesive3D_InsertCohesiveAtFace_Manual(PolyModel *model, int FaceInput);

int Cohesive3D_InsertCohesiveAtFace_Auto(PolyModel *model, int FaceInput);

bool Cohesive3D_InsertCohesiveAtFace_Test(PolyModel *model, int* reflist);

int Test_Nodes_GetFace(PolyModel* model, int Node1, int Node2, int Node3);

void swap(int* a, int* b);

#endif