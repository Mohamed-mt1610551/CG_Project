#include "Texture.h"

#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"
#include <vector>

Texture::Texture()
	:mTexture(0)
{
}
Texture::~Texture()
{
	glDeleteTextures(1, &mTexture);
}

bool Texture::loadTexture(const string& fileName, bool generateMipMaps)
{
	int width, height, nrComponents;

	glGenTextures(1, &mTexture);   // <<<<< has to be before glTexImage2
	glBindTexture(GL_TEXTURE_2D, mTexture); // <<<< has to be before glTexImage2

	//set prameters for wrap
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//set parameters for filers
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//Load and create texture
	stbi_set_flip_vertically_on_load(true);
	unsigned char* texData = stbi_load(fileName.c_str(), &width, &height, &nrComponents, 0);
	if (!texData)
	{
		printf("Failing to find: %s\n", fileName.c_str());
	}

	GLenum format = 0;
	if (nrComponents == 1)
		format = GL_RED;
	else if (nrComponents == 3)
		format = GL_RGB;
	else if (nrComponents == 4)
		format = GL_RGBA;

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, format, GL_UNSIGNED_BYTE, texData);

	if (generateMipMaps)
		glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(texData);
	glBindTexture(GL_TEXTURE_2D, 0);

	return true;
}


unsigned int loadCubemap(vector<std::string> faces)
{
	int width, height, nrComponents;
	unsigned int textureID;
	nrComponents=0;

	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	GLenum format = 0; 
	if (nrComponents == 1) 
		format = GL_RED; 
	else if (nrComponents == 3) 
		format = GL_RGB; 
	else if (nrComponents == 4) 
		format = GL_RGBA;
	

	//Load and create cube map
	stbi_set_flip_vertically_on_load(true);

	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char* texData = stbi_load(faces[i].c_str(), &width, &height, &nrComponents, 0);
		if (texData)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, texData);
			stbi_image_free(texData); 
		}
		else
		{
			std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(texData); 
		}
	}



	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); 
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); 
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE); 
	

	

	
	

	return textureID;
}

void Texture::bind(GLuint texUnit)
{
	glActiveTexture(GL_TEXTURE0 + texUnit);
	glBindTexture(GL_TEXTURE_2D, mTexture);
}

void Texture::unbind(GLuint texUnit)
{
	glActiveTexture(GL_TEXTURE0 + texUnit);
	glBindTexture(GL_TEXTURE_2D, 0);
}