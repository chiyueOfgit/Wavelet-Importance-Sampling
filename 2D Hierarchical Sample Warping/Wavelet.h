#pragma once
#include<math.h>

class CWavelet
{
public:
	CWavelet(double* vOriginalData, int vDataSize);
	~CWavelet();
	int getDataSize() const;
	double* transformToOneDimensionalWavelet();
	double* createOneDimensionalWaveletTree();

private:
	double* m_pOriginalData;
	int m_DataSize;
};

CWavelet::CWavelet(double* vOriginalData, int vDataSize)
{
	if ((vDataSize & (vDataSize - 1)) != 0)//vDataSize扩展至2的次方，空位补0; 
	{
		for (int i = 0;; i++)
		{
			if (pow(2, i) > vDataSize)
			{
				m_pOriginalData = new double[pow(2, i)];
				for (int k = 0; k < pow(2, i); k++)
				{
					if (k < vDataSize)
						*(m_pOriginalData + k) = *(vOriginalData + k);
					else
						*(m_pOriginalData + k) = 0;
				}
				m_DataSize = pow(2, i);
				break;
			}
		}
	}
	else
	{
		m_pOriginalData = vOriginalData;
		m_DataSize = vDataSize;
	}
}

CWavelet::~CWavelet()
{
}

int CWavelet::getDataSize() const
{
	return m_DataSize;
}

double* CWavelet::transformToOneDimensionalWavelet()
{
	int CopyDataSize = m_DataSize;
	double* pCopyOriginalData = new double[CopyDataSize];
	for (int i = 0; i < CopyDataSize; i++)
	{
		*(pCopyOriginalData + i) = *(m_pOriginalData + i);//复制源数组，避免改变源数组内容
	}
	double* voResultOfWaveletTransform = new double[CopyDataSize];
	while (CopyDataSize != 1)
	{
		int LevelLength = CopyDataSize / 2;
		for (int i = 0; i < LevelLength; i++)
		{
			*(voResultOfWaveletTransform + i) = (*(pCopyOriginalData + 2 * i) + *(pCopyOriginalData + 2 * i + 1)) / 2;
			*(voResultOfWaveletTransform + i + LevelLength) = *(pCopyOriginalData + 2 * i) - *(voResultOfWaveletTransform + i);
			*(pCopyOriginalData + i) = *(voResultOfWaveletTransform + i);
		}
		CopyDataSize /= 2;
	}
	delete []pCopyOriginalData;
	return voResultOfWaveletTransform;
}

double* CWavelet::createOneDimensionalWaveletTree()
{
	int CopyDataSize = m_DataSize;
	double* pCopyOriginalData = new double[CopyDataSize];
	double* ResultOfWaveletTransform = new double[CopyDataSize];
	double* voWaveletTree = new double[CopyDataSize * 2 - 1];
	for (int i = 0; i < CopyDataSize; i++)
	{
		*(voWaveletTree + i) = *(m_pOriginalData + i);
		*(pCopyOriginalData+i)= *(m_pOriginalData + i);
	}
	int Level = m_DataSize;
	while (Level != 1)
	{
		int LevelLength = Level / 2;
		for (int i = 0; i < LevelLength; i++)
		{
			*(ResultOfWaveletTransform + i) = (*(pCopyOriginalData + 2 * i) + *(pCopyOriginalData + 2 * i + 1)) / 2;
			*(ResultOfWaveletTransform + i + LevelLength) = *(pCopyOriginalData + 2 * i) - *(ResultOfWaveletTransform + i);
			*(pCopyOriginalData + i) = *(ResultOfWaveletTransform + i);
			*(voWaveletTree + CopyDataSize) = *(ResultOfWaveletTransform + i);
			CopyDataSize++;
		}
		Level /= 2;
	}
	delete[]pCopyOriginalData;
	delete[]ResultOfWaveletTransform;
	return voWaveletTree;
}
