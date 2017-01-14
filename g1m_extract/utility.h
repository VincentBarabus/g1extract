#pragma once
#include <windows.h>
#include <fstream>
#include <fbxsdk.h>

typedef DWORD COLOR;

//=============================================================================
//	Utility Functions
//=============================================================================
typedef unsigned long long QWORD;

void SwapBytes(QWORD& value);

void SwapBytes(QWORD& value);

void SwapBytes(DWORD& value);

void SwapBytes(LONG& value);

void SwapBytes(float& value);

void SwapBytes(WORD& value);

void SwapBytes(SHORT& value);

void AlignTo(std::ifstream& file, DWORD value);

UINT32 crc32(BYTE* input, DWORD len);

UINT32 crc32(std::string input);

namespace std
{
	class membuf : public streambuf
	{
	public:
		membuf(char* p, size_t n) {
			setg(p, p, p + n);
			setp(p, p + n);
		}
	};
}
//=============================================================================
//	Utility Types
//=============================================================================
struct BYTE4
{
	BYTE i0;
	BYTE i1;
	BYTE i2;
	BYTE i3;
};

struct TRIANGLE
{
	DWORD v0;
	DWORD v1;
	DWORD v2;
};

struct VECTOR2
{
	float x, y;

	// inline VECTOR2() : x(0.0f), y(0.0f) { }
	// inline VECTOR2(const float& value) : x(value), y(0.0f) { }
	// inline VECTOR2(const VECTOR2& value) : x(value.x), y(value.y) { }

	void SwapBytes();
	inline operator FbxVector2() const
	{
		return FbxVector2(x, y);
	}
};

struct VECTOR3
{
	float x, y, z;

	// inline VECTOR3() : x(0.0f), y(0.0f), z(0.0f) { }
	// inline VECTOR3(const float& value) : x(value), y(0.0f), z(0.0f) { }
	// inline VECTOR3(const VECTOR2& value) : x(value.x), y(value.y), z(0.0f) { }
	// inline VECTOR3(const VECTOR3& value) : x(value.x), y(value.y), z(value.z) { }

	void SwapBytes();
	inline operator FbxVector4() const
	{
		return FbxVector4(x, y, z);
	}
};

struct VECTOR4
{
	float x, y, z, w;

	// inline VECTOR4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) { }
	// inline VECTOR4(const float& value) : x(value), y(0.0f), z(0.0f), w(0.0f) { }
	// inline VECTOR4(const VECTOR2& value) : x(value.x), y(value.y), z(0.0f), w(0.0f) { }
	// inline VECTOR4(const VECTOR3& value) : x(value.x), y(value.y), z(value.z), w(0.0f) { }
	// inline VECTOR4(const VECTOR4& value) : x(value.x), y(value.y), z(value.z), w(value.z) { }

	void SwapBytes();

	inline operator FbxVector4() const
	{
		return FbxVector4(x, y, z, w);
	}

	inline operator FbxQuaternion() const
	{
		return FbxQuaternion(x, y, z, w);
	}

};

struct BBOX
{
    VECTOR3 min;
    VECTOR3 max;

	void SwapBytes();
};

struct MATRIX44
{
    VECTOR4 r0;
    VECTOR4 r1;
    VECTOR4 r2;
    VECTOR4 r3;

	void SwapBytes();

	inline operator FbxAMatrix() const
	{
		FbxAMatrix matrix;
		matrix.SetRow(0, r0);
		matrix.SetRow(1, r1);
		matrix.SetRow(2, r2);
		matrix.SetRow(3, r3);
		return matrix;
	}
};

struct MATRIX34
{
    VECTOR4 scale;
    VECTOR4 rotation;
    VECTOR4 transform;

	void SwapBytes();

};

struct BONE
{
    VECTOR3 scale;
    LONG parentIndex;
    VECTOR4 rotation;
    VECTOR4 transform;

	void SwapBytes();
};
