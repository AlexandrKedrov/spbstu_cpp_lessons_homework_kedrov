#include <vector>
#include <array>
#include <cmath>
#include <fstream>
#include <functional>
#include <iostream>
#include <iterator>
#include <type_traits>

namespace
{

const double epsilon = 1e-15;

// first two args stand for xn, yn
// next two args stand for xi, yi
template <typename T>
using Comparator = std::function<T(T xn, T yn, T x, T y)>;

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

	IteratorType xMax = 0, yMax = 0, xMin = 0, yMin = 0;

	while (it != end) {
		const IteratorType xValue = *(it++);
		const IteratorType yValue = *(it++);

		const IteratorType value =
			comparator(xnNormalized, ynNormalized, xValue, yValue);
		const IteratorType maxDif = value - maxValue;
		const IteratorType minDif = value - minValue;

		if (std::fabs(maxDif) < epsilon || maxDif > 0) {
			maxValue = value;
			xMax = xValue;
			yMax = yValue;
		} else if (std::fabs(minDif) < epsilon || minDif < 0) {
			minValue = value;
			xMin = xValue;
			yMin = yValue;
		}
	}

	return {xMax, yMax, xMin, yMin};
}

template <typename T> 
T compareByAngle(T xn, T yn, T x, T y) {
  const T length = std::sqrt(x * x + y * y);
  const T xNormalized = x / length;
  const T yNormalized = y / length;

  // use cross product to find sin of angle
  // positive if counter-clockwise, negative if clockwise rotation
  // so flip the sign in the end
  const T sin = xn * yNormalized - yn * xNormalized;
  // dot product for cos of angle
  const T cos = xn * xNormalized + yn * yNormalized;
  // blunt angles are right and left most so substract 1

  return std::fabs(sin) < epsilon || -sin > 0 ? -(cos - 1.1) : cos - 1.1;
}

template <typename T> 
T compareByDistance(T xn, T yn, T x, T y) {
  // Use cross product to find area
  // As direction normalized it will be equal to distance from direction to
  // point positive if counter-clockwise, negative if clockwise rotation so flip
  // the sign in the end
  const T area = xn * y - yn * x;

  return -area;
}
}

int main() {
	std::ifstream inputFile("in.txt");

	std::istream_iterator<double> begin(inputFile);
	std::istream_iterator<double> end;

	const std::array<double, 4> result = findLeftRightMost(begin, end, compareByAngle<double>);
	std::cout << "Leftmost: " << std::lround(result.at(2)) << " " << std::lround(result.at(3)) << std::endl;
	std::cout << "Rightmost: " << std::lround(result.at(0)) << " " << std::lround(result.at(1)) << std::endl;

	return 0;
}