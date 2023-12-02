#ifndef TEXTURE_H
#define TEXTURE_H

#define GLEW_STATIC
#include "GL/glew.h"
#include <string>
#include <vector>


using namespace std;

class Texture
{

public:
		Texture();
		virtual ~Texture();

		bool loadTexture(const string& fileName, bool generateMipMaps = true);
		void bind(GLuint texUnit = 0);
		void unbind(GLuint texUnit);
	
private:
	GLuint mTexture;
};

#endif //TEXTURE_H

