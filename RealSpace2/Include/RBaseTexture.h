#pragma once

#include "d3dx9.h"
#include <string>
#include <map>
#include <algorithm>
#include "MUtil.h"
#include "RNameSpace.h"
#include "TextureLoader.h"

_NAMESPACE_REALSPACE2_BEGIN

#define RTextureType_Etc		0
#define RTextureType_Map		1<<1
#define RTextureType_Object		1<<2
#define RTextureType_All		1<<3

class RTextureManager;

class RBaseTexture final
{
public:
	void Destroy();

	void OnInvalidate();
	bool OnRestore(bool bManaged = false);

	auto GetWidth() const { return m_Info.Width; }
	auto GetHeight() const { return m_Info.Height; }
	auto GetMipLevels() const { return m_Info.MipLevels; }

	auto GetTexLevel() const { return m_nTexLevel; }
	void SetTexLevel(int level) { m_nTexLevel = level;}

	auto GetTexType() const { return m_nTexType; }
	void SetTexType(int type) { m_nTexType = type; }

	auto GetFormat() const { return m_Info.Format; }

	LPDIRECT3DTEXTURE9 GetTexture();

	bool	m_bManaged{};
	u32		m_dwLastUseTime{};
	int		m_nFileSize{};
	char	m_szTextureName[256];
	int		m_nRefCount{};
	bool	m_bUseMipmap{};
	bool	m_bUseFileSystem = true;

	int		m_nTexLevel{};
	u32		m_nTexType = RTextureType_Etc;

	TextureInfo m_Info{};
	D3DPtr<IDirect3DTexture9> m_pTex;

private:
	friend RTextureManager;
	~RBaseTexture();

	bool SubCreateTexture(char* TextureFileBuffer);
};

class RTextureManager final : public std::map<std::string, RBaseTexture*> 
{
public:
	~RTextureManager();

	void Destroy();

	RBaseTexture *CreateBaseTextureSub(bool Managed, const char* filename, int texlevel,
		bool bUseMipmap = false, bool bUseFileSystem = true);
	RBaseTexture *CreateBaseTexture(const char* filename, int texlevel,
		bool bUseMipmap = false, bool bUseFileSystem = true);
	RBaseTexture *CreateBaseTextureMg(const char* filename, int texlevel,
		bool bUseMipmap = false, bool bUseFileSystem = true);

	void DestroyBaseTexture(RBaseTexture*);
	void DestroyBaseTexture(const char* szName);

	void OnInvalidate();
	void OnRestore();
	void OnChangeTextureLevel(u32 flag);

	int UpdateTexture(u32 max_life_time = 5000);
	int CalcUsedSize();
	int CalcAllUsedSize();
	int PrintUsedTexture();
	int CalcUsedCount();
};

void RBaseTexture_Create();
void RBaseTexture_Destroy();

void RBaseTexture_Invalidate();
void RBaseTexture_Restore();

RTextureManager* RGetTextureManager();

void SetObjectTextureLevel(int nLevel);
void SetMapTextureLevel(int nLevel);
void SetTextureFormat(int nLevel);	// 0 = 16 bit , 1 = 32bit

int GetObjectTextureLevel();
int GetMapTextureLevel();
int GetTextureFormat();

RBaseTexture* RCreateBaseTexture(const char* filename, DWORD nTexType = RTextureType_Etc,
	bool bUseMipmap = false, bool bUseFileSystem = true);
RBaseTexture* RCreateBaseTextureMg(const char* filename, DWORD nTexType = RTextureType_Etc,
	bool bUseMipmap = false, bool bUseFileSystem = true);
void RDestroyBaseTexture(RBaseTexture*);
void RChangeBaseTextureLevel(DWORD flag);

struct RBaseTextureDeleter {
	void operator()(RBaseTexture* ptr) const {
		RDestroyBaseTexture(ptr);
	}
};

using RBaseTexturePtr = std::unique_ptr<RBaseTexture, RBaseTextureDeleter>;

_NAMESPACE_REALSPACE2_END