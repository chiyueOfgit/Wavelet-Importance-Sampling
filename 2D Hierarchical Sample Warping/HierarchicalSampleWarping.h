#pragma once
#include "Vector2.h"
#include"Wavelet.h"
#include<vector>

class CHierarchicalSampleWarping
{
public:
	CHierarchicalSampleWarping();
	~CHierarchicalSampleWarping();
	vector<vector<Vector2>> createTwoDimensionalHierarchicalWarping(vector<vector<Vector2>> vSampleData, double** vOriginalData, int vRow, int vColumn);
private:
	double** __matrixTranspose(double** vOrigin, int vRow, int vColumn);
	vector<vector<float>> __vectorTranspose(vector<vector<float>>& matrix);
	vector<float> __createOneDimensionalHierarchicalWarping(vector<float> vioSampleData, double* vWaveletTree, int vTreeSize);
	vector<vector<float>> __createMultiOneDimensionalHierarchicalWarping(vector<vector<float>> vSampleData, double** vOriginalData, int vOriginalDataRowLength);
};

CHierarchicalSampleWarping::CHierarchicalSampleWarping()
{
}

CHierarchicalSampleWarping::~CHierarchicalSampleWarping()
{
}

double** CHierarchicalSampleWarping::__matrixTranspose(double** vOrigin, int vRow, int vColumn)
{
	double** voResult = new double* [vColumn];
	for (int i = 0; i < vColumn; i++)
	{
		*(voResult + i) = new double[vRow];
	}
	for (int i = 0; i < vRow; i++)
	{
		for (int k = 0; k < vColumn; k++)
		{
			voResult[i][k] = vOrigin[k][i];
		}
	}
	return voResult;
}

vector<float> CHierarchicalSampleWarping::__createOneDimensionalHierarchicalWarping(vector<float> vioSampleData, double* vWaveletTree,int vTreeSize)
{
	vWaveletTree = vWaveletTree + vTreeSize - 1;
	int TraversalLength = 0;  //当前遍历长度，用来定位当前处理节点
	double Stride = 1;  //步长，表示当前级别每一次缩放涉及的长度
	int HandleTimes = 1; //处理次数，表示当前级别需进行多少次扭曲
	int SampleDataSize = vioSampleData.size();
	while (TraversalLength != (vTreeSize - 1))
	{
		TraversalLength += HandleTimes * 2;
		double LeftBorder = 0;
		double RightBorder = Stride;
		for (int i = 0; i < HandleTimes; i++)
		{
			int LevelTranslation = TraversalLength - 2 * i;
			double Sum = *(vWaveletTree - LevelTranslation) + *(vWaveletTree - LevelTranslation + 1);
			double FrontProbaility = *(vWaveletTree - LevelTranslation) / Sum;
			double SplitCoordinate = LeftBorder+FrontProbaility * Stride;
			double Median = (LeftBorder + RightBorder) / 2;
			for (int k = 0; k < SampleDataSize; k++)//缩放
			{
				if (vioSampleData[k] < SplitCoordinate && vioSampleData[k] >= LeftBorder)
				{
					double scale = (Median - LeftBorder) / (SplitCoordinate - LeftBorder);
					vioSampleData[k] = LeftBorder + (vioSampleData[k] - LeftBorder) * scale;
				}
				else if (vioSampleData[k] >= SplitCoordinate && vioSampleData[k] <= RightBorder)
				{
					double scale = (RightBorder - Median) / (RightBorder - SplitCoordinate);
					vioSampleData[k] = RightBorder - (RightBorder - vioSampleData[k]) * scale;
				}
			}
			LeftBorder += Stride;
			RightBorder += Stride;
		}
		Stride /= 2;
		HandleTimes *= 2;
	}
	return vioSampleData;
}

vector<vector<Vector2>> CHierarchicalSampleWarping::createTwoDimensionalHierarchicalWarping(vector<vector<Vector2>> vioSampleData, double** vOriginalData, int vRow, int vColumn)
{
	vector<vector<float>> SampleX, SampleY;
	for (int i = 0,SampleRowLength = vioSampleData.size(); i < SampleRowLength; i++)
	{
		vector<float> SampleOneRowX, SampleOneColumnY;
		for (int k = 0, SampleColumnLength = vioSampleData[i].size(); k < SampleColumnLength; k++)
		{
			SampleOneRowX.push_back(vioSampleData[i][k].x);
			SampleOneColumnY.push_back(vioSampleData[i][k].y);
		}
		SampleX.push_back(SampleOneRowX);
		SampleY.push_back(SampleOneColumnY);
	}
	vector<vector<float>> RowWarpResult = __createMultiOneDimensionalHierarchicalWarping(SampleX,vOriginalData,vColumn);  //行扭曲
	double** RowTransformData = new double* [vRow];     //列扭曲
	for (int i = 0; i < vRow; i++)
		*(RowTransformData + i) = new double[vColumn];
	for (int i = 0; i < vRow; i++)
	{
		CWavelet Wavelet(*(vOriginalData + i), vColumn);
		*(RowTransformData + i) = Wavelet.transformToOneDimensionalWavelet();
	}
	double** TransposeRowTransformData = __matrixTranspose(RowTransformData, vRow, vColumn);
	SampleY= __vectorTranspose(SampleY);
	vector<vector<float>> ColumnWarpResult = __createMultiOneDimensionalHierarchicalWarping(SampleY,TransposeRowTransformData, vRow);
	for (int i = 0, SampleRowLength = vioSampleData.size();i<SampleRowLength; i++)
	{
		for (int k = 0,SampleColumnLength = vioSampleData[i].size(); k < SampleColumnLength; k++)
		{
			vioSampleData[i][k].x= RowWarpResult[i][k];
			vioSampleData[i][k].y= ColumnWarpResult[k][i];
		}
	}
	delete[]RowTransformData;
	delete[]TransposeRowTransformData;
	return vioSampleData;
}

vector<vector<float>> CHierarchicalSampleWarping::__createMultiOneDimensionalHierarchicalWarping(vector<vector<float>> vSampleData, double** vOriginalData, int vOriginalDataRowLength)
{
	vector<vector<float>> voResult;
	vector<float> CopySampleDataXY;
	for (int i = 0, SampleRowLength=vSampleData.size(); i < SampleRowLength; i++)
	{
		for (int k = 0,SampleColumnLength= vSampleData[i].size(); k <SampleColumnLength; k++)
			CopySampleDataXY.push_back(vSampleData[i][k]);
		CWavelet Wavelet(*(vOriginalData + i), vOriginalDataRowLength);
		double* WaveletTree = Wavelet.createOneDimensionalWaveletTree();
		voResult.push_back(__createOneDimensionalHierarchicalWarping(CopySampleDataXY, WaveletTree, 2 * vOriginalDataRowLength - 1));
		CopySampleDataXY.clear();
	}
	return voResult;
}

vector<vector<float>> CHierarchicalSampleWarping::__vectorTranspose(vector<vector<float>>& matrix)
{
	vector<vector<float>> voResult;
	for (int i = 0, ColumnLength = matrix[0].size(); i < ColumnLength; i++)
	{
		vector<float> TempStorage;
		for (int k = 0, RowLength = matrix.size(); k < RowLength; k++)
			TempStorage.push_back(matrix[k][i]);
		voResult.push_back(TempStorage);
	}
	return voResult;
}
