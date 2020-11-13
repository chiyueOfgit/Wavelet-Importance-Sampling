#include<iostream>
#include <math.h>
#include<vector>
#include"Vector2.h"

using namespace std;

typedef unsigned int uint;
double* oneDimensionalWaveletTransform(double* vOriginalData, int vSize);
double* oneDimensionalWaveletTree(double* vLeafData, int vSize);
vector<float> oneDimensionalHierarchicalWarping(vector<float> vSampleData, double* vWaveletTree, int vTreeSize);
vector<vector<Vector2>> twoDimensionalHierarchicalWarping(vector<vector<Vector2>> vSampleData, double** vOriginalData, int vRow, int vColumn);
double** matrixTranspose(double** vOrigin, int vRow, int vColumn);
float __radicalInverse(uint vBits);
Vector2 __hammersley2d(uint i, uint N);

int main()
{
	vector<vector<Vector2>> Array;
	vector<Vector2> RowArray1;
	vector<Vector2> RowArray2;
	vector<Vector2> RowArray3;
	vector<Vector2> RowArray4;
	for (int i = 0; i < 32; i++)
	{
		auto UV = __hammersley2d(i, 32);
		if (UV.y >= 0 && UV.y < 0.25)
			RowArray1.push_back(UV);
		else if (UV.y >= 0.25 && UV.y < 0.5)
			RowArray2.push_back(UV);
		else if (UV.y >= 0.5 && UV.y < 0.75)
			RowArray3.push_back(UV);
		else
			RowArray4.push_back(UV);
	}
	Array.push_back(RowArray1);
	Array.push_back(RowArray2);
	Array.push_back(RowArray3);
	Array.push_back(RowArray4);
	double** Data;
	Data = new double* [8];
	for (int i = 0; i < 8; i++)
	{
		*(Data + i) = new double[8];
	}
	for (int i = 0; i < 8; i++)
	{
		for (int k = 0; k < 8; k++)
		{
			Data[i][k] = 1;
		}
	}
	Data[0][0] = 10; Data[0][1] = 10; Data[1][0] = 10; Data[1][1] = 10;
	for (int i = 0; i < 8; i++)
	{
		for (int k = 0; k < 8; k++)
		{
			cout << Data[i][k] << " ";
		}
		cout << endl;
	}
	vector<vector<Vector2>> Result = twoDimensionalHierarchicalWarping(Array, Data, 8, 8);
	cout << "Result:" << endl;
	for (int i = 0; i < Result.size(); i++)
	{
		for (int k = 0; k < Result[i].size(); k++)
		{
			cout << "(" << Result[i][k].x << "," << Result[i][k].y << ")" << endl;
		}
	}
	delete[]Data;
	return 0;
}

vector<vector<Vector2>> twoDimensionalHierarchicalWarping(vector<vector<Vector2>> vSampleData, double** vOriginalData, int vRow, int vColumn)
{
	vector<vector<Vector2>> voResult;
	for (int i = 0; i < vSampleData.size(); i++)
	{
		vector<Vector2> TempAssign;
		for (int k = 0; k < vSampleData[i].size(); k++)
		{
			Vector2 UV(0, 0);
			TempAssign.push_back(UV);
		}
		voResult.push_back(TempAssign);
	}
	vector<vector<float>> TemporaryStorage;
	vector<float> CopySampleDataXY;
	//行扭曲
	for (int i = 0; i < vSampleData.size(); i++)
	{
		for (int k = 0; k < vSampleData[i].size(); k++)
			CopySampleDataXY.push_back(vSampleData[i][k].x);
		double* WaveletTree = oneDimensionalWaveletTree(*(vOriginalData + i), vColumn);
		TemporaryStorage.push_back(oneDimensionalHierarchicalWarping(CopySampleDataXY, WaveletTree, 2 * vColumn - 1));
		CopySampleDataXY.clear();
	}
	for (int i = 0; i < vSampleData.size(); i++)
		for (int k = 0; k < vSampleData[i].size(); k++)
			voResult[i][k].x = TemporaryStorage[i][k];
	TemporaryStorage.clear();
	//列扭曲,此时应使用行变换后的矩阵
	double** RowTransformData = new double* [vRow];
	for (int i = 0; i < vColumn; i++)
		*(RowTransformData + i) = new double[vRow];
	for (int i = 0; i < vRow; i++)
		*(RowTransformData + i) = oneDimensionalWaveletTransform(*(vOriginalData + i), vColumn);
	double** CopyOriginalData = matrixTranspose(RowTransformData, vRow, vColumn);
	for (int i = 0; i < vSampleData[0].size(); i++)//此处使用第一行的长度代表列长度，故要求扭曲样本每一行长度相等
	{
		for (int k = 0; k < vSampleData.size(); k++)
			CopySampleDataXY.push_back(vSampleData[k][i].y);
		double* WaveletTree = oneDimensionalWaveletTree(*(CopyOriginalData + i), vColumn);
		TemporaryStorage.push_back(oneDimensionalHierarchicalWarping(CopySampleDataXY, WaveletTree, 2 * vColumn - 1));
		CopySampleDataXY.clear();
	}
	for (int i = 0; i < vSampleData[0].size(); i++)
		for (int k = 0; k < vSampleData.size(); k++)
			voResult[k][i].y = TemporaryStorage[i][k];
	delete[]CopyOriginalData;
	delete[]RowTransformData;
	return voResult;
}

double** matrixTranspose(double** vOrigin, int vRow, int vColumn)
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

double* oneDimensionalWaveletTransform(double* vOriginalData, int vSize)
{
	if ((vSize & (vSize - 1)) != 0)//vSize扩展至2的次方，空位补0; 
	{
		for (int i = 0;; i++)
		{
			if (pow(2, i) > vSize)
			{
				double* pBeginLocal;
				pBeginLocal = vOriginalData;
				for (int k = vSize; k < pow(2, i); i++)
				{
					*(pBeginLocal + k) = 0;
				}
				vSize = (int)pow(2, i);
				break;
			}
		}
	}
	double* voResultOfWaveletTransform = new double[vSize];
	while (vSize != 1)
	{
		int LevelLength = vSize / 2;
		for (int i = 0; i < LevelLength; i++)
		{
			*(voResultOfWaveletTransform + i) = (*(vOriginalData + 2 * i) + *(vOriginalData + 2 * i + 1)) / 2;
			*(voResultOfWaveletTransform + i + LevelLength) = *(vOriginalData + 2 * i) - *(voResultOfWaveletTransform + i);
			*(vOriginalData + i) = *(voResultOfWaveletTransform + i);
		}
		vSize /= 2;
	}
	return voResultOfWaveletTransform;
}

double* oneDimensionalWaveletTree(double* vLeafData, int vSize)
{
	double* ResultOfWaveletTransform = new double[vSize];
	double* voWaveletTree = new double[vSize * 2 - 1];
	for (int i = 0; i < vSize; i++)
	{
		*(voWaveletTree + i) = *(vLeafData + i);
	}
	int Level = vSize;
	while (Level != 1)
	{
		int LevelLength = Level / 2;
		for (int i = 0; i < LevelLength; i++)
		{
			*(ResultOfWaveletTransform + i) = (*(vLeafData + 2 * i) + *(vLeafData + 2 * i + 1)) / 2;
			*(ResultOfWaveletTransform + i + LevelLength) = *(vLeafData + 2 * i) - *(ResultOfWaveletTransform + i);
			*(vLeafData + i) = *(ResultOfWaveletTransform + i);
			*(voWaveletTree + vSize) = *(ResultOfWaveletTransform + i);
			vSize++;
		}
		Level /= 2;
	}
	delete[]ResultOfWaveletTransform;
	return voWaveletTree;
}

vector<float> oneDimensionalHierarchicalWarping(vector<float> vSampleData, double* vWaveletTree, int vTreeSize)
{
	vWaveletTree = vWaveletTree + vTreeSize - 1;
	int Length = (vTreeSize + 1) / 2;
	int TraversalLength = 0;
	double Stride = 1;
	int HandleFrequency = 1;
	while (TraversalLength != (vTreeSize - 1))
	{
		TraversalLength += HandleFrequency * 2;
		double LeftBorder = 0;
		double RightBorder = Stride;
		for (int i = 0; i < HandleFrequency; i++)
		{
			int LevelTranslation = TraversalLength - 2 * i;
			double sum = *(vWaveletTree - LevelTranslation + 1) + *(vWaveletTree - LevelTranslation);
			double FrontProbaility = *(vWaveletTree - LevelTranslation) / sum;
			double FrontCoordinate = FrontProbaility * Stride + LeftBorder;
			double Median = (LeftBorder + RightBorder) / 2;
			for (int k = 0; k < vSampleData.size(); k++)//缩放
			{
				if (vSampleData[k] < FrontCoordinate && vSampleData[k] >= LeftBorder)
				{
					double scale = (Median - LeftBorder) / (FrontCoordinate - LeftBorder);
					vSampleData[k] = LeftBorder + (vSampleData[k] - LeftBorder) * scale;
				}
				else if (vSampleData[k] >= FrontCoordinate && vSampleData[k] <= RightBorder)
				{
					double scale = (RightBorder - Median) / (RightBorder - FrontCoordinate);
					vSampleData[k] = RightBorder - (RightBorder - vSampleData[k]) * scale;
				}
			}
			LeftBorder += Stride;
			RightBorder += Stride;
		}
		Stride /= 2;
		HandleFrequency *= 2;
	}
	return vSampleData;
}

float __radicalInverse(uint vBits)
{
	vBits = (vBits << 16u) | (vBits >> 16u);
	vBits = ((vBits & 0x55555555u) << 1u) | ((vBits & 0xAAAAAAAAu) >> 1u);
	vBits = ((vBits & 0x33333333u) << 2u) | ((vBits & 0xCCCCCCCCu) >> 2u);
	vBits = ((vBits & 0x0F0F0F0Fu) << 4u) | ((vBits & 0xF0F0F0F0u) >> 4u);
	vBits = ((vBits & 0x00FF00FFu) << 8u) | ((vBits & 0xFF00FF00u) >> 8u);
	return float(vBits) * 2.3283064365386963e-10; // / 0x100000000
}

Vector2 __hammersley2d(uint i, uint N)
{
	return Vector2(float(i) / float(N), __radicalInverse(i));
}


