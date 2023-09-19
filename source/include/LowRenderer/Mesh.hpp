#pragma once

#include <glad/glad.h>

#include <matrix.hpp>
#include <vector>

#include <Material.hpp>
#include <Log.hpp>
#include <Shader.hpp>

struct Vertex
{
	Vectorf3 Position;
	Vectorf2 Uv;
	Vectorf3 Normal;
};

class Mesh
{
private:
	unsigned int VAO = 0, VBO = 0, EBO = 0;
	std::vector<Vertex> m_Vertices;
	std::vector<unsigned int> m_Indices;
	Matrix4x4 local = Matrix4x4(true);

public:
	Mesh() {};
	Mesh(const std::vector<Vertex>& _temp_Vertices, const std::vector<uint32_t>& _temp_idx_Positions, const std::vector<uint32_t>& _temp_idx_Uvs, const std::vector<uint32_t>& _temp_idx_Normals);
	~Mesh();

	void SetVertices(const std::vector<Vertex>& _Vertices);
	void SetIndices(const std::vector<unsigned int>& _Indices);

	void SetupMesh();
	void Draw();
};