#pragma once

#ifndef __VECTOR_H__
#define __VECTOR_H__

#include <cmath>
#include <iostream>

using namespace std;

template <class t> struct Vec2 {
	union {
		struct { t u, v; };
		struct { t x, y; };
		t raw[2];
	};
	Vec2() : u(0), v(0) {}
	Vec2(t _u, t _v) : u(_u), v(_v) {}
	inline t& operator [] (int idx) { return raw[idx]; }
	inline Vec2<t> operator +(const Vec2<t>& V) const { return Vec2<t>(u + V.u, v + V.v); }
	inline Vec2<t> operator -(const Vec2<t>& V) const { return Vec2<t>(u - V.u, v - V.v); }
	inline Vec2<t> operator *(float f)          const { return Vec2<t>(u * f, v * f); }
	template <class > friend ostream& operator<<(ostream& s, Vec2<t>& v);
};

template <class t> struct Vec3 {
	union {
		struct { t x, y, z; };
		struct { t ivert, iuv, inorm; };
		t raw[3];
	};
	Vec3() : x(0), y(0), z(0) {}
	Vec3(t _x, t _y, t _z) : x(_x), y(_y), z(_z) {}
	inline t& operator [] (int idx) { return raw[idx]; }
	inline Vec3<t> operator ^(const Vec3<t>& v) const { return Vec3<t>(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x); }
	inline Vec3<t> operator +(const Vec3<t>& v) const { return Vec3<t>(x + v.x, y + v.y, z + v.z); }
	inline Vec3<t> operator -(const Vec3<t>& v) const { return Vec3<t>(x - v.x, y - v.y, z - v.z); }
	inline Vec3<t> operator *(float f)          const { return Vec3<t>(x * f, y * f, z * f); }
	inline t       operator *(const Vec3<t>& v) const { return x * v.x + y * v.y + z * v.z; }
	float norm() const { return sqrt(x * x + y * y + z * z); }
	Vec3<t>& normalize(t l = 1) { *this = (*this) * (l / norm()); return *this; }
	template <class > friend ostream& operator<<(ostream& s, Vec3<t>& v);
};

template <class t> struct Vec4 {
	union {
		struct { t x, y, z, w; };
		struct { t r, g, b, a; };
		t raw[4];
	};
	Vec4() : x(0), y(0), z(0), w(0) {}
	Vec4(t _x, t _y, t _z, t _w) : x(_x), y(_y), z(_z), w(_w) {}
	Vec4(Vec3<t> vec3) : x(vec3.x), y(vec3.y), z(vec3.z), w(1) {}
	inline t& operator [] (int idx) { return raw[idx]; }
	inline Vec4<t> operator +(const Vec4<t>& v) const { return Vec4<t>(x + v.x, y + v.y, z + v.z, w + v.w); }
	inline Vec4<t> operator -(const Vec4<t>& v) const { return Vec4<t>(x - v.x, y - v.y, z - v.z, w - v.w); }
	inline Vec4<t> operator *(float f)          const { return Vec4<t>(x * f, y * f, z * f, w * f); }
	inline t       operator *(const Vec4<t>& v) const { return x * v.x + y * v.y + z * v.z + w * v.w; }
	inline Vec3<t> toVec3() { return Vec3<t>(x, y, z); }
	Vec3<t> normalize() { return Vec3<float>(x / w, y / w, z / w); }
	template <class > friend ostream& operator<<(ostream& s, Vec4<t>& v);
};

typedef Vec2<float> Vec2f;
typedef Vec2<int>   Vec2i;
typedef Vec3<float> Vec3f;
typedef Vec3<int>   Vec3i;
typedef Vec4<float> Vec4f;
typedef Vec4<int>   Vec4i;


template <class t> ostream& operator<<(ostream& s, Vec2<t>& v) {
	s << "(" << v.x << ", " << v.y << ")\n";
	return s;
}

template <class t> ostream& operator<<(ostream& s, Vec3<t>& v) {
	s << "(" << v.x << ", " << v.y << ", " << v.z << ")\n";
	return s;
}

template <class t> ostream& operator<<(ostream& s, Vec4<t>& v) {
	s << "(" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ")\n";
	return s;
}

struct Matrix4x4 {
	Vec4f raw[4];
	void reset() {
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				raw[i][j] = (i == j ? 1.f : 0.f);
			}
		}
	}
	Matrix4x4() { reset(); }
	Matrix4x4(float num) {
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				raw[i][j] = num;
			}
		}
	}
	inline Vec4f& operator [] (int idx) { return raw[idx]; }
	Vec4f& operator * (Vec4f v) {
		Vec4f ans;
		for (int i = 0; i < 4; i++) {
			ans[i] = raw[i] * v;
		}
		return ans;
	}
	Matrix4x4& operator * (Matrix4x4 mat) {
		Matrix4x4 ans(0);
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				for (int k = 0; k < 4; k++) {
					ans[i][j] += raw[i][k] * mat[k][j];
				}
			}
		}
		return ans;
	}
};

struct Matrix3x3 {
	Vec3f raw[3];
	void reset() {
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				raw[i][j] = (i == j ? 1.f : 0.f);
			}
		}
	}
	Matrix3x3() { reset(); }
	Matrix3x3(float num) {
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				raw[i][j] = num;
			}
		}
	}
	Matrix3x3(Matrix4x4 mat) {
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				raw[i][j] = mat[i][j];
			}
		}
	}
	inline Vec3f& operator [] (int idx) { return raw[idx]; }
	Vec3f& operator * (Vec3f v) {
		Vec3f ans;
		for (int i = 0; i < 3; i++) {
			ans[i] = raw[i] * v;
		}
		return ans;
	}
	Matrix3x3& operator * (Matrix3x3 mat) {
		Matrix3x3 ans(0);
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				for (int k = 0; k < 3; k++) {
					ans[i][j] += raw[i][k] * mat[k][j];
				}
			}
		}
		return ans;
	}
};

static Vec4f& operator * (Vec4f v, Matrix4x4 m) {
	Vec4f out;
	for (int i = 0;i < 4;++i) {
		for (int k = 0;k < 4;++k) {
			out[i] += v[k] * m[k][i];
		}
	}
	return out;
}
static Vec3f& operator * (Vec3f v, Matrix3x3 m) {
	Vec3f out;
	for (int i = 0;i < 3;++i) {
		for (int k = 0;k < 3;++k) {
			out[i] += v[k] * m[k][i];
		}
	}
	return out;
}

#endif