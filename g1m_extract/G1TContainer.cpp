#include "StdAfx.h"

void G1T_HEADER::SwapBytes()
{
	::SwapBytes(FileSize);
	::SwapBytes(DataOffset);
	::SwapBytes(NumTextures);
	::SwapBytes(Version);
}

void G1T_TEXTUTE_DESCRIPTOR::SwapBytes()
{
	::SwapBytes(Flags);
}

void G1TTexture::LoadFromStream(std::ifstream& file)
{
	file.read(reinterpret_cast<char*>(&descriptor), sizeof(descriptor));
	descriptor.SwapBytes();

	if (descriptor.Flags & 0x01)
	{
		// skip 3 unknown values
		file.seekg(sizeof(DWORD) * 3, std::ios_base::cur);
	}

	DWORD dataSize = 0;
	for(DWORD i = 0; i < descriptor.Mipmaps; i++)
	{
		DWORD mipSize = (1 << (descriptor.PowW - i)) * (1 << (descriptor.PowH - i));
		switch(descriptor.Format)
		{
			case RGBA:
			mipSize *= 4;
			break;

			case DXT1:
			mipSize /= 2;
			mipSize = (mipSize >= 8) ? mipSize : 8;
			break;

			case DXT5:
			mipSize = (mipSize >= 16) ? mipSize : 16;
			break;

			default:
			assert(0);
			break;
		}
		dataSize += mipSize;
	}

	data.resize(dataSize);
	file.read(reinterpret_cast<char*>(data.data()), dataSize);
}

void G1TTexture::SaveToDDS(std::string& filename)
{
	std::ofstream file(filename, std::ios_base::binary);

	DDS_HEADER header;
	ZeroMemory(&header, sizeof(DDS_HEADER));
	header.dwSize = sizeof(DDS_HEADER);
	header.dwFlags = DDS_HEADER_FLAGS_TEXTURE | DDS_HEADER_FLAGS_LINEARSIZE;
	header.dwHeight = 0x1 << descriptor.PowH;
	header.dwWidth = 0x1 << descriptor.PowW;
	header.dwPitchOrLinearSize = data.size();
	header.dwCaps = DDS_SURFACE_FLAGS_TEXTURE;
	header.dwMipMapCount = descriptor.Mipmaps - 1;

	if(header.dwMipMapCount)
	{
		header.dwFlags |= DDS_HEADER_FLAGS_MIPMAP;
	}

	switch(descriptor.Format)
	{
		case RGBA:
			header.ddspf = DDSPF_A8R8G8B8;
			break;

		case DXT1:
			header.ddspf = DDSPF_DXT1;
			break;

		case DXT5:
			header.ddspf = DDSPF_DXT5;
			break;
	}

	file.write("DDS ", 4);
	file.write(reinterpret_cast<char*>(&header), header.dwSize);
	file.write(reinterpret_cast<char*>(data.data()), header.dwPitchOrLinearSize);
}


void G1TContainer::LoadFromStream(std::ifstream& stream)
{
	stream.read(reinterpret_cast<char*>(&header), sizeof(G1T_HEADER));
	header.SwapBytes();

	offsets.resize(header.NumTextures);

	stream.seekg(header.DataOffset - sizeof(G1T_HEADER), std::ios_base::cur);
	stream.read(reinterpret_cast<char*>(offsets.data()), sizeof(DWORD) * header.NumTextures);
	for(DWORD i = 0; i < header.NumTextures; i++)
	{
		::SwapBytes(offsets[i]);
	}

	for (DWORD i = 0; i < header.NumTextures; i++)
	{
		std::cout << "Loading texture " << std::to_string(i) << std::endl;
		G1TTexture texture;
		texture.LoadFromStream(stream);
		textures.push_back(texture);
	}

}

G1TContainer::~G1TContainer()
{
	// // delete texture files // we cant
	// for (DWORD i = 0; i < header.NumTextures; i++)
	// {
	// 	std::experimental::filesystem::remove(std::to_string(i) + ".dds");
	// }
}
/*
void G1TContainer::SaveToDDS(std::string& filename)
{
	std::experimental::filesystem::create_directory(filename + ".files");
	DWORD i = 0;
	for(auto& texture : textures)
	{
		texture.SaveToDDS(filename + ".files\\" + std::to_string(i) + ".dds");
		i++;
	}
}
*/