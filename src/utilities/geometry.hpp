#pragma once

#include <string>
#include <vector>
#include "floats.hpp"

class Face;
class Material;
class Mesh;

class Ray {
public:
	float3 origin;
	float3 direction;
	float3 color;
	float traveled;

	Ray(float3 const vdirection) :
		origin(0), direction(vdirection), color(0), traveled(0) {}
	Ray(float3 const vorigin, float3 const vdirection) :
		origin(vorigin), direction(vdirection), color(0), traveled(0) {}
	Ray(float3 const vorigin, float3 const vdirection, float3 const vcolor) :
		origin(vorigin), direction(vdirection), color(vcolor), traveled(0) {}
	Ray(float3 const vorigin, float3 const vdirection, float3 const vcolor, float const vtraveled) :
		origin(vorigin), direction(vdirection), color(vcolor), traveled(vtraveled) {}
};

class globalLight {
public:
	float3 direction;
	float3 colour;
	globalLight(float3 const vdirection, float3 const vcolour) : direction(vdirection), colour(vcolour) {}
};

class Material {
	public:
		static Material None;
		std::string name;
		float Ns;
		float3 Ka;
		float3 Kd;
		float3 Ks;
		float3 Ke;
		float Ni;
		float d;
		unsigned int illum;

		Material() : name("None"), Ns(100.0f), Ka(1.0f, 1.0f, 1.0f),
			Kd(0.5f, 0.5f, 0.5f), Ks(0.5f, 0.5f, 0.5f), Ke(0.0f, 0.0f, 0.0f),
			Ni(1.0), d(1.0), illum(2) {}

		Material(std::string vname) : name(vname), Ns(100.0f), Ka(1.0f, 1.0f, 1.0f),
			Kd(0.5f, 0.5f, 0.5f), Ks(0.5f, 0.5f, 0.5f), Ke(0.0f, 0.0f, 0.0f),
			Ni(1.0), d(1.0), illum(2) {}
};

class Face {
private:
	bool _gotWeights;
	float3 _weights;
	float3 _interpolatedNormal;
public:
	Mesh &parent;
	float4 &v0, &v1, &v2;
	float3 &t0, &t1, &t2;
	float3 &n0, &n1, &n2;

	Face(Mesh &vparent,float4 &vv0, float4 &vv1, float4 &vv2,
		 float3 &vt0, float3 &vt1, float3 &vt2,
	 	 float3 &vn0, float3 &vn1, float3 &vn2) : 	_gotWeights(false), _weights(0),
                                                    _interpolatedNormal(0),
		  											parent(vparent),
		 											v0(vv0), v1(vv1), v2(vv2),
		 									   		t0(vt0), t1(vt1), t2(vt2),
													n0(vn0), n1(vn1), n2(vn2) {}

	float getDepth(float3 const &weights) const;
	float getDepth(float const &u, float const &v, float const &w) const {
		return getDepth(float3(u,v,w));
	}

	float3 getNormal(float3 const &weights) const;
	float3 getNormal(float const &u, float const &v, float const &w) const {
		return getNormal(float3(u,v,w));
	}

	bool inRange(unsigned int const x, unsigned int const y, float &u, float &v, float &w);
	float getDepth(unsigned int const x, unsigned int const y);
};



class Mesh {
public:
	std::string name;
	std::vector<float4> vertices;
	std::vector<float3> textures;
	std::vector<float3> normals;
	Material material;

	bool hasNormals;
	bool hasTextures;

	Mesh(std::string vname) : name(vname), material(Material::None) {}

	unsigned long faceCount();
	Face getFace(unsigned long faceIndex);
	Mesh clone();
};
