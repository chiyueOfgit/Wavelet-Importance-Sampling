#pragma once
#include "Vector2.h"
#include"Wavelet.h"
#include<vector>
#include <fstream> 
#include <string>
#include <iostream>
#include <streambuf> 
#include <iomanip>
#include<math.h>

class CHierarchicalSampleWarping
{
public:
	CHierarchicalSampleWarping();
	~CHierarchicalSampleWarping();
	void warpPoints(vector<vector<Vector2>>& vioSamplePoints, vector<vector<float>>& vSampleData);
	vector<vector<float>> mipMap(vector<vector<float>>& vOriginalData, int vLevel);
private:
	void __warpY(vector<vector<Vector2>>& vSamplePoints, float vXMinBorder, float vXMaxBorder, float vYMinBorder, float vYMaxborder, float vSpiltCoordinate);
	void __warpX(vector<vector<Vector2>>& vSamplePoints, float vXMinBorder, float vXMaxBorder, float vYMinBorder, float vYMaxborder, float vSpiltCoordinate);
	float __coefficient(float vA, float vB, float vC);
	float __scale(float vA, float vB, float vC);
	int __judgePower(int vNumber);
};

CHierarchicalSampleWarping::CHierarchicalSampleWarping()
{
}

CHierarchicalSampleWarping::~CHierarchicalSampleWarping()
{
}

vector<vector<float>> CHierarchicalSampleWarping::mipMap(vector<vector<float>>& vOriginalData, int vLevel)
{
	vector<vector<float>> voResult(vOriginalData);
	while (vLevel--)
	{
		vector<vector<float>> CopyOriginalData(voResult);
		int Row = CopyOriginalData.size();
		int Column = CopyOriginalData[0].size();
		voResult.clear();
		for (int i = 0; i < Row; i = i + 2)
		{
			vector<float> TempRow;
			for (int k = 0; k < Column; k = k + 2)
			{
				float Average = (CopyOriginalData[i][k] + CopyOriginalData[i][k + 1] + CopyOriginalData[i + 1][k + 1] + CopyOriginalData[i + 1][k + 1]) / 4;
				TempRow.push_back(Average);
			}
			voResult.push_back(TempRow);
		}
	}
	return voResult;
}

int CHierarchicalSampleWarping::__judgePower(int vNumber)
{
	int voCount = 0;
	while (vNumber >>= 1)
		voCount++;
	return voCount;
}

void CHierarchicalSampleWarping::warpPoints(vector<vector<Vector2>>& vioSamplePoints, vector<vector<float>>& vSampleData)
{
	int Level = __judgePower(vSampleData.size()) - 1;
	float Stride = 1;
	while (Level!=-1)
	{
		vector<vector<float>> LevelData = mipMap(vSampleData, Level);
		for (int i = 0; i < LevelData.size(); i = i + 2)
		{
			for (int k = 0; k < LevelData[i].size(); k = k + 2)
			{
				float vXMinBorder = (float)k / (float)LevelData[i].size();
				float vXMaxBorder = vXMinBorder + Stride;
				float vYMaxBorder = 1 - (float)i / (float)LevelData.size();
				float vYMinBorder = vYMaxBorder - Stride;
				float Sum = LevelData[i][k] + LevelData[i][k + 1] + LevelData[i + 1][k] + LevelData[i + 1][k + 1];
				float FrontProbaility = (LevelData[i][k] + LevelData[i][k + 1]) / Sum;
				float SpiltCoordinate = vYMaxBorder - FrontProbaility * Stride;
				__warpY(vioSamplePoints, vXMinBorder, vXMaxBorder, vYMinBorder, vYMaxBorder, SpiltCoordinate);
				vYMinBorder = (vYMaxBorder + vYMinBorder) / 2;
				Sum = LevelData[i][k] + LevelData[i][k + 1];
				FrontProbaility = LevelData[i][k + 1] / Sum;
				SpiltCoordinate = vXMaxBorder - FrontProbaility * Stride;
				__warpX(vioSamplePoints, vXMinBorder, vXMaxBorder, vYMinBorder, vYMaxBorder, SpiltCoordinate);
				vYMaxBorder = vYMinBorder;
				vYMinBorder = vYMaxBorder - Stride / 2;
				Sum = LevelData[i + 1][k] + LevelData[i + 1][k + 1];
				FrontProbaility = LevelData[i + 1][k + 1] / Sum;
				SpiltCoordinate = vXMaxBorder - FrontProbaility * Stride;
				__warpX(vioSamplePoints, vXMinBorder, vXMaxBorder, vYMinBorder, vYMaxBorder, SpiltCoordinate);
			}
		}
		Stride /= 2;
		Level--;
	}
}

float CHierarchicalSampleWarping::__coefficient(float vA, float vB, float vC)
{
	return (vA - vB) / (vA - vC);
}

float CHierarchicalSampleWarping::__scale(float vA, float vB, float vC)
{
	return vA - (vA - vB) * vC;
}

void CHierarchicalSampleWarping::__warpY(vector<vector<Vector2>>& vSamplePoints, float vXMinBorder, float vXMaxBorder, float vYMinBorder, float vYMaxborder, float vSpiltCoordinate)
{
	float Median = (vYMinBorder + vYMaxborder) / 2;
	for (int i = 0; i < vSamplePoints.size(); i++)
	{
		for (int k = 0; k < vSamplePoints[i].size(); k++)
		{
			if (vSamplePoints[i][k].x <= vXMaxBorder && vSamplePoints[i][k].x > vXMinBorder)
			{
				if (vSamplePoints[i][k].y >= vSpiltCoordinate && vSamplePoints[i][k].y < vYMaxborder)
				{
					float Coefficient = __coefficient(vYMaxborder, Median, vSpiltCoordinate);
					vSamplePoints[i][k].y = __scale(vYMaxborder, vSamplePoints[i][k].y, Coefficient);
				}
				else if (vSamplePoints[i][k].y < vSpiltCoordinate && vSamplePoints[i][k].y >= vYMinBorder)
				{
					float Coefficient = __coefficient(vYMinBorder, Median, vSpiltCoordinate);
					vSamplePoints[i][k].y = __scale(vYMinBorder, vSamplePoints[i][k].y, Coefficient);
				}
			}
		}
	}
}

void CHierarchicalSampleWarping::__warpX(vector<vector<Vector2>>& vSamplePoints, float vXMinBorder, float vXMaxBorder, float vYMinBorder, float vYMaxborder, float vSpiltCoordinate)
{
	float Median = (vXMinBorder + vXMaxBorder) / 2;
	for (int i = 0; i < vSamplePoints.size(); i++)
	{
		for (int k = 0; k < vSamplePoints[i].size(); k++)
		{
			if (vSamplePoints[i][k].y <= vYMaxborder && vSamplePoints[i][k].y > vYMinBorder)
			{
				if (vSamplePoints[i][k].x >= vSpiltCoordinate && vSamplePoints[i][k].x < vXMaxBorder)
				{
					float Coefficient = __coefficient(vXMaxBorder, Median, vSpiltCoordinate);
					vSamplePoints[i][k].x = __scale(vXMaxBorder, vSamplePoints[i][k].x, Coefficient);
				}
				else if (vSamplePoints[i][k].x < vSpiltCoordinate && vSamplePoints[i][k].x >= vXMinBorder)
				{
					float Coefficient = __coefficient(vXMinBorder, Median, vSpiltCoordinate);
					vSamplePoints[i][k].x = __scale(vXMinBorder, vSamplePoints[i][k].x, Coefficient);
				}
			}
		}
	}
}
