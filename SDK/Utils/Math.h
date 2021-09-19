#pragma once
#include <vector>
#include <array>
#include "SPoint.h"
#include "..\SDK\Vector.h"
#include "..\SDK\VMatrix.h"
#include "..\SDK\IVEngineClient.h"
#include "..\\SDK\CGlobalVarsBase.h"
class CMath
{
public:
	float GRD_TO_BOG(float GRD);
	void VectorTransform(const Vector in1, matrix3x4_t in2, Vector* out);
	void NormalizeAngles(QAngle& angles);
	Vector ExtrapolateTick(Vector p0, Vector v0);
	void NormalizeVector(Vector& vec);
	float NormalizeFloat(float angle);
	void ClampAngles(QAngle& angles);
	Vector CrossProduct(const Vector& a, const Vector& b);
private:
	void AngleVectors(const QAngle& angles, Vector& forward);
	void AngleVectors(const QAngle& angles, Vector& forward, Vector& right, Vector& up);
	void VectorAngles(const Vector& forward, QAngle& angles);
	void VectorAngles(const Vector& forward, Vector& up, QAngle& angles);
public:
	bool screen_transform(const Vector& in, Vector& out);
	bool WorldToScreen(const Vector& in, Vector& out);
	void SinCos(float a, float* s, float* c);
	QAngle CalcAngle(Vector src, Vector dst);
	void AngleMatrix(const QAngle& angles, matrix3x4_t& matrix);
	void MatrixSetColumn(const Vector& in, int column, matrix3x4_t& out);
	void AngleMatrix(const QAngle& angles, const Vector& position, matrix3x4_t& matrix);
	float DegreesToRadians(const float deg);
	float RadiansToDegrees(const float rad);
	void RotateTriangle(std::array<SPoint, 3>& points, float rotation);
	void VectorRotate(const Vector& in1, const QAngle& in2, Vector& out);
	void VectorRotate(const Vector& in1, const matrix3x4_t& in2, Vector& out);
	void VectorRotate(const float* in1, const matrix3x4_t& in2, float* out);
	void MatrixMultiply(matrix3x4_t& in1, const matrix3x4_t& in2);
	void MatrixCopy(const matrix3x4_t& source, matrix3x4_t& target);
	float DotProduct(float* v1, float* v2);
	float DotProduct(Vector v1, Vector v2);
	float Lerp(float start, float end, float speed);
	QAngle VectorToAngle(Vector);
	Vector AngleToVector(QAngle);
	template<typename t> t Clamp(t val, t min, t max)
	{
		if(val < min)
			return min;
		else if(val > max)
			return max;
		return val;
	};
};

extern CMath Math;