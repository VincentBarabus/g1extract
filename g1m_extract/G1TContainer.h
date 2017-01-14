#pragma once
#include "utility.h"
#include "dds.h"
#include <vector>
class G1MFile;

struct G1T_HEADER
{
	CHAR Magic[8];
	DWORD FileSize;
	DWORD DataOffset;
	DWORD NumTextures;
	DWORD Version;

	void SwapBytes();
};

enum G1T_TEXTUTE_FORMAT : BYTE
{
	RGBA = 1,
	DXT1 = 6,
	DXT5 = 8,
};

struct G1T_TEXTUTE_DESCRIPTOR
{
	BYTE Mipmaps;
	G1T_TEXTUTE_FORMAT Format : 8;
	BYTE PowH : 4;
	BYTE PowW : 4;
	BYTE Reserved;
	DWORD Flags;
	// DWORD Unknown[4];

	void SwapBytes();
};


class G1TTexture
{
private:
	G1T_TEXTUTE_DESCRIPTOR descriptor;
	std::vector<BYTE> data;

public:
	void LoadFromStream(std::ifstream& file);
	void SaveToDDS(std::string& filename);

	friend class G1MFile;
};


class G1TContainer
{
private:
	G1T_HEADER header;
	std::vector<G1TTexture> textures;
	std::vector<DWORD> offsets;

public:
	void LoadFromStream(std::ifstream& file);
	// void SaveToDDS(std::string& filename);
	virtual ~G1TContainer();
	friend class G1MFile;
};

