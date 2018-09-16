#pragma once

#include <cmath>
#include <ostream>
#include <algorithm>

class float2 {
public:
	float x;
	float y;

	float2() : x(0.0f), y(0.0f) {}
	float2(float const vx, float const vy) : x(vx), y(vy) {}

	template <class T>
	float2(T val) : x(val), y(val) {}

	float2& operator= (float2 const other) {
		x = other.x;
		y = other.y;
		return *this;
	}

	template <class T>
	float2& operator=(T const other) {
		*this = float2(other);
		return *this;
	}

	float2& operator+= (float2 const other) {
		x += other.x;
		y += other.y;
		return *this;
	}

	float2& operator-= (float2 const other) {
		x -= other.x;
		y -= other.y;
		return *this;
	}

	float2& operator*= (float2 const other) {
		x *= other.x;
		y *= other.y;
		return *this;
	}

	float2& operator/= (float2 const other) {
		x /= other.x;
		y /= other.y;
		return *this;
	}

	friend float2 operator+ (float2 lhs, float2 const &rhs) { lhs += rhs; return lhs; }
	friend float2 operator- (float2 lhs, float2 const &rhs) { lhs -= rhs; return lhs; }
	friend float2 operator* (float2 lhs, float2 const &rhs) { lhs *= rhs; return lhs; }
	friend float2 operator/ (float2 lhs, float2 const &rhs) { lhs /= rhs; return lhs; }

	bool operator== (float2 const other) const {
		return (x == other.x) && (y == other.y);
	}

	bool operator!= (float2 const other) const {
		return (x != other.x) || (y != other.y);
	}

	float2 clamp(float2 const &lo, float2 const &hi) {
		return float2(
			std::max(std::min(x, hi.x),lo.x),
			std::max(std::min(y, hi.y),lo.y)
		);
	}

	friend std::ostream & operator << (std::ostream &os, float2 const &v) {
        os << "[" << v.x << "," << v.y << "]";
        return os;
    }
};


class float3 {
public:
	float x;
	float y;
	float z;

	float3() : x(0.0f), y(0.0f), z(0.0f) {}
	float3(float2 const v, float vz) : x(v.x), y(v.y), z(vz) {}
	float3(float const vx, float const vy, float const vz) : x(vx), y(vy), z(vz) {}

	template <class T>
	float3(T val) : x(val), y(val), z(val) {}

	float3& operator= (float3 const other) {
		this->x = other.x;
		this->y = other.y;
		this->z = other.z;
		return *this;
	}

	template <class T>
	float3& operator=(T other) {
		*this = float3(other);
		return *this;
	}

	float3& operator+= (float3 const other) {
		x += other.x;
		y += other.y;
		z += other.z;
		return *this;
	}

	float3& operator-= (float3 const other) {
		x -= other.x;
		y -= other.y;
		z -= other.z;
		return *this;
	}

	float3& operator*= (float3 const other) {
		x *= other.x;
		y *= other.y;
		z *= other.z;
		return *this;
	}

	float3& operator/= (float3 const other) {
	    x /= other.x;
        y /= other.y;
        z /= other.z;
        return *this;
	}

	friend float3 operator+ (float3 lhs, float3 const &rhs) { lhs += rhs; return lhs; }
	friend float3 operator- (float3 lhs, float3 const &rhs) { lhs -= rhs; return lhs; }
	friend float3 operator* (float3 lhs, float3 const &rhs) { lhs *= rhs; return lhs; }
	friend float3 operator/ (float3 lhs, float3 const &rhs) { lhs /= rhs; return lhs; }

	float3 cross(float3 const &v) const {
		return float3(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
	}

	float dot(float3 const &v) const {
		return (x * v.x + y * v.y + z * v.z);
	}

	float distance(float3 const &v) const {
		return std::sqrt(std::pow(x - v.x, 2) + std::pow(y - v.y, 2) + std::pow(z - v.z,2));
	}

	float3& normalize() {
        float n = x * x + y * y + z * z;
        if (n > 0) {
			(*this) *= 1 / std::sqrt(n);
        }
        return *this;
    }

	bool operator== (float3 const other) const {
		return
			(x == other.x) &&
			(y == other.y) &&
			(z == other.z);
	}

	bool operator!= (float3 const other) const {
		return
			(x != other.x) ||
			(y != other.y) ||
			(z != other.z);
	}

	float3 clamp(float3 const &lo, float3 const &hi) {
		return float3(
			std::max(std::min(x, hi.x),lo.x),
			std::max(std::min(y, hi.y),lo.y),
			std::max(std::min(z, hi.z),lo.z)
		);
	}

	float2 toFloat2() const {
		return float2(x,y);
	}

	friend std::ostream & operator << (std::ostream &os, float3 const &v) {
        os << "[" << v.x << "," << v.y << "," << v.z << "]";
        return os;
    }
};

class float4 {
public:
	float x;
	float y;
	float z;
	float w;

	float4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
	float4(float3 const v, float vw) : x(v.x), y(v.y), z(v.z), w(vw) {}
	float4(float const vx, float const vy, float const vz, float const vw) : x(vx), y(vy), z(vz), w(vw) {}

	template <class T>
	float4(T val) : x(val), y(val), z(val), w(val) {}

	float4& operator= (float4 other) {
		x = other.x;
		y = other.y;
		z = other.z;
		w = other.w;
		return *this;
	}

	template <class T>
	float4& operator=(T other) {
		*this = float4(other);
		return *this;
	}

	float4& operator+= (float4 other) {
		x += other.x;
		y += other.y;
		z += other.z;
		w += other.w;
		return *this;
	}

	float4& operator-= (float4 other) {
		x -= other.x;
		y -= other.y;
		z -= other.z;
		w -= other.w;
		return *this;
	}

	float4& operator*= (float4 other) {
		x *= other.x;
		y *= other.y;
		z *= other.z;
		w *= other.w;
		return *this;
	}

	float4& operator/= (float4 other) {
		x /= other.x;
		y /= other.y;
		z /= other.z;
		w /= other.w;
		return *this;
	}

	float4 clamp(float4 const &lo, float4 const &hi) {
		return float4(
			std::max(std::min(x, hi.x),lo.x),
			std::max(std::min(y, hi.y),lo.y),
			std::max(std::min(z, hi.z),lo.z),
			std::max(std::min(w, hi.w),lo.w)
		);
	}

	friend float4 operator+ (float4 lhs, float4 const &rhs) { lhs += rhs; return lhs; }
	friend float4 operator- (float4 lhs, float4 const &rhs) { lhs -= rhs; return lhs; }
	friend float4 operator* (float4 lhs, float4 const &rhs) { lhs *= rhs; return lhs; }
	friend float4 operator/ (float4 lhs, float4 const &rhs) { lhs /= rhs; return lhs; }

	bool operator!= (float4 other) {
		return (x != other.x) || (y != other.y) || (z != other.z) || (w != other.w);
	}

	bool operator== (float4 other) {
		return (x == other.x) && (y == other.y) && (z == other.z) && (w == other.w);
	}

	float3 toFloat3() {
		return float3(x,y,z);
	}

	friend std::ostream & operator << (std::ostream &os, const float4 &v)
	{
		os << "[" << v.x << "," << v.y << "," << v.z << "," << v.w << "]";
		return os;
	}
};

class mat4x4 {
public:
	// Format: m<row><column>
	float m00, m01, m02, m03;
	float m10, m11, m12, m13;
	float m20, m21, m22, m23;
	float m30, m31, m32, m33;

	constexpr mat4x4(	float v00, float v01, float v02, float v03,
			float v10, float v11, float v12, float v13,
			float v20, float v21, float v22, float v23,
			float v30, float v31, float v32, float v33) :
		m00(v00), m01(v01), m02(v02), m03(v03),
		m10(v10), m11(v11), m12(v12), m13(v13),
		m20(v20), m21(v21), m22(v22), m23(v23),
		m30(v30), m31(v31), m32(v32), m33(v33) {};

	constexpr mat4x4() :
		m00(0), m01(0), m02(0), m03(0),
		m10(0), m11(0), m12(0), m13(0),
		m20(0), m21(0), m22(0), m23(0),
		m30(0), m31(0), m32(0), m33(0) {}

	constexpr mat4x4 operator* (mat4x4 const other) const {
		return mat4x4(
			other.m00 * m00 + other.m10 * m01 + other.m20 * m02 + other.m30 * m03,
			other.m01 * m00 + other.m11 * m01 + other.m21 * m02 + other.m31 * m03,
			other.m02 * m00 + other.m12 * m01 + other.m22 * m02 + other.m32 * m03,
			other.m03 * m00 + other.m13 * m01 + other.m23 * m02 + other.m33 * m03,
			other.m00 * m10 + other.m10 * m11 + other.m20 * m12 + other.m30 * m13,
			other.m01 * m10 + other.m11 * m11 + other.m21 * m12 + other.m31 * m13,
			other.m02 * m10 + other.m12 * m11 + other.m22 * m12 + other.m32 * m13,
			other.m03 * m10 + other.m13 * m11 + other.m23 * m12 + other.m33 * m13,
			other.m00 * m20 + other.m10 * m21 + other.m20 * m22 + other.m30 * m23,
			other.m01 * m20 + other.m11 * m21 + other.m21 * m22 + other.m31 * m23,
			other.m02 * m20 + other.m12 * m21 + other.m22 * m22 + other.m32 * m23,
			other.m03 * m20 + other.m13 * m21 + other.m23 * m22 + other.m33 * m23,
			other.m00 * m30 + other.m10 * m31 + other.m20 * m32 + other.m30 * m33,
			other.m01 * m30 + other.m11 * m31 + other.m21 * m32 + other.m31 * m33,
			other.m02 * m30 + other.m12 * m31 + other.m22 * m32 + other.m32 * m33,
			other.m03 * m30 + other.m13 * m31 + other.m23 * m32 + other.m33 * m33
		);
	}

	mat4x4& operator*= (mat4x4 const other) {
		this->m00 = other.m00 * this->m00 + other.m10 * this->m01 + other.m20 * this->m02 + other.m30 * this->m03;
		this->m01 = other.m01 * this->m00 + other.m11 * this->m01 + other.m21 * this->m02 + other.m31 * this->m03;
		this->m02 = other.m02 * this->m00 + other.m12 * this->m01 + other.m22 * this->m02 + other.m32 * this->m03;
		this->m03 = other.m03 * this->m00 + other.m13 * this->m01 + other.m23 * this->m02 + other.m33 * this->m03;
		this->m10 = other.m00 * this->m10 + other.m10 * this->m11 + other.m20 * this->m12 + other.m30 * this->m13;
		this->m11 = other.m01 * this->m10 + other.m11 * this->m11 + other.m21 * this->m12 + other.m31 * this->m13;
		this->m12 = other.m02 * this->m10 + other.m12 * this->m11 + other.m22 * this->m12 + other.m32 * this->m13;
		this->m13 = other.m03 * this->m10 + other.m13 * this->m11 + other.m23 * this->m12 + other.m33 * this->m13;
		this->m20 = other.m00 * this->m20 + other.m10 * this->m21 + other.m20 * this->m22 + other.m30 * this->m23;
		this->m21 = other.m01 * this->m20 + other.m11 * this->m21 + other.m21 * this->m22 + other.m31 * this->m23;
		this->m22 = other.m02 * this->m20 + other.m12 * this->m21 + other.m22 * this->m22 + other.m32 * this->m23;
		this->m23 = other.m03 * this->m20 + other.m13 * this->m21 + other.m23 * this->m22 + other.m33 * this->m23;
		this->m30 = other.m00 * this->m30 + other.m10 * this->m31 + other.m20 * this->m32 + other.m30 * this->m33;
		this->m31 = other.m01 * this->m30 + other.m11 * this->m31 + other.m21 * this->m32 + other.m31 * this->m33;
		this->m32 = other.m02 * this->m30 + other.m12 * this->m31 + other.m22 * this->m32 + other.m32 * this->m33;
		this->m33 = other.m03 * this->m30 + other.m13 * this->m31 + other.m23 * this->m32 + other.m33 * this->m33;
		return *this;
	}

	float4 operator* (float4 const &other) const {
		return float4(
			this->m00 * other.x + this->m01 * other.y + this->m02 * other.z + this->m03 * other.w,
			this->m10 * other.x + this->m11 * other.y + this->m12 * other.z + this->m13 * other.w,
			this->m20 * other.x + this->m21 * other.y + this->m22 * other.z + this->m23 * other.w,
			this->m30 * other.x + this->m31 * other.y + this->m32 * other.z + this->m33 * other.w
		);
	}
};


template <class T>
T clamp(T const &val, T const &lo, T const &hi) {
	return std::min(std::max(val, lo),hi);
}
