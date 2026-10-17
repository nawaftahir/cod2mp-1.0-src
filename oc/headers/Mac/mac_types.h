#ifndef CLEAN_MAC_MAC_TYPES_H
#define CLEAN_MAC_MAC_TYPES_H

#include "../cod2_fwd.h"

typedef enum D3DBACKBUFFER_TYPE _D3DBACKBUFFER_TYPE;
typedef enum D3DBASISTYPE _D3DBASISTYPE;
typedef enum D3DDEGREETYPE _D3DDEGREETYPE;
typedef enum D3DDEVTYPE _D3DDEVTYPE;
typedef enum D3DLIGHTTYPE _D3DLIGHTTYPE;
typedef enum D3DMULTISAMPLE_TYPE _D3DMULTISAMPLE_TYPE;
typedef enum D3DPOOL _D3DPOOL;
typedef enum D3DPRIMITIVETYPE _D3DPRIMITIVETYPE;
typedef enum D3DQUERYTYPE _D3DQUERYTYPE;
typedef enum D3DRENDERSTATETYPE _D3DRENDERSTATETYPE;
typedef enum D3DSAMPLERSTATETYPE _D3DSAMPLERSTATETYPE;
typedef enum D3DSWAPEFFECT _D3DSWAPEFFECT;
typedef enum D3DTEXTURESTAGESTATETYPE _D3DTEXTURESTAGESTATETYPE;
typedef enum D3DTEXTURETRANSFORMFLAGS _D3DTEXTURETRANSFORMFLAGS;
typedef enum D3DTRANSFORMSTATETYPE _D3DTRANSFORMSTATETYPE;
typedef struct ATI4CompsConverterABGR ATI4CompsConverterABGR;
typedef struct ATI4CompsConverterARGB ATI4CompsConverterARGB;
typedef struct AbsoluteTime AbsoluteTime;
typedef struct AlertStdCFStringAlertParamRec AlertStdCFStringAlertParamRec;
typedef struct AudioChannelLayout AudioChannelLayout;
typedef struct CBaseVA CBaseVA;
typedef struct CCacheInfoBlock CCacheInfoBlock;
typedef struct CCallOfDutyEngine CCallOfDutyEngine;
typedef struct CColorConverter CColorConverter;
typedef struct CDirect3D CDirect3D;
typedef struct CDirect3DCubeTexture CDirect3DCubeTexture;
typedef struct CDirect3DDevice CDirect3DDevice;
typedef struct CDirect3DIndexBuffer CDirect3DIndexBuffer;
typedef struct CDirect3DPixelShader CDirect3DPixelShader;
typedef struct CDirect3DSurface CDirect3DSurface;
typedef struct CDirect3DSwapChain CDirect3DSwapChain;
typedef struct CDirect3DTexture CDirect3DTexture;
typedef struct CDirect3DVertexBuffer CDirect3DVertexBuffer;
typedef struct CDirect3DVertexDeclaration CDirect3DVertexDeclaration;
typedef struct CDirect3DVertexShader CDirect3DVertexShader;
typedef struct CDirect3DVolume CDirect3DVolume;
typedef struct CDirect3DVolumeTexture CDirect3DVolumeTexture;
typedef struct CMacGameEngine CMacGameEngine;
typedef struct CMemoryBuffer CMemoryBuffer;
typedef struct COpenGL COpenGL;
typedef struct COpenGLMatrix COpenGLMatrix;
typedef struct COpenGLTexture COpenGLTexture;
typedef struct COpenGLVAO COpenGLVAO;
typedef struct COpenGLVertexProgram COpenGLVertexProgram;
typedef struct CStaticCacheInfo CStaticCacheInfo;
typedef struct CTexStage CTexStage;
typedef struct CTexUnit CTexUnit;
typedef struct CVAOPacketFixedFunction CVAOPacketFixedFunction;
typedef struct CVAOPacketProgrammable CVAOPacketProgrammable;
typedef struct ComponentDescription ComponentDescription;
typedef struct ComponentRecord ComponentRecord;
typedef struct D3DTEXTUREOP D3DTEXTUREOP;
typedef struct GfxCmdApplyLatePostEffects GfxCmdApplyLatePostEffects;
typedef struct GfxCmdBeginView GfxCmdBeginView;
typedef struct GfxCmdBlendSavedScreen GfxCmdBlendSavedScreen;
typedef struct GfxCmdDrawFullScreenColoredQuad GfxCmdDrawFullScreenColoredQuad;
typedef struct GfxImageFilter GfxImageFilter;
typedef struct HDC__ HDC__;
typedef struct HFSUniStr255 HFSUniStr255;
typedef struct HICommand HICommand;
typedef struct HMONITOR__ HMONITOR__;
typedef struct IDirect3DResource9 IDirect3DResource9;
typedef struct IDirect3DStateBlock9 IDirect3DStateBlock9;
typedef struct IDirect3DVolume9 IDirect3DVolume9;
typedef struct IID IID;
typedef struct IUnknown IUnknown;
typedef struct LSItemInfoRecord LSItemInfoRecord;
typedef struct LightInfoType LightInfoType;
typedef struct OpenGLStateInfoType OpenGLStateInfoType;
typedef struct ParameterType ParameterType;
typedef struct ProgrammableShaderType ProgrammableShaderType;
typedef struct StThreadLock StThreadLock;
typedef struct StdConverterABGR StdConverterABGR;
typedef struct TextureInfoType TextureInfoType;
typedef struct UnsignedWide UnsignedWide;
typedef struct VertexProgramState VertexProgramState;
typedef struct _Alloc_hider _Alloc_hider;
typedef struct _D3DDISPLAYMODE _D3DDISPLAYMODE;
typedef union _LARGE_INTEGER _LARGE_INTEGER;
typedef struct _List_impl _List_impl;
typedef struct _Rb_tree_node_base _Rb_tree_node_base;
typedef struct _Rep _Rep;
typedef struct _Rep_base _Rep_base;
typedef struct _Vector_impl _Vector_impl;
typedef enum ConverterType ConverterType;
typedef enum OpenGLPixelShaderType OpenGLPixelShaderType;
typedef enum OwnershipType OwnershipType;
typedef enum SurfaceType SurfaceType;
typedef enum VAOStatus VAOStatus;
typedef enum _D3DTEXTUREADDRESS _D3DTEXTUREADDRESS;
typedef enum _D3DTEXTUREOP _D3DTEXTUREOP;
typedef struct _D3DADAPTER_IDENTIFIER9 D3DADAPTER_IDENTIFIER9;
typedef struct _D3DBOX D3DBOX;
typedef struct _D3DCLIPSTATUS9 D3DCLIPSTATUS9;
typedef struct _D3DDEVICE_CREATION_PARAMETERS D3DDEVICE_CREATION_PARAMETERS;
typedef struct _D3DGAMMARAMP D3DGAMMARAMP;
typedef struct _D3DINDEXBUFFER_DESC D3DINDEXBUFFER_DESC;
typedef struct _D3DLIGHT9 D3DLIGHT9;
typedef struct _D3DLOCKED_BOX D3DLOCKED_BOX;
typedef struct _D3DLOCKED_RECT D3DLOCKED_RECT;
typedef struct _D3DMATERIAL9 D3DMATERIAL9;
typedef struct _D3DPRESENT_PARAMETERS_ D3DPRESENT_PARAMETERS;
typedef struct _D3DRASTER_STATUS D3DRASTER_STATUS;
typedef struct _D3DRECTPATCH_INFO D3DRECTPATCH_INFO;
typedef struct _D3DSURFACE_DESC D3DSURFACE_DESC;
typedef struct _D3DTRIPATCH_INFO D3DTRIPATCH_INFO;
typedef struct _D3DVERTEXBUFFER_DESC D3DVERTEXBUFFER_DESC;
typedef struct _D3DVERTEXELEMENT9 D3DVERTEXELEMENT9;
typedef struct _D3DVOLUME_DESC D3DVOLUME_DESC;
typedef struct tagPALETTEENTRY PALETTEENTRY;
typedef struct tagPOINT POINT;
typedef struct _RGNDATA RGNDATA;
typedef const __CFDictionary * CFDictionaryRef;
typedef uint32_t CGTableCount;
typedef long int (*Component)();
typedef OpaqueContextRef * ContextRef;
typedef _D3DCUBEMAP_FACES D3DCUBEMAP_FACES;
typedef _D3DTEXTUREFILTERTYPE D3DTEXTUREFILTERTYPE;
typedef OpaqueEventHandlerRef * EventHandlerRef;
typedef OpaqueEventRef * EventRef;
typedef OpaqueFSIterator * FSIterator;
typedef double GLdouble;
typedef unsigned char GLubyte;
typedef HDC__ * HDC;
typedef HMONITOR__ * HMONITOR;
typedef short unsigned int * PUINT16;
typedef long unsigned int ULONG;
typedef unsigned long VAOSet;
typedef long unsigned int __darwin_size_t;
typedef int (*__vtbl_ptr_type)();
typedef int kern_return_t;
typedef DialogPtr DialogRef;
typedef LONG HRESULT;
typedef SInt16 DialogItemIndex;
typedef UInt16 UniChar;
typedef AudioObjectID AudioDeviceID;
typedef io_object_t io_service_t;

typedef struct AEDesc AEDesc;
typedef struct AudioBufferList AudioBufferList;
typedef struct AudioStreamBasicDescription AudioStreamBasicDescription;
typedef struct AudioStreamPacketDescription AudioStreamPacketDescription;
typedef struct CAEDesc CAEDesc;
typedef struct CAEEvent CAEEvent;
typedef struct CAEObject CAEObject;
typedef struct CAERecord CAERecord;
typedef struct CAETarget CAETarget;
typedef struct CAStreamBasicDescription CAStreamBasicDescription;
typedef struct CDisplayInfo CDisplayInfo;
typedef struct CMutex CMutex;
typedef struct CSampleSound CSampleSound;
typedef struct CSoundEngine CSoundEngine;
typedef struct CSoundObject CSoundObject;
typedef struct CStreamSound CStreamSound;
typedef struct CThread CThread;
typedef struct ControlID ControlID;
typedef struct ControlKind ControlKind;
typedef struct D3DXVECTOR3 D3DXVECTOR3;
typedef struct FSSpec FSSpec;
typedef struct CGGammaValue CGGammaValue;
typedef struct ControlFontStyleRec ControlFontStyleRec;
typedef struct OpaqueContextRef OpaqueContextRef;
typedef struct AudioTimeStamp AudioTimeStamp;
typedef struct StMutexLock StMutexLock;
typedef struct StPortState StPortState;
typedef struct StShowCursor StShowCursor;
typedef struct FFItem FFItem;
typedef struct WinCursor WinCursor;
typedef struct WinIcon WinIcon;
typedef struct WinVars_t WinVars_t;
typedef struct CRectList CRectList;
typedef struct _WIN32_FIND_DATAA _WIN32_FIND_DATAA;

enum ConverterType {
    ATI_4_COMPS_CONVERTER_ARGB = 0,
    ATI_4_COMPS_CONVERTER_ABGR = 1,
    STD_CONVERTER_ARGB = 2,
    STD_CONVERTER_ABGR = 3,
    UNKNOWN_CONVERTER = 4
};

enum OpenGLPixelShaderType {
    ATITextFragmentProgram = 0,
    NVidiaRegisterCombinersProgram = 1,
    ARBFragmentProgram = 2
};

enum OwnershipType {
    IS_OWNER = 0,
    IS_NOT_OWNER = 1
};

enum SurfaceType {
    TEXTURE_SURFACE = 0,
    CUBE_SURFACE = 1,
    VOLUME_SURFACE = 2
};

enum VAOStatus {
    UNINITIALIZED_VAO_STATUS = 0,
    USING_GENERIC_VAO = 1,
    USING_VIRGIN_VAO = 2,
    USING_CACHED_VAO = 3
};

enum _D3DTEXTUREADDRESS {
    D3DTADDRESS_WRAP = 1,
    D3DTADDRESS_MIRROR = 2,
    D3DTADDRESS_CLAMP = 3,
    D3DTADDRESS_BORDER = 4,
    D3DTADDRESS_MIRRORONCE = 5,
    D3DTADDRESS_FORCE_DWORD = 2147483647
};

enum _D3DTEXTUREOP {
    D3DTOP_DISABLE = 1,
    D3DTOP_SELECTARG1 = 2,
    D3DTOP_SELECTARG2 = 3,
    D3DTOP_MODULATE = 4,
    D3DTOP_MODULATE2X = 5,
    D3DTOP_MODULATE4X = 6,
    D3DTOP_ADD = 7,
    D3DTOP_ADDSIGNED = 8,
    D3DTOP_ADDSIGNED2X = 9,
    D3DTOP_SUBTRACT = 10,
    D3DTOP_ADDSMOOTH = 11,
    D3DTOP_BLENDDIFFUSEALPHA = 12,
    D3DTOP_BLENDTEXTUREALPHA = 13,
    D3DTOP_BLENDFACTORALPHA = 14,
    D3DTOP_BLENDTEXTUREALPHAPM = 15,
    D3DTOP_BLENDCURRENTALPHA = 16,
    D3DTOP_PREMODULATE = 17,
    D3DTOP_MODULATEALPHA_ADDCOLOR = 18,
    D3DTOP_MODULATECOLOR_ADDALPHA = 19,
    D3DTOP_MODULATEINVALPHA_ADDCOLOR = 20,
    D3DTOP_MODULATEINVCOLOR_ADDALPHA = 21,
    D3DTOP_BUMPENVMAP = 22,
    D3DTOP_BUMPENVMAPLUMINANCE = 23,
    D3DTOP_DOTPRODUCT3 = 24,
    D3DTOP_MULTIPLYADD = 25,
    D3DTOP_LERP = 26,
    D3DTOP_FORCE_DWORD = 2147483647
};

struct ATI4CompsConverterABGR {
    int _placeholder;
};

struct ATI4CompsConverterARGB {
    int _placeholder;
};

struct AbsoluteTime {
    UInt32 lo;
    UInt32 hi;
};

struct AlertStdCFStringAlertParamRec {
    UInt32 version;
    Boolean movable;
    Boolean helpButton;
    CFStringRef defaultText;
    CFStringRef cancelText;
    CFStringRef otherText;
    SInt16 defaultButton;
    SInt16 cancelButton;
    UInt16 position;
    OptionBits flags;
};

struct CBaseVA {
    int _vptr$CBaseVA;
};

struct CCacheInfoBlock {
    int _vptr$CCacheInfoBlock;
};

struct CCallOfDutyEngine {
    int _placeholder;
};

struct CColorConverter {
    int _vptr$CColorConverter;
};

struct CDirect3D {
    int _placeholder;
};

struct CDirect3DCubeTexture {
    int _placeholder;
};

struct CDirect3DDevice {
    int _placeholder;
};

struct CDirect3DIndexBuffer {
    int _placeholder;
};

struct CDirect3DPixelShader {
    int _placeholder;
};

struct CDirect3DSurface {
    int _placeholder;
};

struct CDirect3DSwapChain {
    int _placeholder;
};

struct CDirect3DTexture {
    int _placeholder;
};

struct CDirect3DVertexBuffer {
    int _placeholder;
};

struct CDirect3DVertexDeclaration {
    int _placeholder;
};

struct CDirect3DVertexShader {
    int _placeholder;
};

struct CDirect3DVolume {
    int _placeholder;
};

struct CDirect3DVolumeTexture {
    int _placeholder;
};

struct CMacGameEngine {
    int _vptr$CMacGameEngine;
};

struct CMemoryBuffer {
    int _vptr$CMemoryBuffer;
};

struct COpenGL {
    int _vptr$COpenGL;
};

struct COpenGLMatrix {
    int _placeholder;
};

struct COpenGLTexture {
    int _vptr$COpenGLTexture;
};

struct COpenGLVertexProgram {
    int _vptr$COpenGLVertexProgram;
};

struct CStaticCacheInfo {
    int _vptr$CStaticCacheInfo;
};

struct CTexStage {
    int _placeholder;
};

struct CTexUnit {
    bool mIsProgramableOnly;
    bool mTargetEnabled[3];
    const GLuint * mTexID[3];
    GLenum mTexWrapS[3];
    GLenum mTexWrapT[3];
    GLenum mTexWrapR[3];
    GLuint mTexBorderColor[3];
    GLenum mTexMinFilter[3];
    GLenum mTexMagFilter[3];
    GLfloat mTexAnisotropicFilter[3];
    GLint mTexLastLevel[3];
    GLenum mCombinerColorOp;
    GLenum mCombinerColorSource0;
    GLenum mCombinerColorOperand0;
    GLenum mCombinerColorSource1;
    GLenum mCombinerColorOperand1;
    GLenum mCombinerColorSource2;
    GLenum mCombinerColorOperand2;
    GLenum mCombinerAlphaOp;
    GLenum mCombinerAlphaSource0;
    GLenum mCombinerAlphaOperand0;
    GLenum mCombinerAlphaSource1;
    GLenum mCombinerAlphaOperand1;
    GLenum mCombinerAlphaSource2;
    GLenum mCombinerAlphaOperand2;
    GLfloat mCombinerRGBScale;
    GLfloat mCombinerAlphaScale;
    GLfloat mTexFactor[4];
    GLfloat mLodBias;
    GLfloat mTexMatrix[16];
    CTexCoordArray mTexCoordArray;
    bool mTexGenEnable[4];
    GLenum mTexGenMode[4];
};

struct CVAOPacketFixedFunction {
    int _placeholder;
};

struct CVAOPacketProgrammable {
    int _placeholder;
};

struct ComponentDescription {
    OSType componentType;
    OSType componentSubType;
    OSType componentManufacturer;
    long unsigned int componentFlags;
    long unsigned int componentFlagsMask;
};

struct ComponentRecord {
    long int data[1];
};

struct D3DTEXTUREOP {
    bool unused;
};

struct GfxCmdApplyLatePostEffects {
    GfxCmdHeader header;
    float blurRadius;
};

struct GfxCmdBlendSavedScreen {
    GfxCmdHeader header;
    int fadeMsec;
    const Material *material;
};

struct GfxCmdDrawFullScreenColoredQuad {
    GfxCmdHeader header;
    const Material *material;
    float s0;
    float t0;
    float s1;
    float t1;
    GfxColor color;
};

struct GfxImageFilter {
    int passCount;
    GfxImageFilterPass passes[16];
    GfxImage *sourceImage;
    GfxRenderTargetId finalTarget;
    GfxRenderTargetId pingPongTargets[2];
};

struct GfxCmdBeginView {
    GfxCmdHeader header;
    struct GfxSceneDef sceneDef;
    const GfxViewParms *viewParms;
    struct GfxLodParms lodParms;
    int viewCount;
};

struct HDC__ {
    int unused;
};

struct HICommand {
    UInt32 attributes;
    UInt32 commandID;
    struct { MenuRef menuRef; MenuItemIndex menuItemIndex; } menu;
};

struct HMONITOR__ {
    int unused;
};

struct IDirect3DResource9 {
    int _placeholder;
};

struct IDirect3DStateBlock9 {
    int _placeholder;
};

struct IID {
    long unsigned int Data1;
    short unsigned int Data2;
    short unsigned int Data3;
    unsigned char Data4[8];
};

struct IUnknown {
    int _vptr$IUnknown;
};

struct LSItemInfoRecord {
    LSItemInfoFlags flags;
    OSType filetype;
    OSType creator;
    CFStringRef extension;
    CFStringRef iconFileName;
    LSKindID kindID;
};

struct LightInfoType {
    bool Enabled;
    DWORD Type;
    float Range;
    struct DXVector4 Diffuse;
    DXVector4 Specular;
    DXVector4 Ambient;
    DXVector3 Direction;
    DXVector3 Position;
    DXVector3 DirectionCS;
    float Exponent;
    float Cutoff;
    float Theta;
    float Phi;
    float Falloff;
    float Attenuation0;
    float Attenuation1;
    float Attenuation2;
    DXVector3 CSDirection;
    DXVector3 CSPosition;
    float Range2;
    DXVector3 VdLd;
    float CosHalfTheta;
    float CosHalfPhi;
    float C1;
};

struct OpenGLStateInfoType {
    GLenum PName;
    OGLSITPTypes PType;
    char *SName;
};

struct ParameterType {
    UINT32 NeedsValidation;
    UINT32 v[4];
};

struct ProgrammableShaderType {
    bool mUsed;
    UINT32 mStream;
    UINT32 mStreamOffset;
    GLint mVSize;
    GLenum mVType;
    GLboolean mNormalized;
    GLsizei mStride;
};

struct StThreadLock {
    int _placeholder;
};

struct StdConverterABGR {
    int _placeholder;
};

struct TextureInfoType {
    const GLuint *mpTexID;
    GLenum mTexWrapS;
    GLenum mTexWrapT;
    GLenum mTexWrapR;
    GLuint mTexBorderColor;
    GLenum mTexMinFilter;
    GLenum mTexMagFilter;
    GLint mTexLastLevel;
    GLfloat mTexAnisotropicFilter;
};

struct UnsignedWide {
    UInt32 lo;
    UInt32 hi;
};

struct COpenGLVAO {
    int _vptr$COpenGLVAO;
    const void *mpVAStart;
    UINT32 mSize;
    struct CColorArray mColorArray;
    struct CSecondaryColorArray mSecondaryColorArray;
    struct CNormalArray mNormalArray;
    struct CVertexArray mVertexArray;
    CTexCoordArray mTexCoordArrays[8];
    VertexProgramStreamState mGenericArrays[16];
};

struct VertexProgramState {
    GLuint mOpenGLProgramID;
    VertexProgramStreamState mStreams[16];
};

struct _Alloc_hider {
    char *_M_p;
};

struct _D3DDISPLAYMODE {
    UINT Width;
    UINT Height;
    UINT RefreshRate;
    D3DFORMAT Format;
};

union _LARGE_INTEGER {
    struct { DWORD LowPart; LONG HighPart; };
    struct { DWORD LowPart; LONG HighPart; } u;
    LONGLONG QuadPart;
};

struct _List_impl {
    struct _List_node_base _M_node;
};

struct _Rb_tree_node_base {
    _Rb_tree_color _M_color;
    int _M_parent;
    int _M_left;
    int _M_right;
};

struct _Rep {
    int _placeholder;
};

struct _Rep_base {
    size_t _M_length;
    size_t _M_capacity;
    _Atomic_word _M_refcount;
};

struct _Vector_impl {
    MacRect *_M_start;
    MacRect *_M_finish;
    MacRect *_M_end_of_storage;
};

struct HFSUniStr255 {
    FSRef component_id;
    FSRef component_index;
    FSRef h_samp_factor;
    FSRef v_samp_factor;
    FSRef quant_tbl_no;
    FSRef dc_tbl_no;
    FSRef ac_tbl_no;
    const __CFString *width_in_blocks;
    const __CFString *height_in_blocks;
    FSRef DCT_scaled_size;
    const __CFString *downsampled_width;
    const __CFString *downsampled_height;
    KeyStruct *component_needed;
    FSRef MCU_width;
    FSRef MCU_height;
    FSRef MCU_blocks;
    FSRef MCU_sample_width;
    FSRef last_col_width;
    FSRef last_row_height;
    OffhandClass *quant_table;
    const FSRef *dct_table;
};

struct IDirect3DVolume9 {
    const char *next;
    const TextureInfoType *marker;
    const COpenGLTexture *original_length;
    const COpenGLTexture *data_length;
    unsigned char *data;
};

struct AudioChannelDescription {
    AudioChannelLabel mChannelLabel;
    UInt32 mChannelFlags;
    Float32 mCoordinates[3];
};

struct AudioChannelLayout {
    AudioChannelLayoutTag mChannelLayoutTag;
    UInt32 mChannelBitmap;
    UInt32 mNumberChannelDescriptions;
    AudioChannelDescription mChannelDescriptions[1];
};

struct AEDesc {
    DescType descriptorType;
    AEDataStorage dataHandle;
};

struct AudioBufferList {
    UInt32 mNumberBuffers;
    AudioBuffer mBuffers[1];
};

struct AudioStreamBasicDescription {
    Float64 mSampleRate;
    UInt32 mFormatID;
    UInt32 mFormatFlags;
    UInt32 mBytesPerPacket;
    UInt32 mFramesPerPacket;
    UInt32 mBytesPerFrame;
    UInt32 mChannelsPerFrame;
    UInt32 mBitsPerChannel;
    UInt32 mReserved;
};

struct AudioStreamPacketDescription {
    SInt64 mStartOffset;
    UInt32 mVariableFramesInPacket;
    UInt32 mDataByteSize;
};

struct CAEDesc {
    int _placeholder;
};

struct CAEEvent {
    int _placeholder;
};

struct CAEObject {
    int _placeholder;
};

struct CAERecord {
    int _placeholder;
};

struct CAETarget {
    int _placeholder;
};

struct CAStreamBasicDescription {
    int _placeholder;
};

struct CDisplayInfo {
    CGDirectDisplayID mDisplayID;
    CGRect mBounds;
    CResList mResolutions;
    string mCardDescription;
    UInt32 mCardVendorID;
    UInt32 mCardDeviceID;
    int mCardType;
    string mGLVendor;
    string mGLRenderer;
    string mGLExtensions;
    SInt32 mVideoMemory;
    SInt32 mTextureMemory;
    GLint mMaxTextureUnits;
    GLint mMaxTextureImageUnits;
    UInt32 mPCPixelShaderVersion;
    Boolean mIsValid;
    Boolean mSupportsSeparateBlendFunc;
    Boolean mSupportsAnisotropicFiltering;
    float mMaxSupportedAnisotropy;
    GLint mMaxSampleBuffers;
    GLint mMaxSamples;
    Boolean mDoesSuperSampling;
    Boolean mDoesMultiSampling;
    Boolean mDoesAlphaSampling;
};

struct CMutex {
    int _placeholder;
};

struct CSampleSound {
    int _placeholder;
};

struct CSoundEngine {
    int _placeholder;
};

struct CSoundObject {
    int _vptr$CSoundObject;
};

struct CStreamSound {
    int _placeholder;
};

struct CThread {
    int _vptr$CThread;
};

struct ControlID {
    OSType signature;
    SInt32 id;
};

struct ControlKind {
    OSType signature;
    OSType kind;
};

struct D3DXVECTOR3 {
    int _placeholder;
};

struct FSSpec {
    short int vRefNum;
    long int parID;
    StrFileName name;
};

struct CGGammaValue {
    CFStringRef quantval;
    ITabHandle sent_table;
};

struct ControlFontStyleRec {
    SInt16 flags;
    SInt16 font;
    SInt16 size;
    SInt16 style;
    SInt16 mode;
    SInt16 just;
    struct MacRGBColor foreColor;
    MacRGBColor backColor;
};

struct OpaqueContextRef {
    AGLContext mContext;
    AGLDrawable mDrawable;
    GLint mRendererID;
    Boolean mDoubleBuffered;
};

struct AudioTimeStamp {
    Float64 mSampleTime;
    UInt64 mHostTime;
    Float64 mRateScalar;
    UInt64 mWordClockTime;
    struct SMPTETime mSMPTETime;
    UInt32 mFlags;
    UInt32 mReserved;
};

struct StMutexLock {
    int _placeholder;
};

struct StPortState {
    int _placeholder;
};

struct StShowCursor {
    int _placeholder;
};

struct FFItem {
    struct FSRef ref;
    char name[260];
    UTCDateTime modDate;
    UInt16 flags;
};

struct WinCursor {
    int _placeholder;
};

struct WinIcon {
    int _placeholder;
};

struct WinVars_t {
    HINSTANCE reflib_library;
    qboolean reflib_active;
    HWND hWnd;
    HINSTANCE hInstance;
    qboolean activeApp;
    qboolean isMinimized;
    unsigned int sysMsgTime;
};

struct CRectList {
    MacRGBColor data[1];
};

struct _WIN32_FIND_DATAA {
    DWORD dwFileAttributes;
    FILETIME ftCreationTime;
    FILETIME ftLastAccessTime;
    FILETIME ftLastWriteTime;
    DWORD nFileSizeHigh;
    DWORD nFileSizeLow;
    DWORD dwReserved0;
    DWORD dwReserved1;
    CHAR cFileName[260];
    CHAR cAlternateFileName[14];
    DWORD dwFileType;
    DWORD dwCreatorType;
    WORD wFinderFlags;
};
#endif
