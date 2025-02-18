/* This file is part of the Spring engine (GPL v2 or later), see LICENSE.html */

#ifndef BUMP_WATER_H
#define BUMP_WATER_H

#include "Rendering/GL/FBO.h"
#include "Rendering/GL/myGL.h"
#include "IWater.h"

#include "System/EventClient.h"
#include "System/Misc/RectangleOverlapHandler.h"


namespace Shader {
	struct IProgramObject;
}

class CBumpWater : public IWater, public CEventClient
{
public:
	//! CEventClient interface
	bool WantsEvent(const std::string& eventName) {
		return shoreWaves && (eventName == "UnsyncedHeightMapUpdate");
	}
	bool GetFullRead() const { return true; }
	int GetReadAllyTeam() const { return AllAccessTeam; }

public:
	CBumpWater();
	virtual ~CBumpWater();

	void Update();
	void UpdateWater(CGame* game);
	void OcclusionQuery();
	void DrawReflection(CGame* game);
	void DrawRefraction(CGame* game);
	void Draw();
	int GetID() const { return WATER_RENDERER_BUMPMAPPED; }
	const char* GetName() const { return "bumpmapped"; }

	bool CanDrawReflectionPass() const override { return true; }
	bool CanDrawRefractionPass() const override { return true; }
private:
	void SetUniforms(); ///< @see #useUniforms
	void SetupUniforms( std::string& definitions );
	void GetUniformLocations(const Shader::IProgramObject*);

private:
	//! coastmap (needed for shorewaves)
	struct CoastAtlasRect {
		CoastAtlasRect(const SRectangle& rect);
		bool isCoastline; ///< if false, then the whole rect is either above water or below water (no coastline -> no need to calc/render distfield)
		int ix1, iy1;
		int ix2, iy2;
		int xsize, ysize;
		float x1, y1;
		float x2, y2;
		float tx1, ty1;
		float tx2, ty2;
	};

	std::vector<CoastAtlasRect> coastmapAtlasRects;
	CRectangleOverlapHandler heightmapUpdates;

	void UploadCoastline(const bool forceFull = false);
	void UpdateCoastmap(const bool initialize = false);
	void UpdateDynWaves(const bool initialize = false);

	int atlasX,atlasY;

	void UnsyncedHeightMapUpdate(const SRectangle& rect);

private:
	//! user options
	char  reflection;   ///< 0:=off, 1:=don't render the terrain, 2:=render everything+terrain
	char  refraction;   ///< 0:=off, 1:=screencopy, 2:=own rendering cycle
	int   reflTexSize;
	bool  depthCopy;    ///< uses a screen depth copy, which allows a nicer interpolation between deep sea and shallow water
	float anisotropy;
	char  depthBits;    ///< depthBits for reflection/refraction RBO
	bool  blurRefl;
	bool  shoreWaves;
	bool  endlessOcean; ///< render the water around the whole map
	bool  dynWaves;     ///< only usable if bumpmap/normal texture is a TileSet
	bool  useUniforms;  ///< use Uniforms instead of \#define'd const. Warning: this is much slower, but has the advantage that you can change the params on runtime.

	unsigned char* tileOffsets; ///< used to randomize the wave/bumpmap/normal texture
	int  normalTextureX; ///< needed for dynamic waves
	int  normalTextureY;

	GLuint target; ///< for screen copies (color/depth), can be GL_TEXTURE_RECTANGLE (nvidia) or GL_TEXTURE_2D (others)
	int  screenTextureX;
	int  screenTextureY;

	FBO reflectFBO;
	FBO refractFBO;
	FBO coastFBO;
	FBO dynWavesFBO;

	GLuint displayList;

	GLuint refractTexture;
	GLuint reflectTexture;
	GLuint depthTexture;   ///< screen depth copy
	GLuint waveRandTexture;
	GLuint foamTexture;
	GLuint normalTexture;  ///< final used
	GLuint normalTexture2; ///< updates normalTexture with dynamic waves turned on
	GLuint coastTexture;
	GLuint coastUpdateTexture;
	std::vector<GLuint> caustTextures;

	Shader::IProgramObject* waterShader;
	Shader::IProgramObject* blurShader;

	std::array<GLuint, 26> uniforms; ///< see useUniforms

	bool wasVisibleLastFrame;
	GLuint occlusionQuery;
	GLuint occlusionQueryResult;

	float3 windVec;
	float3 windndir;
//	float  windStrength;
};

#endif // BUMP_WATER_H

