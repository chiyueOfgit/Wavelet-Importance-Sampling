#pragma once
#include"Vector2.h"
#include<vector>

typedef unsigned int uint;
class CHammersley
{
public:
	Vector2 Hammersley2d(uint i, uint N)
	{
		return Vector2(float(i) / float(N), __radicalInverse(i));
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