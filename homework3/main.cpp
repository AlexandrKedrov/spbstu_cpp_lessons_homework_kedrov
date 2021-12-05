#include "vector.h"
#include <vector>
#include <string>
#include <memory>
#include <sstream>
#include <fstream>
#include <iostream>
#include <limits>


namespace
{

class ICollider
{
public:
	virtual Vector find_extreme_point(const Vector& direction) const = 0;
	virtual ~ICollider() = default;
};


class Polygon : public ICollider
{
	std::vector<Vector> _vertices;
public:
	Polygon(const std::vector<Vector>& vertices) : _vertices(vertices)
	{
	}

	Vector find_extreme_point(const Vector &direction) const override {
		double max_dot = std::numeric_limits<double>::lowest();
		Vector ret = {0};

		for (const Vector &point : _vertices) {
			double dot = point.dot(direction);

			if (dot > max_dot) {
				ret = point;
				max_dot = dot;
			}
		}

		return ret;
	}

	~Polygon() = default;
};

class Rectangle : public Polygon
{
public:
	Rectangle(const Vector& lu, const Vector& rl) :
		Polygon({lu, {rl.data[0], lu.data[1], 0}, rl, {lu.data[0], rl.data[1]} } )
	{
	}
};

class Circle : public ICollider
{
	double _radius;
	Vector _center;
public:
	Circle(double radius, const Vector& center) : _radius(radius), _center(center)
	{
	};

	Vector find_extreme_point(const Vector &direction) const override
	{
		return _center + _radius * direction.normalize();
	}

	~Circle() = default;
};

Vector ch_line(std::vector<Vector>& points)
{
	Vector a = points[1];
	Vector b = points[0];

	Vector ab = b - a;
	Vector ao = -a;

	if(ab.dot(ao) > 0)
	{
		return ab.cross(ao).cross(ab);
	}

	points = { points[1] };

	return ao;
};

std::pair<Vector, bool> ch_triangle(std::vector<Vector>& points)
{
	Vector a = points[2];
	Vector b = points[1];
	Vector c = points[0];

	Vector ab = b - a;
	Vector ac = c - a;
	Vector ao = -a;

	Vector abc = ab.cross(ac);

	if (abc.cross(ac).dot(ao) > 0) {
		if (ac.dot(ao) > 0) {
			points = {c, a};
			return std::make_pair(ac.cross(ao).cross(ac), false);
		}

		points = {b, a};
		return std::make_pair(ch_line(points), false);
	}

	if(ab.cross(abc).dot(ao) > 0)
	{
		points = {b, a};
		return std::make_pair(ch_line(points), false);
	}

	return std::make_pair((Vector){0}, true);
}

Vector find_support(const ICollider& col1, const ICollider& col2, const Vector& direction)
{
	return col1.find_extreme_point(direction) - col2.find_extreme_point(-direction);
}

bool gjk(const ICollider &col1, const ICollider &col2) {
	Vector support = find_support(col1, col2, {{1, 0, 0}});

	std::vector<Vector> simplex;
	simplex.push_back(support);

	Vector direction = -support;

	while(true)
	{
		support = find_support(col1, col2, direction);

		if(support.dot(direction) <= 0)
		{
			return false;
		}

		simplex.push_back(support);

		if(simplex.size() == 2)
		{
			direction = ch_line(simplex);
		}
		else
		{
			std::pair<Vector, bool> result = ch_triangle(simplex);

			if(result.second)
			{
				return true;
			}

			direction = result.first;
		}
	}
}
}

int main(int argc, char **argv) {
	std::string filename;
	if (argc == 2) {
		filename = argv[1];
	} else {
		return -1;
	}

	std::ifstream input(filename);

	std::string input_str;

	std::vector<std::unique_ptr<ICollider> > colliders;

	while (std::getline(input, input_str)) {
		if (input_str.find("POLYGON") != std::string::npos) {
			std::vector<Vector> vertices;

			while (std::getline(input, input_str)) {
				double x, y;
				std::istringstream strstream(input_str);
				strstream >> x >> y;

				if (!strstream) {
					break;
				}

				vertices.push_back({{x, y, 0}});
			}

			colliders.push_back(std::make_unique<Polygon>(vertices));
		}

		if (input_str.find("RECTAGNLE") != std::string::npos) {
			double lx, ly;
			double rx, ry;

			{
				std::getline(input, input_str);
				std::istringstream strstream(input_str);
				strstream >> lx >> ly;
			}

			{
				std::getline(input, input_str);
				std::istringstream strstream(input_str);
				strstream >> rx >> ry;
			}

			colliders.push_back(std::make_unique<Rectangle>((Vector){{lx, ly, 0}},
			                                                (Vector){{rx, ry, 0}}));
		} else if (input_str.find("CIRCLE") != std::string::npos) {
			double radius, x, y;

			{
				std::getline(input, input_str);
				std::istringstream strstream(input_str);
				strstream >> radius;
			}

			{
				std::getline(input, input_str);
				std::istringstream strstream(input_str);
				strstream >> x >> y;
			}

			colliders.push_back(
				std::make_unique<Circle>(radius, (Vector){{x, y, 0}}));
		}
	}

	for (int i = 0; i < colliders.size(); ++i) {
		for (int j = i + 1; j < colliders.size(); ++j) {
			if (gjk(*colliders[i], *colliders[j])) {
				std::cout << i << " " << j << std::endl;
			}
		}
	}

	return 0;
}