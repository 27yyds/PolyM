#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

#include "include/typedefines.h"
#include "include/data_structure.h"
#include "include/perf_test.h"
#include "include/poly.h"
#include "include/cohesive.h"
#include "include/cohesive3D.h"



void ReadInput (PolyModel *model, FILE *fo);
void ReadFace(PolyModel *model, FILE *fo);
void ReadElem(PolyModel *model, FILE *fo);
void ReadNode(PolyModel* model, FILE *fo);



int main (int argc, char* argv[])
{   
	// Inputs ******************
  int ihl = 100; // initial hash length
	PolyModel* model = polyModel_Create(3, ihl, ihl, ihl, 1);
	FILE *fo = fopen(argv[1], "r");
	ReadInput(model, fo);
	fclose(fo);
	polyModel_InitializeNE(model);
	// ****************************
//	
//	polyModel_PrintAllFaceData(model);
	Cohesive3D_InsertCohesiveAtFace_Auto(model, 40);
	Cohesive3D_InsertCohesiveAtFace_Auto(model, 44);
//	
	polyModel_PrintAllFaceData(model);
	polyModel_PrintAllElemData(model);
	polyModel_PrintAllNodeData(model);
	polyModel_Destroy(model);
	
}



void ReadInput (PolyModel *model, FILE *fo)
{
	char command[512];
	
	if (fo == NULL)
	{
		printf("Could not open file\n");
		exit(1);   
	}
	
	while (!feof(fo))
	{
		fscanf(fo, "%s\n", command);
		if (strcmp(command, "*NODE")==0)
			ReadNode(model, fo);
		else if (strcmp(command, "*ELEMENT")==0)
			ReadElem(model, fo);
		else if (strcmp(command, "*FACE")==0)
			ReadFace(model, fo);
		else if (strcmp(command, "*END")==0)
		{
			printf("Reading input file ends.\n");
			break;
		}
	}
}

void ReadFace(PolyModel *model, FILE *fo)
{
	int i,j;
	int nface;
	int fid;
	int npf;
	int nids[100];

	fscanf(fo, "%d \n", &nface);
    for (i=0; i<nface; i++)
    {
    	fscanf(fo, "%d, %d", &fid, &npf);
    	for (j=0; j<npf; j++)
    	{
    		fscanf (fo, ", %d", &nids[j]);
    	}
    	fscanf(fo, "\n");
    
		FACE *face = (FACE*)malloc(sizeof(FACE));
		face->Global_id = fid;
		face->AdjEnt_id = (int*)malloc(sizeof(int)*npf); // malloc for list of face id of the faceent
		memcpy(face->AdjEnt_id, nids, sizeof(int)*npf);
		face->AdjEnt_cnt = npf; // Save the node size
		polyModel_InsertFace(model, face); // Add new face to "FaceHash"
		
//		PolyFace polyface = polyFace_GetFaceAtId (model, fid);
//		FaceAtt *fatt = (FaceAtt *) calloc (1, sizeof(FaceAtt));
//		polyFace_SetAttrib (model, polyface, fatt);
	}
	
}

void ReadElem(PolyModel *model, FILE *fo)
{
	int i,j;
	int nelem;
	int eid;
	int fpe;
	int fids[100];
	
	fscanf(fo, "%d \n", &nelem);
	for (i=0; i<nelem; i++)
	{
		fscanf(fo, "%d, %d", &eid, &fpe);
		for (j=0; j<fpe; j++)
		{
			fscanf(fo, ", %d", &fids[j]);
		}
		fscanf(fo, "\n");
		
		ELEM *elem = (ELEM*)malloc(sizeof(ELEM));
		elem->Global_id = eid;
		elem->Element_Type = POLY_ELEMENT_POLY3D;
		elem->AdjEnt_id = (int*)malloc(sizeof(int)*fpe); 
		memcpy(elem->AdjEnt_id,fids,sizeof(int)*fpe);
		elem->AdjEnt_cnt = fpe;
		polyModel_InsertElem(model, elem); // Add new element to "ElemHash"
		
//		PolyElem polyelem = polyElem_GetElemAtId (model, eid);
//		ElemAtt *eatt = (ElemAtt *) calloc (1, sizeof(ElemAtt));
//		polyElem_SetAttrib (model, polyelem, eatt);
	}
	
}

void ReadNode(PolyModel* model, FILE *fo)
{
	int i,j;
	int nnode;
	int nid;
	double x, y, z;

	fscanf(fo, "%d \n", &nnode);
	for (i = 0; i < nnode; i++)
	{
		fscanf(fo,"%d, %lf, %lf, %lf\n",&nid, &x, &y, &z);
    	NODE *node = (NODE*)malloc(sizeof(NODE));
		node->Global_id = nid;
		node->x = x;
		node->y = y;
		node->z = z;
		node->AdjEnt_cnt = 0;
		node->AdjEnt_id = NULL;
		node->isBoundary = 0;
		polyModel_InsertNode(model, node);
		
//		PolyNode polynode = polyNode_GetNodeAtId (model, nid);
//		NodeAtt *natt = (NodeAtt *) calloc (1, sizeof(NodeAtt));
//		polyNode_SetAttrib (model, polynode, natt);
	}

	return;
}
