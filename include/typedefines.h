#ifndef __TYPEDEFINE_H__
#define __TYPEDEFINE_H__


/** 
    Element types. 
    Element templates can be found in "elem/concrete[type].h".
*/
typedef enum{
  POLY_ELEMENT_POLY2D = 0,
  POLY_ELEMENT_POLY3D,
  POLY_ELEMENT_COH2E2,
  POLY_ELEMENT_COH3POLY,
  /** Number of element types */ 
  POLY_ELEMENT_MAX
}ELEMTYPE;


/**
	NODE, FACE and ELEM are Hashnodes that store mesh information
*/
typedef struct NODE
{
	int Global_id; // id
	double x;
	double y;
	double z;
	int *AdjEnt_id; // adjacent entities
	int AdjEnt_cnt; // number of adjacent entities
	bool isBoundary;
	void* Attrib;
	struct NODE* next; // points at next node
}NODE;

typedef struct FACE
{
	int Global_id;
	int *AdjEnt_id;
	int AdjEnt_cnt;
	void* Attrib;
	bool isCohesive;
	bool isBoundary;
	struct FACE *next; // points at next face
}FACE;

typedef struct ELEM
{
	int Global_id;
	ELEMTYPE Element_Type;
	int *AdjEnt_id;
	int AdjEnt_cnt;
	void* Attrib;
	struct ELEM* next; // points at next element
}ELEM;

typedef struct EDGE
//EDGE is only defined for performance tests. 
//We use N<N>(byedge) info. to print Alledge info. Since the edge is represented as a pair of nodes, as long as
//we traverse all the node and find their adjacent nodes, we can retrieve all edges. To partition off repeated
//edges, we simply ignore the adjacent nodes whose number is smaller than that of the viewpoint node. 
{
  int Firstnode_id; //the viewpoint node
	int *Adjnode_id; 
	int Adjnode_cnt; 
	struct EDGE* next;
}EDGE;




typedef struct FileInfo        
{
	int Dim; 
	char NOE[20];  
	char FileE[150];       
	char FileF[150];
	char FileN[150];
	int NoF;
	int NoE;
	int NoN;
	char FileO[50];
}FileInfo;


/**
	PolyNode is an integer that represents the user-defined index of the node, 
	which is copied directly from the input file . 
*/
typedef struct PolyElem
{
	int id;
}PolyElem;

typedef struct PolyNode
{
	int id;
	double x;	// coordinates
	double y;
	double z;
}PolyNode;

typedef struct PolyFace
{
	int id;
}PolyFace;

typedef struct PolyEdge
{
	int EdgeTail;
	int EdgeHead;
}PolyEdge;

typedef struct Node
{
  int cnt;
  int NodeInfo[200];
}Node;

typedef struct Edge
{
  int cnt;
  int EdgeInfo[100][2];
}Edge;

typedef struct Face
{
  int cnt;
  int FaceInfo[200];
}Face;

typedef struct Elem
{
  int cnt;
  int ElemInfo[100];
}Elem;

typedef struct PolyModel
{
	int dimen;
	int NumberOfElems;
	int NumberOfFaces;
	int NumberOfEdges;
	int NumberOfNodes;
	ELEM** ElemHash;
	FACE** FaceHash;
	NODE** NodeHash;
	int ELEMHASHSIZE;
	int FACEHASHSIZE;
	int NODEHASHSIZE;

	struct IdInfo{
		int ELEMIDMAX;
		int FACEIDMAX;
		int NODEIDMAX;
	}IdInfo;
	
	struct CurrInfo{
		int Type; //EE1 EF2 Ee3 EN4 FE5 FF6 Fe7 FN8 eE9 eF10 ee11 eN12 NE13 NF14 Ne15 NN16
		PolyNode CurrInfoPolyNode;
		PolyEdge CurrInfoPolyEdge;
		PolyFace CurrInfoPolyFace;
		PolyElem CurrInfoPolyElem;
		Node CurrInfoNode;
		Edge CurrInfoEdge;
		Face CurrInfoFace;
		Elem CurrInfoElem;
	}CurrInfo;
	FileInfo FileInfo;
}PolyModel;






/**
	Itr_Elem_Ptr is pointer pointing at the current ELEM in ELEMHASH
	parameter : pointer to node list file name // argument : node file
*/
typedef struct PolyElemItr
{
	int Itr_Elem_BucId;
	ELEM* Itr_Elem_Ptr;
	unsigned int Itr_Elem_Cnt;
	PolyModel* Model;
}PolyElemItr;

typedef struct PolyFaceItr
{
	int Itr_Face_BucId;
	FACE* Itr_Face_Ptr;
	unsigned int Itr_Face_Cnt;
	PolyModel* Model;
}PolyFaceItr;

typedef struct PolyNodeItr
{
	int Itr_Node_BucId;
	NODE* Itr_Node_Ptr;
	unsigned int Itr_Node_Cnt;
	PolyModel* Model;
}PolyNodeItr;

/**
	Itr_Edge_Ptr has different algorithm from Itr_Elem_Ptr , because Edge is implicit 
	entity . We use N[e] to iterate through all edges in the mesh . 
	parameter : index of the first node and adjacent node whose indices are greater
	than that of the first node . 
*/
typedef struct PolyEdgeItr
{
	PolyNodeItr* Itr_FirstNodeItr;
	int Itr_SecondNode_Id[100];
	int AdjEnt_cnt;
	int Curr_SecondNode_IdId;
	unsigned int Itr_Edge_Cnt;
	PolyModel* Model;
}PolyEdgeItr;







#endif