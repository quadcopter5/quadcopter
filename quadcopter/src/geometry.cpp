/*
	geometry.cpp

	Structs and functions related to geometry.
*/

#include <math.h>
#include "geometry.h"

// int magnitude(Vector2<int> vector) { }

float magnitude(Vector2<float> vector) {
	return sqrtf(powf(vector.x, 2.0f) + powf(vector.y, 2.0f));
}

double magnitude(Vector2<double> vector) {
	return sqrt(pow(vector.x, 2.0) + pow(vector.y, 2.0));
}

// int magnitude(Vector3<int> vector) { }

float magnitude(Vector3<float> vector) {
	return sqrtf(powf(vector.x, 2.0f)
			+ powf(vector.y, 2.0f)
			+ powf(vector.z, 2.0f));
}

double magnitude(Vector3<double> vector) {
	return sqrt(pow(vector.x, 2.0)
			+ pow(vector.y, 2.0)
			+ pow(vector.z, 2.0));
}

