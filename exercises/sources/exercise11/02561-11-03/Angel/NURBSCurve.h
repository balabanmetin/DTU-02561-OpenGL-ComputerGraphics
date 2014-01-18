#ifndef _NURBS_CURVE_H
#define _NURBS_CURVE_H

#include <vector>
#include "Angel.h"
#include "NURBS.h"


/// NURBSCurve represents a NURBS curve.
/// Each curve object must be given a number of control points for each
/// point - the number of control points is specified
/// in the contructor.
/// The curve object also needs a knot vector in a non-decreasing order.
/// The size of the knot vector must be larger than than the number of 
/// control points.
/// The order of the curve is knot vector size minus the number 
/// of control points.
/// A tesselated curve is created using the methods getMeshData() and getMeshDataIndices()
/// An easy way to render the patch is using the NURBSRenderer class.
class NURBSCurve : public NURBS
{
public:
	NURBSCurve(int numberOfControlPoints, int discretization = 1024);
	~NURBSCurve();
	
	/// Set the controlPoint at index 
	void setControlPoint(int index, vec3 controlPoint);

	/// Set the controlPoint at index, the w-component of the controlpoint is the rational value
	void setControlPoint(int index, vec4 controlPoint);
	
	// set the knot vector (used for NonUniformBSplines and NonUniformRationalBSplines (NURBS)
	bool setKnotVector(int count, float const * knotVector);

	// return the number of control points
	int getNumberOfControlPoints();

	// Return the correct size of the knot vector
	int getKnotVectorSize();
	
	// return the order of the curve (degree + 1)
	int getOrder();

	// return the degree of the curve (knotVectorSize - numberOfControlPoints -1)
	int getDegree();

	std::vector<vec4> getControlPoints();

	std::vector<NURBSVertex> getMeshData();
	std::vector<GLuint> getMeshDataIndices();

	// evaluate the point based on u (between 0 and 1). Note that the v parameter is not used here.
	vec4 evaluate(float u, float v = 0);

	// For NURBSCurve always return line_strip
	GLenum getPrimitiveType();
private:
	int degree;
	int numberOfControlPoints;
	std::vector<float> knotVector;
	int discretization;

	vec4 *controlPoints;
};

#endif // _NURBS_CURVE_H
