#include "Vector2.h"
#include"Wavelet.h"
#include"HierarchicalSampleWarping.h"
#include"Hammersley2d.h"
#include<iostream>
#include<vector>

using namespace std;

int main()
{
	vector<vector<Vector2>> Array;
	vector<Vector2> RowArray1;
	vector<Vector2> RowArray2;
	vector<Vector2> RowArray3;
	vector<Vector2> RowArray4;
	for (int i = 0; i < 32; i++)
	{
		CHammersley Hammersley;
		auto UV = Hammersley.Hammersley2d(i,32);
		if (UV.y >= 0 && UV.y < 0.25)
			RowArray1.push_back(UV);
		else if (UV.y >= 0.25 && UV.y < 0.5)
			RowArray2.push_back(UV);
		else if (UV.y >= 0.5 && UV.y < 0.75)
			RowArray3.push_back(UV);
		else
			RowArray4.push_back(UV);
		cout << "UV:" << "(" << UV.x <<","<< UV.y << ")" << endl;
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
	Data[7][7] = 10; Data[7][6] = 10; Data[6][7] = 10; Data[6][6] = 10;
	for (int i = 0; i < 8; i++)
	{
		for (int k = 0; k < 8; k++)
		{
			cout << Data[i][k] << " ";
		}
		cout << endl;
	}
	CHierarchicalSampleWarping HierarchicalSampleWarping;
	vector<vector<Vector2>> Result = HierarchicalSampleWarping.createTwoDimensionalHierarchicalWarping(Array, Data, 8, 8);
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


