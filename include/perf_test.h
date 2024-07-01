#ifndef __PERF_TEST_3D_H__
#define __PERF_TEST_3D_H__


typedef struct cpu_occupy_        
{
    char name[20];                 
    unsigned int user;             
    unsigned int nice;   
    unsigned int system;   
    unsigned int idle;  
    unsigned int iowait;
    unsigned int irq;
    unsigned int softirq;
}cpu_occupy_t;


typedef struct TimeList
{
	double timeAE;
	double timeAF;
	double timeAN;
	double timeAe;


	double timeEF;
	double timeFN;
	double timeNE;
	double timeFe;
			
	double timeEN;
	double timeEe;

	double timeFE;
	double timeNF;
	double timeNe;
	double timeNN;
	double timeeE;
	double timeeF;
	double timeee;
	double timeeN;
	
	double timeEE;
	double timeFF;
}TimeList;




void ALLTEST(PolyModel*,int);
void ALLPRINT(PolyModel*,int);
TimeList ALLTIME(PolyModel*,int);

void EF_Test(PolyModel*,int); 
void FN_Test(PolyModel*,int);
void NE_Test(PolyModel*,int);
void eN_Test(PolyModel*,int);
void Fe_Test(PolyModel*,int);

void EN_Test(PolyModel*,int);
void Ee_Test(PolyModel*,int);
void NN_Test(PolyModel*,int);
         
void FE_Test(PolyModel*,int);
void EE_Test(PolyModel*,int);

void eE_Test(PolyModel*,int);
void eF_Test(PolyModel*,int);
void FF_Test(PolyModel*,int);

void NF_Test(PolyModel*,int);
void Ne_Test(PolyModel*,int);
void ee_Test(PolyModel*,int);

void AllElem_Test(PolyModel*,int);
void AllFace_Test(PolyModel*,int);
void AllNode_Test(PolyModel*,int);
void AllEdge_Test(PolyModel*,int);



void EF_Print(PolyModel*,int); 
void FN_Print(PolyModel*,int);
void NE_Print(PolyModel*,int);
void eN_Print(PolyModel*,int);
void Fe_Print(PolyModel*,int);

void EN_Print(PolyModel*,int);
void Ee_Print(PolyModel*,int);
void NN_Print(PolyModel*,int);
         
void FE_Print(PolyModel*,int);
void EE_Print(PolyModel*,int);

void eE_Print(PolyModel*,int);
void eF_Print(PolyModel*,int);
void FF_Print(PolyModel*,int);

void NF_Print(PolyModel*,int);
void Ne_Print(PolyModel*,int);
void ee_Print(PolyModel*,int);

void AllElem_Print(PolyModel*,int);
void AllFace_Print(PolyModel*,int);
void AllNode_Print(PolyModel*,int);
void AllEdge_Print(PolyModel*,int);



double EF_Time(PolyModel*,int); 
double FN_Time(PolyModel*,int);
double NE_Time(PolyModel*,int);
double eN_Time(PolyModel*,int);
double Fe_Time(PolyModel*,int);

double EN_Time(PolyModel*,int);
double Ee_Time(PolyModel*,int);
double NN_Time(PolyModel*,int);

double FE_Time(PolyModel*,int);
double EE_Time(PolyModel*,int);

double eE_Time(PolyModel*,int);
double eF_Time(PolyModel*,int);
double FF_Time(PolyModel*,int);

double NF_Time(PolyModel*,int);
double Ne_Time(PolyModel*,int);
double ee_Time(PolyModel*,int);

double AllElem_Time(PolyModel*,int);
double AllFace_Time(PolyModel*,int);
double AllNode_Time(PolyModel*,int);
double AllEdge_Time(PolyModel*,int);

void Attribute_Test(PolyModel *model);

double cal_cpuoccupy (cpu_occupy_t *, cpu_occupy_t *);
void get_cpuoccupy (cpu_occupy_t *);
double get_sysCpuUsage();


#endif