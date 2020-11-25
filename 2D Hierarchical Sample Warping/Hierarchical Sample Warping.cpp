#include "Vector2.h"
#include"Wavelet.h"
#include"Hammersley2d.h"
#include"HierarchicalSampleWarping.h"
#include<vector>
#include<stdlib.h>
#include<algorithm>
#include <fstream> 
#include <string>
#include <iostream>
#include <streambuf> 
using namespace std;

int main()
{
	//�����ļ������ 
	ofstream oFile;
	oFile.open("scoresheet.csv", ios::out | ios::trunc);    //���һ��excel �ļ�
	vector<vector<float>> OriginalData;    //���������
	int OriginalDataRow = 32;
	int OriginalDataColumn = 32;
	for (int i = 0; i < OriginalDataRow; i++)
	{
		vector<float> TempStorage;
		for (int k = 0; k < OriginalDataColumn; k++)
		{
			int RandNumber = rand() % 11 + 10;
			TempStorage.push_back(RandNumber);
		}
		OriginalData.push_back(TempStorage);
	}
	for (int i = 0; i < 8; i++)
	{
		for (int k = 0; k < 8; k++)
		{
			int RandNumber = rand() % 11 + 240;
			OriginalData[i][k] = RandNumber;
		}
	}
	for (int i = 24; i < 32; i++)
	{
		for (int k = 24; k < 32; k++)
		{
			int RandNumber = rand() % 11 + 240;
			OriginalData[i][k] = RandNumber;
		}
	}
	for (int i = 0; i < OriginalDataRow; i++)
	{
		for (int k = 0; k < OriginalDataColumn; k++)
		{
			cout << OriginalData[i][k] << " ";
		}
		cout << endl;
	}

	CHammersley Hammersley;
	vector<vector<Vector2>> SamplePoints = Hammersley.createSamplePoints(64, 8);
	vector<vector<float>> SampleData = Hammersley.mappingOriginalData(SamplePoints, OriginalData);
	CHierarchicalSampleWarping HierarchicalSampleWarping;
	HierarchicalSampleWarping.warpPoints(SamplePoints, SampleData);
	for (int i = 0; i < SamplePoints.size(); i++)
	{
		for (int k = 0; k < SamplePoints[i].size(); k++)
		{
			oFile << SamplePoints[i][k].x << "," << SamplePoints[i][k].y << endl;
		}
	}
	oFile.close();
	return 0;
}