/* This file is part of the Spring engine (GPL v2 or later), see LICENSE.html */

#ifndef _BITMAP_H
#define _BITMAP_H

#include <string>
#include <vector>
#ifndef BITMAP_NO_OPENGL
	#include "nv_dds.h"
#endif // !BITMAP_NO_OPENGL
#include "System/float3.h"
#include "System/Color.h"


struct SDL_Surface;


class CBitmap {
public:
	CBitmap()
		: xsize(0)
		, ysize(0)
		, channels(4)
		, dataType(0x1401) //GL_UNSIGNED_BYTE
		, dataTypeSize(1)
		, compressed(false)
	{}
	CBitmap(const uint8_t* data, int xsize, int ysize, int channels = 4, uint32_t dataType = 0x1401);
	CBitmap(const CBitmap& bmp): CBitmap() { *this = bmp; }
	CBitmap(CBitmap&& bmp) noexcept : CBitmap() { *this = std::move(bmp); }
	CBitmap& operator=(const CBitmap& bmp);
	CBitmap& operator=(CBitmap&& bmp) noexcept;

	~CBitmap();

	CBitmap CanvasResize(const int newx, const int newy, const bool center = true) const;
	CBitmap CreateRescaled(int newx, int newy) const;

	static void InitPool(size_t size);

	void Alloc(int w, int h, int c);
	void Alloc(int w, int h) { Alloc(w, h, channels); }
	void AllocDummy(const SColor fill = SColor(255, 0, 0, 255));

	/// Load data from a file on the VFS
	bool Load(std::string const& filename, float defaultAlpha = 1.0f, int32_t reqNumChannel = 4, uint32_t reqDataType = 0);
	/// Load data from a gray-scale file on the VFS
	bool LoadGrayscale(std::string const& filename) { return Load(filename, 1.0f, 1, 0x1401); };

	bool Save(const std::string& filename, bool opaque = true, bool logged = false) const;
	bool SaveGrayScale(const std::string& filename) const;
	bool SaveFloat(const std::string& filename) const;

	bool Empty() const { return (memIdx == size_t(-1)); } // implies size=0

	uint32_t CreateTexture(float aniso = 0.0f, float lodBias = 0.0f, bool mipmaps = false, uint32_t texID = 0) const;
	uint32_t CreateMipMapTexture(float aniso = 0.0f, float lodBias = 0.0f) const { return (CreateTexture(aniso, lodBias, true)); }
	uint32_t CreateAnisoTexture(float aniso = 0.0f, float lodBias = 0.0f) const { return (CreateTexture(aniso, lodBias, false)); }
	uint32_t CreateDDSTexture(uint32_t texID = 0, float aniso = 0.0f, float lodBias = 0.0f, bool mipmaps = false) const;

	void CreateAlpha(uint8_t red, uint8_t green, uint8_t blue);
	void SetTransparent(const SColor& c, const SColor trans = SColor(0, 0, 0, 0));

	void Renormalize(float3 newCol);
	void Blur(int iterations = 1, float weight = 1.0f);
	void Fill(const SColor& c);

	void CopySubImage(const CBitmap& src, int x, int y);

	void ReverseYAxis();
	void InvertColors();
	void InvertAlpha();
	void MakeGrayScale();
	void Tint(const float tint[3]);

	/**
	 * Allocates a new SDL_Surface, and feeds it with the data of this bitmap.
	 * Note:
	 * - You have to free the surface with SDL_FreeSurface(surface)
	 *   if you do not need it anymore!
	 */
	SDL_Surface* CreateSDLSurface();

	const uint8_t* GetRawMem() const;
	      uint8_t* GetRawMem()      ;

	void UpdateDataTypeSize();
	int32_t GetIntFmt() const;
	int32_t GetExtFmt() const;
	size_t GetMemSize() const { return (xsize * ysize * channels * dataTypeSize); }
private:
	// managed by pool
	size_t memIdx = size_t(-1);
public:
	int32_t xsize;
	int32_t ysize;
	int32_t channels;
	int32_t dataType;
	size_t dataTypeSize;

	#ifndef BITMAP_NO_OPENGL
	// GL_TEXTURE_2D, GL_TEXTURE_CUBE_MAP, ...
	// not set to anything until Load is called
	int32_t textype = 0;

	nv_dds::CDDSImage ddsimage;
	#endif

	bool compressed = false;
};

#endif // _BITMAP_H
