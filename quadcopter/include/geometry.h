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

	Vector2(const Vector2 &other) : x(other.x), y(other.y)
			{ }

	void operator+=(Vector2<T> other) {
		x += other.x;
		y += other.y;
	}

	void operator-=(Vector2<T> other) {
		x -= other.x;
		y -= other.y;
	}

	Vector2<T> operator-() {
		return Vector2<T>(-x, -y);
	}

	Vector2<T> operator+(Vector2<T> other) {
		return Vector2<T>(x + other.x, y + other.y);
	}

	Vector2<T> operator-(Vector2<T> other) {
		return (*this) + (-other);
	}

	Vector2<T> operator*(T scalar) {
		return Vector2<T>(x * scalar, y * scalar);
	}

	Vector2<T> operator/(T scalar) {
		return Vector2<T>(x / scalar, y / scalar);
	}
};

template<typename T>
struct Vector3 {
	T x, y, z;

	Vector3() { }

	Vector3(T nx, T ny, T nz) : x(nx), y(ny), z(nz)
			{ }

	Vector3(const Vector3 &other) : x(other.x), y(other.y), z(other.z)
			{ }

	void operator+=(Vector3<T> other) {
		x += other.x;
		y += other.y;
		z += other.z;
	}

	void operator-=(Vector3<T> other) {
		x -= other.x;
		y -= other.y;
		z -= other.z;
	}

	Vector3<T> operator-() {
		return Vector3<T>(-x, -y, -z);
	}

	Vector3<T> operator+(Vector3<T> other) {
		return Vector3<T>(x + other.x, y + other.y, z + other.z);
	}

	Vector3<T> operator-(Vector3<T> other) {
		return (*this) + (-other);
	}

	Vector3<T> operator*(T scalar) {
		return Vector2<T>(x * scalar, y * scalar, z * scalar);
	}

	Vector3<T> operator/(T scalar) {
		return Vector2<T>(x / scalar, y / scalar, z / scalar);
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

