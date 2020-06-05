#include "TextureManager.h"
#include <SOIL/SOIL.h>
#include <GLCommon.h>
#if __cplusplus >= 201703L
	#include <filesystem>
#endif
#include <TextureSkybox.h>

TextureManager::TextureManager()
{
}

TextureManager::~TextureManager()
{
}

const bool TextureManager::Create2DTexture(ITexture* texture, const bool& generateMIP)
{
	if(texture)
	{
		unsigned int id = 0;
		bool rc = Create2DTexture(texture->path, id, generateMIP);
		if (rc)
		{
			texture->SetID(id);
		}

		return rc;
	}

	return false;
}

const bool TextureManager::CreateCubeMapTexture(ITexture* texture_, const bool& generateMIP)
{
	TextureSkybox* texture = dynamic_cast<TextureSkybox*>(texture_);
	if (texture)
	{
		unsigned int id = 0;
		bool rc = CreateCubeMapTexture(
			texture->GetName(),
			texture->GetPositiveX(),
			texture->GetNegativeX(),
			texture->GetPositiveY(),
			texture->GetNegativeY(),
			texture->GetPositiveZ(),
			texture->GetNegativeZ(),
			id,
			generateMIP);
		if (rc)
		{
			texture->SetID(id);
		}

		return rc;
	}

	return false;
}

const bool TextureManager::Create2DTexture(const std::string& path, unsigned int& id, const bool& generateMIP)
{
#if __cplusplus >= 201703L
	if (!std::filesystem::exists(path))
	{
		char err[255];
		sprintf_s(err, 255, "The texture does not exist at the path %s.", path.c_str());
		SetLastErrorString(err);
		return false;
	}
#endif

	id = 0;

	if (generateMIP)
	{
		id = SOIL_load_OGL_texture(	path.c_str(),
									SOIL_LOAD_AUTO,
									SOIL_CREATE_NEW_ID,
									SOIL_FLAG_POWER_OF_TWO
									| SOIL_FLAG_MIPMAPS
									//| SOIL_FLAG_MULTIPLY_ALPHA
									//| SOIL_FLAG_COMPRESS_TO_DXT
									| SOIL_FLAG_DDS_LOAD_DIRECT
									//| SOIL_FLAG_NTSC_SAFE_RGB
									//| SOIL_FLAG_CoCg_Y
									//| SOIL_FLAG_TEXTURE_RECTANGLE
								);
	}
	else
	{
		id = SOIL_load_OGL_texture(path.c_str(),
			SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID,
			SOIL_FLAG_POWER_OF_TWO
			/*| SOIL_FLAG_MIPMAPS*/
			//| SOIL_FLAG_MULTIPLY_ALPHA
			//| SOIL_FLAG_COMPRESS_TO_DXT
			| SOIL_FLAG_DDS_LOAD_DIRECT
			//| SOIL_FLAG_NTSC_SAFE_RGB
			//| SOIL_FLAG_CoCg_Y
			//| SOIL_FLAG_TEXTURE_RECTANGLE
		);
	}

	if (id != 0)
	{
		textureMap[path] = id;
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, id);

		return true;
	}

	return false;
}

const bool TextureManager::CreateCubeMapTexture(const std::string& name, const std::string& posX_Path, const std::string& negX_Path, const std::string& posY_Path, const std::string& negY_Path, const std::string& posZ_Path, const std::string& negZ_Path, unsigned int& id, const bool& generateMIP)
{
	std::vector<std::string> paths;
	paths.push_back(posX_Path);
	paths.push_back(negX_Path);
	paths.push_back(posY_Path);
	paths.push_back(negY_Path);
	paths.push_back(posZ_Path);
	paths.push_back(negZ_Path);
	
	return CreateCubeMapTexture(name, paths, id, generateMIP);;
}

const bool TextureManager::CreateCubeMapTexture(const std::string& name, const std::vector<std::string> paths, unsigned int& id, const bool& generateMIP)
{
	if (paths.size() != 6)
	{
		return false;
	}

#if __cplusplus >= 201703L
	for (std::string path : paths)
	{
		if (!std::filesystem::exists(path))
		{
			char err[255];
			sprintf_s(err, 255, "The cubemap texture does not exist at the path %s.", path.c_str());
			SetLastErrorString(err);
			return false;
		}
	}
#endif

	int texture_widths[6] = {0, 0, 0, 0, 0, 0};
	int textureheights[6] = { 0, 0, 0, 0, 0, 0 };
	int channels[6] = { 0, 0, 0, 0, 0, 0 };
	GLubyte* data[6] = { 0, 0, 0, 0, 0, 0 };
	id = 0;

	for (size_t i = 0; i < paths.size(); i++)
	{
		data[i] = SOIL_load_image(paths[i].c_str(), &texture_widths[i], &textureheights[i], &channels[i], SOIL_LOAD_AUTO);
	}

	glGenTextures(1, &id);
	/*glActiveTexture(GL_TEXTURE);*/
	glBindTexture(GL_TEXTURE_CUBE_MAP, id);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	GLint format = (channels[0] == 4) ? GL_RGBA : GL_RGB;

	for (size_t i = 0; i < paths.size(); i++)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, texture_widths[i], textureheights[i], 0, format, GL_UNSIGNED_BYTE, data[i]);
		SOIL_free_image_data(data[i]);
	}

	textureMap[name] = id;

	return true;
}

const bool TextureManager::FindTextureIDByString(const std::string& path, unsigned int& id)
{
	auto it = textureMap.find(path);
	if (it != textureMap.end())
	{
		id = it->second;
		return true;
	}

	return false;
}
