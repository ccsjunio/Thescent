#include "BMPImage.h"
#include <fstream>
#include <iostream>

BMPImage::BMPImage()
{
	m_ImageHeight = m_ImageSize = m_ImageWidth = m_FileSize = 0;
	this->m_pData = nullptr;
	this->m_bIsLoaded = false;
	return;
}

BMPImage::BMPImage(const std::string &filename)
{
	this->m_bIsLoaded = false;
	this->LoadBMPFromFile(filename);
	return;
}

BMPImage::~BMPImage()
{
	Dispose();
	return;
}

unsigned long BMPImage::GetFileSize(void)
{
	return this->m_FileSize;
}

unsigned long BMPImage::GetImageWidth(void)
{
	return this->m_ImageWidth;
}

unsigned long BMPImage::GetImageHeight(void)
{
	return this->m_ImageHeight;
}

unsigned long BMPImage::GetImageSize(void)
{
	return this->m_ImageSize;
}

char* BMPImage::GetData(void)
{
	return this->m_pData;
}

void BMPImage::Dispose()
{
	if (this->m_pData)
	{
		delete (this->m_pData);
		this->m_pData = nullptr;
	}
}

const bool BMPImage::LoadBMPFromFile(const std::string &filename)
{

	//
	unsigned int bmpFile;
	if (!gResourceManager.OpenBinaryFile(filename, &bmpFile))
	{
		// Failed to open 
		printf("Failed to open %s\n", filename.c_str());
		return false;
	}

	// Load our BMP Header 
	BMPFileHeader bmpFileHeader;
	// unsigned short headerField;
	gResourceManager.ReadData(bmpFile, &bmpFileHeader.headerField);

	// Check if the file prepends with "BM"
	if (((char*)&bmpFileHeader.headerField)[0] != 'B'
		|| ((char*)&bmpFileHeader.headerField)[1] != 'M')
	{
		printf("%s is not a BitMap.\n", filename.c_str());
		gResourceManager.CloseFile(bmpFile);
		return false;
	}

	// Retrieve the rest of the file information
	gResourceManager.ReadData(bmpFile, &bmpFileHeader.fileSize);
	gResourceManager.ReadData(bmpFile, &bmpFileHeader.reserved1);
	gResourceManager.ReadData(bmpFile, &bmpFileHeader.reserved2);
	gResourceManager.ReadData(bmpFile, &bmpFileHeader.dataOffset);

	// Check if info is correct
	//bmpFileHeader.print();

	// Load our image info header
	BMPInfoHeader bmpInfoHeader;
	gResourceManager.ReadData(bmpFile, &bmpInfoHeader.sizeOfHeader);
	gResourceManager.ReadData(bmpFile, &bmpInfoHeader.imageWidth);
	gResourceManager.ReadData(bmpFile, &bmpInfoHeader.imageHeight);
	gResourceManager.ReadData(bmpFile, &bmpInfoHeader.numColorPlanes);
	gResourceManager.ReadData(bmpFile, &bmpInfoHeader.bitsPerPixel);
	gResourceManager.ReadData(bmpFile, &bmpInfoHeader.compressionMethod);
	gResourceManager.ReadData(bmpFile, &bmpInfoHeader.imageSize);
	gResourceManager.ReadData(bmpFile, &bmpInfoHeader.horizontalResolution);
	gResourceManager.ReadData(bmpFile, &bmpInfoHeader.verticalResolution);
	gResourceManager.ReadData(bmpFile, &bmpInfoHeader.numColorsInPalette);
	gResourceManager.ReadData(bmpFile, &bmpInfoHeader.numImportantColors);
	//gResourceManager.ReadData(bmpFile, (char*)&bmpInfoHeader, sizeof(bmpInfoHeader));

	// Check if info is correct
	//bmpInfoHeader.print();

	// Check if valid bitmap
	if (bmpInfoHeader.numColorPlanes != 1
		|| bmpInfoHeader.bitsPerPixel != 24
		|| bmpInfoHeader.compressionMethod != 0)
	{
		printf("%s is not a raw BMP24.\n", filename.c_str());
		gResourceManager.CloseFile(bmpFile);
		return false;
	}

	// Programs not setting the image size properly
	if (bmpInfoHeader.imageSize == 0)
	{
		printf("Image size is not set.  Calculating it...\n");
		bmpInfoHeader.imageSize = bmpFileHeader.fileSize - bmpFileHeader.dataOffset;
	}

	// Load our image data
	if (this->m_pData)
	{
		delete (this->m_pData);
		this->m_pData = nullptr;
	}
	this->m_pData = new char[bmpInfoHeader.imageSize];

	gResourceManager.Seek(bmpFile, bmpFileHeader.dataOffset);
	gResourceManager.ReadData(bmpFile, this->m_pData, bmpInfoHeader.imageSize);
	// We are done with the file, close it
	gResourceManager.CloseFile(bmpFile);

	// Save the information to the clas
	this->m_ImageHeight = bmpInfoHeader.imageHeight;
	this->m_ImageWidth = bmpInfoHeader.imageWidth;
	this->m_ImageSize = bmpInfoHeader.imageSize;

	// Successfully loaded the BMP file
	this->m_bIsLoaded = true;
	return true;
}

const bool BMPImage::IsLoaded(void)
{
	return this->m_bIsLoaded;
}

