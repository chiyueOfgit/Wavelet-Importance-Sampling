#pragma once
#include<iostream>

using namespace std;

class Vector2
{
public:
	float x;
	float y;
	Vector2();
	Vector2(const float x, const float y);
	Vector2(const Vector2& vec);
	Vector2&  operator=(const Vector2& vec);
	~Vector2();
};

Vector2::Vector2() :x(0), y(0)
{
}

Vector2::Vector2(const float x, const float y) : x(x), y(y)
{
}

Vector2::Vector2(const Vector2& vec) : x(vec.x), y(vec.y)
{
}
Vector2& Vector2::operator=(const Vector2& vec)
{
	x = vec.x;
	y = vec.y;
	return *this;
}


Vector2::~Vector2()
{
}