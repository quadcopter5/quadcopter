/*
	geometry.h

	Structs and functions related to geometry.
*/

#ifndef GEOMETRY_H
#define GEOMETRY_H

template<typename T>
struct Vector2 {
	T x, y;
};

template<typename T>
struct Vector3 {
	T x, y, z;
};

/**
	Calculates the magnitude of the given Vector
*/
int magnitude(Vector2<int> vector);
float magnitude(Vector2<float> vector);
double magnitude(Vector2<double> vector);

int magnitude(Vector3<int> vector);
float magnitude(Vector3<float> vector);
double magnitude(Vector3<double> vector);

#endif

