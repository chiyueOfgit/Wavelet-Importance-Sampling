#include<iostream>
#include <math.h>

using namespace std;

double* oneDimensionalWaveletTransform(double* vOriginalData,int vSize);
double* oneDimensionalWaveletTree(double* vLeafData,int vSize);
double* oneDimensionalHierarchicalWarping(double* vWaveletTree,int vSize,int vPointNumber);
double* oneDHierarchicalWarping(double* vWaveletTree,int vSize,int vPointNumber);

int main()
{
    double test[]={8,10,9,5,1,1,4,4};
    double *out=oneDimensionalWaveletTree(test,8);
    int num=0;
    while(num<15)
    {
        cout<<*(out+num)<<endl;
        num++;
    }
    double *Warp=oneDimensionalHierarchicalWarping(out,15,1000);
    num=0;
    while(num<8)
    {
        cout<<*(Warp+num)<<" ";
        num++;
    }
    delete []out;
    delete []Warp;
    return 0;
}

double* oneDimensionalWaveletTransform(double* vOriginalData,int vSize)
{
    double *voResultOfWaveletTransform=new double[vSize];
    while(vSize!=1)
    {
        int LevelLength=vSize/2;
        for(int i=0;i<LevelLength;i++)
        {
            *(voResultOfWaveletTransform+i)=(*(vOriginalData+2*i)+*(vOriginalData+2*i+1))/2;
            *(voResultOfWaveletTransform+i+LevelLength)=*(vOriginalData+2*i)-*(voResultOfWaveletTransform+i);
            *(vOriginalData+i)=*(voResultOfWaveletTransform+i);
        }
        vSize/=2;
    }
    return voResultOfWaveletTransform;
}

double* oneDimensionalWaveletTree(double* vLeafData,int vSize)
{
	double *ResultOfWaveletTransform=new double[vSize];
	double *voWaveletTree=new double[vSize*2-1];
	for(int i=0;i<vSize;i++)
	{
		*(voWaveletTree+i)=*(vLeafData+i);
	}
	int Level=vSize;
    while(Level!=1)
    {
        int LevelLength=Level/2;
        for(int i=0;i<LevelLength;i++)
        {
            *(ResultOfWaveletTransform+i)=(*(vLeafData+2*i)+*(vLeafData+2*i+1))/2;
            *(ResultOfWaveletTransform+i+LevelLength)=*(vLeafData+2*i)-*(ResultOfWaveletTransform+i);
            *(vLeafData+i)=*(ResultOfWaveletTransform+i);
            *(voWaveletTree+vSize)=*(ResultOfWaveletTransform+i);
            vSize++;
        }
        Level/=2;
    }
    delete []ResultOfWaveletTransform;
    return voWaveletTree;
}

double* oneDimensionalHierarchicalWarping(double* vWaveletTree,int vSize,int vPointNumber)
{
	vWaveletTree=vWaveletTree+vSize-1;
	int Length=(vSize+1)/2;
	double* voResultOfWarping=new double[Length];
	int TraversalLength=0;
	int Stride=Length/2;
	int CopyLength=Length;
	while(CopyLength-->0)
		*(voResultOfWarping+CopyLength)=1;
	double *pBeginLocal;
	pBeginLocal=voResultOfWarping;
	int HandleFrequency=1;
	while(TraversalLength!=(vSize-1))
	{
		TraversalLength+=HandleFrequency*2;
		double sum=0;
		for(int i=0;i<HandleFrequency*2;i++)
			sum+=*(vWaveletTree-TraversalLength+i);
		for(int i=0;i<HandleFrequency;i++)
		{
			int LevelTranslation=TraversalLength-2*i;
			double CurrentProbability=*voResultOfWarping;
			*(voResultOfWarping+Stride)=*(vWaveletTree-LevelTranslation+1)/sum*CurrentProbability;
			*voResultOfWarping=*(vWaveletTree-LevelTranslation)/sum*CurrentProbability;
			voResultOfWarping=voResultOfWarping+2*Stride;
		}
		voResultOfWarping=pBeginLocal;
		Stride/=2;
		HandleFrequency*=2;
	}
	while(Length-->0)
		*(voResultOfWarping+Length)*=(double)vPointNumber;
	return voResultOfWarping;
}

double* oneDHierarchicalWarping(double* vWaveletTree,int vSize,int vPointNumber)
{
	vWaveletTree=vWaveletTree+vSize-1;
	int Length=(vSize+1)/2;
	double* voResultOfWarping=new double[Length];
	int TraversalLength=0;
	int Stride=Length/2;
	int CopyLength=Length;
	while(CopyLength-->0)
		*(voResultOfWarping+CopyLength)=1;
	double *pBeginLocal;
	pBeginLocal=voResultOfWarping;
	int HandleFrequency=1;
	while(TraversalLength!=(vSize-1))
	{
		TraversalLength+=HandleFrequency*2;
		for(int i=0;i<HandleFrequency;i++)
		{
			int LevelTranslation=TraversalLength-2*i;
			double CurrentProbability=*voResultOfWarping;
			double sum=*(vWaveletTree-LevelTranslation+1)+*(vWaveletTree-LevelTranslation);
			*(voResultOfWarping+Stride)=*(vWaveletTree-LevelTranslation+1)/sum*CurrentProbability;
			*voResultOfWarping=*(vWaveletTree-LevelTranslation)/sum*CurrentProbability;
			voResultOfWarping=voResultOfWarping+2*Stride;
		}
		voResultOfWarping=pBeginLocal;
		Stride/=2;
		HandleFrequency*=2;
	}
	while(Length-->0)
		*(voResultOfWarping+Length)*=(double)vPointNumber;
	return voResultOfWarping;
}

