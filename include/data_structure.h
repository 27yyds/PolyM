#ifndef __DATA_STRUCTURE_3D_H__
#define __DATA_STRUCTURE_3D_H__



bool Elem_IsValid(PolyModel* model, int ElemInput);
bool Face_IsValid(PolyModel* model, int FaceInput);
bool Node_IsValid(PolyModel* model, int NodeInput);
bool Edge_IsValid(PolyModel* model, int* EdgeInput);

bool Elem_IsBulk(PolyModel *model, int ElemInput);
bool Elem_IsCohs(PolyModel *model, int ElemInput);

NODE* Node_GetNODEAtId(PolyModel*, int);
FACE* Face_GetFACEAtId(PolyModel*, int);
ELEM* Elem_GetELEMAtId(PolyModel*, int);
int Elem_GetIdAtELEM(PolyModel*, ELEM*);
int Face_GetIdAtFACE(PolyModel*, FACE*);
int Node_GetIdAtNODE(PolyModel*, NODE*);

bool Node_IsBoundary(PolyModel*, int);
int Node_MakeBoundary(PolyModel*, int);
void Node_GetPosition(NODE*, double*, double*, double*);




Face Elem_GetFace(PolyModel*, int);
Node Face_GetNode(PolyModel*, int);
Elem Node_GetElem(PolyModel*, int);
Edge Face_GetEdge(PolyModel*, int);

Node Elem_GetNode(PolyModel*, int);
Edge Elem_GetEdge(PolyModel*, int);
Elem Face_GetElem(PolyModel*, int);
Face Node_GetFace(PolyModel*, int);

Edge Node_GetEdge(PolyModel*, int);
Node Node_GetNode(PolyModel*, int);

Elem Edge_GetElem(PolyModel*, int*);
Face Edge_GetFace(PolyModel*, int*);
Edge Edge_GetEdge(PolyModel*, int*);
Node Edge_GetNode(PolyModel*, int*);

/** 
	Elem to Elem is defined differently in 2D and 3D . 
	In 3D we would like to find those elements that is adjacent to the given element by
	faces . 
	However in 2D , element is one-to-one mapped to face . 
	So in order to avoid triviality , we define Elem to Elem the same as Face to Face . 
*/
Elem Elem_GetElem(PolyModel*, int);
Face Face_GetFace(PolyModel*, int);






char *getDateTime();
int nextPrime(int);
#endif