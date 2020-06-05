#pragma once
#include <map>
#include <string>
#include <vector>
#include <IExtendedInfo.h>
#include <ITexture.h>

class TextureManager : public IExtendedInfo
{
private:
	std::map<std::string, unsigned int> textureMap;

public:
	TextureManager();
	~TextureManager();

	const bool Create2DTexture(ITexture* texture, const bool& generateMIP = true);
	const bool CreateCubeMapTexture(ITexture* texture, const bool& generateMIP = true);
	const bool Create2DTexture(const std::string& path, unsigned int& id, const bool& generateMIP = true);
	const bool CreateCubeMapTexture(const std::string& name,
									const std::string& posX_Path, const std::string& negX_Path,
									const std::string& posY_Path, const std::string& negY_Path,
									const std::string& posZ_Path, const std::string& negZ_Path, 
									unsigned int& id,
									const bool& generateMIP = true);
	const bool CreateCubeMapTexture(const std::string& name, const std::vector<std::string> paths, unsigned int& id, const bool& generateMIP = true);
	const bool FindTextureIDByString(const std::string& path, unsigned int& id);
};

