#pragma once

#include "pch.h"

struct based_vector
{
	XMFLOAT3 pos;
	XMFLOAT3 angle;
};

float distance(const XMFLOAT3& a, const XMFLOAT3& b)
{
	return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2) + pow(a.z - b.z, 2));
}
bool can_see(const based_vector& a, const vector<XMFLOAT3>& b, float max_range);
bool can_see(const based_vector& a, const XMFLOAT3& b, float max_range);