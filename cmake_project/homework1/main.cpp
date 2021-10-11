#include <array>
#include <cmath>
#include <fstream>
#include <functional>
#include <iostream>
#include <iterator>
#include <type_traits>

namespace
{
// first two args stand for xn, yn
// next two args stand for xi, yi
template <typename T>
using Comparator = std::function<T(T, T, T, T)>;

template <typename Iterator>
std::array<typename std::iterator_traits<Iterator>::value_type, 4>
findLeftRightMost(
	Iterator begin, Iterator end,
	const Comparator<typename std::iterator_traits<Iterator>::value_type>&
	comparator) {
	using IteratorType = typename std::iterator_traits<Iterator>::value_type;
	static_assert(std::is_floating_point<IteratorType>::value,
	              "Iterator type is not a floating point type");

	Iterator it = begin;
	const IteratorType xn = *(it++);
	const IteratorType yn = *(it++);

	const IteratorType length = std::sqrt(xn * xn + yn * yn);
	const IteratorType xnNormalized = xn / length;
	const IteratorType ynNormalized = yn / length;

	// We expect max and min to be opposite signs so both initially equal to zero
	IteratorType maxValue = 0; // stands for rightest
	IteratorType minValue = 0; // stands for leftest

	IteratorType xMax, yMax, xMin, yMin;

	while (it != end) {
		const IteratorType xValue = *(it++);
		const IteratorType yValue = *(it++);

		const IteratorType value =
			comparator(xnNormalized, ynNormalized, xValue, yValue);

		if (value > maxValue) {
			maxValue = value;
			xMax = xValue;
			yMax = yValue;
		} else if (value < minValue) {
			minValue = value;
			xMin = xValue;
			yMin = yValue;
		}
	}

	return {xMax, yMax, xMin, yMin};
}

float compareByAngle(float xn, float yn, float x, float y) {
	const float length = std::sqrt(x * x + y * y);
	const float xNormalized = x / length;
	const float yNormalized = y / length;

	// use cross product to find sin of angle
	// positive if counter-clockwise, negative if clockwise rotation
	// so flip the sign in the end
	const float sin = xn * yNormalized - yn * xNormalized;
	// dot product for cos of angle
	const float cos = xn * yNormalized + yn * yNormalized;
	// blunt angles are right and left most so substract 1

	return -sin >= 0.f ? -(cos - 1.f) : cos - 1.f;
}

float compareByDistance(float xn, float yn, float x, float y) {
	// Use cross product to find area
	// As direction normalized it will be equal to distance from direction to
	// point positive if counter-clockwise, negative if clockwise rotation so flip
	// the sign in the end
	const float area = xn * y - yn * x;

	return -area;
}
}

int main() {
	std::ifstream inputFile("in.txt");
	std::istream_iterator<float> begin(inputFile);
	std::istream_iterator<float> end;

	const std::array<float, 4> result = findLeftRightMost(begin, end, compareByAngle);
	std::cout << "Leftmost: " << result.at(2) << " " << result.at(3) << std::endl;
	std::cout << "Rightmost: " << result.at(0) << " " << result.at(1) << std::endl;

	return 0;
}