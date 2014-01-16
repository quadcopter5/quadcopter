/*
	geometry.h

	Structs and functions related to geometry.
*/

#ifndef GEOMETRY_H
#define GEOMETRY_H

#define PI 3.1415926535

// Returns the sign of the given number 
// (+1 / 0 / -1)
template<typename T>
int sign(T val) {
	return (T(0) < val) - (val < T(0));
}

template<typename T>
struct Vector2 {
	T x, y;

	Vector2() { }

	Vector2(T nx, T ny) : x(nx), y(ny)
			{ }

	void operator+=(Vector2<T> other) {
		x += other.x;
		y += other.y;
	}
};

template<typename T>
struct Vector3 {
	T x, y, z;

	Vector3() { }

	Vector3(T nx, T ny, T nz) : x(nx), y(ny), z(nz)
			{ }

	void operator+=(Vector3<T> other) {
		x += other.x;
		y += other.y;
		z += other.z;
	}
};

/**
	Calculates the magnitude of the given Vector
*/
//int magnitude(Vector2<int> vector);
float magnitude(Vector2<float> vector);
double magnitude(Vector2<double> vector);

//int magnitude(Vector3<int> vector);
float magnitude(Vector3<float> vector);
double magnitude(Vector3<double> vector);

#endif

