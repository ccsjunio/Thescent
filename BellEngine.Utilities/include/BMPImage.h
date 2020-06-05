#ifndef BMPImage_HG
#define BMPImage_HG

#include <string>
#include "ResourceManager.h"
#include <iostream>

class BMPImage
{
private:
	ResourceManager			gResourceManager;
	bool m_bIsLoaded;
	unsigned long m_FileSize;
	unsigned long m_ImageWidth;
	unsigned long m_ImageHeight;
	unsigned long m_ImageSize;
	char* m_pData;

public:
	class BMPFileHeader
	{
	public:
		unsigned short headerField;
		unsigned long fileSize;
		unsigned short reserved1;
		unsigned short reserved2;
		unsigned long dataOffset;

		BMPFileHeader()
		{
			this->headerField = 0;
			this->fileSize = 0;
			this->reserved1 = 0;
			this->reserved2 = 0;
			this->dataOffset = 0;
		}
		void print()
		{
			std::cout << "BitMap File Header\n";
			std::cout << "  Header Field: " << headerField << "\n";
			std::cout << "  File Size: " << fileSize << "\n";
			std::cout << "  Reserved 1: " << reserved1 << "\n";
			std::cout << "  Reserved 2: " << reserved2 << "\n";
			std::cout << "  Data Offset: " << dataOffset << "\n";
			std::cout << "\n";
		}
	};

	class BMPInfoHeader
	{
	public:
		unsigned long sizeOfHeader;
		unsigned long imageWidth;
		unsigned long imageHeight;
		unsigned short numColorPlanes;
		unsigned short bitsPerPixel;
		unsigned long compressionMethod;
		unsigned long imageSize;
		unsigned long horizontalResolution;
		unsigned long verticalResolution;
		unsigned long numColorsInPalette;
		unsigned long numImportantColors;

		BMPInfoHeader()
		{
			this->sizeOfHeader = 0;
			this->imageWidth = 0;
			this->imageHeight = 0;
			this->numColorPlanes = 0;
			this->bitsPerPixel = 0;
			this->compressionMethod = 0;
			this->imageSize = 0;
			this->horizontalResolution = 0;
			this->verticalResolution = 0;
			this->numColorsInPalette = 0;
			this->numImportantColors = 0;
		}
		void print()
		{
			std::cout << "BitMap Info Header\n";
			std::cout << "  Size Of Header: " << sizeOfHeader << "\n";
			std::cout << "  Image Width: " << imageWidth << "\n";
			std::cout << "  Image Height: " << imageHeight << "\n";
			std::cout << "  Number Of Colored Planes: " << numColorPlanes << "\n";
			std::cout << "  Bits Per Pixel: " << bitsPerPixel << "\n";
			std::cout << "  Compression Method: " << compressionMethod << "\n";
			std::cout << "  Image Size: " << imageSize << "\n";
			std::cout << "  Horizontal Resolution: " << horizontalResolution << "\n";
			std::cout << "  Vertical Resolution: " << verticalResolution << "\n";
			std::cout << "  Number Of Colors In Palette: " << numColorsInPalette << "\n";
			std::cout << "  Number of Important Colors: " << numImportantColors << "\n";
			std::cout << "\n";
		}
	};

	BMPImage();
	BMPImage(const std::string &filename);
	~BMPImage();

	const bool LoadBMPFromFile(const std::string& filename);
	const bool IsLoaded(void);
	unsigned long GetFileSize(void);
	unsigned long GetImageWidth(void);
	unsigned long GetImageHeight(void);
	unsigned long GetImageSize(void);
	char* GetData(void);
	void Dispose();
};

#endif
