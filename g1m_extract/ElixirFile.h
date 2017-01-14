#pragma once
struct ELIXIR_HEADER
{
	DWORD Magic;
	DWORD Endian;
	DWORD DataSize;
	DWORD HeaderSize;
	DWORD FileTableSize;
	DWORD nFiles;
	DWORD Version;

	void SwapBytes();
};

class ElixirEntry
{
public:
	DWORD ID;
	DWORD Size;
	std::string FileName; // char [48];
	DWORD StreamPos;
	// std::vector<char> Data;

	void ReadFromStream(std::ifstream& file);
};


class ElixirFile
{
	ELIXIR_HEADER header;
	std::vector<ElixirEntry> entries;
	std::ifstream file;
public:
	void LoadFromFile(std::string& filename);

	std::ifstream& GetStreamAt(DWORD fileIndex);

	/* aditional functionality
	void LoadNames(std::string& filename);

	void ExportNames(std::string& filename);

	void ExtractFiles(std::string& dirname);
	//*/
};
