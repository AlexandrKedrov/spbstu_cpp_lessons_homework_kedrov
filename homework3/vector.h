#pragma once

#include <array>
#include <cmath>

struct Vector
{
	std::array<double, 3> data;

	inline double dot(const Vector& vec) const
	{
		return data[0] * vec.data[0] + data[1] * vec.data[1] + data[2] * vec.data[2];
	}

	inline Vector cross(const Vector& vec) const
	{
		return { {
			data[1] * vec.data[2] - data[2] * vec.data[1],
			-data[0] * vec.data[2] + data[2] * vec.data[0],
			data[0] * vec.data[1] - data[1] * vec.data[0],
		} };
	}

	inline double length() const
	{
		return std::sqrt(dot(*this));
	}

	inline Vector operator+(const Vector& vec) const
	{
		return { { data[0] + vec.data[0], data[1] + vec.data[1], data[2] + vec.data[2] } };
	}

	inline Vector operator-(const Vector& vec) const
	{
		return { {data[0] - vec.data[0], data[1] - vec.data[1], data[2] - vec.data[2]} };
	}

	inline Vector operator-() const
	{
		return { { -data[0], -data[1], -data[2]} };
	}

	inline Vector normalize() const
	{
		const double len = length();
		return { { data[0] / len, data[1] / len, data[2] / len } };
	}
};

inline Vector operator*(const Vector& vec, double scale)
{
	return { { vec.data[0] * scale, vec.data[1] * scale, vec.data[2] * scale } };
}

inline Vector operator/(const Vector& vec, double factor)
{
	return { { vec.data[0] / factor, vec.data[1] / factor, vec.data[2] / factor } };
}

inline Vector operator*(double scale, const Vector& vec)
{
	return { { vec.data[0] * scale, vec.data[1] * scale, vec.data[2] * scale } };
}

inline Vector operator/(double factor, const Vector& vec)
{
	return { { vec.data[0] / factor, vec.data[1] / factor, vec.data[2] / factor } };
}