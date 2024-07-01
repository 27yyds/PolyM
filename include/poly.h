#ifndef __POLYFUNCS_H__
#define __POLYFUNCS_H__


PolyModel* polyModel_Create(int dim, int NOE, int NOF, int NON, double HashSizeDensity);
void polyModel_SetFileInfo(PolyModel*, FileInfo);
void polyModel_InitializeNE(PolyModel*);

void polyModel_Destroy(PolyModel*);

int polyModel_GetELEMIDMAX(PolyModel*);
int polyModel_GetFACEIDMAX(PolyModel*);
int polyModel_GetNODEIDMAX(PolyModel*);

/**
	Add / Delete a node structure to NodeHash   OR   Print all node in NodeHash
	parameter : pointer to Polymodel  AND   pointer to a node structure 
*/
bool polyModel_InsertNode(PolyModel*, NODE*);
void polyModel_RemNode(PolyModel* model, int);
void polyModel_PrintAllNodeData(PolyModel*);
bool polyModel_InsertFace(PolyModel*, FACE*);
void polyModel_RemFace(PolyModel* model, int);
void polyModel_PrintAllFaceData(PolyModel*);
bool polyModel_InsertElem(PolyModel*, ELEM*);
void polyModel_RemElem(PolyModel* model, int);
void polyModel_PrintAllElemData(PolyModel*);

void polyModel_PrintAllEdgeData(PolyModel*);

int polyModel_GetNNodes(PolyModel*);
int polyModel_GetNElems(PolyModel*);
int polyModel_GetNFaces(PolyModel*);


PolyElemItr* polyModel_CreateElemItr(PolyModel*);
void polyElemItr_Destroy(PolyElemItr*);
void polyElemItr_Begin(PolyElemItr*);
bool polyElemItr_IsValid(PolyElemItr*);
void polyElemItr_Next(PolyElemItr*);
PolyElem polyElemItr_GetCurr(PolyElemItr*);

PolyFaceItr* polyModel_CreateFaceItr(PolyModel*);
void polyFaceItr_Destroy(PolyFaceItr*);
void polyFaceItr_Begin(PolyFaceItr*);
bool polyFaceItr_IsValid(PolyFaceItr*);
void polyFaceItr_Next(PolyFaceItr*);
PolyFace polyFaceItr_GetCurr(PolyFaceItr*);

PolyNodeItr* polyModel_CreateNodeItr(PolyModel*);
void polyNodeItr_Destroy(PolyNodeItr*);
void polyNodeItr_Begin(PolyNodeItr*);
bool polyNodeItr_IsValid(PolyNodeItr*);
void polyNodeItr_Next(PolyNodeItr*);
PolyNode polyNodeItr_GetCurr(PolyNodeItr*);

PolyEdgeItr* polyModel_CreateEdgeItr(PolyModel*);
void polyEdgeItr_Destroy(PolyEdgeItr*);
void polyEdgeItr_Begin(PolyEdgeItr*);
bool polyEdgeItr_IsValid(PolyEdgeItr*);
void polyEdgeItr_Next(PolyEdgeItr*);
PolyEdge polyEdgeItr_GetCurr(PolyEdgeItr*);


/**
	PolyElem is an integer that represents the user-defined index of the elem, 
	which is copied from the input file . 
	
	//Ver221122 The user-defined indexs of the elems are directly utilized in this code, 
	so this algorithm simply returns the same figure as PolyElem represents . 
*/
int polyElem_GetId(PolyModel*, PolyElem);

PolyElem polyElem_ReturnFromId(PolyModel*, int);

bool polyElem_IsValid(PolyModel*, PolyElem);

/**
	Copy the second PolyElem to the first PolyElem 
*/
void polyElem_Copy(PolyModel*, PolyElem, PolyElem);

/**
	Compare two PolyElems
	//identical 1 different 0
*/
bool polyElem_Compare(PolyModel*, PolyElem, PolyElem);


int polyFace_GetId(PolyModel*, PolyFace);
bool polyFace_IsValid(PolyModel*, PolyFace);
PolyFace polyFace_ReturnFromId(PolyModel*, int);
void polyFace_Copy(PolyModel*, PolyFace, PolyFace);
/**
	Compare two PolyFaces by their indices
	//identical 1 different 0
*/
bool polyFace_Compare(PolyModel*, PolyFace, PolyFace);

int polyNode_GetId(PolyModel*, PolyNode);
PolyNode polyNode_ReturnFromId(PolyModel*, int);

bool polyNode_IsValid(PolyModel*, PolyNode);
bool polyNode_IsBoundary(PolyModel*, PolyNode);
void polyNode_Copy(PolyModel*, PolyNode, PolyNode);
/**
	Compare two PolyNodes by their indices
	//identical 1 different 0
*/
bool polyNode_Compare(PolyModel*, PolyNode, PolyNode);

PolyEdge polyEdge_SetEdge(PolyModel*, int, int);
bool polyEdge_IsValid(PolyModel*, PolyEdge);
/**
	We do NOT expect a GetId or ReturnFromId for an Edge , like those which are 
	defined for PolyElem and PolyNode , because edges are implicitly defined . 
*/
void polyEdge_Copy(PolyModel*, PolyEdge, PolyEdge);
/**
	Compare two PolyEdges by their indices
	//identical 1 different 0
*/
bool polyEdge_Compare(PolyModel*, PolyEdge, PolyEdge);



int polyElem_GetNElems(PolyModel*, PolyElem);
PolyElem polyElem_GetElem(PolyModel*, PolyElem, int);
int polyElem_GetNFaces(PolyModel*, PolyElem);
PolyFace polyElem_GetFace(PolyModel*, PolyElem, int);
int polyElem_GetNEdges(PolyModel*, PolyElem);
PolyEdge polyElem_GetEdge(PolyModel*, PolyElem, int);
int polyElem_GetNNodes(PolyModel*, PolyElem);
PolyNode polyElem_GetNode(PolyModel*, PolyElem, int);

int polyFace_GetNElems(PolyModel*, PolyFace);
PolyElem polyFace_GetElem(PolyModel*, PolyFace, int);
int polyFace_GetNFaces(PolyModel*, PolyFace);
PolyFace polyFace_GetFace(PolyModel*, PolyFace, int);
int polyFace_GetNEdges(PolyModel*, PolyFace);
PolyEdge polyFace_GetEdge(PolyModel*, PolyFace, int);
int polyFace_GetNNodes(PolyModel*, PolyFace);
PolyNode polyFace_GetNode(PolyModel*, PolyFace, int);

int polyEdge_GetNElems(PolyModel*, PolyEdge);
PolyElem polyEdge_GetElem(PolyModel*, PolyEdge, int);
int polyEdge_GetNFaces(PolyModel*, PolyEdge);
PolyFace polyEdge_GetFace(PolyModel*, PolyEdge, int);
int polyEdge_GetNEdges(PolyModel*, PolyEdge);
PolyEdge polyEdge_GetEdge(PolyModel*, PolyEdge, int);
int polyEdge_GetNNodes(PolyModel*, PolyEdge);
PolyNode polyEdge_GetNode(PolyModel*, PolyEdge, int);

int polyNode_GetNElems(PolyModel*, PolyNode);
PolyElem polyNode_GetElem(PolyModel*, PolyNode, int);
int polyNode_GetNFaces(PolyModel*, PolyNode);
PolyFace polyNode_GetFace(PolyModel*, PolyNode, int);
int polyNode_GetNEdges(PolyModel*, PolyNode);
PolyEdge polyNode_GetEdge(PolyModel*, PolyNode, int);
int polyNode_GetNNodes(PolyModel*, PolyNode);
PolyNode polyNode_GetNode(PolyModel*, PolyNode, int);

void polyElem_SetAttrib(PolyModel*, PolyElem, void*);
void polyFace_SetAttrib(PolyModel*, PolyFace, void*);
void polyNode_SetAttrib(PolyModel*, PolyNode, void*);
void* polyElem_GetAttrib(PolyModel*, PolyElem);
void* polyFace_GetAttrib(PolyModel*, PolyFace);
void* polyNode_GetAttrib(PolyModel*, PolyNode);
void polyElem_DeleteAttrib(PolyModel*, PolyElem);
void polyFace_DeleteAttrib(PolyModel*, PolyFace);
void polyNode_DeleteAttrib(PolyModel*, PolyNode);

#endif