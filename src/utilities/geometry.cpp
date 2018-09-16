#include "geometry.hpp"

Material Material::None = Material();

unsigned long Mesh::faceCount() {
	return (this->vertices.size() / 3);
}

Face Mesh::getFace(unsigned long faceIndex) {
	faceIndex *= 3;
	return Face(*this, vertices[faceIndex + 0], vertices[faceIndex + 1], vertices[faceIndex + 2],
				textures[faceIndex + 0], textures[faceIndex + 1], textures[faceIndex + 2],
				normals[faceIndex + 0], normals[faceIndex + 1], normals[faceIndex + 2]);
}

Mesh Mesh::clone() {
	Mesh clonedMesh(this->name);

	clonedMesh.vertices = this->vertices;
	clonedMesh.textures = this->textures;
	clonedMesh.normals = this->normals;
	
	clonedMesh.material = this->material;

	clonedMesh.hasNormals = this->hasNormals;
	clonedMesh.hasTextures = this->hasTextures;

	return clonedMesh;
}


bool Face::inRange(unsigned int const x, unsigned int const y, float &u, float &v, float &w) {
		u = (((v1.y - v2.y) * (x    - v2.x)) + ((v2.x - v1.x) * (y    - v2.y))) /
				 	 (((v1.y - v2.y) * (v0.x - v2.x)) + ((v2.x - v1.x) * (v0.y - v2.y)));
		if (u < 0) {
			return false;
		}
		v = (((v2.y - v0.y) * (x    - v2.x)) + ((v0.x - v2.x) * (y    - v2.y))) /
					(((v1.y - v2.y) * (v0.x - v2.x)) + ((v2.x - v1.x) * (v0.y - v2.y)));
		if (v < 0) {
			return false;
		}
		w = 1 - u - v;
		if (w < 0) {
			return false;
		}
		return true;
}

float3 Face::getNormal(float3 const &weights) const {
	return float3((n0 * weights.x) + (n1 * weights.y) + (n2 * weights.z)).normalize();
}

float Face::getDepth(float3 const &weights) const {
	return weights.x * v0.z + weights.y * v1.z + weights.z * v2.z;
}