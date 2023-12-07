//-----------------------------------------------------------------------------
// Mesh.cpp
//
// Basic OBJ Mesh Importer class
//-----------------------------------------------------------------------------
#include "Mesh.h"
#include <iostream>
#include <sstream>
#include <fstream>


//-----------------------------------------------------------------------------
// split
//
// Params:  s - string to split
//		    t - string to split (ie. delimiter)
//
//Result:  Splits string according to some substring and returns it as a vector.
//-----------------------------------------------------------------------------
std::vector<std::string> split(std::string s, std::string t)
{
	std::vector<std::string> res;
	while(1)
	{
		int pos = s.find(t);
		if(pos == -1)
		{
			res.push_back(s); 
			break;
		}
		res.push_back(s.substr(0, pos));
		s = s.substr(pos+1, s.size() - pos - 1);
	}
	return res;
}


//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
Mesh::Mesh()
	:mLoaded(false)
{
}

//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------
Mesh::~Mesh()
{
	glDeleteVertexArrays(1, &mVAO);
	glDeleteBuffers(1, &mVBO);
}

//-----------------------------------------------------------------------------
// Loads a Wavefront OBJ model
//
// NOTE: This is not a complete, full featured OBJ loader.  It is greatly
// simplified.
// Assumptions!
//  - OBJ file must contain only triangles
//  - We ignore materials
//  - We ignore normals
//  - only commands "v", "vt" and "f" are supported
//-----------------------------------------------------------------------------
bool Mesh::loadOBJ(const std::string& filename)
{
	std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
	std::vector<glm::vec3> tempVertices;
	std::vector<glm::vec2> tempUVs;
	std::vector<glm::vec3> tempNormals;


	if (filename.find(".obj") != std::string::npos)
	{
		std::ifstream fin(filename, std::ios::in);
		if (!fin)
		{
			std::cerr << "Cannot open " << filename << std::endl;
			return false;
		}

		std::cout << "Loading OBJ file " << filename << " ..." << std::endl;

		std::string lineBuffer;
		while (std::getline(fin, lineBuffer))
		{
			std::stringstream ss(lineBuffer);
			std::string cmd;
			ss >> cmd;

			if (cmd == "v")
			{
				glm::vec3 vertex;
				int dim = 0;
				while (dim < 3 && ss >> vertex[dim])
					dim++;

				tempVertices.push_back(vertex);
			}
			else if (cmd == "vt")
			{
				glm::vec2 uv;
				int dim = 0;
				while (dim < 2 && ss >> uv[dim])
					dim++;
				
				tempUVs.push_back(uv);
			}
			else if (cmd == "vn")
			{
				glm::vec3 normal;
				int dim = 0;
				while (dim < 3 && ss >> normal[dim])
					dim++;
				normal = glm::normalize(normal);
				tempNormals.push_back(normal);
			}
			else if (cmd == "f")
			{
				std::string faceData;
				std::vector<int> faceVertexIndices, faceUVIndices, faceNormalIndices;

				while (ss >> faceData)
				{
					std::vector<std::string> data = split(faceData, "/");

					if (!data[0].empty())
					{
						int vertexIndex = std::stoi(data[0]);
						faceVertexIndices.push_back(vertexIndex);
					}

		
					if (data.size() >= 2 && !data[1].empty()) // UVs
					{
						int uvIndex = std::stoi(data[1]);
						faceUVIndices.push_back(uvIndex);
					}

					if (data.size() == 3 && !data[2].empty()) // Normals
					{
						int normalIndex = std::stoi(data[2]);
						faceNormalIndices.push_back(normalIndex);
					}
				}

				// If face is a quad (4 vertices), split it into two triangles
				if (faceVertexIndices.size() == 4)
				{
					// Triangle 1
					vertexIndices.push_back(faceVertexIndices[0]);
					vertexIndices.push_back(faceVertexIndices[1]);
					vertexIndices.push_back(faceVertexIndices[2]);
					uvIndices.push_back(faceUVIndices[0]);
					uvIndices.push_back(faceUVIndices[1]);
					uvIndices.push_back(faceUVIndices[2]);
					normalIndices.push_back(faceNormalIndices[0]);
					normalIndices.push_back(faceNormalIndices[1]);
					normalIndices.push_back(faceNormalIndices[2]);

					// Triangle 2
					vertexIndices.push_back(faceVertexIndices[0]);
					vertexIndices.push_back(faceVertexIndices[2]);
					vertexIndices.push_back(faceVertexIndices[3]);
					uvIndices.push_back(faceUVIndices[0]);
					uvIndices.push_back(faceUVIndices[2]);
					uvIndices.push_back(faceUVIndices[3]);
					normalIndices.push_back(faceNormalIndices[0]);
					normalIndices.push_back(faceNormalIndices[2]);
					normalIndices.push_back(faceNormalIndices[3]);
				}
				else if (faceVertexIndices.size() == 3) // Normal triangle
				{
					vertexIndices.insert(vertexIndices.end(), faceVertexIndices.begin(), faceVertexIndices.end());
					uvIndices.insert(uvIndices.end(), faceUVIndices.begin(), faceUVIndices.end());
					normalIndices.insert(normalIndices.end(), faceNormalIndices.begin(), faceNormalIndices.end());
				}
	
			}
		}

		// Close the file
		fin.close();


		// For each vertex of each triangle
		for (unsigned int i = 0; i < vertexIndices.size(); i++)
		{
			Vertex meshVertex;

			// Get the attributes using the indices

			if (tempVertices.size() > 0)
			{
				glm::vec3 vertex = tempVertices[vertexIndices[i] - 1];
				meshVertex.position = vertex;
			}

			if (tempNormals.size() > 0)
			{
				glm::vec3 normal = tempNormals[normalIndices[i] - 1];
				meshVertex.normal = normal;
			}

			if (tempUVs.size() > 0)
			{
				glm::vec2 uv = tempUVs[uvIndices[i] - 1];
				meshVertex.texCoords = uv;
			}

			mVertices.push_back(meshVertex);
		}

		// Create and initialize the buffers
		initBuffers();

		return (mLoaded = true);
	}

	// We shouldn't get here so return failure
	return false;
}

//-----------------------------------------------------------------------------
// Create and initialize the vertex buffer and vertex array object
// Must have valid, non-empty std::vector of Vertex objects.
//-----------------------------------------------------------------------------
void Mesh::initBuffers()
{
	glGenVertexArrays(1, &mVAO);
	glGenBuffers(1, &mVBO);

	glBindVertexArray(mVAO);
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(Vertex), &mVertices[0], GL_STATIC_DRAW);

	// Vertex Positions
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// Normals attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// Vertex Texture Coords
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	
	// unbind to make sure other code does not change it somewhere else
	glBindVertexArray(0);
}

//-----------------------------------------------------------------------------
// Render the mesh
//-----------------------------------------------------------------------------
void Mesh::draw()
{
	if (!mLoaded) return;

	glBindVertexArray(mVAO);
	glDrawArrays(GL_TRIANGLES, 0, mVertices.size());
	glBindVertexArray(0);
}

