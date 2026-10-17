#ifndef CLEAN_MAC_D3D9_TYPES_H
#define CLEAN_MAC_D3D9_TYPES_H

#include "../cod2_fwd.h"
#include "../cod2_defs.h"

typedef enum D3DSTATEBLOCKTYPE _D3DSTATEBLOCKTYPE;
typedef struct CColorArray CColorArray;
typedef struct CFence CFence;
typedef struct CNormalArray CNormalArray;
typedef struct CSecondaryColorArray CSecondaryColorArray;
typedef struct CTexCoordArray CTexCoordArray;
typedef struct CVertexArray CVertexArray;
typedef struct DXVector3 DXVector3;
typedef struct DXVector4 DXVector4;
typedef struct HWND__ HWND__;
typedef struct IDirect3D9 IDirect3D9;
typedef struct IDirect3DBaseTexture9 IDirect3DBaseTexture9;
typedef struct IDirect3DCubeTexture9 IDirect3DCubeTexture9;
typedef struct IDirect3DDevice9 IDirect3DDevice9;
typedef struct IDirect3DQuery9 IDirect3DQuery9;
typedef struct IDirect3DSurface9 IDirect3DSurface9;
typedef struct IDirect3DSwapChain9 IDirect3DSwapChain9;
typedef struct IDirect3DTexture9 IDirect3DTexture9;
typedef union IDirect3DVertexBuffer9 IDirect3DVertexBuffer9;
typedef struct IDirect3DVertexDeclaration9 IDirect3DVertexDeclaration9;
typedef struct IDirect3DVolumeTexture9 IDirect3DVolumeTexture9;
typedef struct VertexProgramStreamState VertexProgramStreamState;
typedef struct _D3DADAPTER_IDENTIFIER9 _D3DADAPTER_IDENTIFIER9;
typedef struct _D3DBOX _D3DBOX;
typedef struct _D3DCLIPSTATUS9 _D3DCLIPSTATUS9;
typedef struct _D3DCOLORVALUE _D3DCOLORVALUE;
typedef struct _D3DDEVICE_CREATION_PARAMETERS _D3DDEVICE_CREATION_PARAMETERS;
typedef struct _D3DGAMMARAMP _D3DGAMMARAMP;
typedef struct _D3DINDEXBUFFER_DESC _D3DINDEXBUFFER_DESC;
typedef struct _D3DLIGHT9 _D3DLIGHT9;
typedef struct _D3DLOCKED_BOX _D3DLOCKED_BOX;
typedef struct _D3DLOCKED_RECT _D3DLOCKED_RECT;
typedef struct _D3DMATERIAL9 _D3DMATERIAL9;
typedef struct _D3DPRESENT_PARAMETERS_ _D3DPRESENT_PARAMETERS_;
typedef struct _D3DRASTER_STATUS _D3DRASTER_STATUS;
typedef struct _D3DRECTPATCH_INFO _D3DRECTPATCH_INFO;
typedef struct _D3DSURFACE_DESC _D3DSURFACE_DESC;
typedef struct _D3DTRIPATCH_INFO _D3DTRIPATCH_INFO;
typedef struct _D3DVECTOR _D3DVECTOR;
typedef struct _D3DVERTEXBUFFER_DESC _D3DVERTEXBUFFER_DESC;
typedef struct _D3DVERTEXELEMENT9 _D3DVERTEXELEMENT9;
typedef struct _D3DVOLUME_DESC _D3DVOLUME_DESC;
typedef struct _GUID _GUID;
typedef struct _List_node_base _List_node_base;
typedef struct _RGNDATA _RGNDATA;
typedef struct _RGNDATAHEADER _RGNDATAHEADER;
typedef struct tagPALETTEENTRY tagPALETTEENTRY;
typedef struct tagPOINT tagPOINT;
typedef enum D3DBACKBUFFER_TYPE D3DBACKBUFFER_TYPE;
typedef enum D3DBASISTYPE D3DBASISTYPE;
typedef enum D3DDEGREETYPE D3DDEGREETYPE;
typedef enum D3DLIGHTTYPE D3DLIGHTTYPE;
typedef enum D3DPOOL D3DPOOL;
typedef enum D3DPRIMITIVETYPE D3DPRIMITIVETYPE;
typedef enum D3DQUERYTYPE D3DQUERYTYPE;
typedef enum D3DSAMPLERSTATETYPE D3DSAMPLERSTATETYPE;
typedef enum D3DSTATEBLOCKTYPE D3DSTATEBLOCKTYPE;
typedef enum D3DSWAPEFFECT D3DSWAPEFFECT;
typedef enum D3DTEXTURETRANSFORMFLAGS D3DTEXTURETRANSFORMFLAGS;
typedef enum D3DTRANSFORMSTATETYPE D3DTRANSFORMSTATETYPE;
typedef enum _D3DCUBEMAP_FACES _D3DCUBEMAP_FACES;
typedef enum _D3DTEXTUREFILTERTYPE _D3DTEXTUREFILTERTYPE;
typedef enum _Rb_tree_color _Rb_tree_color;
typedef struct _D3DCOLORVALUE D3DCOLORVALUE;
typedef struct _D3DVECTOR D3DVECTOR;
typedef struct _GUID GUID;
typedef struct _RGNDATAHEADER RGNDATAHEADER;

typedef struct CD3DXBuffer CD3DXBuffer;
typedef struct CD3DXConstantTable CD3DXConstantTable;
typedef struct COpenGLARBFragmentProgram COpenGLARBFragmentProgram;
typedef struct COpenGLATITextFragmentShader COpenGLATITextFragmentShader;
typedef struct COpenGLNVidiaRegisterCombinersProgram COpenGLNVidiaRegisterCombinersProgram;
typedef struct D3DXMATRIX D3DXMATRIX;
typedef struct D3DXVECTOR4 D3DXVECTOR4;
typedef struct FIFO FIFO;
typedef struct ID3DXBuffer ID3DXBuffer;
typedef struct ID3DXInclude ID3DXInclude;

enum D3DBACKBUFFER_TYPE {
    D3DBACKBUFFER_TYPE_MONO = 0,
    D3DBACKBUFFER_TYPE_LEFT = 1,
    D3DBACKBUFFER_TYPE_RIGHT = 2,
    D3DBACKBUFFER_TYPE_FORCE_DWORD = 2147483647
};

enum D3DBASISTYPE {
    D3DBASIS_BEZIER = 0,
    D3DBASIS_BSPLINE = 1,
    D3DBASIS_CATMULL_ROM = 2,
    D3DBASIS_FORCE_DWORD = 2147483647
};

enum D3DDEGREETYPE {
    D3DDEGREE_LINEAR = 1,
    D3DDEGREE_QUADRATIC = 2,
    D3DDEGREE_CUBIC = 3,
    D3DDEGREE_QUINTIC = 5,
    D3DDEGREE_FORCE_DWORD = 2147483647
};

enum D3DLIGHTTYPE {
    D3DLIGHT_POINT = 1,
    D3DLIGHT_SPOT = 2,
    D3DLIGHT_DIRECTIONAL = 3,
    D3DLIGHT_FORCE_DWORD = 2147483647
};

enum D3DPOOL {
    D3DPOOL_DEFAULT = 0,
    D3DPOOL_MANAGED = 1,
    D3DPOOL_SYSTEMMEM = 2,
    D3DPOOL_SCRATCH = 3,
    D3DPOOL_FORCE_DWORD = 2147483647
};

enum D3DPRIMITIVETYPE {
    D3DPT_POINTLIST = 1,
    D3DPT_LINELIST = 2,
    D3DPT_LINESTRIP = 3,
    D3DPT_TRIANGLELIST = 4,
    D3DPT_TRIANGLESTRIP = 5,
    D3DPT_TRIANGLEFAN = 6,
    D3DPT_FORCE_DWORD = 2147483647
};

enum D3DQUERYTYPE {
    D3DQUERYTYPE_VCACHE = 4,
    D3DQUERYTYPE_RESOURCEMANAGER = 5,
    D3DQUERYTYPE_VERTEXSTATS = 6,
    D3DQUERYTYPE_EVENT = 8,
    D3DQUERYTYPE_OCCLUSION = 9,
    D3DQUERYTYPE_TIMESTAMP = 10,
    D3DQUERYTYPE_TIMESTAMPDISJOINT = 11,
    D3DQUERYTYPE_TIMESTAMPFREQ = 12,
    D3DQUERYTYPE_PIPELINETIMINGS = 13,
    D3DQUERYTYPE_INTERFACETIMINGS = 14,
    D3DQUERYTYPE_VERTEXTIMINGS = 15,
    D3DQUERYTYPE_PIXELTIMINGS = 16,
    D3DQUERYTYPE_BANDWIDTHTIMINGS = 17,
    D3DQUERYTYPE_CACHEUTILIZATION = 18
};

enum D3DSAMPLERSTATETYPE {
    D3DSAMP_ADDRESSU = 1,
    D3DSAMP_ADDRESSV = 2,
    D3DSAMP_ADDRESSW = 3,
    D3DSAMP_BORDERCOLOR = 4,
    D3DSAMP_MAGFILTER = 5,
    D3DSAMP_MINFILTER = 6,
    D3DSAMP_MIPFILTER = 7,
    D3DSAMP_MIPMAPLODBIAS = 8,
    D3DSAMP_MAXMIPLEVEL = 9,
    D3DSAMP_MAXANISOTROPY = 10,
    D3DSAMP_SRGBTEXTURE = 11,
    D3DSAMP_ELEMENTINDEX = 12,
    D3DSAMP_DMAPOFFSET = 13,
    D3DSAMP_FORCE_DWORD = 2147483647
};

enum D3DSTATEBLOCKTYPE {
    D3DSBT_ALL = 1,
    D3DSBT_PIXELSTATE = 2,
    D3DSBT_VERTEXSTATE = 3,
    D3DSBT_FORCE_DWORD = 2147483647
};

enum D3DSWAPEFFECT {
    D3DSWAPEFFECT_DISCARD = 1,
    D3DSWAPEFFECT_FLIP = 2,
    D3DSWAPEFFECT_COPY = 3,
    D3DSWAPEFFECT_FORCE_DWORD = 2147483647
};

enum D3DTEXTURETRANSFORMFLAGS {
    D3DTTFF_DISABLE = 0,
    D3DTTFF_COUNT1 = 1,
    D3DTTFF_COUNT2 = 2,
    D3DTTFF_COUNT3 = 3,
    D3DTTFF_COUNT4 = 4,
    D3DTTFF_PROJECTED = 256,
    D3DTTFF_FORCE_DWORD = 2147483647
};

enum D3DTRANSFORMSTATETYPE {
    D3DTS_VIEW = 2,
    D3DTS_PROJECTION = 3,
    D3DTS_TEXTURE0 = 16,
    D3DTS_TEXTURE1 = 17,
    D3DTS_TEXTURE2 = 18,
    D3DTS_TEXTURE3 = 19,
    D3DTS_TEXTURE4 = 20,
    D3DTS_TEXTURE5 = 21,
    D3DTS_TEXTURE6 = 22,
    D3DTS_TEXTURE7 = 23,
    D3DTS_FORCE_DWORD = 2147483647
};

enum _D3DCUBEMAP_FACES {
    D3DCUBEMAP_FACE_POSITIVE_X = 0,
    D3DCUBEMAP_FACE_NEGATIVE_X = 1,
    D3DCUBEMAP_FACE_POSITIVE_Y = 2,
    D3DCUBEMAP_FACE_NEGATIVE_Y = 3,
    D3DCUBEMAP_FACE_POSITIVE_Z = 4,
    D3DCUBEMAP_FACE_NEGATIVE_Z = 5,
    D3DCUBEMAP_FACE_FORCE_DWORD = 2147483647
};

enum _D3DTEXTUREFILTERTYPE {
    D3DTEXF_NONE = 0,
    D3DTEXF_POINT = 1,
    D3DTEXF_LINEAR = 2,
    D3DTEXF_ANISOTROPIC = 3,
    D3DTEXF_PYRAMIDALQUAD = 6,
    D3DTEXF_GAUSSIANQUAD = 7,
    D3DTEXF_FORCE_DWORD = 2147483647
};

enum _Rb_tree_color {
    _S_red = 0,
    _S_black = 1
};

struct CColorArray {
    int _placeholder;
};

struct CFence {
    int _placeholder;
};

struct CNormalArray {
    int _placeholder;
};

struct CSecondaryColorArray {
    int _placeholder;
};

struct CTexCoordArray {
    int _placeholder;
};

struct CVertexArray {
    int _placeholder;
};

struct DXVector3 {
    float m[3];
};

struct DXVector4 {
    float m[4];
};

struct HWND__ {
    int unused;
};

struct IDirect3D9 {
    int _placeholder;
};

struct IDirect3DCubeTexture9 {
    int _placeholder;
};

struct IDirect3DDevice9 {
    int _placeholder;
};

struct IDirect3DQuery9 {
    int _placeholder;
};

struct IDirect3DSurface9 {
    long unsigned int min;
    long unsigned int max;
};

struct IDirect3DSwapChain9 {
    int _placeholder;
};

struct IDirect3DTexture9 {
    int _placeholder;
};

struct IDirect3DVertexDeclaration9 {
    int _placeholder;
};

struct IDirect3DVolumeTexture9 {
    int _placeholder;
};

struct VertexProgramStreamState {
    bool mNeedsValidation;
    bool mEnabled;
    GLint mVSize;
    GLenum mVType;
    GLboolean mNormalized;
    GLsizei mStride;
    const void *mpStream;
};

struct _D3DBOX {
    UINT Left;
    UINT Top;
    UINT Right;
    UINT Bottom;
    UINT Front;
    UINT Back;
};

struct _D3DCLIPSTATUS9 {
    DWORD ClipUnion;
    DWORD ClipIntersection;
};

struct _D3DCOLORVALUE {
    float r;
    float g;
    float b;
    float a;
};

struct _D3DDEVICE_CREATION_PARAMETERS {
    UINT AdapterOrdinal;
    D3DDEVTYPE DeviceType;
    HWND hFocusWindow;
    DWORD BehaviorFlags;
};

struct _D3DGAMMARAMP {
    WORD red[256];
    WORD green[256];
    WORD blue[256];
};

struct _D3DINDEXBUFFER_DESC {
    D3DFORMAT Format;
    D3DRESOURCETYPE Type;
    DWORD Usage;
    D3DPOOL Pool;
    UINT Size;
};

struct _D3DLOCKED_BOX {
    INT RowPitch;
    INT SlicePitch;
    void *pBits;
};

struct _D3DLOCKED_RECT {
    INT Pitch;
    void *pBits;
};

struct _D3DMATERIAL9 {
    D3DCOLORVALUE Diffuse;
    D3DCOLORVALUE Ambient;
    D3DCOLORVALUE Specular;
    D3DCOLORVALUE Emissive;
    float Power;
};

union IDirect3DVertexBuffer9 {
    struct { float _11; float _12; float _13; float _14; float _21; float _22; float _23; float _24; float _31; float _32; float _33; float _34; float _41; float _42; float _43; float _44; };
    D3DMATRIX m[4];
};

struct _D3DPRESENT_PARAMETERS_ {
    UINT BackBufferWidth;
    UINT BackBufferHeight;
    D3DFORMAT BackBufferFormat;
    UINT BackBufferCount;
    D3DMULTISAMPLE_TYPE MultiSampleType;
    DWORD MultiSampleQuality;
    D3DSWAPEFFECT SwapEffect;
    HWND hDeviceWindow;
    BOOL Windowed;
    BOOL EnableAutoDepthStencil;
    D3DFORMAT AutoDepthStencilFormat;
    DWORD Flags;
    UINT FullScreen_RefreshRateInHz;
    UINT PresentationInterval;
};

struct _D3DRASTER_STATUS {
    BOOL InVBlank;
    UINT ScanLine;
};

struct _D3DRECTPATCH_INFO {
    UINT StartVertexOffsetWidth;
    UINT StartVertexOffsetHeight;
    UINT Width;
    UINT Height;
    UINT Stride;
    D3DBASISTYPE Basis;
    D3DDEGREETYPE Degree;
};

struct _D3DSURFACE_DESC {
    D3DFORMAT Format;
    D3DRESOURCETYPE Type;
    DWORD Usage;
    D3DPOOL Pool;
    D3DMULTISAMPLE_TYPE MultiSampleType;
    DWORD MultiSampleQuality;
    UINT Width;
    UINT Height;
};

struct _D3DTRIPATCH_INFO {
    UINT StartVertexOffset;
    UINT NumVertices;
    D3DBASISTYPE Basis;
    D3DDEGREETYPE Degree;
};

struct _D3DVECTOR {
    float x;
    float y;
    float z;
};

struct _D3DLIGHT9 {
    D3DLIGHTTYPE Type;
    D3DCOLORVALUE Diffuse;
    D3DCOLORVALUE Specular;
    D3DCOLORVALUE Ambient;
    D3DVECTOR Position;
    D3DVECTOR Direction;
    float Range;
    float Falloff;
    float Attenuation0;
    float Attenuation1;
    float Attenuation2;
    float Theta;
    float Phi;
};

struct _D3DVERTEXBUFFER_DESC {
    D3DFORMAT Format;
    D3DRESOURCETYPE Type;
    DWORD Usage;
    D3DPOOL Pool;
    UINT Size;
    DWORD FVF;
};

struct _D3DVERTEXELEMENT9 {
    WORD Stream;
    WORD Offset;
    BYTE Type;
    BYTE Method;
    BYTE Usage;
    BYTE UsageIndex;
};

struct _D3DVOLUME_DESC {
    D3DFORMAT Format;
    D3DRESOURCETYPE Type;
    DWORD Usage;
    D3DPOOL Pool;
    UINT Width;
    UINT Height;
    UINT Depth;
};

struct _GUID {
    long unsigned int Data1;
    short unsigned int Data2;
    short unsigned int Data3;
    unsigned char Data4[8];
};

struct _List_node_base {
    int _M_next;
    int _M_prev;
};

struct IDirect3DBaseTexture9 {
    struct netProfileStream_t send;
    const D3DRECT recieve;
};

struct tagPALETTEENTRY {
    BYTE peRed;
    BYTE peGreen;
    BYTE peBlue;
    BYTE peFlags;
};

struct tagPOINT {
    LONG x;
    LONG y;
};

struct _D3DADAPTER_IDENTIFIER9 {
    char Driver[512];
    char Description[512];
    char DeviceName[32];
    struct LARGE_INTEGER DriverVersion;
    DWORD VendorId;
    DWORD DeviceId;
    DWORD SubSysId;
    DWORD Revision;
    GUID DeviceIdentifier;
    DWORD WHQLLevel;
};

struct _RGNDATAHEADER {
    DWORD dwSize;
    DWORD iType;
    DWORD nCount;
    DWORD nRgnSize;
    RECT rcBound;
};

struct _RGNDATA {
    RGNDATAHEADER rdh;
    char Buffer[1];
};

struct CD3DXBuffer {
    int _placeholder;
};

struct CD3DXConstantTable {
    int _placeholder;
};

struct COpenGLARBFragmentProgram {
    int _placeholder;
};

struct COpenGLATITextFragmentShader {
    int _placeholder;
};

struct COpenGLNVidiaRegisterCombinersProgram {
    int _placeholder;
};

struct D3DXMATRIX {
    int _placeholder;
};

struct D3DXVECTOR4 {
    FLOAT x;
    FLOAT y;
    FLOAT z;
    FLOAT w;
};

struct FIFO {
    int _vptr$FIFO;
};

struct ID3DXBuffer {
    int _placeholder;
};

struct ID3DXInclude {
    int _vptr$ID3DXInclude;
};
#endif
