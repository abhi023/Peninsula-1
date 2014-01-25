#include "pch.h"
#include "peninsula.h"

void stopwatch::start()
{
	startpoint = high_resolution_clock::now();
}

void stopwatch::stop()
{
	endpoint = high_resolution_clock::now();
}
double stopwatch::difference()
{
	return (duration_cast<duration<double>>(endpoint - startpoint)).count();
}