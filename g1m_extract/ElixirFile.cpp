#include "stdafx.h"
#include "ElixirFile.h"


void ELIXIR_HEADER::SwapBytes()
{
	::SwapBytes(Endian);
	::SwapBytes(DataSize);
	::SwapBytes(HeaderSize);
	::SwapBytes(FileTableSize);
	::SwapBytes(nFiles);
	::SwapBytes(Version);
}


void ElixirEntry::ReadFromStream(std::ifstream& file)
{
	file.read(reinterpret_cast<char*>(&ID), sizeof(DWORD));
	::SwapBytes(ID);
	file.read(reinterpret_cast<char*>(&Size), sizeof(DWORD));
	::SwapBytes(Size);
	FileName.resize(48);
	file.read(const_cast<char*>(FileName.data()), 48);
	FileName.resize(strlen(FileName.c_str()));
}

void ElixirFile::LoadFromFile(std::string& filename)
{
	file.open(filename, std::ios_base::binary);

	file.read(reinterpret_cast<char*>(&header), sizeof(ELIXIR_HEADER));
	header.SwapBytes();

	for (DWORD i = 0; i < header.nFiles; i++)
	{
		ElixirEntry entry;
		entry.ReadFromStream(file);
		entries.push_back(entry);
	}
	//*
	for (auto& entry : entries)
	{
		if (entry.Size)
		{
			entry.StreamPos = static_cast<DWORD>(file.tellg());
			file.seekg(entry.Size, std::ios_base::cur);
		}
	}
	//*/
}

std::ifstream& ElixirFile::GetStreamAt(DWORD fileIndex)
{
	file.seekg(entries[fileIndex].StreamPos);
	return file;
}


/* aditional functionality
void ElixirFile::LoadNames(std::string& filename)
{
	std::ifstream file(filename);
	for (auto& entry : entries)
	{
		char name[MAX_PATH];
		file.getline(name, MAX_PATH);
		entry.FileName = name;
	}
}

void ElixirFile::ExportNames(std::string& filename)
{
	std::ofstream file(filename);
	for (auto& entry : entries)
	{
		file << entry.FileName << std::endl;
	}
}

void ElixirFile::ExtractFiles(std::string& dirname)
{
	// std::string dirname = filename.substr(0, filename.length() - 6) + "files";
	std::experimental::filesystem::create_directory(dirname);
	// std::ofstream log(dirname + ".txt");
	dirname += "\\";
	for (auto& entry : entries)
	{
		if (entry.Size)
		{
			std::string newFileName(entry.FileName);
			// log << entry.FileName<< std::endl;
			// // have a file without extention
			// if ((newFileName.find_first_of('.') == -1) || (newFileName.find_first_of('.') > (newFileName.length() - 4)))
			// {
			// newFileName += "-uuid-";
			// newFileName += std::to_string(entries[i].ID);
			// newFileName += ".";
			// for (DWORD j = 0; j < 3; j++)
			// {
			// newFileName += data[j];
			// }
			// }
			//
			std::ofstream newfile(dirname + newFileName, std::ios_base::binary);
			newfile.write(entry.Data.data(), entry.Size);
			newfile.close();
		}
	}
}
//*/