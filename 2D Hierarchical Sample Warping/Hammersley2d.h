#pragma once
#include"Vector2.h"
#include<vector>
#include<algorithm>

typedef unsigned int uint;
bool comp(const Vector2& vA, const Vector2& vB)
{
	return vA.x < vB.x;
}
class CHammersley
{
public:
	Vector2 Hammersley2d(uint i, uint N)
	{
		return Vector2(__radicalInverse(i), float(i) / float(N));
	}

	vector<vector<Vector2>> createSamplePoints(int vSampleNumber, int vRow)
	{
		vector<vector<Vector2>> voSamplePoints;
		vector<Vector2> RowSamplePoints;
		for (int i = 0; i < vSampleNumber; i++)
		{
			CHammersley Hammersley;
			auto UV = Hammersley.Hammersley2d(i, vSampleNumber);
			RowSamplePoints.push_back(UV);
			if ((i + 1) % vRow == 0)
			{
				sort(RowSamplePoints.begin(), RowSamplePoints.end(), comp);
				voSamplePoints.push_back(RowSamplePoints);
				RowSamplePoints.clear();
			}
		}
		return voSamplePoints;
	}

	vector<vector<float>> mappingOriginalData(vector<vector<Vector2>>& vSamplePoints, vector<vector<float>>& vOriginalData)
	{
		vector<vector<float>> voResult;
		int SampleRow = vSamplePoints.size();
		int SampleColumn = vSamplePoints[0].size();
		int OriginalDataRow = vOriginalData.size();
		int OriginalDataColumn = vOriginalData[0].size();
		for (int i = 0; i < SampleRow; i++)
		{
			vector<float> Temp;
			for (int k = 0; k < SampleColumn; k++)
			{
				int XCoordinate = (vSamplePoints[i][k].x < 1) ? (vSamplePoints[i][k].x * OriginalDataRow) : (OriginalDataRow - 1);
				int YCoordinate = (vSamplePoints[i][k].y < 1) ? (vSamplePoints[i][k].y * OriginalDataColumn) : (OriginalDataColumn - 1);
				Temp.push_back(vOriginalData[XCoordinate][YCoordinate]);
				cout << vOriginalData[XCoordinate][YCoordinate] << " ";
			}
			cout << endl;
			voResult.push_back(Temp);
		}
		return voResult;
	}
private:
	float __radicalInverse(uint vBits)
	{
		vBits = (vBits << 16u) | (vBits >> 16u);
		vBits = ((vBits & 0x55555555u) << 1u) | ((vBits & 0xAAAAAAAAu) >> 1u);
		vBits = ((vBits & 0x33333333u) << 2u) | ((vBits & 0xCCCCCCCCu) >> 2u);
		vBits = ((vBits & 0x0F0F0F0Fu) << 4u) | ((vBits & 0xF0F0F0F0u) >> 4u);
		vBits = ((vBits & 0x00FF00FFu) << 8u) | ((vBits & 0xFF00FF00u) >> 8u);
		return float(vBits) * 2.3283064365386963e-10; // / 0x100000000
	}
};