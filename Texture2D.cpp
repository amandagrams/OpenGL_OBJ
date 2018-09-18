#include "Texture2D.h"
#include <iostream>
#include <cassert>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"

//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
Texture2D::Texture2D()
	: mTexture(0)
{
}

//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------
Texture2D::~Texture2D()
{
	glDeleteTextures(1, &mTexture);
}

//-----------------------------------------------------------------------------
// Carrega uma textura com um determinado nome de arquivo usando o stb image loader
// http://nothings.org/stb_image.h
//-----------------------------------------------------------------------------
bool Texture2D::loadTexture(const string& fileName, bool generateMipMaps)
{
	int width, height, components;

	// Usa stbi image library para carregar nossa imagem
	unsigned char* imageData = stbi_load(fileName.c_str(), &width, &height, &components, STBI_rgb_alpha);

	if (imageData == NULL)
	{
		std::cerr << "Error loading texture '" << fileName << "'" << std::endl;
		return false;
	}

	// Inverte imagem
	int widthInBytes = width * 4;
	unsigned char *top = NULL;
	unsigned char *bottom = NULL;
	unsigned char temp = 0;
	int halfHeight = height / 2;
	for (int row = 0; row < halfHeight; row++)
	{
		top = imageData + row * widthInBytes;
		bottom = imageData + (height - row - 1) * widthInBytes;
		for (int col = 0; col < widthInBytes; col++)
		{ 
			temp = *top;
			*top = *bottom;
			*bottom = temp;
			top++;
			bottom++;
		}
	}

	glGenTextures(1, &mTexture);
	glBindTexture(GL_TEXTURE_2D, mTexture); // todas as próximas operações GL_TEXTURE_2D afetarão nosso objeto de textura (mTexture)

	// Defina as opções de envolvimento / filtragem de textura (no objeto de textura atualmente vinculado)
	// GL_CLAMP_TO_EDGE
	// GL_REPEAT
	// GL_MIRRORED_REPEAT
	// GL_CLAMP_TO_BORDER
	// GL_LINEAR
	// GL_NEAREST
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);

	if (generateMipMaps)
		glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(imageData);
	glBindTexture(GL_TEXTURE_2D, 0); 

	return true;
}

//-----------------------------------------------------------------------------
// Vincular a unidade de textura passada como a textura ativa no shader
//-----------------------------------------------------------------------------
void Texture2D::bind(GLuint texUnit)
{
	assert(texUnit >= 0 && texUnit < 32);

	glActiveTexture(GL_TEXTURE0 + texUnit);
	glBindTexture(GL_TEXTURE_2D, mTexture);
}

//-----------------------------------------------------------------------------
//  Desvincular a unidade de textura passada como a textura ativa no shader
//-----------------------------------------------------------------------------
void Texture2D::unbind(GLuint texUnit)
{
	glActiveTexture(GL_TEXTURE0 + texUnit);
	glBindTexture(GL_TEXTURE_2D, 0);
}
