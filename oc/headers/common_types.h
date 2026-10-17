#include "cod2_feature_config.h"
#include "headers/cod2_types.h"

#if 0
#ifndef COMMON_TYPES_H
#define COMMON_TYPES_H

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <sys/types.h>
#ifndef _WIN32
#include <netinet/in.h>
#endif
#include <pthread.h>
#include <stdio.h>
#include "cod2_feature_config.h"

#if defined(__i386__) && defined(__GNUC__) && !defined(__clang__) && !defined(__EMSCRIPTEN__)
#define COD2_GCC_X86_ABI 1
#else
#define COD2_GCC_X86_ABI 0
#endif

#if COD2_GCC_X86_ABI && defined(__APPLE_CC__)
#define COD2_APPLE_GCC_X86_ABI 1
#else
#define COD2_APPLE_GCC_X86_ABI 0
#endif

#define __attribute_regparm__(n) __attribute__((regparm(n)))
#if COD2_GCC_X86_ABI
#define __attribute_sseregparm__ __attribute__((sseregparm))
#else
#define __attribute_sseregparm__
#endif

#if COD2_GCC_X86_ABI
#define COD2_REGPARM(n) __attribute__((regparm(n)))
#define COD2_SSEREGPARM __attribute__((sseregparm))
#define COD2_CDECL __attribute__((cdecl))
#define COD2_STDCALL __attribute__((stdcall))
#define COD2_FASTCALL __attribute__((fastcall))
#define COD2_FORCE_ALIGN_ARG_POINTER __attribute__((force_align_arg_pointer))
#define COD2_ALIGNED(n) __attribute__((aligned(n)))
#else
#define COD2_REGPARM(n)
#define COD2_SSEREGPARM
#define COD2_CDECL
#define COD2_STDCALL
#define COD2_FASTCALL
#define COD2_FORCE_ALIGN_ARG_POINTER
#define COD2_ALIGNED(n)
#endif

#if COD2_APPLE_GCC_X86_ABI
#define COD2_REGPARM_AND_STACKPARM __attribute__((regparmandstackparm))
#else
#define COD2_REGPARM_AND_STACKPARM
#endif

typedef int BOOL;
typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned int dword;
typedef int qboolean;
typedef unsigned char qbyte;

struct AEDesc;
struct AEDescList;
struct AERecord;
struct ANIHeader;
struct ATI4CompsConverterABGR;
struct ATI4CompsConverterARGB;
struct AURenderCallbackStruct;
struct AbsoluteTime;
struct AccessParam;
struct AlertStdCFStringAlertParamRec;
struct AntilagClientStore;
struct AppleEvent;
struct AttribInfo;
struct AudioBuffer;
struct AudioBufferList;
struct AudioChannelDescription;
struct AudioChannelLayout;
struct AudioStreamBasicDescription;
struct AudioStreamPacketDescription;
struct AudioTimeStamp;
struct BackCompatibleParameters;
struct BreakStatementInfo;
struct BuiltInMaterialTable;
struct BuiltinFunctionDef;
struct BuiltinMethodDef;
struct CAEDesc;
struct CAEEvent;
struct CAEObject;
struct CAERecord;
struct CAETarget;
struct CAStreamBasicDescription;
struct CAudioRecorder;
struct CBaseVA;
struct CCacheInfoBlock;
struct CCallOfDutyEngine;
struct CCircularBuffer;
struct CColorArray;
struct CColorConverter;
struct CD3DXBuffer;
struct CD3DXConstantTable;
struct CDirect3D;
struct CDirect3DCubeTexture;
struct CDirect3DDevice;
struct CDirect3DIndexBuffer;
struct CDirect3DPixelShader;
struct CDirect3DSurface;
struct CDirect3DSwapChain;
struct CDirect3DTexture;
struct CDirect3DVertexBuffer;
struct CDirect3DVertexDeclaration;
struct CDirect3DVertexShader;
struct CDirect3DVolume;
struct CDirect3DVolumeTexture;
struct CDisplayInfo;
struct CFUUIDBytes;
struct CFence;
struct CGGammaValue;
struct CGPoint;
struct CGRect;
struct CGSize;
union CInfoPBRec;
struct CMMapFile;
struct CMacGameEngine;
struct CMacLogFile;
struct CMemoryBuffer;
struct CMutex;
struct CNormalArray;
struct COpenGL;
struct COpenGLARBFragmentProgram;
struct COpenGLATITextFragmentShader;
struct COpenGLMatrix;
struct COpenGLNVidiaRegisterCombinersProgram;
struct COpenGLTexture;
struct COpenGLVAO;
struct COpenGLVertexProgram;
struct CProcRec;
struct CRectList;
struct CResInfo;
struct CSParam;
struct CSampleSound;
struct CSecondaryColorArray;
struct CSoundEngine;
struct CSoundObject;
struct CStaticCacheInfo;
struct CStreamSound;
struct CStringEdPackage;
struct CTexCoordArray;
struct CTexStage;
struct CTexUnit;
struct CThread;
struct CVAOPacket;
struct CVAOPacketFixedFunction;
struct CVAOPacketProgrammable;
struct CVertexArray;
struct CachedAssets_t;
struct CardMemory;
struct CaseStatementInfo;
struct CatPositionRec;
struct ClientVoicePacket_t;
struct Cloud;
struct CodeConstantSource;
struct CodeSamplerSource;
struct CollisionAabbTree;
union CollisionAabbTreeIndex;
struct CollisionBorder;
struct CollisionEdge;
struct CollisionPartition;
struct CollisionTriangle;
struct CollisionVertex;
struct ColorSpec;
struct ColorTable;
struct CombFilterMem;
struct ComponentDescription;
struct ComponentInstanceRecord;
struct ComponentRecord;
struct CompressionInfo;
struct ConDrawInputGlob;
struct Console;
struct ContinueStatementInfo;
struct ControlEditTextSelectionRec;
struct ControlFontStyleRec;
struct ControlID;
struct ControlKind;
struct ConversionArguments;
struct CopyParam;
struct Cursor;
struct Cylinder;
struct D3DDISPLAYMODE;
struct D3DTEXTUREOP;
struct D3DXFLOAT16;
struct D3DXMATRIX;
struct D3DXVECTOR3;
struct D3DXVECTOR4;
struct DIR;
struct DInfo;
struct DObjAnimMat;
struct DObjModel_s;
struct DObjSkelMat_s;
struct DObjTrace_s;
struct DObj_s;
struct DSkel;
struct DSurface_s;
struct DXInfo;
struct DXTexColor;
struct DXVector3;
struct DXVector4;
struct DateTimeRec;
struct DebugGlobals;
struct DecState;
struct DirInfo;
struct DiskCollAabbTree;
struct DiskCollBorder;
struct DiskCollEdge;
struct DiskCollPartition;
struct DiskCollTriangle;
struct DiskCollVertex;
struct DiskGfxAabbTree;
struct DiskGfxCell;
struct DiskGfxCullGroup;
struct DiskGfxOccluder;
struct DiskGfxOccluderEdge;
struct DiskGfxPortal;
struct DiskGfxVertex;
struct DiskLeaf;
struct DiskTriangleSoup;
struct DpvsDebug;
struct DpvsPlane;
union DpvsPlaneInfo;
struct DpvsScene;
union DvarLimits;
union DvarValue;
struct DxCapsCheckBits;
struct DxCapsCheckInteger;
struct DxGlobals;
struct DxState;
struct DxStencilDecode;
struct DxTextureOpDecode;
struct DxTextureStageEnums;
struct Effect;
struct EffectCluster;
struct EffectPrimitive;
struct FxEffectDef;
union EffectTemplateLoadPtr;
struct EffectVisInfo;
struct Emitter;
struct EncState;
struct EventRecord;
struct EventTypeSpec;
struct FFData;
struct FFItem;
struct FIDParam;
struct FIFO;
struct FInfo;
struct FSCatalogInfo;
struct FSRef;
struct FSSpec;
struct FSVolumeInfo;
struct FXInfo;
struct Flash;
struct Font_s;
struct ForeignPrivParam;
struct FreeRequest;
struct FxArchive;
struct FxBoltFrame;
struct FxBoltFramePtr;
struct FxBoltInfo;
struct FxCamera;
struct FxChannel;
struct FxChannelBackwardCompatible;
struct FxChannelInstance;
struct FxCurve;
struct FxCurveIterator;
struct FxFlagEntry;
struct FxGfxEntity;
struct FxHelper;
struct FxImpactTable;
struct FxMemMgr_Cloud;
struct FxMemMgr_Cylinder;
struct FxMemMgr_Effect;
struct FxMemMgr_Emitter;
struct FxMemMgr_FxBoltFrame;
struct FxMemMgr_Light;
struct FxMemMgr_Line;
struct FxMemMgr_OrientedParticle;
struct FxMemMgr_Particle;
struct FxMemMgr_PrimitiveTemplate;
struct FxMemMgr_ScheduledEffect;
struct FxMemMgr_Tail;
struct FxRange;
struct FxScheduler;
struct GDevice;
struct GPGroup;
struct GPObject;
struct GPValue;
struct GenericParser2;
struct GetVolParmsInfoBuffer;
struct GfxAabbTree;
struct GfxBackEndData;
struct GfxBackEndLightSettings;
struct GfxBrushModel;
struct GfxBspLoad;
struct GfxCachedShaderText;
struct GfxCell;
struct GfxCellWritable;
struct GfxCmdApplyLatePostEffects;
struct GfxCmdArray;
struct GfxCmdBeginView;
struct GfxCmdBlendSavedScreen;
struct GfxCmdCall;
struct GfxCmdClearScreen;
struct GfxCmdDrawFullScreenColoredQuad;
struct GfxCmdDrawLines;
struct GfxCmdDrawPoints;
struct GfxCmdDrawQuadPic;
struct GfxCmdDrawSprite;
struct GfxCmdDrawSunPostEffects;
struct GfxCmdDrawSurfs;
struct GfxCmdDrawText;
struct GfxCmdDrawTextInSpace;
struct GfxCmdDrawTriangles;
struct GfxCmdHeader;
struct GfxCmdSetClipPlanes;
struct GfxCmdSetLightProperties;
struct GfxCmdSetMaterialColor;
struct GfxCmdSetRenderTarget;
struct GfxCmdSetShadowCookie;
struct GfxCmdSetStencilRefValue;
struct GfxCmdSetViewport;
struct GfxCmdStencilPlanes;
struct GfxCmdStretchPic;
struct GfxCmdStretchPicRotate;
struct GfxCmdStretchRaw;
struct GfxCodeMatrices;
struct GfxCodeMatrix;
union GfxColor;
struct GfxCullGroup;
struct GfxCullGroupDynamic;
struct GfxDebugFrameGlob;
struct GfxDebugPlume;
struct GfxDebugPoly;
struct GfxDrawGroupCommands;
struct GfxDrawPrimArgs;
struct GfxDrawSurf;
struct GfxEntity;
struct GfxEntityLighting;
struct GfxFog;
struct GfxGammaRamp;
struct GfxImage;
struct GfxImageFileHeader;
struct GfxImageFilter;
struct GfxImageFilterPass;
struct GfxImageLoadDef;
struct GfxLight;
struct GfxLightDef;
struct GfxLightDir;
struct GfxLightGrid;
struct GfxLightGridColors;
struct GfxLightGridPoint;
struct GfxLightImage;
union GfxLightInfo;
struct GfxLodParms;
struct GfxMarkFragment;
struct GfxMarkPoint;
union GfxModel;
struct GfxModelRigidSurface;
struct GfxModelSkinnedSurface;
struct GfxModelSurface;
struct GfxOccluder;
struct GfxOccluderEdge;
struct GfxPointVertex;
struct GfxPortal;
struct GfxPortalWritable;
struct GfxPosTexVertex;
struct GfxPostProcessParms;
struct GfxRenderCommandExecState;
struct GfxRenderTarget;
struct GfxSModelCachedVertex;
struct GfxSModelCachedVertexDx7;
struct GfxScene;
struct GfxSceneDef;
struct GfxSceneEntity;
struct GfxSceneModelCellRef;
struct GfxStateOverride;
struct GfxStaticModelCachedSurface;
struct GfxStaticModelDynamic;
struct GfxStaticModelInstance;
struct GfxStaticModelSurfaceCached;
struct GfxStaticSurface;
struct GfxSurface;
struct GfxSurfaceDynamic;
union GfxTexture;
struct GfxVertex;
struct GfxVertexDx7;
struct GfxViewParms;
struct GfxViewport;
struct GfxWindowParms;
struct GfxWindowTarget;
struct GfxWorld;
struct GfxWorldVertex;
struct GfxWorldVertexData;
struct GfxWorldVertexDx7;
struct Glyph;
struct HCURSOR__;
struct HDC__;
struct HFSUniStr255;
struct HFileInfo;
struct HFileParam;
struct HICommand;
struct HINSTANCE__;
struct HIOParam;
struct HISize;
struct HMONITOR__;
union HParamBlockRec;
struct HVolumeParam;
struct HWND__;
struct HashEntry;
struct ID3DXBuffer;
struct ID3DXConstantTable;
struct ID3DXFont;
struct ID3DXInclude;
struct ID3DXSprite;
struct IDirect3D9;
struct IDirect3DBaseTexture9;
struct IDirect3DCubeTexture9;
struct IDirect3DDevice9;
struct IDirect3DIndexBuffer9;
struct IDirect3DPixelShader9;
struct IDirect3DQuery9;
struct IDirect3DResource9;
struct IDirect3DStateBlock9;
struct IDirect3DSurface9;
struct IDirect3DSwapChain9;
struct IDirect3DTexture9;
union IDirect3DVertexBuffer9;
struct IDirect3DVertexDeclaration9;
struct IDirect3DVertexShader9;
struct IDirect3DVolume9;
struct IDirect3DVolumeTexture9;
struct IID;
struct ITab;
struct IUnknown;
struct ImageDescription;
struct ImageList;
struct Image_MemUsage;
struct IncludeClass;
struct ItemKeyHandler;
struct JHUFF_TBL;
struct JQUANT_TBL;
struct KeyStruct;
struct LARGE_INTEGER;
struct LSItemInfoRecord;
struct LargeLocal;
struct LegacyHacks;
struct Light;
struct LightInfoType;
struct Line;
struct LocalDateTime;
union LongDateRec;
struct MD4_CTX;
struct MacImageInfo;
struct MacRGBColor;
struct MacRect;
struct MantleAnimTransition;
struct MantleResults;
struct MantleState;
struct MarkPoly;
struct Material;
struct MaterialArgumentCodeConst;
union MaterialArgumentDef;
struct MaterialConstantDefObj;
struct MaterialInfo;
struct MaterialInfoObj_s;
struct MaterialMemory;
struct MaterialObj;
union MaterialPassArray;
struct MaterialPassDx7;
struct MaterialPassDx9;
struct MaterialShader;
struct MaterialShaderArgument;
union MaterialShaderPtr;
struct MaterialStateMap;
struct MaterialStateMapRule;
struct MaterialStateMapRuleSet;
struct MaterialStreamRouting;
struct MaterialTechnique;
struct MaterialTechniqueSet;
struct MaterialTextureDef;
union MaterialTextureDefInfo;
struct MaterialVertexDeclaration;
struct MaterialWaterDef;
struct MediaHandles;
struct MemoryFile;
struct MemoryNode;
struct MenuList;
struct MessageBuffer;
struct MessageLine;
struct MessageWindow;
struct MixerDistanceParams;
struct MssLocal;
struct MssSound;
struct MtlStateMapBitGroup;
struct MtlStateMapBitName;
struct MtlTextureFunctionDx7;
struct NetField;
struct NumVersion;
struct ObjParam;
struct ObjectInfo;
struct OpaqueContextRef;
struct OpcodeLookup;
struct OpenGLStateInfoType;
struct OrientedParticle;
struct OutdoorGlob;
struct ParameterType;
struct ParseThreadInfo;
struct Particle;
struct PassOptionDx7;
struct Picmip;
struct PixMap;
struct PlayerKeyState;
struct PlayerProfileStatus;
struct Point;
struct PointLightPartition;
struct PortalHeapNode;
struct PrecacheEntry;
struct PrimitiveTemplate;
struct ProcessInfoRec;
struct ProcessSerialNumber;
struct ProgrammableShaderType;
struct QElem;
struct RTL_CRITICAL_SECTION;
struct RefString;
struct RemoveDirectoryInfo;
struct SBDecState;
struct SBEncState;
struct SMPTETime;
struct SProcRec;
struct SaveSourceBufferInfo;
struct ScheduledEffect;
struct Scr_SourcePos_t;
struct ScreenPlacementGlob;
struct ShaderConstantRouting;
struct ShadowCandidate;
struct ShadowCookie;
struct ShadowCookieGlob;
struct ShadowCookieList;
struct SkinBuffers;
struct SkinRigidXModelCmd;
struct SkinStaticModelCachedCmd;
struct SkinXModelCmd;
struct SndCurve;
struct SortedCluster;
struct SortedEffect;
struct SoundDescription;
struct SoundFile;
struct SoundFileInfo;
struct SourceBufferInfo;
struct SourceLookup;
struct SpawnVar;
struct SpeexBits;
struct SpeexCallback;
struct SpeexMode;
struct SpeexNBMode;
struct SpeexSBMode;
struct SpeexSubmode;
struct StColorConverter;
struct StGWorldState;
struct StMutexLock;
struct StPortState;
struct StSetDirectory;
struct StShowCursor;
struct StThreadLock;
struct StdConverterABGR;
struct StdConverterARGB;
struct SunFlareDynamic;
struct SunLightParseParams;
struct SysInfo;
union TMediaElement;
struct TMediaList;
struct TXNATSUIFeatures;
struct TXNATSUIVariations;
union TXNAttributeData;
struct TXNTypeAttributes;
struct Tail;
struct TestLod;
struct TextPool;
struct TextureInfoType;
struct ThreadDebugInfo;
struct TraceCheckCount;
struct TraceExtents;
struct TraceThreadInfo;
struct Tuple;
struct UI_Component;
struct UI_Component_data_t;
struct UI_LinesComponent;
struct UI_ScrollPane;
struct UI_VerticalDivider;
struct UTCDateTime;
struct UnsignedWide;
struct VBRState;
struct Variable;
struct VariableCompileValue;
struct VariableStackBuffer;
union VariableUnion;
struct VariableValueInternal;
struct VariableValue_s;
struct VertexProgramState;
struct VertexProgramStreamState;
struct VoicePacket_t;
struct VolumeModGroup;
struct WDParam;
struct WSAData;
struct WaterGlob;
struct WaterWritable;
struct WaveletDecode;
struct WaveletHuffmanDecode;
struct WeaponDef;
struct WinCursor;
struct WinIcon;
struct WinVars_t;
struct Window;
struct WindowAttributes;
struct XAnimCalcAnimInfo;
struct XAnimDeltaPart;
struct XAnimDeltaPartQuat;
union XAnimDeltaPartQuatData;
struct XAnimDeltaPartQuatDataFrames;
struct XAnimDeltaPartTrans;
union XAnimDeltaPartTransData;
struct XAnimDeltaPartTransFrames;
union XAnimDynamicIndices;
struct XAnimEntry;
struct XAnimInfo;
struct XAnimNotifyInfo;
struct XAnimNotify_s;
struct XAnimParent;
struct XAnimPart;
struct XAnimPartQuat;
union XAnimPartQuatData;
struct XAnimPartQuatDataFrames;
union XAnimPartQuatFrames;
struct XAnimPartTrans;
union XAnimPartTransData;
struct XAnimPartTransFrames;
struct XAnimParts_s;
struct XAnimSimpleRotPos;
struct XAnimState;
struct XAnimTime;
struct XAnimToXModel;
struct XAnimTree;
struct XAnimTree_s;
struct XAnim_s;
union XAssetHeader;
struct XBlendInfo_s;
struct XBoneHierarchy;
struct XBoneInfo_s;
struct XModel;
struct XModelCollSurf_s;
struct XModelCollTri_s;
struct XModelConfig;
struct XModelConfigEntry;
struct XModelDefault;
struct XModelLodInfo;
struct XModelParts_s;
struct XModelSurfs_s;
struct XRigidSurface;
struct XSurface_s;
struct XVertexBuffer;
struct XVertexInfo_s;
struct _ADPCMDATATAG;
struct _AILMIXINFO;
struct _AILSOUNDINFO;
struct _Alloc_hider;
struct _D3DADAPTER_IDENTIFIER9;
struct _D3DBOX;
struct _D3DCAPS9;
struct _D3DCLIPSTATUS9;
struct _D3DCOLORVALUE;
struct _D3DDEVICE_CREATION_PARAMETERS;
struct _D3DDISPLAYMODE;
struct _D3DGAMMARAMP;
struct _D3DINDEXBUFFER_DESC;
struct _D3DLIGHT9;
struct _D3DLOCKED_BOX;
struct _D3DLOCKED_RECT;
struct _D3DMATERIAL9;
struct _D3DMATRIX;
struct _D3DPRESENT_PARAMETERS_;
struct _D3DPSHADERCAPS2_0;
struct _D3DRASTER_STATUS;
struct _D3DRECT;
struct _D3DRECTPATCH_INFO;
struct _D3DSURFACE_DESC;
struct _D3DTRIPATCH_INFO;
struct _D3DVECTOR;
struct _D3DVERTEXBUFFER_DESC;
struct _D3DVERTEXELEMENT9;
struct _D3DVIEWPORT9;
struct _D3DVOLUME_DESC;
struct _D3DVSHADERCAPS2_0;
struct _D3DXCONSTANTTABLE_DESC;
struct _D3DXCONSTANT_DESC;
struct _D3DXFONT_DESCA;
struct _D3DXFONT_DESCW;
struct _D3DXIMAGE_INFO;
struct _D3DXMACRO;
struct _D3DXSHADER_CONSTANTINFO;
struct _D3DXSHADER_CONSTANTTABLE;
struct _D3DXSHADER_TYPEINFO;
struct _DDCOLORKEY;
struct _DDPIXELFORMAT;
struct _DDSCAPS2;
struct _DDSURFACEDESC2;
struct _Deque_impl;
struct _Deque_iterator_UINT16_UINT16ref_UINT16ptr;
struct _FILETIME;
struct _GUID;
union _LARGE_INTEGER;
struct _List_impl;
struct _List_iterator_CFenceptr;
struct _List_iterator_GLuintptr;
struct _List_iterator_Tuple;
struct _List_node_CFenceptr;
struct _List_node_GLuintptr;
struct _List_node_Tuple;
struct _List_node_base;
struct _MEMORYSTATUS;
struct _MEMORY_BASIC_INFORMATION;
struct _OSVERSIONINFOA;
struct _OVERLAPPED;
struct _RGNDATA;
struct _RGNDATAHEADER;
struct _Rb_tree_node_base;
struct _Rep;
struct _Rep_base;
struct _SECURITY_ATTRIBUTES;
struct _SYSTEMTIME;
union _ULARGE_INTEGER;
struct _Vector_impl;
struct _WIN32_FIND_DATAA;
struct __darwin_pthread_handler_rec;
struct __darwin_pthread_mutex_t;
struct __false_type;
struct __true_type;
struct _opaque_pthread_mutex_t;
struct _opaque_pthread_mutexattr_t;
struct _opaque_pthread_t;
struct allocator_CCacheInfoBlock;
struct allocator_CDirect3DSurfaceptr;
struct allocator_CDirect3DVolumeptr;
struct allocator_CDisplayInfo;
struct allocator_CFenceptr;
struct allocator_CResInfo;
struct allocator_CSoundObjectptr;
struct allocator_CStreamSoundptr;
struct allocator_FFItem;
struct allocator_FSRef;
struct allocator_GLuint;
struct allocator_GLuintptr;
struct allocator_MacRect;
struct allocator_Tuple;
struct allocator_UINT16;
struct allocator_UINT16ptr;
struct allocator_UINT32;
struct allocator_UINT8;
struct allocator_WinCursor;
struct allocator_char;
struct allocator_float;
struct animConditionTable_t;
struct animScriptCommand_t;
struct animScriptCondition_t;
struct animScriptData_t;
struct animScriptItem_t;
struct animScript_t;
struct animStringItem_t;
struct animation_s;
struct archivedEntityShared_t;
struct archivedEntity_s;
struct archivedSnapshot_s;
struct areaParms_t;
struct audioSample_t;
struct backing_store_struct;
struct bgs_s;
struct bidirectional_iterator_tag;
struct binary_function_UINT32_UINT32_bool;
struct bind_t;
struct bitread_perm_state;
struct bitread_working_state;
struct box;
struct builtin_s;
struct cLeafBrushNodeChildren_t;
union cLeafBrushNodeData_t;
struct cLeafBrushNodeLeaf_t;
struct cLeafBrushNode_s;
struct cLeaf_t;
struct cNode_t;
struct cStaticModelWritable;
struct cStaticModel_s;
struct c_derived_tbl;
struct cachedClient_s;
struct cachedSnapshot_t;
struct cameraShake_t;
struct cbrush_t;
struct cbrushside_t;
struct centity_s;
struct cgMedia_t;
struct cg_hudelem_t;
struct cg_t;
struct cgs_t;
struct challenge_t;
struct cin_cache;
struct cinematics_t;
struct clSnapshot_t;
struct clientActive_t;
struct clientConnection_t;
struct clientDebug_t;
struct clientInfo_s;
struct clientLogo_t;
struct clientSession_t;
struct clientSnapshot_t;
struct clientState_t;
struct clientStatic_t;
struct client_fields_s;
struct client_s;
struct clipMap_t;
struct cm_world_t;
struct cmd_function_s;
struct cmd_t;
struct cmodel_t;
struct columnInfo_s;
struct com_parse_mark_t;
struct commandDef_t;
struct compassactor_t;
struct complex_s;
struct consoleCommand_t;
struct controller_info_t;
struct corpseInfo_t;
struct corpse_ent_t;
struct cplane_s;
struct cspField_t;
struct d_derived_tbl;
struct dbrush_t;
struct dbrushside_t;
struct define_s;
struct dheader_s;
struct directive_s;
struct directive_t;
struct directory_t;
struct displayContextDef_s;
struct dleaf_t;
struct dmaterial_t;
struct dmodel_t;
struct dnode_t;
struct dplane_t;
struct dsound_sample_t;
struct dvar_s;
struct editFieldDef_s;
struct ent_field_t;
struct entityHandler_t;
struct entityShared_t;
struct entityState_s;
struct field_t;
struct fileData_s;
struct fileHandleData_t;
struct fileInPack_s;
struct file_in_zip_read_info_s;
struct forward_iterator_tag;
struct function_frame_t;
struct function_stack_t;
struct g_sa_type;
struct gameState_t;
struct gameTypeInfo;
struct gameTypeScript_t;
struct game_hudelem_field_t;
struct game_hudelem_s;
struct gclient_s;
struct gentity_s;
struct gitem_s;
struct grenade_ent_t;
union hudelem_color_t;
struct hudelem_s;
struct huff_entropy_decoder;
struct huff_entropy_encoder;
struct huff_t;
struct huffman_t;
struct hunkHeader_t;
struct hunkUsed_t;
struct indent_s;
struct inflate_blocks_state;
struct inflate_codes_state;
struct inflate_huft_s;
struct infoParm_t;
struct input_iterator_tag;
struct internal_state;
struct ipFilter_s;
union itemDefData_t;
struct itemDef_s;
struct itemInfo_t;
struct item_ent_t;
struct jpeg_alloc;
struct jpeg_c_coef_controller;
struct jpeg_c_main_controller;
struct jpeg_c_prep_controller;
struct jpeg_color_converter;
struct jpeg_color_deconverter;
struct jpeg_color_quantizer;
struct jpeg_common_struct;
struct jpeg_comp_master;
struct jpeg_component_info;
struct jpeg_compress_struct;
struct jpeg_d_coef_controller;
struct jpeg_d_main_controller;
struct jpeg_d_post_controller;
struct jpeg_decomp_master;
struct jpeg_decompress_struct;
struct jpeg_destination_mgr;
struct jpeg_downsampler;
struct jpeg_entropy_decoder;
struct jpeg_entropy_encoder;
struct jpeg_error_mgr;
struct jpeg_forward_dct;
struct jpeg_input_controller;
struct jpeg_inverse_dct;
struct jpeg_marker_reader;
struct jpeg_marker_struct;
struct jpeg_marker_writer;
struct jpeg_memory_mgr;
struct jpeg_progress_mgr;
struct jpeg_scan_info;
struct jpeg_source_mgr;
struct jpeg_upsampler;
struct jvirt_barray_control;
struct jvirt_sarray_control;
struct kbutton_t;
struct keyname_t;
struct keywordHash_s;
struct lagometer_t;
struct languageInfo_t;
union large_pool_struct;
struct leafList_s;
struct lerpFrame_t;
struct less_UINT32;
struct level_locals_t;
struct lightGlob_type;
struct listBoxDef_s;
struct listColumnInfo_t;
struct loadAnim_t;
struct loadAssets_t;
struct locTraceWork_t;
struct localEntity_s;
struct loopback_t;
struct loopmsg_t;
struct ltp_params;
struct lump_t;
struct mapInfo;
struct materialCommands_t;
struct menuDef_t;
union mnode_info_t;
struct mnode_leaf_t;
struct mnode_node_t;
struct mnode_t;
struct modInfo_t;
struct moveclip_t;
struct mover_ent_t;
struct msg_t;
struct multiDef_s;
struct my_coef_controller;
struct my_color_converter;
struct my_color_deconverter;
struct my_comp_master;
struct my_cquantizer;
struct my_decomp_master;
struct my_destination_mgr;
struct my_downsampler;
struct my_fdct_controller;
struct my_idct_controller;
struct my_input_controller;
struct my_main_controller;
struct my_marker_reader;
struct my_marker_writer;
struct my_memory_mgr;
struct my_post_controller;
struct my_prep_controller;
struct my_source_mgr;
struct my_upsampler;
struct netProfileInfo_s;
struct netProfilePacket_t;
struct netProfileStream_t;
struct netadr_t;
struct netchan_t;
struct new_allocator_CCacheInfoBlock;
struct new_allocator_CDirect3DSurfaceptr;
struct new_allocator_CDirect3DVolumeptr;
struct new_allocator_CDisplayInfo;
struct new_allocator_CFenceptr;
struct new_allocator_CResInfo;
struct new_allocator_CSoundObjectptr;
struct new_allocator_CStreamSoundptr;
struct new_allocator_FFItem;
struct new_allocator_FSRef;
struct new_allocator_GLuint;
struct new_allocator_GLuintptr;
struct new_allocator_MacRect;
struct new_allocator_Tuple;
struct new_allocator_UINT16;
struct new_allocator_UINT16ptr;
struct new_allocator_UINT32;
struct new_allocator_UINT8;
struct new_allocator_WinCursor;
struct new_allocator_char;
struct new_allocator_float;
struct nodetype;
struct objective_t;
struct operator_s;
struct orientation_t {
    float origin[3];
    float axis[3][3];
};
struct outPacket_t;
struct pack_t;
struct pair_UINT32_CVAOPacket;
struct pair_constUINT32_CVAOPacket;
struct parseInfo_t;
struct pc_token_s;
struct pendingServerStatus_t;
struct pendingServer_t;
struct phuff_entropy_decoder;
struct phuff_entropy_encoder;
struct ping_t;
struct pinglist_t;
struct playerEntity_t;
struct playerState_s;
struct playerTeamState_t;
struct pml_t;
struct pmoveHandler_t;
struct pmove_t;
struct pointtrace_t;
struct punctuation_s;
struct pushed_t;
union qfile_gus;
struct qfile_us;
struct qkey_t;
struct qtime_s;
struct r_backEndGlobals_t;
struct r_global_permanent_t;
struct r_globals_load_t;
struct r_globals_t;
struct r_ib_state_t;
struct r_lightmapGroup_t;
struct r_lightmapMerge_t;
struct r_vb_state_t;
struct random_access_iterator_tag;
struct rectDef_s;
struct refdef_s;
struct refexport_t;
struct refimport_t;
struct rlimit;
struct saLoadObjGlob_type;
struct savable_state;
struct sbpicinfo_t;
struct score_t;
struct scrAnimGlob_t;
struct scrAnimPub_t;
struct scrCompileDebugPub_t;
struct scrCompileGlob_t;
struct scrCompilePub_t;
struct scrMemTreeGlob_t;
struct scrMemTreePub_t;
struct scrParserGlob_t;
struct scrParserPub_t;
struct scrStringGlob_t;
struct scrVarDebugPub_t;
struct scrVarGlob_t;
struct scrVarPub_t;
struct scrVmDebugPub_t;
struct scrVmGlob_t;
struct scrVmPub_t;
struct scr_anim_s;
struct scr_animtree_t;
struct scr_block_s;
struct scr_classStruct_t;
struct scr_const_t;
struct scr_data_t;
struct scr_entref_t;
struct scr_localVar_t;
struct script_s;
struct scrollInfo_s;
struct searchpath_s;
struct serverAddress_t {
    byte ip[4];
    unsigned short port;
};
struct serverFilter_s;
struct serverInfo_t;
struct serverStatic_t;
struct serverStatusDvar_t;
struct serverStatusInfo_t;
struct serverStatus_s;
struct server_t;
struct sharedUiInfo_t;
struct shellshock_parms_t;
struct shellshock_t;
struct sightclip_t;
struct sightpointtrace_t;
union small_pool_struct;
struct snapshotEntityNumbers_t;
struct snapshot_t;
struct snd_alias_build_s;
struct snd_alias_list_s;
struct snd_alias_t;
struct snd_background_info_t;
struct snd_channel_info_t;
struct snd_channelvolgroup;
struct snd_enveffect;
struct snd_listener;
struct snd_local_t;
struct snd_overlay_info_t;
struct snd_save_2D_sample_t;
struct snd_save_3D_sample_t;
struct snd_save_stream_t;
struct snd_volume_info_t;
struct source_s;
struct spawn_t;
struct split_cb_params;
struct srfPoly_t;
struct srfTriangles_t;
struct static_model_cache_t;
union static_model_leaf_t;
struct static_model_node_list_t;
struct static_model_node_t;
struct static_model_tree_list_t;
struct static_model_tree_t;
struct staticmodeltrace_t;
struct statmonitor_s;
struct stream_dest_info_t;
struct stream_source_info_t;
struct stringDef_s;
struct stype_t;
struct sunflare_t;
struct svEntity_s;
union sval_u;
struct svscmd_info_t;
struct sysEvent_t;
struct tagBITMAPFILEHEADER;
struct tagBITMAPINFO;
struct tagBITMAPINFOHEADER;
struct tagInfo_s;
struct tagPALETTEENTRY;
struct tagPOINT;
struct tagRECT;
struct tagRGBQUAD;
struct tagTEXTMETRICA;
struct tagTEXTMETRICW;
struct tempBanSlot_t;
struct tm_unz_s;
struct token_s;
struct trDebugLine_t;
struct trDebugString_t;
struct trStatistics_t;
struct trXSkin_t;
struct traceWork_t;
struct trace_t;
struct trajectory_t;
struct trigger_ent_t;
struct trigger_info_t;
struct turretInfo_s;
struct ucmd_t;
struct uiClientState_t;
struct uiInfo_t;
struct unz_file_info_internal_s;
struct unz_file_info_s;
struct unz_global_info_s;
struct unz_s;
struct useList_t;
struct usercmd_s;
struct va_info_t;
struct value_s;
struct vidConfig_t;
struct viewDamage_t;
struct viewLerpWaypoint_s;
struct viewState_t;
struct water_t;
struct weaponInfo_s;
struct weaponParms;
struct weaponState_t;
struct windowDef_t;
struct working_state;
struct worldContents_s;
struct worldSector_s;
struct worldTree_s;
struct yy_buffer_state;
struct z_stream_s;

enum CFComparisonResult {
    kCFCompareLessThan = -1,
    kCFCompareEqualTo = 0,
    kCFCompareGreaterThan = 1
};

enum CGLError {
    kCGLNoError = 0,
    kCGLBadAttribute = 10000,
    kCGLBadProperty = 10001,
    kCGLBadPixelFormat = 10002,
    kCGLBadRendererInfo = 10003,
    kCGLBadContext = 10004,
    kCGLBadDrawable = 10005,
    kCGLBadDisplay = 10006,
    kCGLBadState = 10007,
    kCGLBadValue = 10008,
    kCGLBadMatch = 10009,
    kCGLBadEnumeration = 10010,
    kCGLBadOffScreen = 10011,
    kCGLBadFullScreen = 10012,
    kCGLBadWindow = 10013,
    kCGLBadAddress = 10014,
    kCGLBadCodeModule = 10015,
    kCGLBadAlloc = 10016,
    kCGLBadConnection = 10017
};

enum CGLPixelFormatAttribute {
    kCGLPFAAllRenderers = 1,
    kCGLPFADoubleBuffer = 5,
    kCGLPFAStereo = 6,
    kCGLPFAAuxBuffers = 7,
    kCGLPFAColorSize = 8,
    kCGLPFAAlphaSize = 11,
    kCGLPFADepthSize = 12,
    kCGLPFAStencilSize = 13,
    kCGLPFAAccumSize = 14,
    kCGLPFAMinimumPolicy = 51,
    kCGLPFAMaximumPolicy = 52,
    kCGLPFAOffScreen = 53,
    kCGLPFAFullScreen = 54,
    kCGLPFASampleBuffers = 55,
    kCGLPFASamples = 56,
    kCGLPFAAuxDepthStencil = 57,
    kCGLPFAColorFloat = 58,
    kCGLPFAMultisample = 59,
    kCGLPFASupersample = 60,
    kCGLPFASampleAlpha = 61,
    kCGLPFARendererID = 70,
    kCGLPFASingleRenderer = 71,
    kCGLPFANoRecovery = 72,
    kCGLPFAAccelerated = 73,
    kCGLPFAClosestPolicy = 74,
    kCGLPFARobust = 75,
    kCGLPFABackingStore = 76,
    kCGLPFAMPSafe = 78,
    kCGLPFAWindow = 80,
    kCGLPFAMultiScreen = 81,
    kCGLPFACompliant = 83,
    kCGLPFADisplayMask = 84,
    kCGLPFAPBuffer = 90,
    kCGLPFARemotePBuffer = 91,
    kCGLPFAVirtualScreenCount = 128
};

enum ConverterType {
    ATI_4_COMPS_CONVERTER_ARGB = 0,
    ATI_4_COMPS_CONVERTER_ABGR = 1,
    STD_CONVERTER_ARGB = 2,
    STD_CONVERTER_ABGR = 3,
    UNKNOWN_CONVERTER = 4
};

enum CubemapShot {
    CUBEMAPSHOT_NONE = 0,
    CUBEMAPSHOT_UP = 1,
    CUBEMAPSHOT_DOWN = 2,
    CUBEMAPSHOT_LEFT = 3,
    CUBEMAPSHOT_RIGHT = 4,
    CUBEMAPSHOT_FRONT = 5,
    CUBEMAPSHOT_BACK = 6,
    CUBEMAPSHOT_COUNT = 7
};

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

enum D3DDEVTYPE {
    D3DDEVTYPE_HAL = 1,
    D3DDEVTYPE_REF = 2,
    D3DDEVTYPE_SW = 3,
    D3DDEVTYPE_NULLREF = 4,
    D3DDEVTYPE_FORCE_DWORD = 2147483647
};

enum D3DFOGMODE {
    D3DFOG_NONE = 0,
    D3DFOG_EXP = 1,
    D3DFOG_EXP2 = 2,
    D3DFOG_LINEAR = 3,
    D3DFOG_FORCE_DWORD = 2147483647
};

enum D3DLIGHTTYPE {
    D3DLIGHT_POINT = 1,
    D3DLIGHT_SPOT = 2,
    D3DLIGHT_DIRECTIONAL = 3,
    D3DLIGHT_FORCE_DWORD = 2147483647
};

enum D3DMULTISAMPLE_TYPE {
    D3DMULTISAMPLE_NONE = 0,
    D3DMULTISAMPLE_NONMASKABLE = 1,
    D3DMULTISAMPLE_2_SAMPLES = 2,
    D3DMULTISAMPLE_3_SAMPLES = 3,
    D3DMULTISAMPLE_4_SAMPLES = 4,
    D3DMULTISAMPLE_5_SAMPLES = 5,
    D3DMULTISAMPLE_6_SAMPLES = 6,
    D3DMULTISAMPLE_7_SAMPLES = 7,
    D3DMULTISAMPLE_8_SAMPLES = 8,
    D3DMULTISAMPLE_9_SAMPLES = 9,
    D3DMULTISAMPLE_10_SAMPLES = 10,
    D3DMULTISAMPLE_11_SAMPLES = 11,
    D3DMULTISAMPLE_12_SAMPLES = 12,
    D3DMULTISAMPLE_13_SAMPLES = 13,
    D3DMULTISAMPLE_14_SAMPLES = 14,
    D3DMULTISAMPLE_15_SAMPLES = 15,
    D3DMULTISAMPLE_16_SAMPLES = 16,
    D3DMULTISAMPLE_FORCE_DWORD = 2147483647
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

enum D3DRENDERSTATETYPE {
    D3DRS_ZENABLE = 7,
    D3DRS_FILLMODE = 8,
    D3DRS_SHADEMODE = 9,
    D3DRS_ZWRITEENABLE = 14,
    D3DRS_ALPHATESTENABLE = 15,
    D3DRS_LASTPIXEL = 16,
    D3DRS_SRCBLEND = 19,
    D3DRS_DESTBLEND = 20,
    D3DRS_CULLMODE = 22,
    D3DRS_ZFUNC = 23,
    D3DRS_ALPHAREF = 24,
    D3DRS_ALPHAFUNC = 25,
    D3DRS_DITHERENABLE = 26,
    D3DRS_ALPHABLENDENABLE = 27,
    D3DRS_FOGENABLE = 28,
    D3DRS_SPECULARENABLE = 29,
    D3DRS_FOGCOLOR = 34,
    D3DRS_FOGTABLEMODE = 35,
    D3DRS_FOGSTART = 36,
    D3DRS_FOGEND = 37,
    D3DRS_FOGDENSITY = 38,
    D3DRS_RANGEFOGENABLE = 48,
    D3DRS_STENCILENABLE = 52,
    D3DRS_STENCILFAIL = 53,
    D3DRS_STENCILZFAIL = 54,
    D3DRS_STENCILPASS = 55,
    D3DRS_STENCILFUNC = 56,
    D3DRS_STENCILREF = 57,
    D3DRS_STENCILMASK = 58,
    D3DRS_STENCILWRITEMASK = 59,
    D3DRS_TEXTUREFACTOR = 60,
    D3DRS_WRAP0 = 128,
    D3DRS_WRAP1 = 129,
    D3DRS_WRAP2 = 130,
    D3DRS_WRAP3 = 131,
    D3DRS_WRAP4 = 132,
    D3DRS_WRAP5 = 133,
    D3DRS_WRAP6 = 134,
    D3DRS_WRAP7 = 135,
    D3DRS_CLIPPING = 136,
    D3DRS_LIGHTING = 137,
    D3DRS_AMBIENT = 139,
    D3DRS_FOGVERTEXMODE = 140,
    D3DRS_COLORVERTEX = 141,
    D3DRS_LOCALVIEWER = 142,
    D3DRS_NORMALIZENORMALS = 143,
    D3DRS_DIFFUSEMATERIALSOURCE = 145,
    D3DRS_SPECULARMATERIALSOURCE = 146,
    D3DRS_AMBIENTMATERIALSOURCE = 147,
    D3DRS_EMISSIVEMATERIALSOURCE = 148,
    D3DRS_VERTEXBLEND = 151,
    D3DRS_CLIPPLANEENABLE = 152,
    D3DRS_POINTSIZE = 154,
    D3DRS_POINTSIZE_MIN = 155,
    D3DRS_POINTSPRITEENABLE = 156,
    D3DRS_POINTSCALEENABLE = 157,
    D3DRS_POINTSCALE_A = 158,
    D3DRS_POINTSCALE_B = 159,
    D3DRS_POINTSCALE_C = 160,
    D3DRS_MULTISAMPLEANTIALIAS = 161,
    D3DRS_MULTISAMPLEMASK = 162,
    D3DRS_PATCHEDGESTYLE = 163,
    D3DRS_DEBUGMONITORTOKEN = 165,
    D3DRS_POINTSIZE_MAX = 166,
    D3DRS_INDEXEDVERTEXBLENDENABLE = 167,
    D3DRS_COLORWRITEENABLE = 168,
    D3DRS_TWEENFACTOR = 170,
    D3DRS_BLENDOP = 171,
    D3DRS_POSITIONDEGREE = 172,
    D3DRS_NORMALDEGREE = 173,
    D3DRS_SCISSORTESTENABLE = 174,
    D3DRS_SLOPESCALEDEPTHBIAS = 175,
    D3DRS_ANTIALIASEDLINEENABLE = 176,
    D3DRS_MINTESSELLATIONLEVEL = 178,
    D3DRS_MAXTESSELLATIONLEVEL = 179,
    D3DRS_ADAPTIVETESS_X = 180,
    D3DRS_ADAPTIVETESS_Y = 181,
    D3DRS_ADAPTIVETESS_Z = 182,
    D3DRS_ADAPTIVETESS_W = 183,
    D3DRS_ENABLEADAPTIVETESSELLATION = 184,
    D3DRS_TWOSIDEDSTENCILMODE = 185,
    D3DRS_CCW_STENCILFAIL = 186,
    D3DRS_CCW_STENCILZFAIL = 187,
    D3DRS_CCW_STENCILPASS = 188,
    D3DRS_CCW_STENCILFUNC = 189,
    D3DRS_COLORWRITEENABLE1 = 190,
    D3DRS_COLORWRITEENABLE2 = 191,
    D3DRS_COLORWRITEENABLE3 = 192,
    D3DRS_BLENDFACTOR = 193,
    D3DRS_SRGBWRITEENABLE = 194,
    D3DRS_DEPTHBIAS = 195,
    D3DRS_WRAP8 = 198,
    D3DRS_WRAP9 = 199,
    D3DRS_WRAP10 = 200,
    D3DRS_WRAP11 = 201,
    D3DRS_WRAP12 = 202,
    D3DRS_WRAP13 = 203,
    D3DRS_WRAP14 = 204,
    D3DRS_WRAP15 = 205,
    D3DRS_SEPARATEALPHABLENDENABLE = 206,
    D3DRS_SRCBLENDALPHA = 207,
    D3DRS_DESTBLENDALPHA = 208,
    D3DRS_BLENDOPALPHA = 209,
    D3DRS_FORCE_DWORD = 2147483647
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

enum D3DTEXTURESTAGESTATETYPE {
    D3DTSS_COLOROP = 1,
    D3DTSS_COLORARG1 = 2,
    D3DTSS_COLORARG2 = 3,
    D3DTSS_ALPHAOP = 4,
    D3DTSS_ALPHAARG1 = 5,
    D3DTSS_ALPHAARG2 = 6,
    D3DTSS_BUMPENVMAT00 = 7,
    D3DTSS_BUMPENVMAT01 = 8,
    D3DTSS_BUMPENVMAT10 = 9,
    D3DTSS_BUMPENVMAT11 = 10,
    D3DTSS_TEXCOORDINDEX = 11,
    D3DTSS_BUMPENVLSCALE = 22,
    D3DTSS_BUMPENVLOFFSET = 23,
    D3DTSS_TEXTURETRANSFORMFLAGS = 24,
    D3DTSS_COLORARG0 = 26,
    D3DTSS_ALPHAARG0 = 27,
    D3DTSS_RESULTARG = 28,
    D3DTSS_CONSTANT = 32,
    D3DTSS_FORCE_DWORD = 2147483647
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

enum D3DXIMAGE_FILEFORMAT {
    D3DXIFF_BMP = 0,
    D3DXIFF_JPG = 1,
    D3DXIFF_TGA = 2,
    D3DXIFF_PNG = 3,
    D3DXIFF_DDS = 4,
    D3DXIFF_PPM = 5,
    D3DXIFF_DIB = 6,
    D3DXIFF_HDR = 7,
    D3DXIFF_PFM = 8,
    D3DXIFF_FORCE_DWORD = 2147483647
};

enum D3DXINCLUDE_TYPE {
    D3DXINC_LOCAL = 0,
    D3DXINC_SYSTEM = 1,
    D3DXINC_FORCE_DWORD = 2147483647
};

enum D3DXPARAMETER_CLASS {
    D3DXPC_SCALAR = 0,
    D3DXPC_VECTOR = 1,
    D3DXPC_MATRIX_ROWS = 2,
    D3DXPC_MATRIX_COLUMNS = 3,
    D3DXPC_OBJECT = 4,
    D3DXPC_STRUCT = 5,
    D3DXPC_FORCE_DWORD = 2147483647
};

enum D3DXPARAMETER_TYPE {
    D3DXPT_VOID = 0,
    D3DXPT_BOOL = 1,
    D3DXPT_INT = 2,
    D3DXPT_FLOAT = 3,
    D3DXPT_STRING = 4,
    D3DXPT_TEXTURE = 5,
    D3DXPT_TEXTURE1D = 6,
    D3DXPT_TEXTURE2D = 7,
    D3DXPT_TEXTURE3D = 8,
    D3DXPT_TEXTURECUBE = 9,
    D3DXPT_SAMPLER = 10,
    D3DXPT_SAMPLER1D = 11,
    D3DXPT_SAMPLER2D = 12,
    D3DXPT_SAMPLER3D = 13,
    D3DXPT_SAMPLERCUBE = 14,
    D3DXPT_PIXELSHADER = 15,
    D3DXPT_VERTEXSHADER = 16,
    D3DXPT_PIXELFRAGMENT = 17,
    D3DXPT_VERTEXFRAGMENT = 18,
    D3DXPT_FORCE_DWORD = 2147483647
};

enum D3DXREGISTER_SET {
    D3DXRS_BOOL = 0,
    D3DXRS_INT4 = 1,
    D3DXRS_FLOAT4 = 2,
    D3DXRS_SAMPLER = 3,
    D3DXRS_FORCE_DWORD = 2147483647
};

enum DemoType {
    DEMO_TYPE_NONE = 0,
    DEMO_TYPE_CLIENT = 1,
    DEMO_TYPE_SERVER = 2
};

enum DpvsClipChildren {
    DPVS_DONT_CLIP_CHILDREN = 0,
    DPVS_CLIP_CHILDREN = 1
};

enum DpvsForceBevels {
    DPVS_DONT_FORCE_BEVELS = 0,
    DPVS_FORCE_BEVELS = 1
};

enum DvarSetSource {
    DVAR_SOURCE_INTERNAL = 0,
    DVAR_SOURCE_EXTERNAL = 1,
    DVAR_SOURCE_SCRIPT = 2
};

enum DxCapsResponse {
    DX_CAPS_RESPONSE_QUIT = 0,
    DX_CAPS_RESPONSE_WARN = 1,
    DX_CAPS_RESPONSE_INFO = 2,
    DX_CAPS_RESPONSE_FORBID_DX7 = 3,
    DX_CAPS_RESPONSE_FORBID_DX9 = 4
};

enum EMediaTypes {
    MEDIA_MATERIALS = 0,
    MEDIA_MODELS = 1,
    MEDIA_EFFECTS = 2
};

enum FFType {
    kExactItem = 0,
    kAnyItem = 1,
    kAnyItemType = 2
};

enum FogColorSrcEnum {
    FOG_COLOR_SRC_UNSPECIFIED = 0,
    FOG_COLOR_SRC_STANDARD = 1,
    FOG_COLOR_SRC_BLACK = 2
};

enum Format {
    ARGB = 0,
    ABGR = 1
};

enum FsListBehavior {
    FS_LIST_PURE_ONLY = 0,
    FS_LIST_ALL = 1
};

typedef enum FsListBehavior FsListBehavior_e;

enum FxChannelId {
    FXCHAN_COLOR = 0,
    FXCHAN_COLOR_RAND = 1,
    FXCHAN_ALPHA = 2,
    FXCHAN_ALPHA_RAND = 3,
    FXCHAN_SIZE = 4,
    FXCHAN_SIZE_RAND = 5,
    FXCHAN_SIZE2 = 6,
    FXCHAN_SIZE2_RAND = 7,
    FXCHAN_LENGTH = 8,
    FXCHAN_LENGTH_RAND = 9,
    FXCHAN_ROTATION_DELTA = 10,
    FXCHAN_ROTATION_DELTA_RAND = 11,
    FXCHAN_VELOCITY_X = 12,
    FXCHAN_VELOCITY_Y = 13,
    FXCHAN_VELOCITY_Z = 14,
    FXCHAN_VELOCITY_X_RAND = 15,
    FXCHAN_VELOCITY_Y_RAND = 16,
    FXCHAN_VELOCITY_Z_RAND = 17,
    FXCHAN_VELOCITY2_X = 18,
    FXCHAN_VELOCITY2_Y = 19,
    FXCHAN_VELOCITY2_Z = 20,
    FXCHAN_VELOCITY2_X_RAND = 21,
    FXCHAN_VELOCITY2_Y_RAND = 22,
    FXCHAN_VELOCITY2_Z_RAND = 23,
    FXCHAN_COUNT = 24
};

enum GfxDrawGroupGlueBehavior {
    R_DRAWGROUP_GLUE_ISSUE = 0,
    R_DRAWGROUP_GLUE_SKIP = 1
};

enum GfxDrawGroupType {
    R_DRAWGROUP_DEPTH_PREPASS = 0,
    R_DRAWGROUP_SHADOW_COOKIE = 1,
    R_DRAWGROUP_WORLD_SUN = 2,
    R_DRAWGROUP_WORLD_REMAINDER = 3,
    R_DRAWGROUP_POST_AA = 4,
    R_DRAWGROUP_COUNT = 5
};

enum GfxDrawSurfOrder {
    GFX_DRAW_SURFS_ITER_FORWARD = 0,
    GFX_DRAW_SURFS_ITER_BACKWARD = 1
};

enum GfxFogOffset {
    FOG_OFFSET_NONE = 0,
    FOG_OFFSET_LINEAR = 1,
    FOG_OFFSET_EXP = 2,
    FOG_OFFSET_COUNT = 3
};

enum GfxLightType {
    GFX_LIGHT_TYPE_SUN = 0,
    GFX_LIGHT_TYPE_SKY = 1,
    GFX_LIGHT_TYPE_POINT = 2,
    GFX_LIGHT_TYPE_COUNT = 3
};

enum GfxLockType {
    GFX_LOCK_BEGIN_FRAME = 0,
    GFX_LOCK_RESUME_FRAME = 1
};

enum GfxOptimizedVertexSource {
    OPT_VERTS_NONE = 0,
    OPT_VERTS_WORLD = 1,
    OPT_VERTS_SMC = 2,
    OPT_VERTS_COUNT = 3
};

enum GfxPrimStatsTarget {
    GFX_PRIM_STATS_WORLD = 0,
    GFX_PRIM_STATS_SMODELCACHED = 1,
    GFX_PRIM_STATS_SMODELRIGID = 2,
    GFX_PRIM_STATS_XMODELCACHED = 3,
    GFX_PRIM_STATS_XMODELRIGID = 4,
    GFX_PRIM_STATS_XMODELSKINNED = 5,
    GFX_PRIM_STATS_BMODEL = 6,
    GFX_PRIM_STATS_FX = 7,
    GFX_PRIM_STATS_HUD = 8,
    GFX_PRIM_STATS_DEBUG = 9,
    GFX_PRIM_STATS_CODE = 10,
    GFX_PRIM_STATS_COUNT = 11
};

enum GfxRenderTargetId {
    R_RENDERTARGET_FRAME_BUFFER = 0,
    R_RENDERTARGET_RESOLVED_POST_SUN = 1,
    R_RENDERTARGET_RESOLVED_SCENE = 2,
    R_RENDERTARGET_DYNAMICSHADOWS = 3,
    R_RENDERTARGET_SHADOWCOOKIE = 4,
    R_RENDERTARGET_SHADOWCOOKIE_BLUR = 5,
    R_RENDERTARGET_FLOAT_Z = 6,
    R_RENDERTARGET_SAVED_SCREEN = 7,
    R_RENDERTARGET_BLURRED_SCREEN = 8,
    R_RENDERTARGET_GLOW_0 = 9,
    R_RENDERTARGET_GLOW_1 = 10,
    R_RENDERTARGET_PINGPONG_0 = 11,
    R_RENDERTARGET_PINGPONG_1 = 12,
    R_RENDERTARGET_COUNT = 13,
    R_RENDERTARGET_NONE = 14
};

enum GfxScreenshotType {
    R_SCREENSHOT_JPG = 0,
    R_SCREENSHOT_TGA = 1
};

enum GfxViewportBehavior {
    GFX_USE_VIEWPORT_FOR_VIEW = 0,
    GFX_USE_VIEWPORT_FULL = 1
};

enum JPEG_MARKER {
    M_SOF0 = 192,
    M_SOF1 = 193,
    M_SOF2 = 194,
    M_SOF3 = 195,
    M_SOF5 = 197,
    M_SOF6 = 198,
    M_SOF7 = 199,
    M_JPG = 200,
    M_SOF9 = 201,
    M_SOF10 = 202,
    M_SOF11 = 203,
    M_SOF13 = 205,
    M_SOF14 = 206,
    M_SOF15 = 207,
    M_DHT = 196,
    M_DAC = 204,
    M_RST0 = 208,
    M_RST1 = 209,
    M_RST2 = 210,
    M_RST3 = 211,
    M_RST4 = 212,
    M_RST5 = 213,
    M_RST6 = 214,
    M_RST7 = 215,
    M_SOI = 216,
    M_EOI = 217,
    M_SOS = 218,
    M_DQT = 219,
    M_DNL = 220,
    M_DRI = 221,
    M_DHP = 222,
    M_EXP = 223,
    M_APP0 = 224,
    M_APP1 = 225,
    M_APP2 = 226,
    M_APP3 = 227,
    M_APP4 = 228,
    M_APP5 = 229,
    M_APP6 = 230,
    M_APP7 = 231,
    M_APP8 = 232,
    M_APP9 = 233,
    M_APP10 = 234,
    M_APP11 = 235,
    M_APP12 = 236,
    M_APP13 = 237,
    M_APP14 = 238,
    M_APP15 = 239,
    M_JPG0 = 240,
    M_JPG13 = 253,
    M_COM = 254,
    M_TEM = 1,
    M_ERROR = 256
};

enum J_BUF_MODE {
    JBUF_PASS_THRU = 0,
    JBUF_SAVE_SOURCE = 1,
    JBUF_CRANK_DEST = 2,
    JBUF_SAVE_AND_PASS = 3
};

enum J_COLOR_SPACE {
    JCS_UNKNOWN = 0,
    JCS_GRAYSCALE = 1,
    JCS_RGB = 2,
    JCS_YCbCr = 3,
    JCS_CMYK = 4,
    JCS_YCCK = 5
};

enum J_DCT_METHOD {
    JDCT_ISLOW = 0,
    JDCT_IFAST = 1,
    JDCT_FLOAT = 2
};

enum J_DITHER_MODE {
    JDITHER_NONE = 0,
    JDITHER_ORDERED = 1,
    JDITHER_FS = 2
};

enum LoopMode {
    LM_FOREVER = 0,
    LM_TIMES = 1,
    LM_MODE_COUNT = 2
};

enum MaterialShaderType {
    MTL_VERTEX_SHADER = 0,
    MTL_PIXEL_SHADER = 1
};

enum MaterialTechniqueType {
    TECHNIQUE_DEPTH_PREPASS = 0,
    TECHNIQUE_SKY = 1,
    TECHNIQUE_BUILD_FLOAT_Z = 2,
    TECHNIQUE_FOGABLE_FIRST = 3,
    TECHNIQUE_UNLIT = 3,
    TECHNIQUE_UNLIT_FOG_LINEAR = 4,
    TECHNIQUE_UNLIT_FOG_EXP = 5,
    TECHNIQUE_LIGHTING_FIRST = 6,
    TECHNIQUE_LIGHTMAP = 6,
    TECHNIQUE_LIGHTMAP_FOG_LINEAR = 7,
    TECHNIQUE_LIGHTMAP_FOG_EXP = 8,
    TECHNIQUE_LIGHTPROBE_NOT_CACHED_FIRST = 9,
    TECHNIQUE_LIGHTPROBE = 9,
    TECHNIQUE_LIGHTPROBE_FOG_LINEAR = 10,
    TECHNIQUE_LIGHTPROBE_FOG_EXP = 11,
    TECHNIQUE_LIGHTPROBE_SMODEL = 12,
    TECHNIQUE_LIGHTPROBE_SMODEL_FOG_LINEAR = 13,
    TECHNIQUE_LIGHTPROBE_SMODEL_FOG_EXP = 14,
    TECHNIQUE_LIGHTPROBE_NOT_CACHED_LAST = 14,
    TECHNIQUE_LIGHTPROBE_SMODEL_CACHED = 15,
    TECHNIQUE_LIGHTPROBE_SMODEL_CACHED_FOG_LINEAR = 16,
    TECHNIQUE_LIGHTPROBE_SMODEL_CACHED_FOG_EXP = 17,
    TECHNIQUE_LIGHTING_LAST = 17,
    TECHNIQUE_POINTLIGHT = 18,
    TECHNIQUE_POINTLIGHT_FOG_LINEAR = 19,
    TECHNIQUE_POINTLIGHT_FOG_EXP = 20,
    TECHNIQUE_EMISSIVE = 21,
    TECHNIQUE_EMISSIVE_FOG_LINEAR = 22,
    TECHNIQUE_EMISSIVE_FOG_EXP = 23,
    TECHNIQUE_EMISSIVE_FEATHERED = 24,
    TECHNIQUE_EMISSIVE_FEATHERED_FOG_LINEAR = 25,
    TECHNIQUE_EMISSIVE_FEATHERED_FOG_EXP = 26,
    TECHNIQUE_FOGABLE_LAST = 26,
    TECHNIQUE_FAKELIGHT_NORMAL = 27,
    TECHNIQUE_FAKELIGHT_VIEW = 28,
    TECHNIQUE_WIREFRAME_SOLID = 29,
    TECHNIQUE_WIREFRAME_SHADED = 30,
    TECHNIQUE_SHADOWCOOKIE_CASTER = 31,
    TECHNIQUE_SHADOWCOOKIE_RECEIVER = 32,
    TECHNIQUE_DEBUG_BUMPMAP = 33,
    TECHNIQUE_COUNT = 34,
    TECHNIQUE_TOTAL_COUNT = 35,
    TECHNIQUE_NONE = -1
};

enum MaterialTextureSource {
    TEXTURE_SRC_CODE_BLACK = 0,
    TEXTURE_SRC_CODE_WHITE = 1,
    TEXTURE_SRC_CODE_IDENTITY_NORMAL_MAP = 2,
    TEXTURE_SRC_CODE_SPECULARITY = 3,
    TEXTURE_SRC_CODE_SMODEL_LIGHTING = 4,
    TEXTURE_SRC_CODE_LIGHT_GRID_WEIGHTS_0 = 5,
    TEXTURE_SRC_CODE_LIGHT_GRID_WEIGHTS_1 = 6,
    TEXTURE_SRC_CODE_LIGHTMAP_WEIGHTS = 7,
    TEXTURE_SRC_CODE_LIGHTMAP_0 = 8,
    TEXTURE_SRC_CODE_LIGHTMAP_1 = 9,
    TEXTURE_SRC_CODE_LIGHTMAP_2 = 10,
    TEXTURE_SRC_CODE_LIGHTMAP_3 = 11,
    TEXTURE_SRC_CODE_SHADOWCOOKIE = 12,
    TEXTURE_SRC_CODE_FEEDBACK = 13,
    TEXTURE_SRC_CODE_RESOLVED_POST_SUN = 14,
    TEXTURE_SRC_CODE_RESOLVED_SCENE = 15,
    TEXTURE_SRC_CODE_SKY = 16,
    TEXTURE_SRC_CODE_LIGHT0_ATTENUATION = 17,
    TEXTURE_SRC_CODE_LIGHT1_ATTENUATION = 18,
    TEXTURE_SRC_CODE_DYNAMIC_SHADOWS = 19,
    TEXTURE_SRC_CODE_OUTDOOR = 20,
    TEXTURE_SRC_CODE_FLOATZ = 21,
    TEXTURE_SRC_CODE_SUN_HALF_ANGLE = 22,
    TEXTURE_SRC_CODE_WATER_COLOR = 23
};

enum MaterialVertexDeclType {
    VERTDECL_GENERIC = 0,
    VERTDECL_WORLD = 1,
    VERTDECL_POS_TEX = 2,
    VERTDECL_STATICMODELCACHE = 3,
    VERTDECL_COUNT = 4
};

enum MtlParseSuccess {
    MTL_PARSE_SUCCESS = 0,
    MTL_PARSE_NO_MATCH = 1,
    MTL_PARSE_ERROR = 2
};

enum MtlTextureFunctionValidDx7 {
    MTL_TEXFUNC_VALID_COLOR = 1,
    MTL_TEXFUNC_VALID_ALPHA = 2,
    MTL_TEXFUNC_VALID_BOTH = 3
};

typedef int OGLSITPTypes;

enum OffhandClass {
    OFFHAND_CLASS_NONE = 0,
    OFFHAND_CLASS_FRAG_GRENADE = 1,
    OFFHAND_CLASS_SMOKE_GRENADE = 2,
    OFFHAND_CLASS_COUNT = 3
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

enum PlayRateMode {
    PRM_FIXED_FPS = 0,
    PRM_SYNC_TO_PARTICLE_LIFETIME = 1,
    PRM_MODE_COUNT = 2
};

enum PrimType {
    PT_NONE = 0,
    PT_PARTICLE = 1,
    PT_LINE = 2,
    PT_TAIL = 3,
    PT_CYLINDER = 4,
    PT_EMITTER = 5,
    PT_DECAL = 6,
    PT_ORIENTEDPARTICLE = 7,
    PT_FXRUNNER = 8,
    PT_LIGHT = 9,
    PT_CAMERASHAKE = 10,
    PT_SCREENFLASH = 11,
    PT_CLOUD = 12
};

enum RenderTargetUsage {
    RENDERTARGET_USAGE_RENDER = 0,
    RENDERTARGET_USAGE_TEXTURE = 1,
    RENDERTARGET_USAGE_READBACK = 2
};

enum ShadowType {
    SHADOW_NONE = 0,
    SHADOW_COOKIE = 1
};

enum StartFrameMode {
    SFM_FIXED_FRAME = 0,
    SFM_RANDOM = 1,
    SFM_INDEXED = 2,
    SFM_MODE_COUNT = 3
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

typedef enum CGLError _CGLError;

typedef enum CGLPixelFormatAttribute _CGLPixelFormatAttribute;

typedef enum D3DBACKBUFFER_TYPE _D3DBACKBUFFER_TYPE;

typedef enum D3DBASISTYPE _D3DBASISTYPE;

enum _D3DCUBEMAP_FACES {
    D3DCUBEMAP_FACE_POSITIVE_X = 0,
    D3DCUBEMAP_FACE_NEGATIVE_X = 1,
    D3DCUBEMAP_FACE_POSITIVE_Y = 2,
    D3DCUBEMAP_FACE_NEGATIVE_Y = 3,
    D3DCUBEMAP_FACE_POSITIVE_Z = 4,
    D3DCUBEMAP_FACE_NEGATIVE_Z = 5,
    D3DCUBEMAP_FACE_FORCE_DWORD = 2147483647
};

typedef enum D3DDEGREETYPE _D3DDEGREETYPE;

typedef enum D3DDEVTYPE _D3DDEVTYPE;

typedef enum D3DFOGMODE _D3DFOGMODE;

enum _D3DFORMAT {
    D3DFMT_UNKNOWN = 0,
    D3DFMT_R8G8B8 = 20,
    D3DFMT_A8R8G8B8 = 21,
    D3DFMT_X8R8G8B8 = 22,
    D3DFMT_R5G6B5 = 23,
    D3DFMT_X1R5G5B5 = 24,
    D3DFMT_A1R5G5B5 = 25,
    D3DFMT_A4R4G4B4 = 26,
    D3DFMT_R3G3B2 = 27,
    D3DFMT_A8 = 28,
    D3DFMT_A8R3G3B2 = 29,
    D3DFMT_X4R4G4B4 = 30,
    D3DFMT_A2B10G10R10 = 31,
    D3DFMT_A8B8G8R8 = 32,
    D3DFMT_X8B8G8R8 = 33,
    D3DFMT_G16R16 = 34,
    D3DFMT_A2R10G10B10 = 35,
    D3DFMT_A16B16G16R16 = 36,
    D3DFMT_A8P8 = 40,
    D3DFMT_P8 = 41,
    D3DFMT_L8 = 50,
    D3DFMT_A8L8 = 51,
    D3DFMT_A4L4 = 52,
    D3DFMT_V8U8 = 60,
    D3DFMT_L6V5U5 = 61,
    D3DFMT_X8L8V8U8 = 62,
    D3DFMT_Q8W8V8U8 = 63,
    D3DFMT_V16U16 = 64,
    D3DFMT_A2W10V10U10 = 67,
    D3DFMT_UYVY = 1498831189,
    D3DFMT_R8G8_B8G8 = 1195525970,
    D3DFMT_YUY2 = 844715353,
    D3DFMT_G8R8_G8B8 = 1111970375,
    D3DFMT_DXT1 = 827611204,
    D3DFMT_DXT2 = 844388420,
    D3DFMT_DXT3 = 861165636,
    D3DFMT_DXT4 = 877942852,
    D3DFMT_DXT5 = 894720068,
    D3DFMT_D16_LOCKABLE = 70,
    D3DFMT_D32 = 71,
    D3DFMT_D15S1 = 73,
    D3DFMT_D24S8 = 75,
    D3DFMT_D24X8 = 77,
    D3DFMT_D24X4S4 = 79,
    D3DFMT_D16 = 80,
    D3DFMT_D32F_LOCKABLE = 82,
    D3DFMT_D24FS8 = 83,
    D3DFMT_L16 = 81,
    D3DFMT_VERTEXDATA = 100,
    D3DFMT_INDEX16 = 101,
    D3DFMT_INDEX32 = 102,
    D3DFMT_Q16W16V16U16 = 110,
    D3DFMT_MULTI2_ARGB8 = 827606349,
    D3DFMT_R16F = 111,
    D3DFMT_G16R16F = 112,
    D3DFMT_A16B16G16R16F = 113,
    D3DFMT_R32F = 114,
    D3DFMT_G32R32F = 115,
    D3DFMT_A32B32G32R32F = 116,
    D3DFMT_CxV8U8 = 117,
    D3DFMT_FORCE_DWORD = 2147483647
};

typedef enum D3DLIGHTTYPE _D3DLIGHTTYPE;

typedef enum D3DMULTISAMPLE_TYPE _D3DMULTISAMPLE_TYPE;

typedef enum D3DPOOL _D3DPOOL;

typedef enum D3DPRIMITIVETYPE _D3DPRIMITIVETYPE;

typedef enum D3DQUERYTYPE _D3DQUERYTYPE;

typedef enum D3DRENDERSTATETYPE _D3DRENDERSTATETYPE;

enum _D3DRESOURCETYPE {
    D3DRTYPE_SURFACE = 1,
    D3DRTYPE_VOLUME = 2,
    D3DRTYPE_TEXTURE = 3,
    D3DRTYPE_VOLUMETEXTURE = 4,
    D3DRTYPE_CUBETEXTURE = 5,
    D3DRTYPE_VERTEXBUFFER = 6,
    D3DRTYPE_INDEXBUFFER = 7,
    D3DRTYPE_FORCE_DWORD = 2147483647
};

typedef enum D3DSAMPLERSTATETYPE _D3DSAMPLERSTATETYPE;

typedef enum D3DSTATEBLOCKTYPE _D3DSTATEBLOCKTYPE;

typedef enum D3DSWAPEFFECT _D3DSWAPEFFECT;

enum _D3DTEXTUREADDRESS {
    D3DTADDRESS_WRAP = 1,
    D3DTADDRESS_MIRROR = 2,
    D3DTADDRESS_CLAMP = 3,
    D3DTADDRESS_BORDER = 4,
    D3DTADDRESS_MIRRORONCE = 5,
    D3DTADDRESS_FORCE_DWORD = 2147483647
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

typedef enum D3DTEXTURESTAGESTATETYPE _D3DTEXTURESTAGESTATETYPE;

typedef enum D3DTEXTURETRANSFORMFLAGS _D3DTEXTURETRANSFORMFLAGS;

typedef enum D3DTRANSFORMSTATETYPE _D3DTRANSFORMSTATETYPE;

typedef enum D3DXIMAGE_FILEFORMAT _D3DXIMAGE_FILEFORMAT;

typedef enum D3DXINCLUDE_TYPE _D3DXINCLUDE_TYPE;

typedef enum D3DXPARAMETER_CLASS _D3DXPARAMETER_CLASS;

typedef enum D3DXPARAMETER_TYPE _D3DXPARAMETER_TYPE;

typedef enum D3DXREGISTER_SET _D3DXREGISTER_SET;

enum _Rb_tree_color {
    _S_red = 0,
    _S_black = 1
};

typedef enum J_DITHER_MODE __darwin_suseconds_t;

enum aistateEnum_t {
    AISTATE_NULL = -1,
    AISTATE_RELAXED = 0,
    AISTATE_QUERY = 1,
    AISTATE_ALERT = 2,
    AISTATE_COMBAT = 3,
    MAX_AISTATES = 4
};

enum animBodyPart_t {
    ANIM_BP_UNUSED = 0,
    ANIM_BP_LEGS = 1,
    ANIM_BP_TORSO = 2,
    ANIM_BP_BOTH = 3,
    NUM_ANIM_BODYPARTS = 4
};

enum animScriptConditionTypes_t {
    ANIM_CONDTYPE_BITFLAGS = 0,
    ANIM_CONDTYPE_VALUE = 1,
    NUM_ANIM_CONDTYPES = 2
};

enum animScriptParseMode_t {
    PARSEMODE_DEFINES = 0,
    PARSEMODE_ANIMATION = 1,
    PARSEMODE_CANNED_ANIMATIONS = 2,
    PARSEMODE_STATECHANGES = 3,
    PARSEMODE_EVENTS = 4,
    NUM_PARSEMODES = 5
};

enum c_pass_type {
    main_pass = 0,
    huff_opt_pass = 1,
    output_pass = 2
};

enum clientConnected_t {
    CON_DISCONNECTED = 0,
    CON_CONNECTING = 1,
    CON_CONNECTED = 2
};

enum connstate_t {
    CA_DISCONNECTED = 0,
    CA_CINEMATIC = 1,
    CA_LOGO = 2,
    CA_CONNECTING = 3,
    CA_CHALLENGING = 4,
    CA_CONNECTED = 5,
    CA_LOADING = 6,
    CA_PRIMED = 7,
    CA_ACTIVE = 8
};

enum e_status {
    FMV_IDLE = 0,
    FMV_PLAY = 1,
    FMV_EOF = 2,
    FMV_ID_BLT = 3,
    FMV_ID_IDLE = 4,
    FMV_LOOPED = 5,
    FMV_ID_WAIT = 6
};

enum errorParm_t {
    ERR_FATAL = 0,
    ERR_DROP = 1,
    ERR_SERVERDISCONNECT = 2,
    ERR_DISCONNECT = 3,
    ERR_SCRIPT = 4,
    ERR_SCRIPT_DROP = 5,
    ERR_LOCALIZATION = 6
};

enum fieldtype_t {
    F_INT = 0,
    F_FLOAT = 1,
    F_LSTRING = 2,
    F_STRING = 3,
    F_VECTOR = 4,
    F_ENTITY = 5,
    F_VECTORHACK = 6,
    F_OBJECT = 7,
    F_MODEL = 8
};

enum fsMode_t {
    FS_READ = 0,
    FS_WRITE = 1,
    FS_APPEND = 2,
    FS_APPEND_SYNC = 3
};

enum he_type_t {
    HE_TYPE_FREE = 0,
    HE_TYPE_TEXT = 1,
    HE_TYPE_VALUE = 2,
    HE_TYPE_PLAYERNAME = 3,
    HE_TYPE_MAPNAME = 4,
    HE_TYPE_GAMETYPE = 5,
    HE_TYPE_MATERIAL = 6,
    HE_TYPE_TIMER_DOWN = 7,
    HE_TYPE_TIMER_UP = 8,
    HE_TYPE_TENTHS_TIMER_DOWN = 9,
    HE_TYPE_TENTHS_TIMER_UP = 10,
    HE_TYPE_CLOCK_DOWN = 11,
    HE_TYPE_CLOCK_UP = 12,
    HE_TYPE_WAYPOINT = 13,
    HE_TYPE_COUNT = 14
};

enum hitLocation_t {
    HITLOC_NONE = 0,
    HITLOC_HELMET = 1,
    HITLOC_HEAD = 2,
    HITLOC_NECK = 3,
    HITLOC_TORSO_UPR = 4,
    HITLOC_TORSO_LWR = 5,
    HITLOC_R_ARM_UPR = 6,
    HITLOC_L_ARM_UPR = 7,
    HITLOC_R_ARM_LWR = 8,
    HITLOC_L_ARM_LWR = 9,
    HITLOC_R_HAND = 10,
    HITLOC_L_HAND = 11,
    HITLOC_R_LEG_UPR = 12,
    HITLOC_L_LEG_UPR = 13,
    HITLOC_R_LEG_LWR = 14,
    HITLOC_L_LEG_LWR = 15,
    HITLOC_R_FOOT = 16,
    HITLOC_L_FOOT = 17,
    HITLOC_GUN = 18,
    HITLOC_NUM = 19
};

enum hudelem_update_t {
    HUDELEM_UPDATE_ARCHIVAL = 1,
    HUDELEM_UPDATE_CURRENT = 2,
    HUDELEM_UPDATE_ARCHIVAL_AND_CURRENT = 3
};

enum inflate_block_mode {
    TYPE = 0,
    LENS = 1,
    STORED = 2,
    TABLE = 3,
    BTREE = 4,
    DTREE = 5,
    CODES = 6,
    DRY = 7,
    DONE = 8,
    BAD = 9
};

enum inflate_codes_mode {
    START = 0,
    LEN = 1,
    LENEXT = 2,
    DIST = 3,
    DISTEXT = 4,
    COPY = 5,
    LIT = 6,
    WASH = 7,
    END = 8,
    BADCODE = 9
};

typedef int inflate_mode;

enum itemType_t {
    IT_BAD = 0,
    IT_WEAPON = 1,
    IT_AMMO = 2,
    IT_HEALTH = 3
};

enum language_t {
    LANGUAGE_ENGLISH = 0,
    LANGUAGE_FRENCH = 1,
    LANGUAGE_GERMAN = 2,
    LANGUAGE_ITALIAN = 3,
    LANGUAGE_SPANISH = 4,
    LANGUAGE_BRITISH = 5,
    LANGUAGE_RUSSIAN = 6,
    LANGUAGE_POLISH = 7,
    LANGUAGE_KOREAN = 8,
    LANGUAGE_TAIWANESE = 9,
    LANGUAGE_JAPANESE = 10,
    LANGUAGE_CHINESE = 11,
    LANGUAGE_THAI = 12,
    LANGUAGE_LEET = 13,
    MAX_LANGUAGES = 14
};

enum leType_t {
    LE_MOVING_TRACER = 0
};

enum listColumnTypes_t {
    LCT_NAME = 0,
    LCT_CLAN = 1,
    LCT_SCORE = 2,
    LCT_DEATHS = 3,
    LCT_PING = 4,
    LCT_STATUS_ICON = 5,
    LCT_TALKING_ICON = 6,
    LCT_NUM = 7
};

enum msgLocErrType_t {
    LOCMSG_SAFE = 0,
    LOCMSG_NOERR = 1
};

enum msgwnd_mode_t {
    MWM_TOPDOWN = 0,
    MWM_TOPDOWN_REVERSED = 1,
    MWM_BOTTOMUP = 2,
    MWM_BOTTOMUP_CENTERED = 3
};

enum netadrtype_t {
    NA_BOT = 0,
    NA_BAD = 1,
    NA_LOOPBACK = 2,
    NA_BROADCAST = 3,
    NA_IP = 4
};

enum netsrc_t {
    NS_CLIENT1 = 0,
    NS_SERVER = 1,
    NS_MAXCLIENTS = 1,
    NS_PACKET = 2
};

enum objectiveState_t {
    OBJST_EMPTY = 0,
    OBJST_ACTIVE = 1,
    OBJST_INVISIBLE = 2,
    OBJST_DONE = 3,
    OBJST_CURRENT = 4,
    OBJST_FAILED = 5,
    OBJST_NUMSTATES = 6
};

enum print_msg_type_t {
    PMSG_CONSOLE = 0,
    PMSG_GAME = 1,
    PMSG_BOLDGAME = 2,
    PMSG_SUBTITLE = 3,
    PMSG_LOGFILE = 4
};

enum proneCheckType_t {
    PCT_CLIENT = 0,
    PCT_ACTOR = 1
};

enum refEntityType_t {
    RT_DOBJ = 0,
    RT_XMODEL = 1,
    RT_STATICMODEL = 2,
    RT_BRUSHMODEL = 3,
    RT_SPRITE = 4,
    RT_RAIL_CORE = 5,
    RT_PARTICLE_CLOUD = 6,
    RT_ORIENTED_QUAD = 7,
    RT_LINE = 8,
    RT_CYLINDER = 9,
    RT_MAX_REF_ENTITY_TYPE = 10
};

enum scriptAnimEventTypes_t {
    ANIM_ET_PAIN = 0,
    ANIM_ET_DEATH = 1,
    ANIM_ET_FIREWEAPON = 2,
    ANIM_ET_JUMP = 3,
    ANIM_ET_JUMPBK = 4,
    ANIM_ET_LAND = 5,
    ANIM_ET_DROPWEAPON = 6,
    ANIM_ET_RAISEWEAPON = 7,
    ANIM_ET_CLIMB_MOUNT = 8,
    ANIM_ET_CLIMB_DISMOUNT = 9,
    ANIM_ET_RELOAD = 10,
    ANIM_ET_CROUCH_TO_PRONE = 11,
    ANIM_ET_PRONE_TO_CROUCH = 12,
    ANIM_ET_STAND_TO_CROUCH = 13,
    ANIM_ET_CROUCH_TO_STAND = 14,
    ANIM_ET_STAND_TO_PRONE = 15,
    ANIM_ET_PRONE_TO_STAND = 16,
    ANIM_ET_MELEEATTACK = 17,
    ANIM_ET_SHELLSHOCK = 18,
    NUM_ANIM_EVENTTYPES = 19
};

enum scriptAnimMoveTypes_t {
    ANIM_MT_UNUSED = 0,
    ANIM_MT_IDLE = 1,
    ANIM_MT_IDLECR = 2,
    ANIM_MT_IDLEPRONE = 3,
    ANIM_MT_WALK = 4,
    ANIM_MT_WALKBK = 5,
    ANIM_MT_WALKCR = 6,
    ANIM_MT_WALKCRBK = 7,
    ANIM_MT_WALKPRONE = 8,
    ANIM_MT_WALKPRONEBK = 9,
    ANIM_MT_RUN = 10,
    ANIM_MT_RUNBK = 11,
    ANIM_MT_RUNCR = 12,
    ANIM_MT_RUNCRBK = 13,
    ANIM_MT_TURNRIGHT = 14,
    ANIM_MT_TURNLEFT = 15,
    ANIM_MT_TURNRIGHTCR = 16,
    ANIM_MT_TURNLEFTCR = 17,
    ANIM_MT_CLIMBUP = 18,
    ANIM_MT_CLIMBDOWN = 19,
    ANIM_MT_MANTLE_ROOT = 20,
    ANIM_MT_MANTLE_UP_57 = 21,
    ANIM_MT_MANTLE_UP_51 = 22,
    ANIM_MT_MANTLE_UP_45 = 23,
    ANIM_MT_MANTLE_UP_39 = 24,
    ANIM_MT_MANTLE_UP_33 = 25,
    ANIM_MT_MANTLE_UP_27 = 26,
    ANIM_MT_MANTLE_UP_21 = 27,
    ANIM_MT_MANTLE_OVER_HIGH = 28,
    ANIM_MT_MANTLE_OVER_MID = 29,
    ANIM_MT_MANTLE_OVER_LOW = 30,
    ANIM_MT_FLINCH_FORWARD = 31,
    ANIM_MT_FLINCH_BACKWARD = 32,
    ANIM_MT_FLINCH_LEFT = 33,
    ANIM_MT_FLINCH_RIGHT = 34,
    ANIM_MT_STUMBLE_FORWARD = 35,
    ANIM_MT_STUMBLE_BACKWARD = 36,
    ANIM_MT_STUMBLE_WALK_FORWARD = 37,
    ANIM_MT_STUMBLE_WALK_BACKWARD = 38,
    ANIM_MT_STUMBLE_CROUCH_FORWARD = 39,
    ANIM_MT_STUMBLE_CROUCH_BACKWARD = 40,
    NUM_ANIM_MOVETYPES = 41
};

enum serverState_t {
    SS_DEAD = 0,
    SS_LOADING = 1,
    SS_GAME = 2
};

enum sessionState_t {
    SESS_STATE_PLAYING = 0,
    SESS_STATE_DEAD = 1,
    SESS_STATE_SPECTATOR = 2,
    SESS_STATE_INTERMISSION = 3
};

enum snd_alias_members_t {
    SA_INVALID = 0,
    SA_NAME = 1,
    SA_SEQUENCE = 2,
    SA_FILE = 3,
    SA_SUBTITLE = 4,
    SA_VOL_MIN = 5,
    SA_VOL_MAX = 6,
    SA_VOL_MOD = 7,
    SA_PITCH_MIN = 8,
    SA_PITCH_MAX = 9,
    SA_DIST_MIN = 10,
    SA_DIST_MAX = 11,
    SA_CHANNEL = 12,
    SA_TYPE = 13,
    SA_LOOP = 14,
    SA_PROBABILITY = 15,
    SA_LOADSPEC = 16,
    SA_MASTERSLAVE = 17,
    SA_SECONDARYALIASNAME = 18,
    SA_VOLUMEFALLOFFCURVE = 19,
    SA_STARTDELAY = 20,
    SA_SPEAKERMAP = 21,
    SA_REVERB = 22,
    SA_LFEPERCENTAGE = 23,
    SA_NUMFIELDS = 24
};

enum snd_alias_system_t {
    SASYS_UI = 0,
    SASYS_CGAME = 1,
    SASYS_GAME = 2,
    SASYS_COUNT = 3
};

enum snd_alias_type_t {
    SAT_UNKNOWN = 0,
    SAT_LOADED = 1,
    SAT_STREAMED = 2,
    SAT_PRIMED = 3,
    SAT_COUNT = 4
};

enum snd_overlay_type_t {
    SND_OVERLAY_NONE = 0,
    SND_OVERLAY_3D = 1,
    SND_OVERLAY_STREAM = 2,
    SND_OVERLAY_2D = 3
};

enum snd_stopsounds_arg_t {
    SND_STOP_ALL = 0,
    SND_KEEP_REVERB = 1,
    SND_KEEP_MUSIC = 2,
    SND_KEEP_AMBIENT = 4,
    SND_KEEP_MUSIC_AND_AMBIENT = 6,
    SND_STOP_STREAMED = 8,
    SND_KEEP_CHANNEL_VOLUMES = 16
};

enum sscType_t {
    SSC_STRING = 0,
    SSC_YESNO = 1
};

enum surfaceType_t {
    SF_BAD = 0,
    SF_POLY = 1,
    SF_ENTITY = 2,
    SF_XMODEL_SKINNED = 3,
    SF_XMODEL_RIGID = 4,
    SF_STATICMODEL_CACHED = 5,
    SF_TRIANGLES = 6,
    SF_RAW_GEOMETRY = 7,
    SF_NUM_SURFACE_TYPES = 8,
    SF_MAX = 4294967295
};

enum svscmd_type {
    SV_CMD_CAN_IGNORE = 0,
    SV_CMD_RELIABLE = 1
};

enum sysEventType_t {
    SE_NONE = 0,
    SE_KEY = 1,
    SE_CHAR = 2,
    SE_GAMEPAD_AXIS = 3,
    SE_CONSOLE = 4,
    SE_PACKET = 5
};

enum team_t {
    TEAM_FREE = 0,
    TEAM_AXIS = 1,
    TEAM_ALLIES = 2,
    TEAM_SPECTATOR = 3,
    TEAM_NUM_TEAMS = 4
};

enum trType_t {
    TR_STATIONARY = 0,
    TR_INTERPOLATE = 1,
    TR_LINEAR = 2,
    TR_LINEAR_STOP = 3,
    TR_SINE = 4,
    TR_GRAVITY = 5,
    TR_GRAVITY_PAUSED = 6,
    TR_ACCELERATE = 7,
    TR_DECELERATE = 8
};

enum uiMenuCommand_t {
    UIMENU_NONE = 0,
    UIMENU_MAIN = 1,
    UIMENU_INGAME = 2,
    UIMENU_NEED_CD = 3,
    UIMENU_BAD_CD_KEY = 4,
    UIMENU_TEAM = 5,
    UIMENU_PREGAME = 6,
    UIMENU_POSTGAME = 7,
    UIMENU_WM_QUICKMESSAGE = 8,
    UIMENU_WM_AUTOUPDATE = 9,
    UIMENU_SCRIPT_POPUP = 10,
    UIMENU_SCRIPT_POPUP_NO_MOUSE = 11,
    UIMENU_PLAYER_PROFILE = 12
};

enum weapClass_t {
    WEAPCLASS_RIFLE = 0,
    WEAPCLASS_MG = 1,
    WEAPCLASS_SMG = 2,
    WEAPCLASS_SPREAD = 3,
    WEAPCLASS_PISTOL = 4,
    WEAPCLASS_GRENADE = 5,
    WEAPCLASS_ROCKETLAUNCHER = 6,
    WEAPCLASS_TURRET = 7,
    WEAPCLASS_NON_PLAYER = 8,
    WEAPCLASS_ITEM = 9,
    WEAPCLASS_NUM = 10
};

enum weapOverlayReticle_t {
    WEAPOVERLAYRETICLE_NONE = 0,
    WEAPOVERLAYRETICLE_CROSSHAIR = 1,
    WEAPOVERLAYRETICLE_FG42 = 2,
    WEAPOVERLAYRETICLE_SPRINGFIELD = 3,
    WEAPOVERLAYRETICLE_BINOCULARS = 4,
    WEAPOVERLAYRETICLE_NUM = 5
};

enum weapProjExposion_t {
    WEAPPROJEXP_GRENADE = 0,
    WEAPPROJEXP_ROCKET = 1,
    WEAPPROJEXP_NONE = 2,
    WEAPPROJEXP_NUM = 3
};

enum weapSlot_t {
    WEAPSLOT_NONE = 0,
    WEAPSLOT_PRIMARY = 1,
    WEAPSLOT_PRIMARYB = 2,
    WEAPSLOT_NUM = 3
};

enum weapStance_t {
    WEAPSTANCE_STAND = 0,
    WEAPSTANCE_DUCK = 1,
    WEAPSTANCE_PRONE = 2,
    WEAPSTANCE_NUM = 3
};

enum weapType_t {
    WEAPTYPE_BULLET = 0,
    WEAPTYPE_GRENADE = 1,
    WEAPTYPE_PROJECTILE = 2,
    WEAPTYPE_BINOCULARS = 3,
    WEAPTYPE_NUM = 4
};

typedef int Bits;
typedef int CGContext;
typedef int CGDataProvider;
typedef int CGImage;
typedef int CSpecArray;
typedef int Exop;
typedef int FxImpactEntry;
typedef int HighPart;
typedef int LocalizeEntry;
typedef int LowPart;
typedef int MarkVertAssemblyBuffer;
typedef int MediaType;
typedef int MovieType;
typedef int MultiSampleCaps;
typedef int Offset;
typedef int OffsetHigh;
typedef int OpaqueAEDataStorageType;
typedef int OpaqueAUGraph;
typedef int OpaqueAudioConverter;
typedef int OpaqueControlRef;
typedef int OpaqueDialogPtr;
typedef int OpaqueEventHandlerRef;
typedef int OpaqueEventRef;
typedef int OpaqueEventTargetRef;
typedef int OpaqueFSIterator;
typedef int OpaqueGrafPtr;
typedef int OpaqueIBNibRef;
typedef int OpaqueIconRef;
typedef int OpaqueMenuRef;
typedef int OpaqueRgnHandle;
typedef int OpaqueTXNObject;
typedef int OpaqueWindowPtr;
typedef int Pointer;
typedef int RawFile;
typedef int SndDriverGlobals;
typedef int TrackType;
typedef int UINT;
typedef int _11;
typedef int _12;
typedef int _13;
typedef int _14;
typedef int _21;
typedef int _22;
typedef int _23;
typedef int _24;
typedef int _31;
typedef int _32;
typedef int _33;
typedef int _34;
typedef int _41;
typedef int _42;
typedef int _43;
typedef int _44;
typedef int _CGDirectDisplayID;
typedef int _CGLContextObject;
typedef int _CGLPixelFormatObject;
typedef int _CGLRendererInfoObject;
typedef int _ReferenceType2;
typedef int _SAMPLE;
typedef int _STREAM;
typedef int __AGLContextRec;
typedef int __AGLPixelFormatRec;
typedef int __CFArray;
typedef int __CFBundle;
typedef int __CFData;
typedef int __CFDictionary;
typedef int __CFString;
typedef int __CFURL;
typedef int __EventLoopTimer;
typedef int __sFILEX;
typedef int _telldir;
typedef int activeOccluderBuffer_t;
typedef int allocatedVerts;
typedef int archival;
typedef int baseCoords;
typedef int baseIndex;
typedef int baseVertex;
typedef int bb;
typedef int blens;
#if !defined(__cplusplus) && (!defined(__STDC_VERSION__) || __STDC_VERSION__ < 202311L)
typedef int bool;
#endif
typedef int boolean;
typedef int bound;
typedef int brushmodel;

typedef int bufRate;
typedef int bufReady;
typedef int bufSize;
typedef int bufUsed;

typedef int byteLen;
typedef int bytes_left;
typedef int bytes_used;
typedef int centityArray_t;
typedef int cg_eachClientLocalEntities_t;
typedef int cg_itemsArray_t;
typedef int cg_solidEntities_t;
typedef int cg_weaponsArray_t;
typedef int channels;
typedef int channelvolume;
typedef int check;
typedef int classnum;
typedef int code;
typedef int codes;

typedef int colorForDir;
typedef int copy;
typedef int corpse;
typedef int current;
typedef int data;
typedef int day;
typedef int dayOfWeek;
typedef int dayOfYear;
typedef int ddckCKDestOverlay;
typedef int ddpfPixelFormat;
typedef int decode;
typedef int density;
typedef int dist;
typedef int drylevel;
typedef int duration;
typedef int dwAlphaBitDepth;
typedef int dwBBitMask;
typedef int dwBackBufferCount;
typedef int dwBumpBitCount;
typedef int dwBumpDuBitMask;
typedef int dwBumpDvBitMask;
typedef int dwBumpLuminanceBitMask;
typedef int dwCaps4;
typedef int dwDepth;
typedef int dwEmptyFaceColor;
typedef int dwFVF;
typedef int dwGBitMask;
typedef int dwLinearSize;
typedef int dwLuminanceAlphaBitMask;
typedef int dwLuminanceBitCount;
typedef int dwLuminanceBitMask;
typedef int dwMipMapCount;
typedef int dwOperations;
typedef int dwPrivateFormatBitCount;
typedef int dwRBitMask;
typedef int dwRGBAlphaBitMask;
typedef int dwRGBBitCount;
typedef int dwRGBZBitMask;
typedef int dwRefreshRate;
typedef int dwSrcVBHandle;
typedef int dwStencilBitDepth;
typedef int dwStencilBitMask;
typedef int dwUBitMask;
typedef int dwVBitMask;
typedef int dwVolumeDepth;
typedef int dwYBitMask;
typedef int dwYUVAlphaBitMask;
typedef int dwYUVBitCount;
typedef int dwYUVZBitMask;
typedef int dwZBitMask;
typedef int dwZBufferBitDepth;
typedef int dx7;
typedef int dx9;
typedef int effectClusterArray_t;
typedef int effectListArray_t;
typedef int effectTime;
typedef int end;
typedef int entnum;
typedef int era;
typedef int eraAlt;
typedef int eventParm2;
typedef int fadeInTime;
typedef int fadeOutTime;
typedef int fadeTime;
typedef int firstChildIndex;
typedef int generic;
typedef int get;
typedef int grenade;
typedef int handle;
typedef int hour;
typedef int iNumGameTypes;
typedef int item;
typedef int kickRadius;
typedef int kickRate;
typedef int lPitch;
typedef int left;
typedef int legs;
typedef int list;
typedef int lit;
typedef int loopEndDelay;
typedef int loopFadeTime;

typedef int marker;
typedef int max;
typedef int maxPitchSpeed;
typedef int maxYawSpeed;
typedef int menuItemIndex;
typedef int menuRef;
typedef int method;
typedef int min;
typedef int minute;
typedef int modEndDelay;
typedef int mode;
typedef int month;
typedef int mover;

typedef int need;
typedef int next;
typedef int nextEntId;
typedef int nextFree;
typedef int notifyName;
typedef int offset;
typedef int oldDate;
typedef int origin;
typedef int pad;
typedef int parent;
typedef int parentLocalId;
typedef int partitionIndex;
typedef int parts;
typedef int pathnode_t;
typedef int pathnode_tree_t;
typedef int planeNum;
typedef int playerAnims_t;
typedef int playerconnect;
typedef int playerdamage;
typedef int playerdisconnect;
typedef int playerkilled;
typedef int playervote;
typedef int pm;
typedef int prev;
typedef int prevSibling;
typedef int rate;
typedef int readBuf;
typedef int refCount;
typedef int res1;
typedef int res2;
typedef int res3;
typedef int roomtype;
typedef int root;
typedef int sampleTime;
typedef int scale;
typedef struct script_s script_t;
typedef int second;
typedef int self;
typedef int sensitivity;
typedef int size;

typedef int startupgametype;
typedef int status;

typedef int stride;
typedef int stringCount;
typedef int strings;
typedef int sunVisibility;
typedef int table;
typedef int tb;

typedef int torso;
typedef int tree;
typedef int trees;
typedef int trigger;
typedef int tris;
typedef int turning;
typedef int type;
typedef int use;
typedef int usedVerts;
typedef int user;
typedef int variant;
typedef int vb;
typedef int vertexStride;
typedef int votecalled;
typedef int wBltMSTypes;
typedef int wFlipMSTypes;
typedef int waitTime;
typedef int was;
typedef int weekOfYear;
typedef int wetlevel;
typedef int what;
typedef int width;
typedef int world;
typedef int writeBuf;
typedef int xmodel;
typedef int year;

typedef struct AEDesc AEDesc;
typedef struct AEDescList AEDescList;
typedef struct AERecord AERecord;
typedef struct ANIHeader ANIHeader;
typedef struct ATI4CompsConverterABGR ATI4CompsConverterABGR;
typedef struct ATI4CompsConverterARGB ATI4CompsConverterARGB;
typedef struct AURenderCallbackStruct AURenderCallbackStruct;
typedef struct AbsoluteTime AbsoluteTime;
typedef struct AccessParam AccessParam;
typedef struct AlertStdCFStringAlertParamRec AlertStdCFStringAlertParamRec;
typedef struct AntilagClientStore AntilagClientStore;
typedef struct AppleEvent AppleEvent;
typedef struct AttribInfo AttribInfo;
typedef struct AudioBuffer AudioBuffer;
typedef struct AudioBufferList AudioBufferList;
typedef struct AudioChannelDescription AudioChannelDescription;
typedef struct AudioChannelLayout AudioChannelLayout;
typedef struct AudioStreamBasicDescription AudioStreamBasicDescription;
typedef struct AudioStreamPacketDescription AudioStreamPacketDescription;
typedef struct AudioTimeStamp AudioTimeStamp;
typedef struct BackCompatibleParameters BackCompatibleParameters;
typedef struct BreakStatementInfo BreakStatementInfo;
typedef struct BuiltInMaterialTable BuiltInMaterialTable;
typedef struct BuiltinFunctionDef BuiltinFunctionDef;
typedef struct BuiltinMethodDef BuiltinMethodDef;
typedef struct CAEDesc CAEDesc;
typedef struct CAEEvent CAEEvent;
typedef struct CAEObject CAEObject;
typedef struct CAERecord CAERecord;
typedef struct CAETarget CAETarget;
typedef struct CAStreamBasicDescription CAStreamBasicDescription;
typedef struct CAudioRecorder CAudioRecorder;
typedef struct CBaseVA CBaseVA;
typedef struct CCacheInfoBlock CCacheInfoBlock;
typedef struct CCallOfDutyEngine CCallOfDutyEngine;
typedef struct CCircularBuffer CCircularBuffer;
typedef struct CColorArray CColorArray;
typedef struct CColorConverter CColorConverter;
typedef struct CD3DXBuffer CD3DXBuffer;
typedef struct CD3DXConstantTable CD3DXConstantTable;
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
typedef struct CDisplayInfo CDisplayInfo;
typedef struct CFUUIDBytes CFUUIDBytes;
typedef struct CFence CFence;
typedef struct CGGammaValue CGGammaValue;
typedef struct CGPoint CGPoint;
typedef struct CGRect CGRect;
typedef struct CGSize CGSize;
typedef union CInfoPBRec CInfoPBRec;
typedef struct CMMapFile CMMapFile;
typedef struct CMacGameEngine CMacGameEngine;
typedef struct CMacLogFile CMacLogFile;
typedef struct CMemoryBuffer CMemoryBuffer;
typedef struct CMutex CMutex;
typedef struct CNormalArray CNormalArray;
typedef struct COpenGL COpenGL;
typedef struct COpenGLARBFragmentProgram COpenGLARBFragmentProgram;
typedef struct COpenGLATITextFragmentShader COpenGLATITextFragmentShader;
typedef struct COpenGLMatrix COpenGLMatrix;
typedef struct COpenGLNVidiaRegisterCombinersProgram COpenGLNVidiaRegisterCombinersProgram;
typedef struct COpenGLTexture COpenGLTexture;
typedef struct COpenGLVAO COpenGLVAO;
typedef struct COpenGLVertexProgram COpenGLVertexProgram;
typedef struct CProcRec CProcRec;
typedef struct CRectList CRectList;
typedef struct CResInfo CResInfo;
typedef struct CSParam CSParam;
typedef struct CSampleSound CSampleSound;
typedef struct CSecondaryColorArray CSecondaryColorArray;
typedef struct CSoundEngine CSoundEngine;
typedef struct CSoundObject CSoundObject;
typedef struct CStaticCacheInfo CStaticCacheInfo;
typedef struct CStreamSound CStreamSound;
typedef struct CStringEdPackage CStringEdPackage;
typedef struct CTexCoordArray CTexCoordArray;
typedef struct CTexStage CTexStage;
typedef struct CTexUnit CTexUnit;
typedef struct CThread CThread;
typedef struct CVAOPacket CVAOPacket;
typedef struct CVAOPacketFixedFunction CVAOPacketFixedFunction;
typedef struct CVAOPacketProgrammable CVAOPacketProgrammable;
typedef struct CVertexArray CVertexArray;
typedef struct CachedAssets_t CachedAssets_t;
typedef struct CardMemory CardMemory;
typedef struct CaseStatementInfo CaseStatementInfo;
typedef struct CatPositionRec CatPositionRec;
typedef struct ClientVoicePacket_t ClientVoicePacket_t;
typedef struct Cloud Cloud;
typedef struct CodeConstantSource CodeConstantSource;
typedef struct CodeSamplerSource CodeSamplerSource;
typedef struct CollisionAabbTree CollisionAabbTree;
typedef union CollisionAabbTreeIndex CollisionAabbTreeIndex;
typedef struct CollisionBorder CollisionBorder;
typedef struct CollisionEdge CollisionEdge;
typedef struct CollisionPartition CollisionPartition;
typedef struct CollisionTriangle CollisionTriangle;
typedef struct CollisionVertex CollisionVertex;
typedef struct ColorSpec ColorSpec;
typedef struct ColorTable ColorTable;
typedef struct CombFilterMem CombFilterMem;
typedef struct ComponentDescription ComponentDescription;
typedef struct ComponentInstanceRecord ComponentInstanceRecord;
typedef struct ComponentRecord ComponentRecord;
typedef struct CompressionInfo CompressionInfo;
typedef struct ConDrawInputGlob ConDrawInputGlob;
typedef struct Console Console;
typedef struct ContinueStatementInfo ContinueStatementInfo;
typedef struct ControlEditTextSelectionRec ControlEditTextSelectionRec;
typedef struct ControlFontStyleRec ControlFontStyleRec;
typedef struct ControlID ControlID;
typedef struct ControlKind ControlKind;
typedef struct ConversionArguments ConversionArguments;
typedef struct CopyParam CopyParam;
typedef struct Cursor Cursor;
typedef struct Cylinder Cylinder;
typedef struct D3DDISPLAYMODE D3DDISPLAYMODE;
typedef struct D3DTEXTUREOP D3DTEXTUREOP;
typedef struct D3DXFLOAT16 D3DXFLOAT16;
typedef struct D3DXMATRIX D3DXMATRIX;
typedef struct D3DXVECTOR3 D3DXVECTOR3;
typedef struct D3DXVECTOR4 D3DXVECTOR4;

typedef struct DInfo DInfo;
typedef struct DObjAnimMat DObjAnimMat_s;
typedef struct DObjModel_s DObjModel_s;
typedef struct DObjSkelMat_s DObjSkelMat_s;
typedef struct DObjTrace_s DObjTrace_s;
typedef struct DObj_s DObj_s;
typedef struct DSkel DSkel;
typedef struct DSurface_s DSurface_s;
typedef struct DXInfo DXInfo;
typedef struct DXTexColor DXTexColor;
typedef struct DXVector3 DXVector3;
typedef struct DXVector4 DXVector4;
typedef struct DateTimeRec DateTimeRec;
typedef struct DebugGlobals DebugGlobals;
typedef struct DecState DecState;
typedef struct DirInfo DirInfo;
typedef struct DiskCollAabbTree DiskCollAabbTree;
typedef struct DiskCollBorder DiskCollBorder;
typedef struct DiskCollEdge DiskCollEdge;
typedef struct DiskCollPartition DiskCollPartition;
typedef struct DiskCollTriangle DiskCollTriangle;
typedef struct DiskCollVertex DiskCollVertex;
typedef struct DiskGfxAabbTree DiskGfxAabbTree;
typedef struct DiskGfxCell DiskGfxCell;
typedef struct DiskGfxCullGroup DiskGfxCullGroup;
typedef struct DiskGfxOccluder DiskGfxOccluder;
typedef struct DiskGfxOccluderEdge DiskGfxOccluderEdge;
typedef struct DiskGfxPortal DiskGfxPortal;
typedef struct DiskGfxVertex DiskGfxVertex;
typedef struct DiskLeaf DiskLeaf;
typedef struct DiskTriangleSoup DiskTriangleSoup;
typedef struct DpvsDebug DpvsDebug;
typedef struct DpvsPlane DpvsPlane;
typedef union DpvsPlaneInfo DpvsPlaneInfo;
typedef struct DpvsScene DpvsScene;
typedef union DvarLimits DvarLimits;
typedef union DvarValue DvarValue;
typedef struct DxCapsCheckBits DxCapsCheckBits;
typedef struct DxCapsCheckInteger DxCapsCheckInteger;
typedef struct DxGlobals DxGlobals;
typedef struct DxState DxState;
typedef struct DxStencilDecode DxStencilDecode;
typedef struct DxTextureOpDecode DxTextureOpDecode;
typedef struct DxTextureStageEnums DxTextureStageEnums;
typedef struct Effect Effect;
typedef struct EffectCluster EffectCluster;
typedef struct EffectPrimitive EffectPrimitive;
typedef struct FxEffectDef EffectTemplate;
typedef union EffectTemplateLoadPtr EffectTemplateLoadPtr;
typedef struct EffectVisInfo EffectVisInfo;
typedef struct Emitter Emitter;
typedef struct EncState EncState;
typedef struct EventRecord EventRecord;
typedef struct EventTypeSpec EventTypeSpec;
typedef struct FFData FFData;
typedef struct FFItem FFItem;
typedef struct FIDParam FIDParam;
typedef struct FIFO FIFO;
typedef struct FInfo FInfo;
typedef struct FSCatalogInfo FSCatalogInfo;
typedef struct FSRef FSRef;
typedef struct FSSpec FSSpec;
typedef struct FSVolumeInfo FSVolumeInfo;
typedef struct FXInfo FXInfo;
typedef struct Flash Flash;
typedef struct Font_s Font_s;
typedef struct ForeignPrivParam ForeignPrivParam;
typedef struct FreeRequest FreeRequest;
typedef struct FxArchive FxArchive;
typedef struct FxBoltFrame FxBoltFrame;
typedef struct FxBoltFramePtr FxBoltFramePtr;
typedef struct FxBoltInfo FxBoltInfo;
typedef struct FxCamera FxCamera;
typedef struct FxChannel FxChannel;
typedef struct FxChannelBackwardCompatible FxChannelBackwardCompatible;
typedef struct FxChannelInstance FxChannelInstance;
typedef struct FxCurve FxCurve;
typedef struct FxCurveIterator FxCurveIterator;
typedef struct FxFlagEntry FxFlagEntry;
typedef struct FxGfxEntity FxGfxEntity;
typedef struct FxHelper FxHelper;
typedef struct FxImpactTable FxImpactTable;
typedef struct FxMemMgr_Cloud FxMemMgr_Cloud;
typedef struct FxMemMgr_Cylinder FxMemMgr_Cylinder;
typedef struct FxMemMgr_Effect FxMemMgr_Effect;
typedef struct FxMemMgr_Emitter FxMemMgr_Emitter;
typedef struct FxMemMgr_FxBoltFrame FxMemMgr_FxBoltFrame;
typedef struct FxMemMgr_Light FxMemMgr_Light;
typedef struct FxMemMgr_Line FxMemMgr_Line;
typedef struct FxMemMgr_OrientedParticle FxMemMgr_OrientedParticle;
typedef struct FxMemMgr_Particle FxMemMgr_Particle;
typedef struct FxMemMgr_PrimitiveTemplate FxMemMgr_PrimitiveTemplate;
typedef struct FxMemMgr_ScheduledEffect FxMemMgr_ScheduledEffect;
typedef struct FxMemMgr_Tail FxMemMgr_Tail;
typedef struct FxRange FxRange;
typedef struct FxScheduler FxScheduler;
typedef struct GDevice GDevice;
typedef struct GPGroup GPGroup;
typedef struct GPObject GPObject;
typedef struct GPValue GPValue;
typedef struct GenericParser2 GenericParser2;
typedef struct GetVolParmsInfoBuffer GetVolParmsInfoBuffer;
typedef struct GfxAabbTree GfxAabbTree;
typedef struct GfxBackEndData GfxBackEndData;
typedef struct GfxBackEndLightSettings GfxBackEndLightSettings;
typedef struct GfxBrushModel GfxBrushModel;
typedef struct GfxBspLoad GfxBspLoad;
typedef struct GfxCachedShaderText GfxCachedShaderText;
typedef struct GfxCell GfxCell;
typedef struct GfxCellWritable GfxCellWritable;
typedef struct GfxCmdApplyLatePostEffects GfxCmdApplyLatePostEffects;
typedef struct GfxCmdArray GfxCmdArray;
typedef struct GfxCmdBeginView GfxCmdBeginView;
typedef struct GfxCmdBlendSavedScreen GfxCmdBlendSavedScreen;
typedef struct GfxCmdCall GfxCmdCall;
typedef struct GfxCmdClearScreen GfxCmdClearScreen;
typedef struct GfxCmdDrawFullScreenColoredQuad GfxCmdDrawFullScreenColoredQuad;
typedef struct GfxCmdDrawLines GfxCmdDrawLines;
typedef struct GfxCmdDrawPoints GfxCmdDrawPoints;
typedef struct GfxCmdDrawQuadPic GfxCmdDrawQuadPic;
typedef struct GfxCmdDrawSprite GfxCmdDrawSprite;
typedef struct GfxCmdDrawSunPostEffects GfxCmdDrawSunPostEffects;
typedef struct GfxCmdDrawSurfs GfxCmdDrawSurfs;
typedef struct GfxCmdDrawText GfxCmdDrawText;
typedef struct GfxCmdDrawTextInSpace GfxCmdDrawTextInSpace;
typedef struct GfxCmdDrawTriangles GfxCmdDrawTriangles;
typedef struct GfxCmdHeader GfxCmdHeader;
typedef struct GfxCmdSetClipPlanes GfxCmdSetClipPlanes;
typedef struct GfxCmdSetLightProperties GfxCmdSetLightProperties;
typedef struct GfxCmdSetMaterialColor GfxCmdSetMaterialColor;
typedef struct GfxCmdSetRenderTarget GfxCmdSetRenderTarget;
typedef struct GfxCmdSetShadowCookie GfxCmdSetShadowCookie;
typedef struct GfxCmdSetStencilRefValue GfxCmdSetStencilRefValue;
typedef struct GfxCmdSetViewport GfxCmdSetViewport;
typedef struct GfxCmdStencilPlanes GfxCmdStencilPlanes;
typedef struct GfxCmdStretchPic GfxCmdStretchPic;
typedef struct GfxCmdStretchPicRotate GfxCmdStretchPicRotate;
typedef struct GfxCmdStretchRaw GfxCmdStretchRaw;
typedef struct GfxCodeMatrices GfxCodeMatrices;
typedef struct GfxCodeMatrix GfxCodeMatrix;
typedef union GfxColor GfxColor;
typedef struct GfxCullGroup GfxCullGroup;
typedef struct GfxCullGroupDynamic GfxCullGroupDynamic;
typedef struct GfxDebugFrameGlob GfxDebugFrameGlob;
typedef struct GfxDebugPlume GfxDebugPlume;
typedef struct GfxDebugPoly GfxDebugPoly;
typedef struct GfxDrawGroupCommands GfxDrawGroupCommands;
typedef struct GfxDrawPrimArgs GfxDrawPrimArgs;
typedef struct GfxDrawSurf GfxDrawSurf;
typedef struct GfxEntity GfxEntity;
typedef struct GfxEntityLighting GfxEntityLighting;
typedef struct GfxFog GfxFog;
typedef struct GfxGammaRamp GfxGammaRamp;
typedef struct GfxImage GfxImage;
typedef struct GfxImageFileHeader GfxImageFileHeader;
typedef struct GfxImageFilter GfxImageFilter;
typedef struct GfxImageFilterPass GfxImageFilterPass;
typedef struct GfxImageLoadDef GfxImageLoadDef;
typedef struct GfxLight GfxLight;
typedef struct GfxLightDef GfxLightDef;
typedef struct GfxLightDir GfxLightDir;
typedef struct GfxLightGrid GfxLightGrid;
typedef struct GfxLightGridColors GfxLightGridColors;
typedef struct GfxLightGridPoint GfxLightGridPoint;
typedef struct GfxLightImage GfxLightImage;
typedef union GfxLightInfo GfxLightInfo;
typedef struct GfxLodParms GfxLodParms;
typedef struct GfxMarkFragment GfxMarkFragment;
typedef struct GfxMarkPoint GfxMarkPoint;
typedef union GfxModel GfxModel;
typedef struct GfxModelRigidSurface GfxModelRigidSurface;
typedef struct GfxModelSkinnedSurface GfxModelSkinnedSurface;
typedef struct GfxModelSurface GfxModelSurface;
typedef struct GfxOccluder GfxOccluder;
typedef struct GfxOccluderEdge GfxOccluderEdge;
typedef struct GfxPointVertex GfxPointVertex;
typedef struct GfxPortal GfxPortal;
typedef struct GfxPortalWritable GfxPortalWritable;
typedef struct GfxPosTexVertex GfxPosTexVertex;
typedef struct GfxPostProcessParms GfxPostProcessParms;
typedef struct GfxRenderCommandExecState GfxRenderCommandExecState;
typedef struct GfxRenderTarget GfxRenderTarget;
typedef struct GfxSModelCachedVertex GfxSModelCachedVertex;
typedef struct GfxSModelCachedVertexDx7 GfxSModelCachedVertexDx7;
typedef struct GfxScene GfxScene;
typedef struct GfxSceneDef GfxSceneDef;
typedef struct GfxSceneEntity GfxSceneEntity;
typedef struct GfxSceneModelCellRef GfxSceneModelCellRef;
typedef struct GfxStateOverride GfxStateOverride;
typedef struct GfxStaticModelCachedSurface GfxStaticModelCachedSurface;
typedef struct GfxStaticModelDynamic GfxStaticModelDynamic;
typedef struct GfxStaticModelInstance GfxStaticModelInstance;
typedef struct GfxStaticModelSurfaceCached GfxStaticModelSurfaceCached;
typedef struct GfxStaticSurface GfxStaticSurface;
typedef struct GfxSurface GfxSurface;
typedef struct GfxSurfaceDynamic GfxSurfaceDynamic;
typedef union GfxTexture GfxTexture;
typedef struct GfxVertex GfxVertex;
typedef struct GfxVertexDx7 GfxVertexDx7;
typedef struct GfxViewParms GfxViewParms;
typedef struct GfxViewport GfxViewport;
typedef struct GfxWindowParms GfxWindowParms;
typedef struct GfxWindowTarget GfxWindowTarget;
typedef struct GfxWorld GfxWorld;
typedef struct GfxWorldVertex GfxWorldVertex;
typedef struct GfxWorldVertexData GfxWorldVertexData;
typedef struct GfxWorldVertexDx7 GfxWorldVertexDx7;
typedef struct Glyph Glyph;
typedef struct HCURSOR__ HCURSOR__;
typedef struct HDC__ HDC__;
typedef struct HFSUniStr255 HFSUniStr255;
typedef struct HFileInfo HFileInfo;
typedef struct HFileParam HFileParam;
typedef struct HICommand HICommand;
typedef struct HINSTANCE__ HINSTANCE__;
typedef struct HIOParam HIOParam;
typedef struct HISize HISize;
typedef struct HMONITOR__ HMONITOR__;
typedef union HParamBlockRec HParamBlockRec;
typedef struct HVolumeParam HVolumeParam;
typedef struct HWND__ HWND__;
typedef struct HashEntry HashEntry;
typedef struct ID3DXBuffer ID3DXBuffer;
typedef struct ID3DXConstantTable ID3DXConstantTable;
typedef struct ID3DXFont ID3DXFont;
typedef struct ID3DXInclude ID3DXInclude;
typedef struct ID3DXSprite ID3DXSprite;
typedef struct IDirect3D9 IDirect3D9;
typedef struct IDirect3DBaseTexture9 IDirect3DBaseTexture9;
typedef struct IDirect3DCubeTexture9 IDirect3DCubeTexture9;
typedef struct IDirect3DDevice9 IDirect3DDevice9;
typedef struct IDirect3DIndexBuffer9 IDirect3DIndexBuffer9;
typedef struct IDirect3DPixelShader9 IDirect3DPixelShader9;
typedef struct IDirect3DQuery9 IDirect3DQuery9;
typedef struct IDirect3DResource9 IDirect3DResource9;
typedef struct IDirect3DStateBlock9 IDirect3DStateBlock9;
typedef struct IDirect3DSurface9 IDirect3DSurface9;
typedef struct IDirect3DSwapChain9 IDirect3DSwapChain9;
typedef struct IDirect3DTexture9 IDirect3DTexture9;
typedef union IDirect3DVertexBuffer9 IDirect3DVertexBuffer9;
typedef struct IDirect3DVertexDeclaration9 IDirect3DVertexDeclaration9;
typedef struct IDirect3DVertexShader9 IDirect3DVertexShader9;
typedef struct IDirect3DVolume9 IDirect3DVolume9;
typedef struct IDirect3DVolumeTexture9 IDirect3DVolumeTexture9;
typedef struct IID IID;
typedef struct ITab ITab;
typedef struct IUnknown IUnknown;
typedef struct ImageDescription ImageDescription;
typedef struct ImageList ImageList;
typedef struct Image_MemUsage Image_MemUsage;
typedef struct IncludeClass IncludeClass;
typedef struct ItemKeyHandler ItemKeyHandler;
typedef struct JHUFF_TBL JHUFF_TBL;
typedef struct JQUANT_TBL JQUANT_TBL;
typedef struct KeyStruct KeyStruct;
typedef struct LARGE_INTEGER LARGE_INTEGER;
typedef struct LSItemInfoRecord LSItemInfoRecord;
typedef struct LargeLocal LargeLocal;
typedef struct LegacyHacks LegacyHacks;
typedef struct Light Light;
typedef struct LightInfoType LightInfoType;
typedef struct Line Line;
typedef struct LocalDateTime LocalDateTime;
typedef union LongDateRec LongDateRec;
typedef struct MD4_CTX MD4_CTX;
typedef struct MacImageInfo MacImageInfo;
typedef struct MacRGBColor MacRGBColor;
typedef struct MacRect MacRect;
typedef struct MantleAnimTransition MantleAnimTransition;
typedef struct MantleResults MantleResults;
typedef struct MantleState MantleState;
typedef struct MarkPoly MarkPoly;
typedef struct Material Material;
typedef struct MaterialArgumentCodeConst MaterialArgumentCodeConst;
typedef union MaterialArgumentDef MaterialArgumentDef;
typedef struct MaterialConstantDefObj MaterialConstantDefObj;
typedef struct MaterialInfo MaterialInfo;
typedef struct MaterialInfoObj_s MaterialInfoObj_s;
typedef struct MaterialMemory MaterialMemory;
typedef struct MaterialObj MaterialObj;
typedef union MaterialPassArray MaterialPassArray;
typedef struct MaterialPassDx7 MaterialPassDx7;
typedef struct MaterialPassDx9 MaterialPassDx9;
typedef struct MaterialShader MaterialShader;
typedef struct MaterialShaderArgument MaterialShaderArgument;
typedef union MaterialShaderPtr MaterialShaderPtr;
typedef struct MaterialStateMap MaterialStateMap;
typedef struct MaterialStateMapRule MaterialStateMapRule;
typedef struct MaterialStateMapRuleSet MaterialStateMapRuleSet;
typedef struct MaterialStreamRouting MaterialStreamRouting;
typedef struct MaterialTechnique MaterialTechnique;
typedef struct MaterialTechniqueSet MaterialTechniqueSet;
typedef struct MaterialTextureDef MaterialTextureDef;
typedef union MaterialTextureDefInfo MaterialTextureDefInfo;
typedef struct MaterialVertexDeclaration MaterialVertexDeclaration;
typedef struct MaterialWaterDef MaterialWaterDef;
typedef struct MediaHandles MediaHandles;
typedef struct MemoryFile MemoryFile;
typedef struct MemoryNode MemoryNode;
typedef struct MenuList MenuList;
typedef struct MessageBuffer MessageBuffer;
typedef struct MessageLine MessageLine;
typedef struct MessageWindow MessageWindow;
typedef struct MixerDistanceParams MixerDistanceParams;
typedef struct MssLocal MssLocal;
typedef struct MssSound MssSound;
typedef struct MtlStateMapBitGroup MtlStateMapBitGroup;
typedef struct MtlStateMapBitName MtlStateMapBitName;
typedef struct MtlTextureFunctionDx7 MtlTextureFunctionDx7;
typedef struct NetField NetField;
typedef struct NumVersion NumVersion;
typedef struct ObjParam ObjParam;
typedef struct ObjectInfo ObjectInfo;
typedef struct OpaqueContextRef OpaqueContextRef;
typedef struct OpcodeLookup OpcodeLookup;
typedef struct OpenGLStateInfoType OpenGLStateInfoType;
typedef struct OrientedParticle OrientedParticle;
typedef struct OutdoorGlob OutdoorGlob;
typedef struct ParameterType ParameterType;
typedef struct ParseThreadInfo ParseThreadInfo;
typedef struct Particle Particle;
typedef struct PassOptionDx7 PassOptionDx7;
typedef struct Picmip Picmip;
typedef struct PixMap PixMap;
typedef struct PlayerKeyState PlayerKeyState;
typedef struct PlayerProfileStatus PlayerProfileStatus;
typedef struct Point Point;
typedef struct PointLightPartition PointLightPartition;
typedef struct PortalHeapNode PortalHeapNode;
typedef struct PrecacheEntry PrecacheEntry;
typedef struct PrimitiveTemplate PrimitiveTemplate;
typedef struct ProcessInfoRec ProcessInfoRec;
typedef struct ProcessSerialNumber ProcessSerialNumber;
typedef struct ProgrammableShaderType ProgrammableShaderType;
typedef struct QElem QElem;
typedef struct RTL_CRITICAL_SECTION RTL_CRITICAL_SECTION;
typedef struct RefString RefString;
typedef struct RemoveDirectoryInfo RemoveDirectoryInfo;
typedef struct SBDecState SBDecState;
typedef struct SBEncState SBEncState;
typedef struct SMPTETime SMPTETime;
typedef struct SProcRec SProcRec;
typedef struct SaveSourceBufferInfo SaveSourceBufferInfo;
typedef struct ScheduledEffect ScheduledEffect;
typedef struct Scr_SourcePos_t Scr_SourcePos_t;
typedef struct ScreenPlacementGlob ScreenPlacementGlob;
typedef struct ShaderConstantRouting ShaderConstantRouting;
typedef struct ShadowCandidate ShadowCandidate;
typedef struct ShadowCookie ShadowCookie;
typedef struct ShadowCookieGlob ShadowCookieGlob;
typedef struct ShadowCookieList ShadowCookieList;
typedef struct SkinBuffers SkinBuffers;
typedef struct SkinRigidXModelCmd SkinRigidXModelCmd;
typedef struct SkinStaticModelCachedCmd SkinStaticModelCachedCmd;
typedef struct SkinXModelCmd SkinXModelCmd;
typedef struct SndCurve SndCurve;
typedef struct SortedCluster SortedCluster;
typedef struct SortedEffect SortedEffect;
typedef struct SoundDescription SoundDescription;
typedef struct SoundFile SoundFile;
typedef struct SoundFileInfo SoundFileInfo;
typedef struct SourceBufferInfo SourceBufferInfo;
typedef struct SourceLookup SourceLookup;
typedef struct SpawnVar SpawnVar;
typedef struct SpeexBits SpeexBits;
typedef struct SpeexCallback SpeexCallback;
typedef struct SpeexMode SpeexMode;
typedef struct SpeexNBMode SpeexNBMode;
typedef struct SpeexSBMode SpeexSBMode;
typedef struct SpeexSubmode SpeexSubmode;
typedef struct StColorConverter StColorConverter;
typedef struct StGWorldState StGWorldState;
typedef struct StMutexLock StMutexLock;
typedef struct StPortState StPortState;
typedef struct StSetDirectory StSetDirectory;
typedef struct StShowCursor StShowCursor;
typedef struct StThreadLock StThreadLock;
typedef struct StdConverterABGR StdConverterABGR;
typedef struct StdConverterARGB StdConverterARGB;
typedef struct SunFlareDynamic SunFlareDynamic;
typedef struct SunLightParseParams SunLightParseParams;
typedef struct SysInfo SysInfo;
typedef union TMediaElement TMediaElement;
typedef struct TMediaList TMediaList;
typedef struct TXNATSUIFeatures TXNATSUIFeatures;
typedef struct TXNATSUIVariations TXNATSUIVariations;
typedef union TXNAttributeData TXNAttributeData;
typedef struct TXNTypeAttributes TXNTypeAttributes;
typedef struct Tail Tail;
typedef struct TestLod TestLod;
typedef struct TextPool TextPool;
typedef struct TextureInfoType TextureInfoType;
typedef struct ThreadDebugInfo ThreadDebugInfo;
typedef struct TraceCheckCount TraceCheckCount;
typedef struct TraceExtents TraceExtents;
typedef struct TraceThreadInfo TraceThreadInfo;
typedef struct Tuple Tuple;
typedef struct UI_Component UI_Component;
typedef struct UI_Component_data_t UI_Component_data_t;
typedef struct UI_LinesComponent UI_LinesComponent;
typedef struct UI_ScrollPane UI_ScrollPane;
typedef struct UI_VerticalDivider UI_VerticalDivider;
typedef struct UTCDateTime UTCDateTime;
typedef struct UnsignedWide UnsignedWide;
typedef struct VBRState VBRState;
typedef struct Variable Variable;
typedef struct VariableCompileValue VariableCompileValue;
typedef struct VariableStackBuffer VariableStackBuffer;
typedef union VariableUnion VariableUnion;
typedef struct VariableValueInternal VariableValueInternal;
typedef struct VariableValue_s VariableValue_s;
typedef struct VertexProgramState VertexProgramState;
typedef struct VertexProgramStreamState VertexProgramStreamState;
typedef struct VoicePacket_t VoicePacket_t;
typedef struct VolumeModGroup VolumeModGroup;
typedef struct WDParam WDParam;
typedef struct WSAData WSAData;
typedef struct WaterGlob WaterGlob;
typedef struct WaterWritable WaterWritable;
typedef struct WaveletDecode WaveletDecode;
typedef struct WaveletHuffmanDecode WaveletHuffmanDecode;
typedef struct WeaponDef WeaponDef;
typedef struct WinCursor WinCursor;
typedef struct WinIcon WinIcon;
typedef struct WinVars_t WinVars_t;
typedef struct Window Window;
typedef struct WindowAttributes WindowAttributes;
typedef struct XAnimCalcAnimInfo XAnimCalcAnimInfo;
typedef struct XAnimDeltaPart XAnimDeltaPart;
typedef struct XAnimDeltaPartQuat XAnimDeltaPartQuat;
typedef union XAnimDeltaPartQuatData XAnimDeltaPartQuatData;
typedef struct XAnimDeltaPartQuatDataFrames XAnimDeltaPartQuatDataFrames;
typedef struct XAnimDeltaPartTrans XAnimDeltaPartTrans;
typedef union XAnimDeltaPartTransData XAnimDeltaPartTransData;
typedef struct XAnimDeltaPartTransFrames XAnimDeltaPartTransFrames;
typedef union XAnimDynamicIndices XAnimDynamicIndices;
typedef struct XAnimEntry XAnimEntry;
typedef struct XAnimInfo XAnimInfo;
typedef struct XAnimNotifyInfo XAnimNotifyInfo;
typedef struct XAnimNotify_s XAnimNotify_s;
typedef struct XAnimParent XAnimParent;
typedef struct XAnimPart XAnimPart;
typedef struct XAnimPartQuat XAnimPartQuat;
typedef union XAnimPartQuatData XAnimPartQuatData;
typedef struct XAnimPartQuatDataFrames XAnimPartQuatDataFrames;
typedef union XAnimPartQuatFrames XAnimPartQuatFrames;
typedef struct XAnimPartTrans XAnimPartTrans;
typedef union XAnimPartTransData XAnimPartTransData;
typedef struct XAnimPartTransFrames XAnimPartTransFrames;
typedef struct XAnimParts_s XAnimParts_s;
typedef struct XAnimSimpleRotPos XAnimSimpleRotPos;
typedef struct XAnimState XAnimState;
typedef struct XAnimTime XAnimTime;
typedef struct XAnimToXModel XAnimToXModel;
typedef struct XAnimTree XAnimTree;
typedef struct XAnimTree_s XAnimTree_s;
typedef struct XAnim_s XAnim_s;
typedef union XAssetHeader XAssetHeader;
typedef struct XBlendInfo_s XBlendInfo_s;
typedef struct XBoneHierarchy XBoneHierarchy;
typedef struct XBoneInfo_s XBoneInfo_s;
typedef struct XModel XModel;
typedef struct XModelCollSurf_s XModelCollSurf_s;
typedef struct XModelCollTri_s XModelCollTri_s;
typedef struct XModelConfig XModelConfig;
typedef struct XModelConfigEntry XModelConfigEntry;
typedef struct XModelDefault XModelDefault;
typedef struct XModelLodInfo XModelLodInfo;
typedef struct XModelParts_s XModelParts_s;
typedef struct XModelSurfs_s XModelSurfs_s;
typedef struct XRigidSurface XRigidSurface;
typedef struct XSurface_s XSurface_s;
typedef struct XVertexBuffer XVertexBuffer;
typedef struct XVertexInfo_s XVertexInfo_s;
typedef struct _ADPCMDATATAG _ADPCMDATATAG;
typedef struct _AILMIXINFO _AILMIXINFO;
typedef struct _AILSOUNDINFO _AILSOUNDINFO;
typedef struct _Alloc_hider _Alloc_hider;
typedef struct _D3DADAPTER_IDENTIFIER9 _D3DADAPTER_IDENTIFIER9;
typedef struct _D3DBOX _D3DBOX;
typedef struct _D3DCAPS9 _D3DCAPS9;
typedef struct _D3DCLIPSTATUS9 _D3DCLIPSTATUS9;
typedef struct _D3DCOLORVALUE _D3DCOLORVALUE;
typedef struct _D3DDEVICE_CREATION_PARAMETERS _D3DDEVICE_CREATION_PARAMETERS;
typedef struct _D3DDISPLAYMODE _D3DDISPLAYMODE;
typedef struct _D3DGAMMARAMP _D3DGAMMARAMP;
typedef struct _D3DINDEXBUFFER_DESC _D3DINDEXBUFFER_DESC;
typedef struct _D3DLIGHT9 _D3DLIGHT9;
typedef struct _D3DLOCKED_BOX _D3DLOCKED_BOX;
typedef struct _D3DLOCKED_RECT _D3DLOCKED_RECT;
typedef struct _D3DMATERIAL9 _D3DMATERIAL9;
typedef struct _D3DMATRIX _D3DMATRIX;
typedef struct _D3DPRESENT_PARAMETERS_ _D3DPRESENT_PARAMETERS_;
typedef struct _D3DPSHADERCAPS2_0 _D3DPSHADERCAPS2_0;
typedef struct _D3DRASTER_STATUS _D3DRASTER_STATUS;
typedef struct _D3DRECT _D3DRECT;
typedef struct _D3DRECTPATCH_INFO _D3DRECTPATCH_INFO;
typedef struct _D3DSURFACE_DESC _D3DSURFACE_DESC;
typedef struct _D3DTRIPATCH_INFO _D3DTRIPATCH_INFO;
typedef struct _D3DVECTOR _D3DVECTOR;
typedef struct _D3DVERTEXBUFFER_DESC _D3DVERTEXBUFFER_DESC;
typedef struct _D3DVERTEXELEMENT9 _D3DVERTEXELEMENT9;
typedef struct _D3DVIEWPORT9 _D3DVIEWPORT9;
typedef struct _D3DVOLUME_DESC _D3DVOLUME_DESC;
typedef struct _D3DVSHADERCAPS2_0 _D3DVSHADERCAPS2_0;
typedef struct _D3DXCONSTANTTABLE_DESC _D3DXCONSTANTTABLE_DESC;
typedef struct _D3DXCONSTANT_DESC _D3DXCONSTANT_DESC;
typedef struct _D3DXFONT_DESCA _D3DXFONT_DESCA;
typedef struct _D3DXFONT_DESCW _D3DXFONT_DESCW;
typedef struct _D3DXIMAGE_INFO _D3DXIMAGE_INFO;
typedef struct _D3DXMACRO _D3DXMACRO;
typedef struct _D3DXSHADER_CONSTANTINFO _D3DXSHADER_CONSTANTINFO;
typedef struct _D3DXSHADER_CONSTANTTABLE _D3DXSHADER_CONSTANTTABLE;
typedef struct _D3DXSHADER_TYPEINFO _D3DXSHADER_TYPEINFO;
typedef struct _DDCOLORKEY _DDCOLORKEY;
typedef struct _DDPIXELFORMAT _DDPIXELFORMAT;
typedef struct _DDSCAPS2 _DDSCAPS2;
typedef struct _DDSURFACEDESC2 _DDSURFACEDESC2;
typedef struct _Deque_impl _Deque_impl;
typedef struct _Deque_iterator_UINT16_UINT16ref_UINT16ptr _Deque_iterator_UINT16_UINT16ref_UINT16ptr;
typedef struct _FILETIME _FILETIME;
typedef struct _GUID _GUID;
typedef union _LARGE_INTEGER _LARGE_INTEGER;
typedef struct _List_impl _List_impl;
typedef struct _List_iterator_CFenceptr _List_iterator_CFenceptr;
typedef struct _List_iterator_GLuintptr _List_iterator_GLuintptr;
typedef struct _List_iterator_Tuple _List_iterator_Tuple;
typedef struct _List_node_CFenceptr _List_node_CFenceptr;
typedef struct _List_node_GLuintptr _List_node_GLuintptr;
typedef struct _List_node_Tuple _List_node_Tuple;
typedef struct _List_node_base _List_node_base;
typedef struct _MEMORYSTATUS _MEMORYSTATUS;
typedef struct _MEMORY_BASIC_INFORMATION _MEMORY_BASIC_INFORMATION;
typedef struct _OSVERSIONINFOA _OSVERSIONINFOA;
typedef struct _OVERLAPPED _OVERLAPPED;
typedef struct _RGNDATA _RGNDATA;
typedef struct _RGNDATAHEADER _RGNDATAHEADER;
typedef struct _Rb_tree_node_base _Rb_tree_node_base;
typedef struct _Rep _Rep;
typedef struct _Rep_base _Rep_base;
typedef struct _SECURITY_ATTRIBUTES _SECURITY_ATTRIBUTES;
typedef struct _SYSTEMTIME _SYSTEMTIME;
typedef union _ULARGE_INTEGER _ULARGE_INTEGER;
typedef struct _Vector_impl _Vector_impl;
typedef struct _WIN32_FIND_DATAA _WIN32_FIND_DATAA;
typedef struct __darwin_pthread_handler_rec __darwin_pthread_handler_rec;
typedef struct __darwin_pthread_mutex_t __darwin_pthread_mutex_t;
typedef struct __false_type __false_type;
typedef struct __true_type __true_type;
typedef struct _opaque_pthread_mutex_t _opaque_pthread_mutex_t;
typedef struct _opaque_pthread_mutexattr_t _opaque_pthread_mutexattr_t;
typedef struct _opaque_pthread_t _opaque_pthread_t;
typedef struct allocator_CCacheInfoBlock allocator_CCacheInfoBlock;
typedef struct allocator_CDirect3DSurfaceptr allocator_CDirect3DSurfaceptr;
typedef struct allocator_CDirect3DVolumeptr allocator_CDirect3DVolumeptr;
typedef struct allocator_CDisplayInfo allocator_CDisplayInfo;
typedef struct allocator_CFenceptr allocator_CFenceptr;
typedef struct allocator_CResInfo allocator_CResInfo;
typedef struct allocator_CSoundObjectptr allocator_CSoundObjectptr;
typedef struct allocator_CStreamSoundptr allocator_CStreamSoundptr;
typedef struct allocator_FFItem allocator_FFItem;
typedef struct allocator_FSRef allocator_FSRef;
typedef struct allocator_GLuint allocator_GLuint;
typedef struct allocator_GLuintptr allocator_GLuintptr;
typedef struct allocator_MacRect allocator_MacRect;
typedef struct allocator_Tuple allocator_Tuple;
typedef struct allocator_UINT16 allocator_UINT16;
typedef struct allocator_UINT16ptr allocator_UINT16ptr;
typedef struct allocator_UINT32 allocator_UINT32;
typedef struct allocator_UINT8 allocator_UINT8;
typedef struct allocator_WinCursor allocator_WinCursor;
typedef struct allocator_char allocator_char;
typedef struct allocator_float allocator_float;
typedef struct animConditionTable_t animConditionTable_t;
typedef struct animScriptCommand_t animScriptCommand_t;
typedef struct animScriptCondition_t animScriptCondition_t;
typedef struct animScriptData_t animScriptData_t;
typedef struct animScriptItem_t animScriptItem_t;
typedef struct animScript_t animScript_t;
typedef struct animStringItem_t animStringItem_t;
typedef struct animation_s animation_s;
typedef struct archivedEntityShared_t archivedEntityShared_t;
typedef struct archivedEntity_s archivedEntity_s;
typedef struct archivedSnapshot_s archivedSnapshot_s;
typedef struct areaParms_t areaParms_t;
typedef struct audioSample_t audioSample_t;
typedef struct backing_store_struct backing_store_struct;
typedef struct bgs_s bgs_t;
typedef struct bidirectional_iterator_tag bidirectional_iterator_tag;
typedef struct binary_function_UINT32_UINT32_bool binary_function_UINT32_UINT32_bool;
typedef struct bind_t bind_t;
typedef struct bitread_perm_state bitread_perm_state;
typedef struct bitread_working_state bitread_working_state;
typedef struct box box;
typedef struct builtin_s builtin_s;
typedef struct cLeafBrushNodeChildren_t cLeafBrushNodeChildren_t;
typedef union cLeafBrushNodeData_t cLeafBrushNodeData_t;
typedef struct cLeafBrushNodeLeaf_t cLeafBrushNodeLeaf_t;
typedef struct cLeafBrushNode_s cLeafBrushNode_s;
typedef struct cLeaf_t cLeaf_t;
typedef struct cNode_t cNode_t;
typedef struct cStaticModelWritable cStaticModelWritable;
typedef struct cStaticModel_s cStaticModel_s;
typedef struct c_derived_tbl c_derived_tbl;
typedef struct cachedClient_s cachedClient_s;
typedef struct cachedSnapshot_t cachedSnapshot_t;
typedef struct cameraShake_t cameraShake_t;
typedef struct cbrush_t cbrush_t;
typedef struct cbrushside_t cbrushside_t;
typedef struct centity_s centity_s;
typedef struct cgMedia_t cgMedia_t;
typedef struct cg_hudelem_t cg_hudelem_t;
typedef struct cg_t cg_t;
typedef struct cgs_t cgs_t;
typedef struct challenge_t challenge_t;
typedef struct cin_cache cin_cache;
typedef struct cinematics_t cinematics_t;
typedef struct clSnapshot_t clSnapshot_t;
typedef struct clientActive_t clientActive_t;
typedef struct clientConnection_t clientConnection_t;
typedef struct clientDebug_t clientDebug_t;
typedef struct clientInfo_s clientInfo_t;
typedef struct clientLogo_t clientLogo_t;
typedef struct clientSession_t clientSession_t;
typedef struct clientSnapshot_t clientSnapshot_t;
typedef struct clientState_t clientState_s;
typedef struct clientStatic_t clientStatic_t;
typedef struct client_fields_s client_fields_s;
typedef struct client_s client_s;
typedef struct clipMap_t clipMap_t;
typedef struct cm_world_t cm_world_t;
typedef struct cmd_function_s cmd_function_s;
typedef struct cmd_t cmd_t;
typedef struct cmodel_t cmodel_t;
typedef struct columnInfo_s columnInfo_s;
typedef struct com_parse_mark_t com_parse_mark_t;
typedef struct commandDef_t commandDef_t;
typedef struct compassactor_t compassactor_t;
typedef struct complex_s complex_s;
typedef struct consoleCommand_t consoleCommand_t;
typedef struct controller_info_t controller_info_t;
typedef struct corpseInfo_t corpseInfo_t;
typedef struct corpse_ent_t corpse_ent_t;
typedef struct cplane_s cplane_s;
typedef struct cspField_t cspField_t;
typedef struct d_derived_tbl d_derived_tbl;
typedef struct dbrush_t dbrush_t;
typedef struct dbrushside_t dbrushside_t;
typedef struct define_s define_s;
typedef struct dheader_s dheader_s;
typedef struct directive_s directive_s;
typedef struct directive_t directive_t;
typedef struct directory_t directory_t;
typedef struct dirent dirent;
typedef struct displayContextDef_s displayContextDef_s;
typedef struct dleaf_t dleaf_t;
typedef struct dmaterial_t dmaterial_t;
typedef struct dmodel_t dmodel_t;
typedef struct dnode_t dnode_t;
typedef struct dplane_t dplane_t;
typedef struct dsound_sample_t dsound_sample_t;
typedef struct dvar_s dvar_s;
typedef struct editFieldDef_s editFieldDef_s;
typedef struct ent_field_t ent_field_t;
typedef struct entityHandler_t entityHandler_t;
typedef struct entityShared_t entityShared_t;
typedef struct entityState_s entityState_s;
typedef struct field_t field_t;
typedef struct fileData_s fileData_s;
typedef struct fileHandleData_t fileHandleData_t;
typedef struct fileInPack_s fileInPack_s;
typedef struct file_in_zip_read_info_s file_in_zip_read_info_s;
typedef struct forward_iterator_tag forward_iterator_tag;
typedef struct function_frame_t function_frame_t;
typedef struct function_stack_t function_stack_t;
typedef struct g_sa_type g_sa_type;
typedef struct gameState_t gameState_t;
typedef struct gameTypeInfo gameTypeInfo;
typedef struct gameTypeScript_t gameTypeScript_t;
typedef struct game_hudelem_field_t game_hudelem_field_t;
typedef struct game_hudelem_s game_hudelem_s;
typedef struct gclient_s gclient_s;
typedef struct gentity_s gentity_s;
typedef struct gitem_s gitem_s;
typedef struct grenade_ent_t grenade_ent_t;
typedef struct hostent hostent;
typedef union hudelem_color_t hudelem_color_t;
typedef struct hudelem_s hudelem_s;
typedef struct huff_entropy_decoder huff_entropy_decoder;
typedef struct huff_entropy_encoder huff_entropy_encoder;
typedef struct huff_t huff_t;
typedef struct huffman_t huffman_t;
typedef struct hunkHeader_t hunkHeader_t;
typedef struct hunkUsed_t hunkUsed_t;
typedef struct in_addr in_addr;
typedef struct indent_s indent_s;
typedef struct inflate_blocks_state inflate_blocks_state;
typedef struct inflate_codes_state inflate_codes_state;
typedef struct inflate_huft_s inflate_huft_s;
typedef struct infoParm_t infoParm_t;
typedef struct input_iterator_tag input_iterator_tag;
typedef struct internal_state internal_state;
typedef struct ipFilter_s ipFilter_s;
typedef union itemDefData_t itemDefData_t;
typedef struct itemDef_s itemDef_s;
typedef struct itemInfo_t itemInfo_t;
typedef struct item_ent_t item_ent_t;
typedef struct jpeg_alloc jpeg_alloc;
typedef struct jpeg_c_coef_controller jpeg_c_coef_controller;
typedef struct jpeg_c_main_controller jpeg_c_main_controller;
typedef struct jpeg_c_prep_controller jpeg_c_prep_controller;
typedef struct jpeg_color_converter jpeg_color_converter;
typedef struct jpeg_color_deconverter jpeg_color_deconverter;
typedef struct jpeg_color_quantizer jpeg_color_quantizer;
typedef struct jpeg_common_struct jpeg_common_struct;
typedef struct jpeg_comp_master jpeg_comp_master;
typedef struct jpeg_component_info jpeg_component_info;
typedef struct jpeg_compress_struct jpeg_compress_struct;
typedef struct jpeg_d_coef_controller jpeg_d_coef_controller;
typedef struct jpeg_d_main_controller jpeg_d_main_controller;
typedef struct jpeg_d_post_controller jpeg_d_post_controller;
typedef struct jpeg_decomp_master jpeg_decomp_master;
typedef struct jpeg_decompress_struct jpeg_decompress_struct;
typedef struct jpeg_destination_mgr jpeg_destination_mgr;
typedef struct jpeg_downsampler jpeg_downsampler;
typedef struct jpeg_entropy_decoder jpeg_entropy_decoder;
typedef struct jpeg_entropy_encoder jpeg_entropy_encoder;
typedef struct jpeg_error_mgr jpeg_error_mgr;
typedef struct jpeg_forward_dct jpeg_forward_dct;
typedef struct jpeg_input_controller jpeg_input_controller;
typedef struct jpeg_inverse_dct jpeg_inverse_dct;
typedef struct jpeg_marker_reader jpeg_marker_reader;
typedef struct jpeg_marker_struct jpeg_marker_struct;
typedef struct jpeg_marker_writer jpeg_marker_writer;
typedef struct jpeg_memory_mgr jpeg_memory_mgr;
typedef struct jpeg_progress_mgr jpeg_progress_mgr;
typedef struct jpeg_scan_info jpeg_scan_info;
typedef struct jpeg_source_mgr jpeg_source_mgr;
typedef struct jpeg_upsampler jpeg_upsampler;
typedef struct jvirt_barray_control jvirt_barray_control;
typedef struct jvirt_sarray_control jvirt_sarray_control;
typedef struct kbutton_t kbutton_t;
typedef struct keyname_t keyname_t;
typedef struct keywordHash_s keywordHash_s;
typedef struct lagometer_t lagometer_t;
typedef struct languageInfo_t languageInfo_t;
typedef union large_pool_struct large_pool_struct;
typedef struct leafList_s leafList_s;
typedef struct lerpFrame_t lerpFrame_t;
typedef struct less_UINT32 less_UINT32;
typedef struct level_locals_t level_locals_t;
typedef struct lightGlob_type lightGlob_type;
typedef struct listBoxDef_s listBoxDef_s;
typedef struct listColumnInfo_t listColumnInfo_t;
typedef struct loadAnim_t loadAnim_t;
typedef struct loadAssets_t loadAssets_t;
typedef struct locTraceWork_t locTraceWork_t;
typedef struct localEntity_s localEntity_s;
typedef struct loopback_t loopback_t;
typedef struct loopmsg_t loopmsg_t;
typedef struct ltp_params ltp_params;
typedef struct lump_t lump_t;
typedef struct mapInfo mapInfo;
typedef struct materialCommands_t materialCommands_t;
typedef struct menuDef_t menuDef_t;
typedef union mnode_info_t mnode_info_t;
typedef struct mnode_leaf_t mnode_leaf_t;
typedef struct mnode_node_t mnode_node_t;
typedef struct mnode_t mnode_t;
typedef struct modInfo_t modInfo_t;
typedef struct moveclip_t moveclip_t;
typedef struct mover_ent_t mover_ent_t;
typedef struct msg_t msg_t;
typedef struct multiDef_s multiDef_s;
typedef struct my_coef_controller my_coef_controller;
typedef struct my_color_converter my_color_converter;
typedef struct my_color_deconverter my_color_deconverter;
typedef struct my_comp_master my_comp_master;
typedef struct my_cquantizer my_cquantizer;
typedef struct my_decomp_master my_decomp_master;
typedef struct my_destination_mgr my_destination_mgr;
typedef struct my_downsampler my_downsampler;
typedef struct my_fdct_controller my_fdct_controller;
typedef struct my_idct_controller my_idct_controller;
typedef struct my_input_controller my_input_controller;
typedef struct my_main_controller my_main_controller;
typedef struct my_marker_reader my_marker_reader;
typedef struct my_marker_writer my_marker_writer;
typedef struct my_memory_mgr my_memory_mgr;
typedef struct my_post_controller my_post_controller;
typedef struct my_prep_controller my_prep_controller;
typedef struct my_source_mgr my_source_mgr;
typedef struct my_upsampler my_upsampler;
typedef struct netProfileInfo_s netProfileInfo_t;
typedef struct netProfilePacket_t netProfilePacket_t;
typedef struct netProfileStream_t netProfileStream_t;
typedef struct netadr_t netadr_t;
typedef struct netchan_t netchan_t;
typedef struct new_allocator_CCacheInfoBlock new_allocator_CCacheInfoBlock;
typedef struct new_allocator_CDirect3DSurfaceptr new_allocator_CDirect3DSurfaceptr;
typedef struct new_allocator_CDirect3DVolumeptr new_allocator_CDirect3DVolumeptr;
typedef struct new_allocator_CDisplayInfo new_allocator_CDisplayInfo;
typedef struct new_allocator_CFenceptr new_allocator_CFenceptr;
typedef struct new_allocator_CResInfo new_allocator_CResInfo;
typedef struct new_allocator_CSoundObjectptr new_allocator_CSoundObjectptr;
typedef struct new_allocator_CStreamSoundptr new_allocator_CStreamSoundptr;
typedef struct new_allocator_FFItem new_allocator_FFItem;
typedef struct new_allocator_FSRef new_allocator_FSRef;
typedef struct new_allocator_GLuint new_allocator_GLuint;
typedef struct new_allocator_GLuintptr new_allocator_GLuintptr;
typedef struct new_allocator_MacRect new_allocator_MacRect;
typedef struct new_allocator_Tuple new_allocator_Tuple;
typedef struct new_allocator_UINT16 new_allocator_UINT16;
typedef struct new_allocator_UINT16ptr new_allocator_UINT16ptr;
typedef struct new_allocator_UINT32 new_allocator_UINT32;
typedef struct new_allocator_UINT8 new_allocator_UINT8;
typedef struct new_allocator_WinCursor new_allocator_WinCursor;
typedef struct new_allocator_char new_allocator_char;
typedef struct new_allocator_float new_allocator_float;
typedef struct nodetype nodetype;
typedef struct objective_t objective_t;
typedef struct operator_s operator_s;
typedef struct orientation_t orientation_t;
typedef struct outPacket_t outPacket_t;
typedef struct pack_t pack_t;
typedef struct pair_UINT32_CVAOPacket pair_UINT32_CVAOPacket;
typedef struct pair_constUINT32_CVAOPacket pair_constUINT32_CVAOPacket;
typedef struct parseInfo_t parseInfo_t;
typedef struct pc_token_s pc_token_s;
typedef struct pendingServerStatus_t pendingServerStatus_t;
typedef struct pendingServer_t pendingServer_t;
typedef struct phuff_entropy_decoder phuff_entropy_decoder;
typedef struct phuff_entropy_encoder phuff_entropy_encoder;
typedef struct ping_t ping_t;
typedef struct pinglist_t pinglist_t;
typedef struct playerEntity_t playerEntity_t;
typedef struct playerState_s playerState_s;
typedef struct playerTeamState_t playerTeamState_t;
typedef struct pml_t pml_t;
typedef struct pmoveHandler_t pmoveHandler_t;
typedef struct pmove_t pmove_t;
typedef struct pointtrace_t pointtrace_t;
typedef struct punctuation_s punctuation_s;
typedef struct pushed_t pushed_t;
typedef union qfile_gus qfile_gus;
typedef struct qfile_us qfile_us;
typedef struct qkey_t qkey_t;
typedef struct qtime_s qtime_s;
typedef struct r_backEndGlobals_t r_backEndGlobals_t;
typedef struct r_global_permanent_t r_global_permanent_t;
typedef struct r_globals_load_t r_globals_load_t;
typedef struct r_globals_t r_globals_t;
typedef struct r_ib_state_t r_ib_state_t;
typedef struct r_lightmapGroup_t r_lightmapGroup_t;
typedef struct r_lightmapMerge_t r_lightmapMerge_t;
typedef struct r_vb_state_t r_vb_state_t;
typedef struct random_access_iterator_tag random_access_iterator_tag;
typedef struct rectDef_s rectDef_s;
typedef struct refdef_s refdef_s;
typedef struct refexport_t refexport_t;
typedef struct refimport_t refimport_t;
typedef struct rlimit rlimit;
typedef struct saLoadObjGlob_type saLoadObjGlob_type;
typedef struct savable_state savable_state;
typedef struct sbpicinfo_t sbpicinfo_t;
typedef struct sched_param sched_param;
typedef struct score_t score_t;
typedef struct scrAnimGlob_t scrAnimGlob_t;
typedef struct scrAnimPub_t scrAnimPub_t;
typedef struct scrCompileDebugPub_t scrCompileDebugPub_t;
typedef struct scrCompileGlob_t scrCompileGlob_t;
typedef struct scrCompilePub_t scrCompilePub_t;
typedef struct scrMemTreeGlob_t scrMemTreeGlob_t;
typedef struct scrMemTreePub_t scrMemTreePub_t;
typedef struct scrParserGlob_t scrParserGlob_t;
typedef struct scrParserPub_t scrParserPub_t;
typedef struct scrStringGlob_t scrStringGlob_t;
typedef struct scrVarDebugPub_t scrVarDebugPub_t;
typedef struct scrVarGlob_t scrVarGlob_t;
typedef struct scrVarPub_t scrVarPub_t;
typedef struct scrVmDebugPub_t scrVmDebugPub_t;
typedef struct scrVmGlob_t scrVmGlob_t;
typedef struct scrVmPub_t scrVmPub_t;
typedef struct scr_anim_s scr_anim_s;
typedef struct scr_animtree_t scr_animtree_t;
typedef struct scr_block_s scr_block_s;
typedef struct scr_classStruct_t scr_classStruct_t;
typedef struct scr_const_t scr_const_t;
typedef struct scr_data_t scr_data_t;
typedef struct scr_entref_t scr_entref_t;
typedef struct scr_localVar_t scr_localVar_t;
typedef struct script_s script_s;
typedef struct scrollInfo_s scrollInfo_s;
typedef struct searchpath_s searchpath_s;
typedef struct serverAddress_t serverAddress_t;
typedef struct serverFilter_s serverFilter_s;
typedef struct serverInfo_t serverInfo_t;
typedef struct serverStatic_t serverStatic_t;
typedef struct serverStatusDvar_t serverStatusDvar_t;
typedef struct serverStatusInfo_t serverStatusInfo_t;
typedef struct serverStatus_s serverStatus_s;
typedef struct server_t server_t;
typedef struct sharedUiInfo_t sharedUiInfo_t;
typedef struct shellshock_parms_t shellshock_parms_t;
typedef struct shellshock_t shellshock_t;
typedef struct sightclip_t sightclip_t;
typedef struct sightpointtrace_t sightpointtrace_t;
typedef union small_pool_struct small_pool_struct;
typedef struct snapshotEntityNumbers_t snapshotEntityNumbers_t;
typedef struct snapshot_t snapshot_t;
typedef struct snd_alias_build_s snd_alias_build_s;
typedef struct snd_alias_list_s snd_alias_list_t;
typedef struct snd_alias_t snd_alias_t;
typedef struct snd_background_info_t snd_background_info_t;
typedef struct snd_channel_info_t snd_channel_info_t;
typedef struct snd_channelvolgroup snd_channelvolgroup;
typedef struct snd_enveffect snd_enveffect;
typedef struct snd_listener snd_listener;
typedef struct snd_local_t snd_local_t;
typedef struct snd_overlay_info_t snd_overlay_info_t;
typedef struct snd_save_2D_sample_t snd_save_2D_sample_t;
typedef struct snd_save_3D_sample_t snd_save_3D_sample_t;
typedef struct snd_save_stream_t snd_save_stream_t;
typedef struct snd_volume_info_t snd_volume_info_t;
typedef struct sockaddr sockaddr;
typedef struct sockaddr_in sockaddr_in;
typedef struct source_s source_s;
typedef struct spawn_t spawn_t;
typedef struct split_cb_params split_cb_params;
typedef struct srfPoly_t srfPoly_t;
typedef struct srfTriangles_t srfTriangles_t;

typedef struct static_model_cache_t static_model_cache_t;
typedef union static_model_leaf_t static_model_leaf_t;
typedef struct static_model_node_list_t static_model_node_list_t;
typedef struct static_model_node_t static_model_node_t;
typedef struct static_model_tree_list_t static_model_tree_list_t;
typedef struct static_model_tree_t static_model_tree_t;
typedef struct staticmodeltrace_t staticmodeltrace_t;
typedef struct statmonitor_s statmonitor_s;
typedef struct stream_dest_info_t stream_dest_info_t;
typedef struct stream_source_info_t stream_source_info_t;
typedef struct stringDef_s stringDef_s;
typedef struct stype_t stype_t;
typedef struct sunflare_t sunflare_t;
typedef struct svEntity_s svEntity_s;
typedef union sval_u sval_u;
typedef struct svscmd_info_t svscmd_info_t;
typedef struct sysEvent_t sysEvent_t;
typedef struct tagBITMAPFILEHEADER tagBITMAPFILEHEADER;
typedef struct tagBITMAPINFO tagBITMAPINFO;
typedef struct tagBITMAPINFOHEADER tagBITMAPINFOHEADER;
typedef struct tagInfo_s tagInfo_s;
typedef struct tagPALETTEENTRY tagPALETTEENTRY;
typedef struct tagPOINT tagPOINT;
typedef struct tagRECT tagRECT;
typedef struct tagRGBQUAD tagRGBQUAD;
typedef struct tagTEXTMETRICA tagTEXTMETRICA;
typedef struct tagTEXTMETRICW tagTEXTMETRICW;
typedef struct tempBanSlot_t tempBanSlot_t;
typedef struct timespec timespec;
typedef struct timeval timeval;
typedef struct tm tm;
typedef struct tm_unz_s tm_unz_s;
typedef struct token_s token_s;
typedef struct trDebugLine_t trDebugLine_t;
typedef struct trDebugString_t trDebugString_t;
typedef struct trStatistics_t trStatistics_t;
typedef struct trXSkin_t trXSkin_t;
typedef struct traceWork_t traceWork_t;
typedef struct trace_t trace_t;
typedef struct trajectory_t trajectory_t;
typedef struct trigger_ent_t trigger_ent_t;
typedef struct trigger_info_t trigger_info_t;
typedef struct turretInfo_s turretInfo_s;
typedef struct ucmd_t ucmd_t;
typedef struct uiClientState_t uiClientState_t;
typedef struct uiInfo_t uiInfo_t;
typedef struct unz_file_info_internal_s unz_file_info_internal_s;
typedef struct unz_file_info_s unz_file_info_s;
typedef struct unz_global_info_s unz_global_info_s;
typedef struct unz_s unz_s;
typedef struct useList_t useList_t;
typedef struct usercmd_s usercmd_s;
typedef struct va_info_t va_info_t;
typedef struct value_s value_s;
typedef struct vidConfig_t vidConfig_t;
typedef struct viewDamage_t viewDamage_t;
typedef struct viewLerpWaypoint_s viewLerpWaypoint_s;
typedef struct viewState_t viewState_t;
typedef struct water_t water_t;
typedef struct weaponInfo_s weaponInfo_s;
typedef struct weaponParms weaponParms;
typedef struct weaponState_t weaponState_t;
typedef struct windowDef_t windowDef_t;
typedef struct working_state working_state;
typedef struct worldContents_s worldContents_s;
typedef struct worldSector_s worldSector_s;
typedef struct worldTree_s worldTree_s;
typedef struct yy_buffer_state yy_buffer_state;
typedef struct z_stream_s z_stream_s;

typedef enum CFComparisonResult CFComparisonResult;
typedef enum CGLError CGLError;
typedef enum CGLPixelFormatAttribute CGLPixelFormatAttribute;
typedef enum ConverterType ConverterType;
typedef enum CubemapShot CubemapShot;
typedef enum D3DBACKBUFFER_TYPE D3DBACKBUFFER_TYPE;
typedef enum D3DBASISTYPE D3DBASISTYPE;
typedef enum D3DDEGREETYPE D3DDEGREETYPE;
typedef enum D3DDEVTYPE D3DDEVTYPE;
typedef enum D3DFOGMODE D3DFOGMODE;
typedef enum D3DLIGHTTYPE D3DLIGHTTYPE;
typedef enum D3DMULTISAMPLE_TYPE D3DMULTISAMPLE_TYPE;
typedef enum D3DPOOL D3DPOOL;
typedef enum D3DPRIMITIVETYPE D3DPRIMITIVETYPE;
typedef enum D3DQUERYTYPE D3DQUERYTYPE;
typedef enum D3DRENDERSTATETYPE D3DRENDERSTATETYPE;
typedef enum D3DSAMPLERSTATETYPE D3DSAMPLERSTATETYPE;
typedef enum D3DSTATEBLOCKTYPE D3DSTATEBLOCKTYPE;
typedef enum D3DSWAPEFFECT D3DSWAPEFFECT;
typedef enum D3DTEXTURESTAGESTATETYPE D3DTEXTURESTAGESTATETYPE;
typedef enum D3DTEXTURETRANSFORMFLAGS D3DTEXTURETRANSFORMFLAGS;
typedef enum D3DTRANSFORMSTATETYPE D3DTRANSFORMSTATETYPE;
typedef enum D3DXIMAGE_FILEFORMAT D3DXIMAGE_FILEFORMAT;
typedef enum D3DXINCLUDE_TYPE D3DXINCLUDE_TYPE;
typedef enum D3DXPARAMETER_CLASS D3DXPARAMETER_CLASS;
typedef enum D3DXPARAMETER_TYPE D3DXPARAMETER_TYPE;
typedef enum D3DXREGISTER_SET D3DXREGISTER_SET;
typedef enum DemoType DemoType;
typedef enum DpvsClipChildren DpvsClipChildren;
typedef enum DpvsForceBevels DpvsForceBevels;
typedef enum DvarSetSource DvarSetSource;
typedef enum DxCapsResponse DxCapsResponse;
typedef enum EMediaTypes EMediaTypes;
typedef enum FFType FFType;
typedef enum FogColorSrcEnum FogColorSrcEnum;
typedef enum Format Format;
typedef enum FsListBehavior FsListBehavior;
typedef enum FxChannelId FxChannelId;
typedef enum GfxDrawGroupGlueBehavior GfxDrawGroupGlueBehavior;
typedef enum GfxDrawGroupType GfxDrawGroupType;
typedef enum GfxDrawSurfOrder GfxDrawSurfOrder;
typedef enum GfxFogOffset GfxFogOffset;
typedef enum GfxLightType GfxLightType;
typedef enum GfxLockType GfxLockType;
typedef enum GfxOptimizedVertexSource GfxOptimizedVertexSource;
typedef enum GfxPrimStatsTarget GfxPrimStatsTarget;
typedef enum GfxRenderTargetId GfxRenderTargetId;
typedef enum GfxScreenshotType GfxScreenshotType;
typedef enum GfxViewportBehavior GfxViewportBehavior;
typedef enum JPEG_MARKER JPEG_MARKER;
typedef enum J_BUF_MODE J_BUF_MODE;
typedef enum J_COLOR_SPACE J_COLOR_SPACE;
typedef enum J_DCT_METHOD J_DCT_METHOD;
typedef enum J_DITHER_MODE J_DITHER_MODE;
typedef enum LoopMode LoopMode;
typedef enum MaterialShaderType MaterialShaderType;
typedef enum MaterialTechniqueType MaterialTechniqueType;
typedef enum MaterialTextureSource MaterialTextureSource;
typedef enum MaterialVertexDeclType MaterialVertexDeclType;
typedef enum MtlParseSuccess MtlParseSuccess;
typedef enum MtlTextureFunctionValidDx7 MtlTextureFunctionValidDx7;
typedef enum OffhandClass OffhandClass;
typedef enum OpenGLPixelShaderType OpenGLPixelShaderType;
typedef enum OwnershipType OwnershipType;
typedef enum PlayRateMode PlayRateMode;
typedef enum PrimType PrimType;
typedef enum RenderTargetUsage RenderTargetUsage;
typedef enum ShadowType ShadowType;
typedef enum StartFrameMode StartFrameMode;
typedef enum SurfaceType SurfaceType;
typedef enum VAOStatus VAOStatus;
typedef enum _D3DCUBEMAP_FACES _D3DCUBEMAP_FACES;
typedef enum _D3DFORMAT _D3DFORMAT;
typedef enum _D3DRESOURCETYPE _D3DRESOURCETYPE;
typedef enum _D3DTEXTUREADDRESS _D3DTEXTUREADDRESS;
typedef enum _D3DTEXTUREFILTERTYPE _D3DTEXTUREFILTERTYPE;
typedef enum _D3DTEXTUREOP _D3DTEXTUREOP;
typedef enum _Rb_tree_color _Rb_tree_color;
typedef enum aistateEnum_t aistateEnum_t;
typedef enum animBodyPart_t animBodyPart_t;
typedef enum animScriptConditionTypes_t animScriptConditionTypes_t;
typedef enum animScriptParseMode_t animScriptParseMode_t;
typedef enum c_pass_type c_pass_type;
typedef enum clientConnected_t clientConnected_t;
typedef enum connstate_t connstate_t;
typedef enum e_status e_status;
typedef enum errorParm_t errorParm_t;
typedef enum fieldtype_t fieldtype_t;
typedef enum fsMode_t fsMode_t;
typedef enum he_type_t he_type_t;
typedef enum hitLocation_t hitLocation_t;
typedef enum hudelem_update_t hudelem_update_t;
typedef enum inflate_block_mode inflate_block_mode;
typedef enum inflate_codes_mode inflate_codes_mode;
typedef enum itemType_t itemType_t;
typedef enum language_t language_t;
typedef enum leType_t leType_t;
typedef enum listColumnTypes_t listColumnTypes_t;
typedef enum msgLocErrType_t msgLocErrType_t;
typedef enum msgwnd_mode_t msgwnd_mode_t;
typedef enum netadrtype_t netadrtype_t;
typedef enum netsrc_t netsrc_t;
typedef enum objectiveState_t objectiveState_t;
typedef enum print_msg_type_t print_msg_type_t;
typedef enum proneCheckType_t proneCheckType_t;
typedef enum refEntityType_t refEntityType_t;
typedef enum scriptAnimEventTypes_t scriptAnimEventTypes_t;
typedef enum scriptAnimMoveTypes_t scriptAnimMoveTypes_t;
typedef enum serverState_t serverState_t;
typedef enum sessionState_t sessionState_t;
typedef enum snd_alias_members_t snd_alias_members_t;
typedef enum snd_alias_system_t snd_alias_system_t;
typedef enum snd_alias_type_t snd_alias_type_t;
typedef enum snd_overlay_type_t snd_overlay_type_t;
typedef enum snd_stopsounds_arg_t snd_stopsounds_arg_t;
typedef enum sscType_t sscType_t;
typedef enum surfaceType_t surfaceType_t;
typedef enum svscmd_type svscmd_type;
typedef enum sysEventType_t sysEventType_t;
typedef enum team_t team_t;
typedef enum trType_t trType_t;
typedef enum uiMenuCommand_t uiMenuCommand_t;
typedef enum weapClass_t weapClass_t;
typedef enum weapOverlayReticle_t weapOverlayReticle_t;
typedef enum weapProjExposion_t weapProjExposion_t;
typedef enum weapSlot_t weapSlot_t;
typedef enum weapStance_t weapStance_t;
typedef enum weapType_t weapType_t;

typedef struct _ADPCMDATATAG ADPCMDATA;
typedef struct _AILMIXINFO AILMIXINFO;
typedef struct _AILSOUNDINFO AILSOUNDINFO;
typedef struct tagBITMAPFILEHEADER BITMAPFILEHEADER;
typedef struct tagBITMAPINFO BITMAPINFO;
typedef struct tagBITMAPINFOHEADER BITMAPINFOHEADER;
typedef struct _D3DADAPTER_IDENTIFIER9 D3DADAPTER_IDENTIFIER9;
typedef struct _D3DBOX D3DBOX;
typedef struct _D3DCAPS9 D3DCAPS9;
typedef struct _D3DCLIPSTATUS9 D3DCLIPSTATUS9;
typedef struct _D3DCOLORVALUE D3DCOLORVALUE;
typedef struct _D3DDEVICE_CREATION_PARAMETERS D3DDEVICE_CREATION_PARAMETERS;
typedef struct _D3DGAMMARAMP D3DGAMMARAMP;
typedef struct _D3DINDEXBUFFER_DESC D3DINDEXBUFFER_DESC;
typedef struct _D3DLIGHT9 D3DLIGHT9;
typedef struct _D3DLOCKED_BOX D3DLOCKED_BOX;
typedef struct _D3DLOCKED_RECT D3DLOCKED_RECT;
typedef struct _D3DMATERIAL9 D3DMATERIAL9;
typedef struct _D3DMATRIX D3DMATRIX;
typedef struct _D3DPRESENT_PARAMETERS_ D3DPRESENT_PARAMETERS;
typedef struct _D3DPSHADERCAPS2_0 D3DPSHADERCAPS2_0;
typedef struct _D3DRASTER_STATUS D3DRASTER_STATUS;
typedef struct _D3DRECT D3DRECT;
typedef struct _D3DRECTPATCH_INFO D3DRECTPATCH_INFO;
typedef struct _D3DSURFACE_DESC D3DSURFACE_DESC;
typedef struct _D3DTRIPATCH_INFO D3DTRIPATCH_INFO;
typedef struct _D3DVECTOR D3DVECTOR;
typedef struct _D3DVERTEXBUFFER_DESC D3DVERTEXBUFFER_DESC;
typedef struct _D3DVERTEXELEMENT9 D3DVERTEXELEMENT9;
typedef struct _D3DVIEWPORT9 D3DVIEWPORT9;
typedef struct _D3DVOLUME_DESC D3DVOLUME_DESC;
typedef struct _D3DVSHADERCAPS2_0 D3DVSHADERCAPS2_0;
typedef struct _D3DXCONSTANTTABLE_DESC D3DXCONSTANTTABLE_DESC;
typedef struct _D3DXCONSTANT_DESC D3DXCONSTANT_DESC;
typedef struct _D3DXFONT_DESCA D3DXFONT_DESCA;
typedef struct _D3DXFONT_DESCW D3DXFONT_DESCW;
typedef struct _D3DXIMAGE_INFO D3DXIMAGE_INFO;
typedef struct _D3DXMACRO D3DXMACRO;
typedef struct _D3DXSHADER_CONSTANTTABLE D3DXSHADER_CONSTANTTABLE;
typedef struct _D3DXSHADER_TYPEINFO D3DXSHADER_TYPEINFO;
typedef struct _DDCOLORKEY DDCOLORKEY;
typedef struct _DDPIXELFORMAT DDPIXELFORMAT;
typedef struct _DDSCAPS2 DDSCAPS2;
typedef struct _DDSURFACEDESC2 DDSURFACEDESC2;
typedef struct DObj_s DObj;
typedef struct DObjAnimMat DObjAnimMat;
typedef struct DObjModel_s DObjModel;
typedef struct DObjSkelMat_s DObjSkelMat;
typedef struct DObjTrace_s DObjTrace;
typedef struct DSurface_s DSurface;
typedef struct _FILETIME FILETIME;
typedef struct Font_s Font;
typedef struct _GUID GUID;
typedef struct MaterialConstantDefObj MaterialConstantDef;
typedef struct MaterialInfoObj_s MaterialInfoObj;
typedef struct tagPALETTEENTRY PALETTEENTRY;
typedef struct tagPOINT POINT;
typedef struct tagRECT RECT;
typedef struct tagRGBQUAD RGBQUAD;
typedef struct _RGNDATA RGNDATA;
typedef struct _RGNDATAHEADER RGNDATAHEADER;
typedef struct _SYSTEMTIME SYSTEMTIME;
typedef struct tagTEXTMETRICA TEXTMETRICA;
typedef struct tagTEXTMETRICW TEXTMETRICW;
typedef union _ULARGE_INTEGER ULARGE_INTEGER;
typedef struct VariableValue_s VariableValue;
typedef struct WSAData WSADATA;
typedef struct XAnim_s XAnim;
typedef struct XAnimNotify_s XAnimNotify;
typedef struct XBlendInfo_s XBlendInfo;
typedef struct XBoneInfo_s XBoneInfo;
typedef struct XModelCollSurf_s XModelCollSurf;
typedef struct XModelCollTri_s XModelCollTri;
typedef struct XModelParts_s XModelParts;
typedef struct XModelSurfs_s XModelSurfs;
typedef struct XSurface_s XSurface;
typedef struct XVertexInfo_s XVertexInfo;
typedef struct CResInfo _ValueType;
typedef struct _opaque_pthread_mutexattr_t __darwin_pthread_mutexattr_t;
typedef struct animation_s animation_t;
typedef struct archivedEntity_s archivedEntity_t;
typedef struct archivedSnapshot_s archivedSnapshot_t;
typedef struct backing_store_struct backing_store_info;
typedef struct cLeafBrushNode_s cLeafBrushNode_t;
typedef struct cStaticModel_s cStaticModel_t;
typedef struct cachedClient_s cachedClient_t;
typedef struct centity_s centity_t;
typedef struct clientState_t clientState_t;
typedef struct client_fields_s client_fields_t;
typedef struct client_s client_t;
typedef struct cmd_function_s cmd_function_t;
typedef struct columnInfo_s columnInfo_t;
typedef struct complex_s complex_t;
typedef struct cplane_s cplane_t;
typedef struct define_s define_t;
typedef struct dheader_s dheader_t;
typedef struct displayContextDef_s displayContextDef_t;
typedef struct dvar_s dvar_t;
typedef struct editFieldDef_s editFieldDef_t;
typedef struct entityState_s entityState_t;
typedef struct fileData_s fileData_t;
typedef struct fileInPack_s fileInPack_t;
typedef struct game_hudelem_s game_hudelem_t;
typedef struct gclient_s gclient_t;
typedef struct gentity_s gentity_t;
typedef struct gitem_s gitem_t;
typedef struct hudelem_s hudelem_t;
typedef struct indent_s indent_t;
typedef struct inflate_blocks_state inflate_blocks_statef;
typedef struct inflate_codes_state inflate_codes_statef;
typedef struct inflate_huft_s inflate_huft;
typedef struct ipFilter_s ipFilter_t;
typedef struct itemDef_s itemDef_t;
typedef struct keywordHash_s keywordHash_t;
typedef struct leafList_s leafList_t;
typedef struct listBoxDef_s listBoxDef_t;
typedef struct localEntity_s localEntity_t;
typedef struct multiDef_s multiDef_t;
typedef struct nodetype node_t;
typedef struct operator_s operator_t;
typedef struct pc_token_s pc_token_t;
typedef struct playerState_s playerState_t;
typedef struct punctuation_s punctuation_t;
typedef union qfile_gus qfile_gut;
typedef struct qfile_us qfile_ut;
typedef struct qtime_s qtime_t;
typedef struct rectDef_s rectDef_t;
typedef struct refdef_s refdef_t;
typedef struct scr_anim_s scr_anim_t;
typedef struct scr_block_s scr_block_t;
typedef struct scrollInfo_s scrollInfo_t;
typedef struct searchpath_s searchpath_t;
typedef struct serverFilter_s serverFilter_t;
typedef struct serverStatus_s serverStatus_t;
typedef struct statmonitor_s statmonitor_t;
typedef struct stringDef_s stringDef_t;
typedef struct svEntity_s svEntity_t;
typedef union sval_u sval_t;
typedef struct tagInfo_s tagInfo_t;
typedef struct tm_unz_s tm_unz;
typedef struct token_s token_t;
typedef struct turretInfo_s turretInfo_t;
typedef struct unz_file_info_s unz_file_info;
typedef struct unz_file_info_internal_s unz_file_info_internal;
typedef struct unz_global_info_s unz_global_info;
typedef struct usercmd_s usercmd_t;
typedef struct value_s value_t;
typedef struct viewLerpWaypoint_s viewLerpWaypoint_t;
typedef struct weaponInfo_s weaponInfo_t;
typedef struct worldContents_s worldContents_t;
typedef struct worldSector_s worldSector_t;
typedef struct worldTree_s worldTree_t;
typedef struct z_stream_s z_stream;

typedef OpaqueAEDataStorageType * AEDataStorageType;
typedef __AGLContextRec * AGLContext;
typedef __AGLPixelFormatRec * AGLPixelFormat;
typedef void (*AIL_file_close_callback)();
typedef long unsigned int (*AIL_file_open_callback)();
typedef long unsigned int (*AIL_file_read_callback)();
typedef long int (*AIL_file_seek_callback)();
typedef OpaqueAUGraph * AUGraph;
typedef void * (*Alloc_t)(int);   /* allocator(size); was unprototyped -> C++ call sites failed */
typedef OpaqueAudioConverter * AudioConverterRef;
typedef void (*BG_RegisterWeapon)();
typedef unsigned char BYTE;
typedef short int Bits16[16];
typedef unsigned char Bool;
typedef unsigned char Boolean;
typedef unsigned int (*BuiltinFunction)();
typedef unsigned int (*BuiltinMethod)();
typedef unsigned char Bytef;
typedef void *CDisplayList;
typedef const __CFArray * CFArrayRef;
typedef __CFBundle * CFBundleRef;
typedef const __CFData * CFDataRef;
typedef const __CFDictionary * CFDictionaryRef;
typedef __CFDictionary * CFMutableDictionaryRef;
typedef __CFString * CFMutableStringRef;
typedef const __CFString * CFStringRef;
typedef const void * CFTypeRef;
typedef const __CFURL * CFURLRef;
typedef CGContext * CGContextRef;
typedef CGDataProvider * CGDataProviderRef;
typedef _CGDirectDisplayID * CGDirectDisplayID;
typedef uint32_t CGDisplayFadeReservationToken;
typedef int32_t CGError;
typedef CGImage * CGImageRef;
typedef _CGLContextObject * CGLContextObj;
typedef _CGLPixelFormatObject * CGLPixelFormatObj;
typedef _CGLRendererInfoObject * CGLRendererInfoObj;
typedef uint32_t CGOpenGLDisplayMask;
typedef uint32_t CGTableCount;
typedef char CHAR;
typedef void (*CInfoPBPtr)();
typedef CProcRec * CProcPtr;
typedef ColorTable * CTabPtr;
typedef long unsigned int CodecQ;
typedef long int (*Component)();
typedef ComponentInstanceRecord * ComponentInstance;
typedef const unsigned char * ConstStr255Param;
typedef const unsigned char * ConstStringPtr;
typedef OpaqueContextRef * ContextRef;
typedef inflate_blocks_statef (*ControlEditTextValidationProcPtr)();
typedef OpaqueControlRef * ControlRef;
typedef inflate_blocks_statef (*ControlUserPaneDrawProcPtr)();
typedef Cursor * CursPtr;
typedef _D3DCUBEMAP_FACES D3DCUBEMAP_FACES;
typedef _D3DFORMAT D3DFORMAT;
typedef _D3DRESOURCETYPE D3DRESOURCETYPE;
typedef _D3DTEXTUREFILTERTYPE D3DTEXTUREFILTERTYPE;
typedef _D3DXSHADER_CONSTANTINFO D3DXSHADER_CONSTANTINFO;
typedef int DCTELEM;
typedef long unsigned int DWORD;
typedef OpaqueDialogPtr * DialogPtr;
typedef OpaqueEventHandlerRef * EventHandlerRef;
typedef __EventLoopTimer * EventLoopTimerRef;
typedef byte EventParm;
typedef OpaqueEventRef * EventRef;
typedef OpaqueEventTargetRef * EventTargetRef;
typedef double EventTime;
typedef float FLOAT;
typedef float FLOAT_MULT_TYPE;
typedef OpaqueFSIterator * FSIterator;
typedef void (*FSSpecPtr)();
typedef float Float32;
typedef double Float64;
typedef void (*Free_t)(void *, int);   /* real Free(ptr,size) sig; byte-neutral fn-ptr */
typedef GDevice * GDPtr;
typedef long unsigned int GLbitfield;
typedef unsigned char GLboolean;
typedef double GLdouble;
typedef long unsigned int GLenum;
typedef float GLfloat;
typedef long int GLint;
typedef long int GLsizei;
typedef unsigned char GLubyte;
typedef long unsigned int GLuint;
typedef unsigned int GfxDrawSurfSort;
typedef unsigned char GfxImageCategory;
typedef GfxImage * GfxImagePtr;
typedef IDirect3DPixelShader9 GfxPixelShader;
typedef byte GfxSamplerState;
typedef IDirect3DVertexShader9 GfxVertexShader;
typedef OpaqueGrafPtr * GrafPtr;
typedef short unsigned int GridSamplePoint[3];
typedef void * H3DPOBJECT;
typedef jpeg_alloc * HANDLE;
typedef HCURSOR__ * HCURSOR;
typedef HDC__ * HDC;
typedef HINSTANCE__ * HINSTANCE;
typedef HMONITOR__ * HMONITOR;
typedef long unsigned int HPROENUM;
typedef long unsigned int HPROVIDER;
typedef _SAMPLE * HSAMPLE;
typedef _STREAM * HSTREAM;
typedef HWND__ * HWND;
typedef short unsigned int HashIndex;
typedef OpaqueIBNibRef * IBNibRef;
typedef int IFAST_MULT_TYPE;
typedef int INT;
typedef short int INT16;
typedef int INT32;
typedef unsigned char (*IOCompletionProcPtr)();
typedef int ISLOW_MULT_TYPE;
typedef ITab * ITabPtr;
typedef OpaqueIconRef * IconRef;
typedef ImageDescription * ImageDescriptionPtr;
typedef void (*JBLOCKROW)();
typedef short int JCOEF;
typedef unsigned int JDIMENSION;
typedef unsigned char JOCTET;
typedef boolean (*JSAMPARRAY)();
typedef unsigned char JSAMPLE;
typedef int LOCFSERROR;
typedef long int LONG;
typedef long long int LONGLONG;
typedef long int LONG_PTR;
typedef BOOL * LPBOOL;
typedef const jpeg_alloc * LPCVOID;
typedef ID3DXBuffer * LPD3DXBUFFER;
typedef ID3DXConstantTable * LPD3DXCONSTANTTABLE;
typedef ID3DXFont * LPD3DXFONT;
typedef ID3DXInclude * LPD3DXINCLUDE;
typedef _D3DXSHADER_CONSTANTINFO * LPD3DXSHADER_CONSTANTINFO;
typedef _D3DXSHADER_TYPEINFO * LPD3DXSHADER_TYPEINFO;
typedef ID3DXSprite * LPD3DXSPRITE;
typedef IDirect3DDevice9 * LPDIRECT3DDEVICE9;
typedef IDirect3DSurface9 * LPDIRECT3DSURFACE9;
typedef _FILETIME * LPFILETIME;
typedef long int * LPLONG;
typedef _MEMORYSTATUS * LPMEMORYSTATUS;
typedef _OSVERSIONINFOA * LPOSVERSIONINFOA;
typedef _OVERLAPPED * LPOVERLAPPED;
typedef tagPOINT * LPPOINT;
typedef tagRECT * LPRECT;
typedef _SECURITY_ATTRIBUTES * LPSECURITY_ATTRIBUTES;
typedef _SYSTEMTIME * LPSYSTEMTIME;
typedef jpeg_alloc * LPVOID;
typedef _WIN32_FIND_DATAA * LPWIN32_FIND_DATAA;
typedef WSADATA * LPWSADATA;
typedef long int M3DRESULT;
typedef UINT MMRESULT;
typedef void * (*MantleAnimAlloc)(int);
typedef unsigned char MaterialConstantSource;
typedef struct Material * MaterialHandle;
typedef unsigned char MaterialStreamDest;
typedef unsigned char MaterialStreamSource;
typedef MediaType * * Media;
typedef OpaqueMenuRef * MenuRef;
typedef MovieType * * Movie;
typedef int (*ODITHER_MATRIX_PTR)[16];
typedef long unsigned int OSAID;
typedef _MEMORY_BASIC_INFORMATION * PMEMORY_BASIC_INFORMATION;
typedef RTL_CRITICAL_SECTION * PRTL_CRITICAL_SECTION;
typedef short unsigned int * PUINT16;
typedef unsigned int * PUINT32;
typedef jpeg_alloc * PVOID;
typedef PixMap * PixMapPtr;
typedef char * Ptr;
typedef QElem * QElemPtr;
typedef OpaqueRgnHandle * RgnHandle;
typedef short int SHORT;
typedef short int SInt16;
typedef long int SInt32;
typedef long long int SInt64;
typedef signed char SInt8;
typedef SProcRec * SProcPtr;
typedef void (*ScriptCallbackClient)();
typedef my_upsampler (*ScriptCallbackEnt)();
typedef void (*ScriptCallbackHudElem)();
typedef short unsigned int ScriptString;
typedef long int Size;
typedef byte SoundAlias;
typedef SoundDescription * SoundDescriptionPtr;
typedef unsigned char Str15[16];
typedef unsigned char Str255[256];
typedef unsigned char Str31[32];
typedef unsigned char Str63[64];
typedef unsigned char * StringPtr;
typedef size_t (*TCursorList)();
typedef OpaqueTXNObject * TXNObject;
typedef byte TextureSemantic;
typedef TrackType * * Track;
typedef short unsigned int UINT16;
typedef unsigned int UINT32;
typedef long unsigned int UINT4;
typedef unsigned char UINT8;
typedef unsigned int UINT_PTR;
typedef short unsigned int UInt16;
typedef long unsigned int UInt32;
typedef long long unsigned int UInt64;
typedef unsigned char UInt8;
typedef long unsigned int ULONG;
typedef long long unsigned int ULONGLONG;
typedef long unsigned int ULONG_PTR;
typedef unsigned long VAOSet;
typedef wchar_t WCHAR;
typedef short unsigned int WORD;
typedef OpaqueWindowPtr * WindowPtr;
typedef XAnimParts_s XAnimParts;
typedef void (*XAssetEnum)(void *, void *);   /* real func(data,inData) sig; byte-neutral fn-ptr */
typedef int XPartBits[4];
typedef short int XQuat[4];
typedef short int XQuat2[2];
typedef yy_buffer_state * YY_BUFFER_STATE;
typedef int _Atomic_word;
typedef random_access_iterator_tag _Category;
typedef ptrdiff_t _DistanceType;
typedef _ReferenceType2 _ReferenceType1;
typedef CResInfo _ValueType1;
typedef CResInfo _ValueType2;
#if !defined(__GNUC__) && !defined(__clang__)
typedef char * __builtin_va_list;
#endif
typedef int __darwin_ct_rune_t;
typedef unsigned int __darwin_natural_t;
typedef _opaque_pthread_t * __darwin_pthread_t;
typedef long unsigned int __darwin_size_t;
typedef long int __darwin_time_t;
typedef int __int32_t;
#if defined(__linux__) && defined(__x86_64__)

#else
typedef long long int __int64_t;
#endif
typedef float __m128[4];
typedef short unsigned int __uint16_t;
typedef unsigned int __uint32_t;
typedef unsigned char __uint8_t;
typedef int (*__vtbl_ptr_type)();
typedef backing_store_struct * backing_store_ptr;
typedef box * boxptr;
typedef char cl_key_t[8192];
typedef int clipHandle_t;
typedef byte cmd_text_buf_t[1][65536];
typedef int (*decode_func)();
typedef int (*decoder_ctl_func)();
typedef void (*decoder_destroy_func)();
typedef void * (*decoder_init_func)();
typedef void (*downsample1_ptr)();
typedef int (*encode_func)();
typedef int (*encoder_ctl_func)();
typedef void (*encoder_destroy_func)();
typedef void * (*encoder_init_func)();
typedef int fileHandle_t;
typedef float float16;
typedef void (*float_DCT_method_ptr)();
typedef int (*fn_blocked)();
typedef int (*fn_controller)();
typedef int (*fn_die)();
typedef int (*fn_pain)();
typedef int (*fn_reached)();
typedef int (*fn_think)();
typedef void (*fn_touch)(struct gentity_s *, struct gentity_s *, int);   /* real touch-handler sig (was unprototyped); unblocks C++ call sites */
typedef unsigned int (*fn_use)();
typedef void (*forward_DCT_method_ptr)();
typedef double (*free_func)();
typedef huff_entropy_decoder * huff_entropy_ptr;
typedef unsigned int (*innovation_quant_func)();
typedef unsigned int (*innovation_unquant_func)();
typedef void (*inverse_DCT_method_ptr)();
typedef char io_name_t[128];
typedef jpeg_common_struct * j_common_ptr;
typedef jpeg_compress_struct * j_compress_ptr;
typedef jpeg_decompress_struct * j_decompress_ptr;
#ifdef _WIN32
#include <setjmp.h>
#else
typedef int jmp_buf[39];
#endif
typedef boolean (*jpeg_marker_parser_method)();
typedef jpeg_marker_struct * jpeg_saved_marker_ptr;
typedef struct jvirt_barray_control * jvirt_barray_ptr;
typedef struct jvirt_sarray_control * jvirt_sarray_ptr;
typedef int kern_return_t;
typedef large_pool_struct * large_pool_ptr;
typedef unsigned int (*lsp_quant_func)();
typedef unsigned int (*lsp_unquant_func)();
typedef int (*ltp_quant_func)();
typedef unsigned int (*ltp_unquant_func)();
typedef long unsigned int * mapStringEntries_t;
typedef byte max_msg_buf[16384];
typedef char max_msg_buf_char[16384];
typedef int (*mode_query_func)();
typedef long long int movs;
typedef byte msg_buf_t[131072];
typedef my_color_deconverter * my_cconvert_ptr;
typedef my_coef_controller * my_coef_ptr;
typedef my_cquantizer * my_cquantize_ptr;
typedef my_destination_mgr * my_dest_ptr;
typedef my_downsampler * my_downsample_ptr;
typedef my_idct_controller * my_idct_ptr;
typedef my_input_controller * my_inputctl_ptr;
typedef my_main_controller * my_main_ptr;
typedef short unsigned int (*my_marker_ptr)[256];
typedef my_decomp_master * my_master_ptr;
typedef my_memory_mgr * my_mem_ptr;
typedef my_post_controller * my_post_ptr;
typedef my_prep_controller * my_prep_ptr;
typedef int (*my_src_ptr)();
typedef my_upsampler * my_upsample_ptr;
#if defined(COD2_X64) || defined(__x86_64__)
typedef char objBufEntry[160]; /* x64 DObj_s is 152 B; 100 (x86 size) overlaps pool slots */
#else
typedef char objBufEntry[100];
#endif
typedef phuff_entropy_decoder * phuff_entropy_ptr;
typedef void (*pmove_PlayerEvent)();
typedef int (*pmove_pointcontents)();
typedef void (*pmove_trace)(trace_t *, const vec_t *, const vec_t *, const vec_t *, const vec_t *, int, int);   /* real PM_trace handler sig; byte-neutral fn-ptr */
typedef jpeg_alloc * (*pthread_callback_type)();
typedef int qhandle_t;
typedef short unsigned int r_index_t;
typedef dsound_sample_t sample_t;
typedef int scr_func_t;
typedef short unsigned int scr_string_t;
typedef short unsigned int scr_thread_t;
typedef byte seq_t[65536];
typedef small_pool_struct * small_pool_ptr;
typedef snd_alias_build_s snd_alias_build_t;
typedef source_s source_t;
typedef int (*speex_callback_func)();
typedef float spx_coef_t;
typedef int16_t spx_int16_t;
typedef float spx_lsp_t;
typedef float spx_mem_t;
typedef float spx_sig_t;
typedef float spx_word16_t;
typedef float spx_word32_t;
typedef float spx_word64_t;
typedef short int tbuf_t[65536];
typedef unsigned int uInt;
typedef long unsigned int uLong;
typedef unsigned int u_int;
typedef unsigned int u_int32_t;
typedef long unsigned int u_long;
typedef void (*upsample1_ptr)();
typedef float vec_t;
typedef unsigned char * voidp;
typedef unsigned char * voidpf;
typedef void (*xcommand_t)();
typedef unsigned int yy_size_t;
typedef int yy_state_type;
typedef z_stream * z_streamp;
typedef AEDataStorageType * AEDataStorage;
typedef Boolean (*CResList)();
typedef Boolean (*ColorComplementProcPtr)();
typedef Boolean (*ColorSearchProcPtr)();
typedef Boolean (*ImageDescriptionHandle)();
typedef Boolean (*MacBuilderEditHookPtr)();
typedef Boolean (*MacBuilderProcPtr)();
typedef Boolean (*string)();
typedef CFTypeRef CFPropertyListRef;
typedef const CHAR * LPCSTR;
typedef CHAR * LPSTR;
typedef CProcPtr * CProcHndl;
typedef CTabPtr * CTabHandle;
typedef ComponentInstance AudioUnit;
typedef ComponentInstance GraphicsExportComponent;
typedef ComponentInstance GraphicsImportComponent;
typedef ControlEditTextValidationProcPtr ControlEditTextValidationUPP;
typedef ControlRef HIViewRef;
typedef ControlUserPaneDrawProcPtr ControlUserPaneDrawUPP;
typedef CursPtr * CursHandle;
typedef DWORD D3DCOLOR;
typedef void *FFItemList;
typedef DWORD LCID;
typedef DWORD * LPDWORD;
typedef DWORD (*PTHREAD_START_ROUTINE)();
typedef DWORD (*PULARGE_INTEGER)();
typedef DialogPtr DialogRef;
typedef Font * FontHandle;
typedef GDPtr * GDHandle;
typedef GfxImagePtr GfxLightmapArray[4];
typedef GrafPtr CGrafPtr;
typedef H3DPOBJECT H3DSAMPLE;
typedef HANDLE HGLOBAL;
typedef HINSTANCE HMODULE;
typedef INT16 FSERROR;
typedef INT32 bit_buf_type;
typedef IOCompletionProcPtr IOCompletionUPP;
typedef ITabPtr * ITabHandle;
typedef JBLOCKROW * JBLOCKARRAY;
typedef JCOEF JBLOCK[64];
typedef JCOEF * JCOEFPTR;
typedef JCOEF LocalizeString;
typedef JSAMPARRAY * JSAMPIMAGE;
typedef JSAMPARRAY (*hist3d)[257];
typedef JSAMPLE * JSAMPROW;
typedef LONG HRESULT;
typedef LONG * PLONG;
typedef LONG_PTR LRESULT;
typedef PRTL_CRITICAL_SECTION LPCRITICAL_SECTION;
typedef PixMapPtr * PixMapHandle;
typedef Ptr * Handle;
typedef SInt16 ControlKeyFilterResult;
typedef SInt16 ControlPartCode;
typedef SInt16 DialogItemIndex;
typedef SInt16 FMFontFamily;
typedef SInt16 FMFontSize;
typedef SInt16 FSVolumeRefNum;
typedef SInt16 OSErr;
typedef SInt32 AUNode;
typedef SInt32 CFIndex;
typedef SInt32 Fixed;
typedef SInt32 OSStatus;
typedef SInt32 TimeScale;
typedef SInt32 TimeValue;
typedef SInt64 LongDateTime;
typedef SProcPtr * SProcHndl;
typedef SoundDescriptionPtr * SoundDescriptionHandle;
typedef Str63 StrFileName;
typedef UINT16 histcell;
typedef UINT32 D3DXHANDLE;
typedef UINT_PTR SOCKET;
typedef UInt16 ATSUFontFeatureSelector;
typedef UInt16 ATSUFontFeatureType;
typedef UInt16 EventKind;
typedef UInt16 EventModifiers;
typedef UInt16 MenuItemIndex;
typedef UInt16 UniChar;
typedef UInt32 AudioChannelLabel;
typedef UInt32 AudioChannelLayoutTag;
typedef UInt32 AudioObjectID;
typedef UInt32 AudioUnitParameterID;
typedef UInt32 AudioUnitPropertyID;
typedef UInt32 AudioUnitRenderActionFlags;
typedef UInt32 ByteCount;
typedef UInt32 CFStringEncoding;
typedef UInt32 FourCharCode;
typedef UInt32 HDIGDRIVER;
typedef UInt32 ItemCount;
typedef UInt32 LSKindID;
typedef UInt32 OptionBits;
typedef UInt32 TextEncoding;
typedef UInt32 ThemeCursor;
typedef UInt32 UniCharCount;
typedef UInt32 UnsignedFixed;
typedef UInt8 Byte;
typedef ULONG_PTR SIZE_T;
typedef const WCHAR * LPCWSTR;
typedef WCHAR * LPWSTR;
typedef WindowPtr WindowRef;
#if !defined(__GNUC__) && !defined(__clang__)
typedef __builtin_va_list __gnuc_va_list;
#endif
typedef __darwin_natural_t natural_t;
typedef __int32_t __darwin_blksize_t;
typedef __int32_t __darwin_dev_t;
typedef __int64_t __darwin_blkcnt_t;
typedef __int64_t __darwin_off_t;
typedef __int64_t rlim_t;
typedef __uint16_t __darwin_mode_t;
typedef __uint32_t __darwin_gid_t;
typedef __uint32_t __darwin_ino_t;
typedef __uint32_t __darwin_socklen_t;
typedef __uint32_t __darwin_uid_t;
typedef rectDef_t UiRectangle;
typedef sample_t recordingSample_t;
typedef uInt uIntf;
typedef uLong (*check_func)();
typedef uLong uLongf;
typedef u_int32_t spx_uint32_t;
typedef vec_t vec2_t[2];
typedef vec_t vec3_t[3];
typedef vec_t vec4_t[4];
typedef voidp unzFile;
typedef voidpf (*alloc_func)();
typedef ColorComplementProcPtr ColorComplementUPP;
typedef ColorSearchProcPtr ColorSearchUPP;
typedef LPCSTR LPCTSTR;
typedef PTHREAD_START_ROUTINE LPTHREAD_START_ROUTINE;
typedef CGrafPtr AGLDrawable;
typedef CGrafPtr GWorldPtr;
typedef FSERROR * FSERRPTR;
typedef ControlKeyFilterResult (*ControlKeyFilterProcPtr)();
typedef ControlPartCode (*ControlUserPaneHitTestProcPtr)();
typedef ControlPartCode (*ControlUserPaneTrackingProcPtr)();
typedef FMFontFamily FontFamilyID;
typedef FMFontSize FontPointSize;
typedef Fixed ATSUFontVariationValue;
typedef OSStatus (*AURenderCallback)();
typedef histcell hist1d[32];
typedef AudioObjectID AudioDeviceID;
typedef FourCharCode AEEventClass;
typedef FourCharCode AEEventID;
typedef FourCharCode AEKeyword;
typedef FourCharCode ATSUFontVariationAxis;
typedef FourCharCode OSType;
typedef FourCharCode ResType;
typedef FourCharCode TXNTypeRunAttributes;
typedef OptionBits LSItemInfoFlags;
#if !defined(__GNUC__) && !defined(__clang__)
typedef __gnuc_va_list va_list;
#endif
typedef natural_t mach_port_name_t;
typedef vec2_t hullPointsPool_t[16384];
typedef vec3_t GfxPackedVector;
typedef ControlKeyFilterProcPtr ControlKeyFilterUPP;
typedef ControlUserPaneHitTestProcPtr ControlUserPaneHitTestUPP;
typedef ControlUserPaneTrackingProcPtr ControlUserPaneTrackingUPP;
typedef OSType CodecType;
typedef ResType DescType;
typedef mach_port_name_t mach_port_t;
typedef mach_port_t io_object_t;
typedef io_object_t io_iterator_t;
typedef io_object_t io_registry_entry_t;
typedef io_object_t io_service_t;

struct AEDesc {
    DescType descriptorType;
    AEDataStorage dataHandle;
};

struct AEDescList {
    DescType descriptorType;
    AEDataStorage dataHandle;
};

struct AERecord {
    DescType descriptorType;
    AEDataStorage dataHandle;
};

struct ANIHeader {
    UInt32 cbSizeOf;
    UInt32 cFrames;
    UInt32 cSteps;
    UInt32 cx;
    UInt32 cy;
    UInt32 cBitCount;
    UInt32 cPlanes;
    UInt32 JifRate;
    UInt32 flags;
};

struct ATI4CompsConverterABGR {
    int _placeholder;
};

struct ATI4CompsConverterARGB {
    int _placeholder;
};

struct AURenderCallbackStruct {
    AURenderCallback inputProc;
    void *inputProcRefCon;
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

struct AntilagClientStore {
    vec3_t realClientPositions[64];
    Bool clientMoved[64];
};

struct AppleEvent {
    DescType descriptorType;
    AEDataStorage dataHandle;
};

struct AttribInfo {
    GLint Size;
    GLint Stride;
    GLint Type;
    void *Ptr;
    UINT32 Storage[4];
};

struct AudioBuffer {
    UInt32 mNumberChannels;
    UInt32 mDataByteSize;
    void *mData;
};

struct AudioBufferList {
    UInt32 mNumberBuffers;
    AudioBuffer mBuffers[1];
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

struct BreakStatementInfo {
    const char *codePos;
    const char *nextCodePos;
    struct BreakStatementInfo *next;   /* compile-time temp list; was int -> truncated the pointer on x64 */
};

struct BuiltInMaterialTable {
    const char *name;
    _ValueType *material;
};

struct BuiltinFunctionDef {
    const char *actionString;
    BuiltinFunction actionFunc;
    int type;
};

struct BuiltinMethodDef {
    const char *actionString;
    BuiltinMethod actionFunc;
    int type;
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

struct CAudioRecorder {
    int _placeholder;
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

struct CCircularBuffer {
    int _placeholder;
};

struct CColorArray {
    int _placeholder;
};

struct CColorConverter {
    int _vptr$CColorConverter;
};

struct CD3DXBuffer {
    int _placeholder;
};

struct CD3DXConstantTable {
    int _placeholder;
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

struct CFUUIDBytes {
    UInt8 byte0;
    UInt8 byte1;
    UInt8 byte2;
    UInt8 byte3;
    UInt8 byte4;
    UInt8 byte5;
    UInt8 byte6;
    UInt8 byte7;
    UInt8 byte8;
    UInt8 byte9;
    UInt8 byte10;
    UInt8 byte11;
    UInt8 byte12;
    UInt8 byte13;
    UInt8 byte14;
    UInt8 byte15;
};

struct CFence {
    int _placeholder;
};

struct CGPoint {
    float x;
    float y;
};

struct CGSize {
    float width;
    float height;
};

struct CGRect {
    struct CGPoint origin;
    struct CGSize size;
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

struct CMMapFile {
    int _placeholder;
};

struct CMacGameEngine {
    int _vptr$CMacGameEngine;
};

struct CMacLogFile {
    int _placeholder;
};

struct CMemoryBuffer {
    int _vptr$CMemoryBuffer;
};

struct CMutex {
    int _placeholder;
};

struct CNormalArray {
    int _placeholder;
};

struct COpenGL {
    int _vptr$COpenGL;
};

struct COpenGLARBFragmentProgram {
    int _placeholder;
};

struct COpenGLATITextFragmentShader {
    int _placeholder;
};

struct COpenGLMatrix {
    int _placeholder;
};

struct COpenGLNVidiaRegisterCombinersProgram {
    int _placeholder;
};

struct COpenGLTexture {
    int _vptr$COpenGLTexture;
};

struct COpenGLVertexProgram {
    int _vptr$COpenGLVertexProgram;
};

struct CProcRec {
    Handle nxtComp;
    ColorComplementUPP compProc;
};

struct CResInfo {
    SInt32 width;
    SInt32 height;
    SInt32 depth;
    SInt32 rate;
};

struct CSampleSound {
    int _placeholder;
};

struct CSecondaryColorArray {
    int _placeholder;
};

struct CSoundEngine {
    int _placeholder;
};

struct CSoundObject {
    int _vptr$CSoundObject;
};

struct CStaticCacheInfo {
    int _vptr$CStaticCacheInfo;
};

struct CStreamSound {
    int _placeholder;
};

struct CStringEdPackage {
    mapStringEntries_t m_StringEntries;
};

struct CTexCoordArray {
    int _placeholder;
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

struct CThread {
    int _vptr$CThread;
};

struct CVAOPacket {
    int _placeholder;
};

struct CVAOPacketFixedFunction {
    int _placeholder;
};

struct CVAOPacketProgrammable {
    int _placeholder;
};

struct CVertexArray {
    int _placeholder;
};

struct CardMemory {
    int platform[2];
};

struct CaseStatementInfo {
    unsigned int name;
    const char *codePos;
    unsigned int sourcePos;
    struct CaseStatementInfo *next;   /* compile-time temp list; was int -> truncated the pointer on x64 */
};

struct CatPositionRec {
    long int initialize;
    short int priv[6];
};

struct ClientVoicePacket_t {
    int dataSize;
    byte data[256];
};

struct CodeConstantSource {
    const char *name;
    MaterialConstantSource source;
    intptr_t subtable;
    int arrayCount;
    int arrayStride;
};

struct CodeSamplerSource {
    const char *name;
    MaterialTextureSource source;
    intptr_t subtable;
    int arrayCount;
    int arrayStride;
};

union CollisionAabbTreeIndex {
    int firstChildIndex;
    int partitionIndex;
};

struct CollisionAabbTree {
    vec3_t origin;
    vec3_t halfSize;
    short unsigned int materialIndex;
    short unsigned int childCount;
    union CollisionAabbTreeIndex u;
};

struct CollisionBorder {
    vec3_t distEq;
    vec_t zBase;
    vec_t zSlope;
    vec_t start;
    vec_t length;
};

struct CollisionEdge {
    vec3_t origin;
    vec3_t axis[3];
};

struct CollisionPartition {
    byte triCount;
    byte borderCount;
    CollisionTriangle *tris;
    CollisionBorder *borders;
};

struct CollisionTriangle {
    vec4_t plane;
    vec4_t svec;
    vec4_t tvec;
    int verts[3];
    int edges[3];
};

struct CollisionVertex {
    vec3_t xyz;
};

struct CombFilterMem {
    int last_pitch;
    spx_word16_t last_pitch_gain[3];
    spx_word16_t smooth_gain;
};

struct ComponentDescription {
    OSType componentType;
    OSType componentSubType;
    OSType componentManufacturer;
    long unsigned int componentFlags;
    long unsigned int componentFlagsMask;
};

struct ComponentInstanceRecord {
    long int data[1];
};

struct ComponentRecord {
    long int data[1];
};

struct CompressionInfo {
    long int recordSize;
    OSType format;
    short int compressionID;
    short unsigned int samplesPerPacket;
    short unsigned int bytesPerPacket;
    short unsigned int bytesPerFrame;
    short unsigned int bytesPerSample;
    short unsigned int futureUse1;
};

struct ConDrawInputGlob {
    int matchCount;
    const char *inputText;
    int inputTextLen;
    Bool hasExactMatch;
    float x;
    float y;
    float leftX;
    float fontHeight;
};

struct ContinueStatementInfo {
    const char *codePos;
    const char *nextCodePos;
    struct ContinueStatementInfo *next;   /* compile-time temp list; was int -> truncated the pointer on x64 */
};

struct ControlEditTextSelectionRec {
    SInt16 selStart;
    SInt16 selEnd;
};

struct ControlID {
    OSType signature;
    SInt32 id;
};

struct ControlKind {
    OSType signature;
    OSType kind;
};

struct ConversionArguments {
    int argCount;
    const char * args[9];
};

struct Cylinder {
    int _placeholder;
};

struct D3DTEXTUREOP {
    bool unused;
};

struct D3DXFLOAT16 {
    int _placeholder;
};

struct D3DXMATRIX {
    int _placeholder;
};

struct D3DXVECTOR3 {
    int _placeholder;
};

struct D3DXVECTOR4 {
    FLOAT x;
    FLOAT y;
    FLOAT z;
    FLOAT w;
};

struct DObjAnimMat {
    float quat[4];
    float trans[3];
    float transWeight;
};

struct DObjModel_s {
    struct XModel *model;
    const char *boneName;
    qboolean ignoreCollision;
};

struct DObjSkelMat_s {
    float axis[3][4];
    float origin[4];
};

struct DObjTrace_s {
    float fraction;
    int surfaceflags;
    vec3_t normal;
    short unsigned int partName;
    short unsigned int partGroup;
};

struct DObj_s {
    XAnimTree *tree;
    DSkel *skel;
    int timeStamp;
    short unsigned int *animToModel;
    short unsigned int duplicateParts;
    volatile int locked;
    unsigned char numModels;
    unsigned char numBones;
    unsigned char ignoreCollision;
    XModel * models[8];
    unsigned char modelParents[8];
    unsigned char matOffset[8];
    vec3_t mins;
    vec3_t maxs;
};

struct DSkel {
    int animPartBits[4];
    int controlPartBits[4];
    int skelPartBits[4];
    DObjAnimMat mat[1];
};

struct DSurface_s {
    short int modelIndex;
    short int subMatIndex;
};

struct DXTexColor {
    UINT8 a;
    UINT8 r;
    UINT8 g;
    UINT8 b;
};

struct DXVector3 {
    float m[3];
};

struct DXVector4 {
    float m[4];
};

struct DateTimeRec {
    short int year;
    short int month;
    short int day;
    short int hour;
    short int minute;
    short int second;
    short int dayOfWeek;
};

struct DebugGlobals {
    vec3_t *verts;
    int vertCount;
    int vertLimit;
    GfxDebugPoly *polys;
    int polyCount;
    int polyLimit;
    trDebugString_t *strings;
    int stringCount;
    int stringLimit;
    trDebugString_t *externStrings;
    int externStringCount;
    int externMaxStringCount;
    trDebugLine_t *lines;
    int lineCount;
    int lineLimit;
    trDebugLine_t *externLines;
    int externLineCount;
    int externMaxLineCount;
    GfxDebugPlume *plumes;
    int plumeCount;
    int plumeLimit;
};

struct DiskCollAabbTree {
    vec3_t origin;
    vec3_t halfSize;
    short unsigned int materialIndex;
    short unsigned int childCount;
    union { int firstChildIndex; int partitionIndex; } u;
};

struct DiskCollBorder {
    vec3_t distEq;
    vec_t zBase;
    vec_t zSlope;
    vec_t start;
    vec_t length;
};

struct DiskCollEdge {
    int checkStamp;
    vec3_t origin;
    vec3_t axis[3];
    vec_t length;
};

struct DiskCollPartition {
    short unsigned int checkStamp;
    byte triCount;
    byte borderCount;
    int firstTriIndex;
    int firstBorderIndex;
};

struct DiskCollTriangle {
    vec4_t plane;
    vec4_t svec;
    vec4_t tvec;
    int vertIndices[3];
    int edgeIndices[3];
};

struct DiskCollVertex {
    int checkStamp;
    vec3_t xyz;
};

struct DiskGfxAabbTree {
    int firstSurface;
    int surfaceCount;
    int childCount;
};

struct DiskGfxCell {
    vec3_t mins;
    vec3_t maxs;
    int aabbTreeIndex;
    int firstPortal;
    int portalCount;
    int firstCullGroup;
    int cullGroupCount;
    int firstOccluder;
    int occluderCount;
};

struct DiskGfxCullGroup {
    vec3_t mins;
    vec3_t maxs;
    int firstSurface;
    int surfaceCount;
};

struct DiskGfxOccluder {
    int firstPlane;
    short int planeCount;
    short int edgeCount;
    int firstEdge;
    int firstPortalVertex;
    short int portalVertexCount;
};

struct DiskGfxOccluderEdge {
    byte planeIndex[2];
    byte vertexIndex[2];
};

struct DiskGfxPortal {
    int planeIndex;
    int cellIndex;
    int firstPortalVertex;
    int portalVertexCount;
};

struct DiskGfxVertex {
    vec3_t xyz;
    vec3_t normal;
    byte color[4];
    vec2_t texCoord;
    vec2_t lmapCoord;
    vec3_t tangent;
    vec3_t binormal;
};

struct DiskLeaf {
    int cluster;
    int area;
    int firstCollAabbIndex;
    int collAabbCount;
    int firstLeafBrush;
    int numLeafBrushes;
    int cellNum;
    int firstLightIndex;
    int numLights;
};

struct DiskTriangleSoup {
    short unsigned int materialIndex;
    short unsigned int lightmapIndex;
    int firstVertex;
    short unsigned int vertexCount;
    short unsigned int indexCount;
    int firstIndex;
};

struct DpvsDebug {
    Bool drawWorld;
    Bool drawEntities;
    Bool drawSModels;
    Bool drawXModels;
    Bool drawBModels;
};

union DpvsPlaneInfo {
    byte frontal;
    byte ignoreStackLevel;
};

struct DpvsPlane {
    vec4_t coeffs;
    byte side[3];
    union DpvsPlaneInfo u;
};

union DvarLimits {
    struct { int stringCount; const char * *strings; } enumeration;
    struct { int min; int max; } integer;
    struct { float min; float max; } value;
    struct { vec_t min; vec_t max; } vector;
};

union DvarValue {
    Bool enabled;
    int integer;
    float value;
    vec_t *vector;
    const char *string;
    byte color[4];
};

struct DxCapsCheckBits {
    int offset;
    DWORD setBits;
    DWORD clearBits;
    DxCapsResponse response;
    const char *msg;
};

struct DxCapsCheckInteger {
    int offset;
    int min;
    int max;
    DxCapsResponse response;
    const char *msg;
};

struct DxStencilDecode {
    int shift;
    D3DRENDERSTATETYPE renderState;
};

struct DxTextureOpDecode {
    DWORD enumerant;
    int usedArgs;
};

struct DxTextureStageEnums {
    D3DTEXTURESTAGESTATETYPE op;
    D3DTEXTURESTAGESTATETYPE arg[3];
};

struct EffectCluster {
    vec3_t origin;
    int refCount;
};

struct FxEffectDef {
    const char *mEffectName;
    int mPrimitiveCount;
    PrimitiveTemplate * mPrimitives[24];
};

union EffectTemplateLoadPtr {
    EffectTemplate *fx;
    const char *name;
};

struct EffectVisInfo {
    vec3_t origin;
    float distSq;
    float vis;
};

struct Emitter {

    char _base[0x24c];

    vec3_t emitPos;
    vec3_t initialVel;
    vec3_t velocityDelta;
    int emitLastTime;
    float emitStep;
    float spawnSize;
    char _gap2[8];
    float spawnDensity;
    char _gap3[4];
    float spawnStep;
    void *emitFx;
    float spawnVariance;
    char _tail[4];

};

struct EncState {
    const SpeexMode *mode;
    int first;
    int frameSize;
    int subframeSize;
    int nbSubframes;
    int windowSize;
    int lpcSize;
    int min_pitch;
    int max_pitch;
    int safe_pitch;
    int bounded_pitch;
    int ol_pitch;
    int ol_voiced;
    int *pitch;
    spx_word16_t gamma1;
    spx_word16_t gamma2;
    float lag_factor;
    float lpc_floor;
    char *stack;
    spx_sig_t *inBuf;
    spx_sig_t *frame;
    spx_sig_t *excBuf;
    spx_sig_t *exc;
    spx_sig_t *swBuf;
    spx_sig_t *sw;
    spx_sig_t *innov;
    spx_word16_t *window;
    spx_word16_t *autocorr;
    spx_word16_t *lagWindow;
    spx_coef_t *lpc;
    spx_lsp_t *lsp;
    spx_lsp_t *qlsp;
    spx_lsp_t *old_lsp;
    spx_lsp_t *old_qlsp;
    spx_lsp_t *interp_lsp;
    spx_lsp_t *interp_qlsp;
    spx_coef_t *interp_lpc;
    spx_coef_t *interp_qlpc;
    spx_coef_t *bw_lpc1;
    spx_coef_t *bw_lpc2;
    spx_mem_t *mem_sp;
    spx_mem_t *mem_sw;
    spx_mem_t *mem_sw_whole;
    spx_mem_t *mem_exc;
    spx_word32_t *pi_gain;
    VBRState *vbr;
    float vbr_quality;
    float relative_quality;
    int vbr_enabled;
    int vad_enabled;
    int dtx_enabled;
    int dtx_count;
    int abr_enabled;
    float abr_drift;
    float abr_drift2;
    float abr_count;
    int complexity;
    int sampling_rate;
    int plc_tuning;
    int encode_submode;
    const const SpeexSubmode * *submodes;
    int submodeID;
    int submodeSelect;
};

struct EventTypeSpec {
    UInt32 eventClass;
    UInt32 eventKind;
};

struct FFData {
    FFType findType;
    char wildcard[32];
    void * (*items)();
    Boolean iterValid;
};

struct FIFO {
    int _vptr$FIFO;
};

struct FSRef {
    UInt8 hidden[80];
};

struct FSSpec {
    short int vRefNum;
    long int parID;
    StrFileName name;
};

struct FXInfo {
    SInt16 fdIconID;
    SInt16 fdReserved[3];
    SInt8 fdScript;
    SInt8 fdXFlags;
    SInt16 fdComment;
    SInt32 fdPutAway;
};

struct Flash {
    int _placeholder;
};

struct FreeRequest {
    UINT8 *mPtr;
    UINT32 mLength;
    UINT32 mDelay;
};

struct FxArchive {
    struct MemoryFile *memFile;
    char isReading;
    char unusedFlag;
    char pad[2];
    int byteCounter;
    int repeatCount;
    int zeroCount;
    int startPos;
};

struct FxBoltFramePtr {
    union {
        struct FxBoltFrame *value;
        int _placeholder;
    };
};

struct EffectPrimitive {
    const EffectTemplate *fx;
    const PrimitiveTemplate *primTemp;
    FxBoltFramePtr boltFrame;
};

struct FxBoltInfo {
    int dobjHandle;
    int boneIndex;
};

struct FxBoltFrame {
    int refCount;
    union {
        int cachedServerTime;
        int mTime;
    };
    orientation_t orientation;
    struct FxBoltFrame *next;
    struct FxBoltInfo mBolt;

};

struct FxCamera {
    vec3_t vieworg;
    vec4_t frustum[6];
    int numPlanes;
};

struct FxCurve {
    int dimensionCount;
    int keyCount;
    float keys[1];
};

struct FxCurveIterator {
    const FxCurve *master;
    int currentKeyIndex;
};

struct FxChannelInstance {
    struct FxCurveIterator curveIterator;
    float scale;
};

struct FxFlagEntry {
    const char *flag;
    unsigned int masks[2];
};

struct FxHelper {
    int time;
    int mTime;
    int mOldTime;
    int mFrameTime;
    qboolean mTimeFrozen;
    struct FxCamera mCamera;
    FxCamera mPrevCamera;
    int mSeed;
    float adsZoomFactor;

    bool CullSphere(const float worldPos[3], float radius, int planeCount);
};

struct FxImpactTable {
    const char *name;
    FxImpactEntry *table;
};

struct FxMemMgr_Cloud {
    int _placeholder;
};

struct FxMemMgr_Cylinder {
    int _placeholder;
};

struct FxMemMgr_Effect {
    int _placeholder;
};

struct FxMemMgr_Emitter {
    int _placeholder;
};

struct FxMemMgr_FxBoltFrame {
    int _placeholder;
};

struct FxMemMgr_Light {
    int _placeholder;
};

struct FxMemMgr_Line {
    int _placeholder;
};

struct FxMemMgr_OrientedParticle {
    int _placeholder;
};

struct FxMemMgr_Particle {
    int _placeholder;
};

struct FxMemMgr_PrimitiveTemplate {
    int _placeholder;
};

struct FxMemMgr_ScheduledEffect {
    int _placeholder;
};

struct FxMemMgr_Tail {
    int _placeholder;
};

struct FxRange {
    float mMin;
    float mMax;
};

struct FxChannel {
    const FxCurve *curve;
    FxRange scaleRange;
};

struct FxChannelBackwardCompatible {
    FxRange start[3];
    FxRange end[3];
    FxRange parm;
    int flags;
    Bool containsData;
};

struct BackCompatibleParameters {
    FxChannelBackwardCompatible fxChannels[24];
};

struct FxScheduler {
    int mSeed;
    ScheduledEffect *mScheduledHead;
    int mScheduledCount;
};

struct GPObject {
    const char *name;
    struct GPObject *next;
    struct GPObject *sortedNext;
    struct GPObject *sortedPrev;
};

struct GPValue {
    const char *name;
    struct GPObject *next;
    struct GPObject *sortedNext;
    struct GPObject *sortedPrev;
    struct GPValue *valueList;
};

struct GPGroup {
    const char *name;
    struct GPObject *nextUnsorted;
    struct GPObject *sortedNext;
    struct GPObject *sortedPrev;
    struct GPValue *pairList;
    struct GPValue *pairSorted;
    struct GPValue *pairLast;
    struct GPGroup *subGroupList;
    struct GPGroup *subGroupSorted;
    struct GPGroup *subGroupLast;
    void *parent;
    byte cleanFlag;
};

struct GenericParser2 {
    struct GPGroup group;
    struct TextPool *textPoolList;
    byte writeable;
};

struct GetVolParmsInfoBuffer {
    short int vMVersion;
    long int vMAttrib;
    Handle vMLocalHand;
    long int vMServerAdr;
    long int vMVolumeGrade;
    short int vMForeignPrivID;
    long int vMExtendedAttributes;
    void *vMDeviceID;
    UniCharCount vMMaxNameLength;
};

struct GfxAabbTree {
    vec3_t mins;
    vec3_t maxs;
    int surfaceCount;
    int startSurfIndex;
    int staticModelCount;
    int *staticModels;
    int childCount;
    struct GfxAabbTree *children;
};

struct GfxBackEndLightSettings {
    vec4_t position;
    vec4_t ambient;
    vec4_t color;
    vec4_t specular;
    GfxLightDef *def;
};

struct GfxBrushModel {
    vec3_t bounds[2];
    int surfaceCount;
    int startSurfIndex;
};

struct GfxCachedShaderText {
    const char *name;
    const char *text;
    int textSize;
};

struct GfxCellWritable {
    int markCount;
};

struct GfxCell {
    struct GfxCellWritable writable;
    vec3_t mins;
    vec3_t maxs;
    GfxAabbTree *aabbTree;
    int portalCount;
    GfxPortal *portals;
    int cullGroupCount;
    int *cullGroups;
    int occluderCount;
    GfxOccluder * *occluders;
    GfxSceneModelCellRef *modelRefs;
};

struct GfxCmdArray {
    byte cmds[196608];
    int usedTotal;
    int usedCritical;
    GfxCmdHeader *lastCmd;
};

struct GfxCmdHeader {
    short unsigned int id;
    short unsigned int byteCount;
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

struct GfxCmdCall {
    struct GfxCmdHeader header;
    const void *subCmd;
};

struct GfxCmdClearScreen {
    GfxCmdHeader header;
    byte whichToClear;
    byte stencil;
    float depth;
    vec4_t color;
};

struct GfxCmdDrawSunPostEffects {
    GfxCmdHeader header;
    int viewIndex;
};

struct GfxCmdDrawSurfs {
    GfxCmdHeader header;
    GfxDrawSurfOrder order;
    GfxDrawSurf *drawSurfs;
    int drawSurfCount;
    MaterialTechniqueType techType;
};

struct GfxCmdDrawTriangles {
    GfxCmdHeader header;
    const Material *material;
    MaterialTechniqueType techType;
    short int indexCount;
    short int vertexCount;
};

struct GfxCmdSetClipPlanes {
    GfxCmdHeader header;
    int clipPlaneCount;
    vec4_t clipPlanes[1];
};

struct GfxCmdSetLightProperties {
    GfxCmdHeader header;
    int lightIndex;
    vec4_t position;
    vec4_t ambient;
    vec4_t color;
    vec4_t specular;
    GfxLightDef *lightDef;
};

struct GfxCmdSetMaterialColor {
    GfxCmdHeader header;
    vec4_t color;
};

struct GfxCmdSetRenderTarget {
    GfxCmdHeader header;
    GfxRenderTargetId renderTargetId;
};

struct GfxCmdSetStencilRefValue {
    GfxCmdHeader header;
    int refValue;
};

struct GfxCmdStencilPlanes {
    GfxCmdHeader header;
    vec_t nearDist;
    int planeCount;
    vec_t planeDists[1];
};

struct GfxCmdStretchRaw {
    GfxCmdHeader header;
    int x;
    int y;
    int w;
    int h;
    int cols;
    int rows;
    const byte *data;
    int client;
    qboolean dirty;
};

union GfxColor {
    unsigned int packed;
    byte array[4];
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

struct GfxCmdDrawQuadPic {
    GfxCmdHeader header;
    const Material *material;
    vec2_t verts[4];
    GfxColor color;
};

struct GfxCmdDrawSprite {
    GfxCmdHeader header;
    Material *material;
    GfxColor rgbaColor;
    vec3_t pos;
    float radius;
    float minScreenRadius;
    int renderFxFlags;
};

struct GfxCmdStretchPic {
    GfxCmdHeader header;
    const Material *material;
    float x;
    float y;
    float w;
    float h;
    float s0;
    float t0;
    float s1;
    float t1;
    GfxColor color;
};

struct GfxCmdStretchPicRotate {
    GfxCmdHeader header;
    const Material *material;
    float x;
    float y;
    float w;
    float h;
    float s0;
    float t0;
    float s1;
    float t1;
    GfxColor color;
    float rotation;
};

struct GfxCullGroup {
    vec3_t mins;
    vec3_t maxs;
    int surfaceCount;
    int startSurfIndex;
};

struct GfxCullGroupDynamic {
    int viewCount;
};

struct GfxDebugPlume {
    vec3_t origin;
    vec4_t color;
    int score;
    int startTime;
    int duration;
};

struct GfxDebugPoly {
    vec4_t color;
    int firstVert;
    int vertCount;
};

struct GfxDrawGroupCommands {
    Bool isIssuingGlue;
    int begin;
    int perView[4];
    int end;
};

struct GfxDrawPrimArgs {
    int firstVertexFromBase;
    int vertexCount;
    int primCount;
    union { struct { int baseVertex; int baseIndex; } buf; struct { int vertexStride; } tess; } u;
};

struct GfxDrawSurf {
    GfxDrawSurfSort sort;
    const surfaceType_t *surface;
};

struct GfxEntityLighting {
    int viewCount;
    float sunVisibility;
    vec4_t colorForDir[6];
};

struct GfxFog {
    GfxFogOffset techniqueOffset;
    int startTime;
    int finishTime;
    union GfxColor color;
    float fogStart;
    float fogEnd;
    float density;
    Bool registered;
    Bool drawSky;
    Bool clearScreen;
};

struct GfxGammaRamp {
    short unsigned int entries[256];
};

struct GfxImageFileHeader {
    char tag[3];
    byte version;
    byte format;
    byte flags;
    short int dimensions[3];
    int fileSizeForPicmip[4];
};

struct GfxImageFilterPass {
    const Material *material;
    float srcWidth;
    float srcHeight;
    int dstWidth;
    int dstHeight;
    int tapHalfCount;
    vec4_t tapOffsetsAndWeights[8];
};

struct GfxImageFilter {
    int passCount;
    GfxImageFilterPass passes[16];
    GfxImage *sourceImage;
    GfxRenderTargetId finalTarget;
    GfxRenderTargetId pingPongTargets[2];
};

struct GfxImageLoadDef {
    byte format;
    byte flags;
    short int dimensions[3];
    byte *data;
    int resourceSize;
};

struct GfxLightDir {
    vec3_t ambientColor;
};

struct GfxLightGrid {
    int pointCount;
    GfxLightGridPoint *points;
    int colorCount;
    GfxLightGridColors *colors;
};

struct GfxLightGridColors {
    byte rgb[3][8];
};

struct GfxLightGridPoint {
    unsigned int xyzHighBits;
    unsigned char xyzLowBitsAndSunVisible;
    unsigned char needsTrace;
    short unsigned int colorsIndex;
};

struct GfxLightImage {
    GfxImage *image;
    GfxSamplerState samplerState;
};

struct GfxLightDef {
    GfxLightType type;
    struct GfxLightImage cookie;
    GfxLightImage attenuation;
    const char *name;
};

union GfxLightInfo {
    struct GfxLightDir dir;
};

struct GfxLight {
    GfxLightDef *def;
    vec4_t position;
    vec3_t color;
    union GfxLightInfo u;
};

struct GfxLodParms {
    vec3_t origin;
    float scale;
    float bias;
};

struct GfxMarkPoint {
    vec3_t xyz;
    vec2_t lmapCoord;
    vec3_t normal;
};

union GfxModel {
    const struct DObj_s *obj;
    const struct XModel *model;
    const struct GfxBrushModel *bmodel;
    const void *data;
};

struct GfxModelSurface {
    surfaceType_t surfType;
    XSurface *xsurf;
};

struct GfxModelRigidSurface {
    GfxModelSurface surf;
    vec3_t boneAxis[4];
};

struct GfxModelSkinnedSurface {
    struct GfxModelSurface surf;
    int skinnedCachedOffset;
    union { void *variant; GfxVertexDx7 *dx7; GfxVertex *dx9; } skinnedVert;
};

struct GfxOccluder {
    int planeCount;
    DpvsPlane *planes;
    int edgeCount;
    GfxOccluderEdge *edges;
    int vertexCount;
    vec3_t *vertices;
    int ignoreStackLevel;
    int viewPlaneCount;
    DpvsPlane *viewPlanes;
};

struct GfxOccluderEdge {
    DpvsPlane * plane[2];
    vec3_t * vertex[2];
};

struct GfxPointVertex {
    vec3_t xyz;
    byte color[4];
};

struct GfxCmdDrawLines {
    GfxCmdHeader header;
    short int lineCount;
    short int width;
    GfxPointVertex verts[2];
};

struct GfxCmdDrawPoints {
    GfxCmdHeader header;
    short int pointCount;
    short int size;
    GfxPointVertex verts[1];
};

struct GfxPortalWritable {
    Bool isQueued;
    Bool isAncestor;
    GfxPortal *queuedParent;
};

struct GfxPortal {
    struct GfxPortalWritable writable;
    struct DpvsPlane plane;
    GfxCell *cell;
    vec3_t *vertices;
    byte vertexCount;
    byte hullPointCount;
    vec2_t *hullPoints;
    vec3_t hullAxis[2];
};

struct GfxPosTexVertex {
    vec3_t xyz;
    vec2_t texCoord;
};

struct GfxPostProcessParms {
    float blurRadius;
    Bool isRenderingFullScreen;
};

struct GfxRenderCommandExecState {
    const void *cmd;
    int stackPos;
    const void * retCmd[2];
};

struct GfxRenderTarget {
    GfxImage *image;
    IDirect3DSurface9 *colorSurface;
    IDirect3DSurface9 *depthStencilSurface;
    int width;
    int height;
};

struct GfxSModelCachedVertex {
    vec3_t xyz;
    vec3_t normal;
    GfxColor color;
    vec2_t texCoord;
    short unsigned int baseLightingCoords[2];
    vec3_t binormal;
    vec3_t tangent;
};

struct GfxSModelCachedVertexDx7 {
    vec3_t xyz;
    GfxColor color;
    vec2_t texCoord;
};

struct GfxSceneDef {
    int time;
    float floatTime;
    int entityCount;
    GfxEntity *entities;
};

struct GfxCmdBeginView {
    GfxCmdHeader header;
    struct GfxSceneDef sceneDef;
    const GfxViewParms *viewParms;
    struct GfxLodParms lodParms;
    int viewCount;
};

struct GfxSceneEntity {
    int refCount;
    union GfxModel u;
    const struct centity_s *cent;
    volatile int cullState;
    int surfCount;
    vec3_t curMins;
    vec3_t curMaxs;
    const Material * *materials;
    surfaceType_t *surfs;
};

struct GfxScene {
    int viewCount;
    struct GfxSceneDef def;
    int dlightCount;
    GfxLight dlights[32];
    Bool dlightCulled[32];
    int drawSurfCount;
    GfxDrawSurf *drawSurfs;
    int polyCount;
    GfxSceneEntity sceneEnts[2046];
    int sceneEntMaterialCount;
    const Material * sceneMaterials[4096];
};

struct GfxSceneModelCellRef {
    int entIndex;
    vec3_t mins;
    vec3_t maxs;
    intptr_t next;
};

struct DpvsScene {
    GfxSceneModelCellRef modelRefs[4096];
    int modelRefCount;
};

struct GfxStateOverride {
    int stateBitsMaskClear[2];
    int stateBitsMaskSet[2];
};

struct GfxStaticModelCachedSurface {
    GfxModelSurface surf;
    GfxStaticModelSurfaceCached *surface;
    GfxEntity *ent;
};

struct GfxStaticModelDynamic {
    int viewCount;
    GfxStaticSurface *staticSurfs;
};

struct GfxStaticModelInstance {
    float cullDist;
    vec3_t origin;
    struct XModel *model;
    vec3_t mins;
    vec3_t maxs;
    vec3_t axis[3];
    float scale;
    vec3_t baseLightingCoords;
};

struct GfxStaticModelSurfaceCached {
    int baseVertIndex;
    int smodelIndex;
    const XSurface *xsurf;
    GfxStaticSurface *surface;
};

struct GfxStaticSurface {
    GfxStaticModelSurfaceCached * cachedLods[4];
};

struct GfxSurface {
    const Material *material;
    short unsigned int lightmapIndex;
    short unsigned int sortGroup;
    union { surfaceType_t *data; srfTriangles_t *tris; };
};

struct GfxSurfaceDynamic {
    int viewCount;
};

union GfxTexture {
    IDirect3DBaseTexture9 *basemap;
    IDirect3DTexture9 *map;
    IDirect3DVolumeTexture9 *volmap;
    IDirect3DCubeTexture9 *cubemap;
    GfxImageLoadDef *loadDef;
};

struct GfxVertex {
    vec4_t xyzw;
    vec3_t normal;
    GfxColor color;
    vec2_t texCoord;
    vec3_t binormal;
    vec3_t tangent;
};

struct GfxVertexDx7 {
    vec3_t xyz;
    vec3_t normal;
    GfxColor color;
    vec2_t texCoord;
};

struct GfxViewport {
    int x;
    int y;
    int width;
    int height;
};

struct GfxCmdSetViewport {
    GfxCmdHeader header;
    GfxViewport viewport;
};

struct GfxWorldVertex {
    vec3_t xyz;
    GfxPackedVector normal;
    union GfxColor color;
    vec2_t texCoord;
    vec2_t lmapCoord;
    GfxPackedVector binormal;
    GfxPackedVector tangent;
};

struct GfxWorldVertexData {
    GfxWorldVertex *vertices;
    IDirect3DVertexBuffer9 *worldVb;
};

struct GfxWorldVertexDx7 {
    vec3_t xyz;
    GfxColor color;
    vec2_t texCoord;
    vec2_t lmapCoord;
};

struct Glyph {
    short unsigned int letter;
    signed char x0;
    signed char y0;
    byte dx;
    byte pixelWidth;
    byte pixelHeight;
    float s0;
    float t0;
    float s1;
    float t1;
};

struct HCURSOR__ {
    int unused;
};

struct HDC__ {
    int unused;
};

struct HICommand {
    UInt32 attributes;
    UInt32 commandID;
    struct { MenuRef menuRef; MenuItemIndex menuItemIndex; } menu;
};

struct HINSTANCE__ {
    int unused;
};

struct HISize {
    float width;
    float height;
};

struct HMONITOR__ {
    int unused;
};

struct HWND__ {
    int unused;
};

struct GfxWindowParms {
    HWND hwnd;
    int hz;
    Bool fullscreen;
    int x;
    int y;
    int width;
    int height;
    int tileCount;
    int aaSamples;
};

struct GfxWindowTarget {
    HWND hwnd;
    IDirect3DSwapChain9 *swapChain;
    int width;
    int height;
};

struct HashEntry {
    HashIndex status_next;
    union { HashIndex prev; HashIndex str; } u;
};

struct ID3DXBuffer {
    int _placeholder;
};

struct ID3DXConstantTable {
    int _placeholder;
};

struct ID3DXFont {
    int _placeholder;
};

struct ID3DXInclude {
    int _vptr$ID3DXInclude;
};

struct ID3DXSprite {
    int _placeholder;
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

struct IDirect3DIndexBuffer9 {
    int _placeholder;
};

struct IDirect3DQuery9 {
    int _placeholder;
};

struct IDirect3DResource9 {
    int _placeholder;
};

struct IDirect3DStateBlock9 {
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

struct IDirect3DVertexShader9 {
    int _placeholder;
};

struct IDirect3DVolumeTexture9 {
    int _placeholder;
};

struct IID {
    long unsigned int Data1;
    short unsigned int Data2;
    short unsigned int Data3;
    unsigned char Data4[8];
};

struct ITab {
    long int iTabSeed;
    short int iTabRes;
    Byte iTTable[1];
};

struct CGGammaValue {
    CFStringRef quantval;
    ITabHandle sent_table;
};

struct IUnknown {
    int _vptr$IUnknown;
};

struct ImageDescription {
    long int idSize;
    CodecType cType;
    long int resvd1;
    short int resvd2;
    short int dataRefIndex;
    short int version;
    short int revisionLevel;
    long int vendor;
    CodecQ temporalQuality;
    CodecQ spatialQuality;
    short int width;
    short int height;
    Fixed hRes;
    Fixed vRes;
    long int dataSize;
    short int frameCount;
    Str31 name;
    short int depth;
    short int clutID;
};

struct ImageList {
    int count;
    GfxImage * image[2048];
};

struct Image_MemUsage {
    int total;
    int lightmap;
    int minspec;
};

struct IncludeClass {
    int _placeholder;
};

struct ItemKeyHandler {
    int key;
    const char *action;
    intptr_t next;
};

struct JHUFF_TBL {
    UINT8 bits[17];
    UINT8 huffval[256];
    boolean sent_table;
};

struct JQUANT_TBL {
    UINT16 quantval[64];
    boolean sent_table;
};

struct KeyStruct {
    UInt32 ignore0[16];
    struct CFUUIDBytes uuid1;
    UInt32 ignore1[1];
    UInt8 creatorName[64];
    UInt32 ignore2[1];
    UInt8 creatorMachine[64];
    UInt32 ignore3[8];
    LongDateTime dateCreated;
    UInt32 ignore4[1];
    UInt8 gameName[64];
    UInt32 ignore5[4];
    OSType gameCreator;
    UInt32 ignore6[1];
    UInt8 playerName[64];
    UInt32 ignore7[16];
    CFUUIDBytes uuid2;
    UInt32 ignore8[4];
    UInt32 checkSum;
    UInt32 ignore9[128];
};

struct LSItemInfoRecord {
    LSItemInfoFlags flags;
    OSType filetype;
    OSType creator;
    CFStringRef extension;
    CFStringRef iconFileName;
    LSKindID kindID;
};

// same layout and members as the universal/com_memory.h LargeLocal
struct LargeLocal {
    LargeLocal(int size);
    ~LargeLocal();
    void *GetBuf();
private:
    int pos;
    int reserved[3];
};

struct LegacyHacks {
    int cg_norender;
    int cl_running;
    int cl_stance;
    int cl_stanceTemp;
    int cl_downloadSize;
    int cl_downloadCount;
    int cl_downloadTime;
    char cl_downloadName[64];
    char cl_serverloadmap[64];
    char cl_serverloadgametype[64];
    Bool cl_serverloadwaiting;
    Bool sv_killserver;
    char sv_killreason[256];
    char ui_scriptMenu[256];
    int ui_scriptMenuIndex;
    char ui_newScriptMenu[256];
    int ui_newScriptMenuIndex;
    char ui_waitingScriptMenu[256];
    int ui_waitingScriptMenuIndex;
    Bool ui_waitingScriptMenuNoMouse;
    Bool ui_scriptMenuAllowResponse;
    char ui_savegameName[256];
    char ui_savegameInfo[256];
    Bool winnt;
    int persid;
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

struct Line {

    char _base[0x1C4];
    vec3_t endpoint;
};

struct LocalDateTime {
    UInt16 highSeconds;
    UInt32 lowSeconds;
    UInt16 fraction;
};

union LongDateRec {
    struct { short int era; short int year; short int month; short int day; short int hour; short int minute; short int second; short int dayOfWeek; short int dayOfYear; short int weekOfYear; short int pm; short int res1; short int res2; short int res3; } ld;
    short int list[14];
    struct { short int eraAlt; DateTimeRec oldDate; } od;
};

struct MD4_CTX {
    UINT4 state[4];
    UINT4 count[2];
    unsigned char buffer[64];
};

struct MacImageInfo {
    SInt16 width;
    SInt16 height;
    SInt32 rowBytes;
    unsigned int *data;
};

struct MacRGBColor {
    short unsigned int red;
    short unsigned int green;
    short unsigned int blue;
};

struct ColorSpec {
    short int value;
    struct MacRGBColor rgb;
};

struct ColorTable {
    long int ctSeed;
    short int ctFlags;
    short int ctSize;
    CSpecArray ctTable;
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

struct MacRect {
    short int top;
    short int left;
    short int bottom;
    short int right;
};

struct MantleAnimTransition {
    int upAnimIndex;
    int overAnimIndex;
    float height;
};

struct MantleResults {
    vec3_t dir;
    vec3_t startPos;
    vec3_t ledgePos;
    vec3_t endPos;
    int flags;
    int duration;
};

struct MantleState {
    float yaw;
    int timer;
    int transIndex;
    int flags;
};

struct MaterialArgumentCodeConst {
    short unsigned int index;
    byte firstRow;
    byte rowCount;
};

union MaterialArgumentDef {
    const float16 *literalConst;
    struct MaterialArgumentCodeConst codeConst;
    int codeSampler;
    const char *name;
};

struct MaterialConstantDefObj {
    const char *name;
    vec4_t literal;
};

struct MaterialInfo {
    const char *name;
    const char *refImageName;
    short unsigned int hashIndex;
    short unsigned int sortedIndex;
    byte gameFlags;
    byte sortKey;
    byte textureAtlasRowCount;
    byte textureAtlasColumnCount;
    float maxDeformMove;
    byte deformFlags;
    byte usage;
    short unsigned int toolFlags;
    unsigned int locale;
    short unsigned int autoTexScaleWidth;
    short unsigned int autoTexScaleHeight;
    float tessSize;
    int surfaceFlags;
    int contents;
};

struct Material {
    struct MaterialInfo info;
    int stateBits[2];
    short unsigned int textureCount;
    short unsigned int constantCount;
    MaterialTechniqueSet *techniqueSet;
    MaterialTextureDef *textures;
    MaterialConstantDef *constants;
};

struct Font_s {
    const char *name;
    int pixelHeight;
    int glyphCount;
    MaterialHandle material;
    Glyph *glyphs;
};

struct CachedAssets_t {
    MaterialHandle scrollBarArrowUp;
    MaterialHandle scrollBarArrowDown;
    MaterialHandle scrollBarArrowLeft;
    MaterialHandle scrollBarArrowRight;
    MaterialHandle scrollBar;
    MaterialHandle scrollBarThumb;
    MaterialHandle sliderBar;
    MaterialHandle sliderThumb;
    MaterialHandle whiteMaterial;
    MaterialHandle cursor;
    FontHandle bigFont;
    FontHandle smallFont;
    FontHandle consoleFont;
    FontHandle boldFont;
    FontHandle textFont;
    FontHandle extraBigFont;
    snd_alias_list_t *itemFocusSound;
};

struct FxGfxEntity {
    MaterialHandle customMaterial;
    float rotation;
    vec3_t axis[3];
    vec3_t dlightColor;
    float materialTime;
    vec3_t origin;
    float radius[2];
    byte materialRGBA[4];
    int materialSubimageIndex;
    float scale;
    vec3_t endpos;
};

struct Effect {

    int _vptr$Effect;
    vec3_t origin;
    int field_0x10;
    vec3_t axis[2];

    struct Effect *deathEffect;
    struct Effect *emitEffect;
    struct Effect *impactEffect;
    int field_0x38;
    float normTime;
    struct FxGfxEntity mRefEnt;
    int mFlags;
    int mClusterId;
    int mSortGroup;
    struct XModel *mModelPtr;
    int mTimeStart;
    int mTimeEnd;
    struct FxBoltFramePtr mBolt;

};

struct Light {
    struct Effect base;
    float colorBlendFactor;
    float sizeBlendFactor;
    FxChannelInstance colorChannelInstance;
    FxChannelInstance colorRandChannelInstance;
    FxChannelInstance sizeChannelInstance;
    FxChannelInstance sizeRandChannelInstance;

};

struct GfxCmdDrawText {
    GfxCmdHeader header;
    float x;
    float y;
    FontHandle font;
    float xScale;
    float yScale;
    GfxColor color;
    int style;
    int cursorPos;
    char cursor;
    int maxChars;
    char text[3];
};

struct GfxCmdDrawTextInSpace {
    GfxCmdHeader header;
    vec3_t org;
    FontHandle font;
    vec3_t xPixelStep;
    vec3_t yPixelStep;
    GfxColor color;
    char text[4];
};

struct GfxEntity {
    refEntityType_t reType;
    int renderFxFlags;
    union { struct { FxMemMgr_Emitter * (*colorForDir)[2]; float sunVisibility; } dx7; vec3_t baseCoords; vec3_t origin; } lighting;
    vec3_t axis[3];
    float scale;
    vec3_t origin;
    vec3_t endpos;
    MaterialHandle customMaterial;
    byte materialRGBA[4];
    float materialTime;
    int materialSubimageIndex;
    vec2_t radius;
    float rotation;
    float minScreenRadius;
};

struct GfxMarkFragment {
    MaterialHandle markMaterial;
    short unsigned int lmapIndex;
    byte pointCount;
    int firstPoint;
};

struct MarkPoly {
    struct MarkPoly *prevMark;
    struct MarkPoly *nextMark;
    int lastFrameDrawn;
    vec3_t origin;
    float radius;
    MaterialHandle mtlHandle;
    short unsigned int lmapIndex;
    byte vertCount;
    GfxWorldVertex verts[9];
};

struct MaterialInfoObj_s {
    const char *name;
    const char *refImageName;
    short unsigned int hashIndex;
    short unsigned int sortedIndex;
    byte gameFlags;
    byte sortKey;
    byte textureAtlasRowCount;
    byte textureAtlasColumnCount;
    float maxDeformMove;
    byte deformFlags;
    byte usage;
    short unsigned int toolFlags;
    unsigned int locale;
    short unsigned int autoTexScaleWidth;
    short unsigned int autoTexScaleHeight;
    float tessSize;
    int surfaceFlags;
    int contents;
};

struct MaterialMemory {
    const Material *material;
    int memory;
};

struct MaterialObj {
    MaterialInfoObj info;
    int stateBits[2];
    short unsigned int textureCount;
    short unsigned int constantCount;
    MaterialTechniqueSet *techniqueSet;
    MaterialTextureDef *textures;
    MaterialConstantDefObj *constants;
};

struct MaterialPassDx9 {
    MaterialStateMap *stateMap;
    MaterialVertexDeclaration *vertexDecl;
    MaterialShader *vertexShader;
    MaterialShader *pixelShader;
    short unsigned int vertexArgCount;
    short unsigned int pixelArgCount;
    MaterialShaderArgument *vertexArgs;
    MaterialShaderArgument *pixelArgs;
};

struct MaterialShaderArgument {
    short unsigned int type;
    short unsigned int dest;
    union MaterialArgumentDef u;
};

struct MaterialPassDx7 {
    MaterialStateMap *stateMap;
    Bool gridLighting;
    Bool projectToInfinity;
    Bool ambientLighting;
    Bool objectiveGlow;
    Bool fogToBlack;
    byte genTexCoords[2];
    MaterialShaderArgument samplers[2];
    int colorStageBits[8];
    int alphaStageBits[8];
};

union MaterialPassArray {
    MaterialPassDx7 dx7[1];
    MaterialPassDx9 dx9[1];
};

union MaterialShaderPtr {
    GfxVertexShader *vs;
    GfxPixelShader *ps;
};

struct MaterialShader {
    const char *name;
    void (*program)();
    short unsigned int programLen;
    byte shaderType;
    byte shaderVersion;
    union MaterialShaderPtr u;
};

struct MaterialStateMap {
    const char *name;
    const MaterialStateMapRuleSet * ruleSet[11];
};

struct MaterialStateMapRule {
    int stateBitsMask[2];
    int stateBitsValue[2];
    int stateBitsSet[2];
    int stateBitsClear[2];
};

struct MaterialStateMapRuleSet {
    int ruleCount;
    MaterialStateMapRule rules[1];
};

struct MaterialStreamRouting {
    MaterialStreamSource source;
    MaterialStreamDest dest;
};

struct MaterialTechnique {
    const char *name;
    short unsigned int flags;
    short unsigned int passCount;
    union MaterialPassArray passArray;
};

struct MaterialTechniqueSet {
    const char *name;
    MaterialTechnique * techniques[34];
};

union MaterialTextureDefInfo {
    GfxImage *image;
    MaterialWaterDef *water;
};

struct MaterialTextureDef {
    const char *name;
    GfxSamplerState samplerState;
    TextureSemantic semantic;
    byte unused_0;
    byte unused_1;
    union MaterialTextureDefInfo u;
};

struct MaterialVertexDeclaration {
    MaterialStreamRouting *data;
    int streamCount;
    void (*decl[4])();
};


struct MaterialWaterDef {
    int textureWidth;
    float horizontalWorldLength;
    float verticalWorldLength;
    float amplitude;
    float windSpeed;
    vec2_t windDirection;
    water_t *map;
};

struct MemoryFile {
    byte *buffer;
    int bufferSize;
    int bytesUsed;
    Bool errorOnOverflow;
    Bool memoryOverflow;
    jpeg_component_info (*archiveProc)();
};

struct MemoryNode {
    short unsigned int prev;
    short unsigned int next;
    unsigned int padding;
};

struct MenuList {
    int menuCount;
    menuDef_t * *menus;
};

struct MessageLine {
    short int textBuffer[78];
    int startTime;
    int endTime;
};

struct MessageWindow {
    MessageLine *lines;
    int current_line;
    int count;
    int padding;
    int scrolltime;
    int fadein;
    int fadeout;
};

struct MessageBuffer {
    MessageWindow gamemsg;
    MessageLine gamemsg_lines[8];
    MessageWindow boldgamemsg;
    MessageLine boldgamemsg_lines[8];
    MessageWindow subtitle;
    MessageLine subtitle_lines[8];
    MessageWindow minicon;
    MessageLine minicon_lines[100];
};

struct Console {
    qboolean initialized;
    short int textBuffer[65536];
    int currentLine;
    int lineOffset;
    int display;
    print_msg_type_t prevType;
    int linewidth;
    int totallines;
    Bool outputVisible;
    vec2_t screenMin;
    vec2_t screenMax;
    MessageBuffer *messageBuffer;
    MessageBuffer messageBufferArray[1];
    vec4_t color;
};

struct MixerDistanceParams {
    Float32 mReferenceDistance;
    Float32 mMaxDistance;
    Float32 mMaxAttenuation;
};

struct MssLocal {
    HDIGDRIVER driver_2D;
    HPROVIDER provider_3D;
    HSAMPLE handle_2D[8];
    void * handle_3D[32];
    HSTREAM handle_stream[13];
    struct { HSAMPLE handle; int rate; int width; int channels; byte *buf; byte bufReady[32]; int bufSize; int bufUsed; int readBuf; int writeBuf; double sampleTime; double bufRate; } raw;
};

struct MtlStateMapBitGroup {
    const char *name;
    const MtlStateMapBitName *bitNames;
    int stateBitsMask[2];
};

struct MtlStateMapBitName {
    const char *name;
    int bits;
};

struct MtlTextureFunctionDx7 {
    const char *name;
    int enumerant;
    int argCount;
    MtlTextureFunctionValidDx7 valid;
};

struct NetField {
    char *name;
    int offset;
    int bits;
};

struct NumVersion {
    UInt8 nonRelRev;
    UInt8 stage;
    UInt8 minorAndBugRev;
    UInt8 majorRev;
};

struct ObjectInfo {
    short unsigned int refCount;
    union { short unsigned int size; short unsigned int entnum; short unsigned int nextEntId; short unsigned int self; } u;
};

struct OpaqueContextRef {
    AGLContext mContext;
    AGLDrawable mDrawable;
    GLint mRendererID;
    Boolean mDoubleBuffered;
};

struct OpcodeLookup {
    const char *codePos;
    unsigned int sourcePosIndex;
    unsigned int sourcePosCount;
    int profileTime;
    int profileUsage;
};

struct OpenGLStateInfoType {
    GLenum PName;
    OGLSITPTypes PType;
    char *SName;
};

struct OrientedParticle {

    char _base[0x1C4];
    vec3_t normal;
};

struct OutdoorGlob {
    vec3_t bbox[2];
    vec3_t scale;
    vec3_t invScale;
    vec3_t add;
    byte *pic;
};

struct ParameterType {
    UINT32 NeedsValidation;
    UINT32 v[4];
};

struct Particle {
    struct Effect base;

    int subclassField0;
    int subclassField1;
    int subclassField2;
    float displayAxis[3][3];
    float gravity;
    float windModifier;
    int _unk_0xfc;
    float elasticity;
    byte nonUniformScale;
    byte _pad_0x105[3];
    int startFrame;
    float frameRate;
    int sequenceLoopMode;
    int sequenceLoopTimes;
    float blendWeight[5];
    float velocityWeightX;
    float velocityWeightY;
    float velocityWeightZ;
    float velocity2WeightX;
    float velocity2WeightY;
    float velocity2WeightZ;
    FxChannelInstance colorChannelInstance;
    FxChannelInstance colorRandChannelInstance;
    FxChannelInstance alphaChannelInstance;
    FxChannelInstance alphaRandChannelInstance;
    FxChannelInstance sizeChannelInstance;
    FxChannelInstance sizeRandChannelInstance;
    FxChannelInstance size2ChannelInstance;
    FxChannelInstance size2RandChannelInstance;
    FxChannelInstance rotationDeltaChannelInstance;
    FxChannelInstance rotationDeltaRandChannelInstance;
    FxChannelInstance velocityXChannelInstance;
    FxChannelInstance velocityYChannelInstance;
    FxChannelInstance velocityZChannelInstance;
    FxChannelInstance velocityXRandChannelInstance;
    FxChannelInstance velocityYRandChannelInstance;
    FxChannelInstance velocityZRandChannelInstance;
    FxChannelInstance velocity2XChannelInstance;
    FxChannelInstance velocity2YChannelInstance;
    FxChannelInstance velocity2ZChannelInstance;
    FxChannelInstance velocity2XRandChannelInstance;
    FxChannelInstance velocity2YRandChannelInstance;
    FxChannelInstance velocity2ZRandChannelInstance;

};

struct Cloud {

    struct Particle base;
    vec3_t randomDirection;
    char _gap[4];
    Bool useLengthFlag;
    char _pad[3];
    float randomLengthBlend;
    FxChannelInstance heightChannel;
    FxChannelInstance heightRandChannel;

};

struct PassOptionDx7 {
    const char *name;
    size_t valueOffset;
};

struct Picmip {
    byte platform[2];
};

struct GfxImage {
    int mapType;
    union GfxTexture texture;
    struct Picmip picmip;
    TextureSemantic semantic;
    Bool noPicmip;
    byte track;
    struct CardMemory cardMemory;
    short unsigned int width;
    short unsigned int height;
    short unsigned int depth;
    GfxImageCategory category;
    const char *name;
};

struct PixMap {
    Ptr baseAddr;
    short int rowBytes;
    MacRect bounds;
    short int pmVersion;
    short int packType;
    long int packSize;
    Fixed hRes;
    Fixed vRes;
    short int pixelType;
    short int pixelSize;
    short int cmpCount;
    short int cmpSize;
    OSType pixelFormat;
    CTabHandle pmTable;
    short unsigned int *pmExt;
};

struct PlayerProfileStatus {
    int sortDir;
    int displayProfile[64];
};

struct Point {
    short int v;
    short int h;
};

struct Cursor {
    Bits16 data;
    Bits16 mask;
    struct Point hotSpot;
};

struct DInfo {
    struct MacRect frRect;
    UInt16 frFlags;
    Point frLocation;
    SInt16 frView;
};

struct DXInfo {
    Point frScroll;
    SInt32 frOpenChain;
    SInt8 frScript;
    SInt8 frXFlags;
    SInt16 frComment;
    SInt32 frPutAway;
};

struct EventRecord {
    EventKind what;
    UInt32 message;
    UInt32 when;
    Point where;
    EventModifiers modifiers;
};

struct FInfo {
    OSType fdType;
    OSType fdCreator;
    UInt16 fdFlags;
    struct Point fdLocation;
    SInt16 fdFldr;
};

struct PointLightPartition {
    const GfxLight *light;
    int firstDrawSurf;
    int drawSurfCount;
};

struct PortalHeapNode {
    GfxPortal *portal;
    float dist;
};

struct PrecacheEntry {
    scr_string_t filename;
    Bool include;
    unsigned int sourcePos;
    struct PrecacheEntry *next;   /* links per-file precache arrays; was int -> truncated the pointer on x64 */
};

struct ProcessSerialNumber {
    UInt32 highLongOfPSN;
    UInt32 lowLongOfPSN;
};

struct ProcessInfoRec {
    long unsigned int processInfoLength;
    StringPtr processName;
    ProcessSerialNumber processNumber;
    long unsigned int processType;
    OSType processSignature;
    long unsigned int processMode;
    Ptr processLocation;
    long unsigned int processSize;
    long unsigned int processFreeMem;
    ProcessSerialNumber processLauncher;
    long unsigned int processLaunchDate;
    long unsigned int processActiveTime;
    FSSpecPtr processAppSpec;
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

struct QElem {
    int qLink;
    short int qType;
    short int qData[1];
};

struct AccessParam {
    QElemPtr qLink;
    short int qType;
    short int ioTrap;
    Ptr ioCmdAddr;
    IOCompletionUPP ioCompletion;
    volatile OSErr ioResult;
    StringPtr ioNamePtr;
    short int ioVRefNum;
    short int ioRefNum;
    short int ioDenyModes;
    short int filler4;
    SInt8 filler5;
    SInt8 ioACUser;
    long int filler6;
    long int ioACOwnerID;
    long int ioACGroupID;
    long int ioACAccess;
    long int ioDirID;
};

struct CSParam {
    QElemPtr qLink;
    short int qType;
    short int ioTrap;
    Ptr ioCmdAddr;
    IOCompletionUPP ioCompletion;
    volatile OSErr ioResult;
    StringPtr ioNamePtr;
    short int ioVRefNum;
    FSSpecPtr ioMatchPtr;
    long int ioReqMatchCount;
    long int ioActMatchCount;
    long int ioSearchBits;
    CInfoPBPtr ioSearchInfo1;
    CInfoPBPtr ioSearchInfo2;
    long int ioSearchTime;
    struct CatPositionRec ioCatPosition;
    Ptr ioOptBuffer;
    long int ioOptBufSize;
};

struct CopyParam {
    QElemPtr qLink;
    short int qType;
    short int ioTrap;
    Ptr ioCmdAddr;
    IOCompletionUPP ioCompletion;
    volatile OSErr ioResult;
    StringPtr ioNamePtr;
    short int ioVRefNum;
    short int ioDstVRefNum;
    short int filler8;
    StringPtr ioNewName;
    StringPtr ioCopyName;
    long int ioNewDirID;
    long int filler14;
    long int filler15;
    long int ioDirID;
};

struct DirInfo {
    QElemPtr qLink;
    short int qType;
    short int ioTrap;
    Ptr ioCmdAddr;
    IOCompletionUPP ioCompletion;
    volatile OSErr ioResult;
    StringPtr ioNamePtr;
    short int ioVRefNum;
    short int ioFRefNum;
    SInt8 ioFVersNum;
    SInt8 filler1;
    short int ioFDirIndex;
    SInt8 ioFlAttrib;
    SInt8 ioACUser;
    struct DInfo ioDrUsrWds;
    long int ioDrDirID;
    short unsigned int ioDrNmFls;
    short int filler3[9];
    long unsigned int ioDrCrDat;
    long unsigned int ioDrMdDat;
    long unsigned int ioDrBkDat;
    struct DXInfo ioDrFndrInfo;
    long int ioDrParID;
};

struct FIDParam {
    QElemPtr qLink;
    short int qType;
    short int ioTrap;
    Ptr ioCmdAddr;
    IOCompletionUPP ioCompletion;
    volatile OSErr ioResult;
    StringPtr ioNamePtr;
    short int ioVRefNum;
    long int filler14;
    StringPtr ioDestNamePtr;
    long int filler15;
    long int ioDestDirID;
    long int filler16;
    long int filler17;
    long int ioSrcDirID;
    short int filler18;
    long int ioFileID;
};

struct ForeignPrivParam {
    QElemPtr qLink;
    short int qType;
    short int ioTrap;
    Ptr ioCmdAddr;
    IOCompletionUPP ioCompletion;
    volatile OSErr ioResult;
    StringPtr ioNamePtr;
    short int ioVRefNum;
    long int ioFiller21;
    long int ioFiller22;
    Ptr ioForeignPrivBuffer;
    long int ioForeignPrivActCount;
    long int ioForeignPrivReqCount;
    long int ioFiller23;
    long int ioForeignPrivDirID;
    long int ioForeignPrivInfo1;
    long int ioForeignPrivInfo2;
    long int ioForeignPrivInfo3;
    long int ioForeignPrivInfo4;
};

struct HFileInfo {
    QElemPtr qLink;
    short int qType;
    short int ioTrap;
    Ptr ioCmdAddr;
    IOCompletionUPP ioCompletion;
    volatile OSErr ioResult;
    StringPtr ioNamePtr;
    short int ioVRefNum;
    short int ioFRefNum;
    SInt8 ioFVersNum;
    SInt8 filler1;
    short int ioFDirIndex;
    SInt8 ioFlAttrib;
    SInt8 ioACUser;
    struct FInfo ioFlFndrInfo;
    long int ioDirID;
    short unsigned int ioFlStBlk;
    long int ioFlLgLen;
    long int ioFlPyLen;
    short unsigned int ioFlRStBlk;
    long int ioFlRLgLen;
    long int ioFlRPyLen;
    long unsigned int ioFlCrDat;
    long unsigned int ioFlMdDat;
    long unsigned int ioFlBkDat;
    struct FXInfo ioFlXFndrInfo;
    long int ioFlParID;
    long int ioFlClpSiz;
};

union CInfoPBRec {
    struct HFileInfo hFileInfo;
    struct DirInfo dirInfo;
};

struct HFileParam {
    QElemPtr qLink;
    short int qType;
    short int ioTrap;
    Ptr ioCmdAddr;
    IOCompletionUPP ioCompletion;
    volatile OSErr ioResult;
    StringPtr ioNamePtr;
    short int ioVRefNum;
    short int ioFRefNum;
    SInt8 ioFVersNum;
    SInt8 filler1;
    short int ioFDirIndex;
    SInt8 ioFlAttrib;
    SInt8 ioFlVersNum;
    struct FInfo ioFlFndrInfo;
    long int ioDirID;
    short unsigned int ioFlStBlk;
    long int ioFlLgLen;
    long int ioFlPyLen;
    short unsigned int ioFlRStBlk;
    long int ioFlRLgLen;
    long int ioFlRPyLen;
    long unsigned int ioFlCrDat;
    long unsigned int ioFlMdDat;
};

struct HIOParam {
    QElemPtr qLink;
    short int qType;
    short int ioTrap;
    Ptr ioCmdAddr;
    IOCompletionUPP ioCompletion;
    volatile OSErr ioResult;
    StringPtr ioNamePtr;
    short int ioVRefNum;
    short int ioRefNum;
    SInt8 ioVersNum;
    SInt8 ioPermssn;
    Ptr ioMisc;
    Ptr ioBuffer;
    long int ioReqCount;
    long int ioActCount;
    short int ioPosMode;
    long int ioPosOffset;
};

struct HVolumeParam {
    QElemPtr qLink;
    short int qType;
    short int ioTrap;
    Ptr ioCmdAddr;
    IOCompletionUPP ioCompletion;
    volatile OSErr ioResult;
    StringPtr ioNamePtr;
    short int ioVRefNum;
    long int filler2;
    short int ioVolIndex;
    long unsigned int ioVCrDate;
    long unsigned int ioVLsMod;
    short int ioVAtrb;
    short unsigned int ioVNmFls;
    short unsigned int ioVBitMap;
    short unsigned int ioAllocPtr;
    short unsigned int ioVNmAlBlks;
    long unsigned int ioVAlBlkSiz;
    long unsigned int ioVClpSiz;
    short unsigned int ioAlBlSt;
    long unsigned int ioVNxtCNID;
    short unsigned int ioVFrBlk;
    short unsigned int ioVSigWord;
    short int ioVDrvInfo;
    short int ioVDRefNum;
    short int ioVFSID;
    long unsigned int ioVBkUp;
    short int ioVSeqNum;
    long unsigned int ioVWrCnt;
    long unsigned int ioVFilCnt;
    long unsigned int ioVDirCnt;
    long int ioVFndrInfo[8];
};

struct ObjParam {
    QElemPtr qLink;
    short int qType;
    short int ioTrap;
    Ptr ioCmdAddr;
    IOCompletionUPP ioCompletion;
    volatile OSErr ioResult;
    StringPtr ioNamePtr;
    short int ioVRefNum;
    short int filler7;
    short int ioObjType;
    StringPtr ioObjNamePtr;
    long int ioObjID;
};

struct RTL_CRITICAL_SECTION {
    long int __sig;
    char __opaque[40];
};

struct RefString {
    union { struct { unsigned int byteLen; unsigned int user; unsigned int refCount; }; };
    char str[1];
};

struct SBDecState {
    const SpeexMode *mode;
    int *st_low;
    int full_frame_size;
    int frame_size;
    int subframeSize;
    int nbSubframes;
    int lpcSize;
    int first;
    int sampling_rate;
    int lpc_enh_enabled;
    char *stack;
    spx_sig_t *x0d;
    spx_sig_t *x1d;
    spx_sig_t *high;
    spx_sig_t *y0;
    spx_sig_t *y1;
    spx_word32_t *g0_mem;
    spx_word32_t *g1_mem;
    spx_sig_t *exc;
    void * *qlsp;
    void * *old_qlsp;
    void * *interp_qlsp;
    spx_coef_t *interp_qlpc;
    spx_mem_t *mem_sp;
    spx_word32_t *pi_gain;
    int encode_submode;
    const const SpeexSubmode * *submodes;
    int submodeID;
};

struct SBEncState {
    const SpeexMode *mode;
    int *st_low;
    int full_frame_size;
    int frame_size;
    int subframeSize;
    int nbSubframes;
    int windowSize;
    int lpcSize;
    int bufSize;
    int first;
    float lag_factor;
    float lpc_floor;
    spx_word16_t gamma1;
    spx_word16_t gamma2;
    char *stack;
    spx_sig_t *x0d;
    spx_sig_t *x1d;
    spx_sig_t *high;
    spx_sig_t *y0;
    spx_sig_t *y1;
    char * (*h0_mem)[16];
    char * (*h1_mem)[16];
    spx_word32_t *g0_mem;
    spx_word32_t *g1_mem;
    spx_sig_t *excBuf;
    spx_sig_t *exc;
    spx_sig_t *buf;
    spx_sig_t *res;
    spx_sig_t *sw;
    spx_sig_t *target;
    char * (*window)[16];
    char * (*lagWindow)[16];
    char * (*autocorr)[16];
    spx_coef_t *lpc;
    void * *lsp;
    void * *qlsp;
    void * *old_lsp;
    void * *old_qlsp;
    void * *interp_lsp;
    void * *interp_qlsp;
    spx_coef_t *interp_lpc;
    spx_coef_t *interp_qlpc;
    spx_coef_t *bw_lpc1;
    spx_coef_t *bw_lpc2;
    spx_mem_t *mem_sp;
    spx_mem_t *mem_sp2;
    spx_mem_t *mem_sw;
    spx_word32_t *pi_gain;
    float vbr_quality;
    int vbr_enabled;
    int abr_enabled;
    float abr_drift;
    float abr_drift2;
    float abr_count;
    int vad_enabled;
    float relative_quality;
    int encode_submode;
    const const SpeexSubmode * *submodes;
    int submodeID;
    int submodeSelect;
    int complexity;
    int sampling_rate;
};

struct SMPTETime {
    SInt16 mSubframes;
    SInt16 mSubframeDivisor;
    UInt32 mCounter;
    UInt32 mType;
    UInt32 mFlags;
    SInt16 mHours;
    SInt16 mMinutes;
    SInt16 mSeconds;
    SInt16 mFrames;
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

struct SProcRec {
    Handle nxtSrch;
    ColorSearchUPP srchProc;
};

struct GDevice {
    short int gdRefNum;
    short int gdID;
    short int gdType;
    ITabHandle gdITable;
    short int gdResPref;
    SProcHndl gdSearchProc;
    CProcHndl gdCompProc;
    short int gdFlags;
    PixMapHandle gdPMap;
    long int gdRefCon;
    int gdNextGD;
    struct MacRect gdRect;
    long int gdMode;
    short int gdCCBytes;
    short int gdCCDepth;
    Handle gdCCXData;
    Handle gdCCXMask;
    Handle gdExt;
};

struct SaveSourceBufferInfo {
    char *sourceBuf;
    int len;
};

struct ScheduledEffect {
    const EffectTemplate *mFx;
    int mPrimIndex;
    int mStartTime;
    FxBoltInfo mBolt;
    vec3_t mOrigin;
    vec3_t mAxis[3];
    int mSeed;
    int mIndexInBatch;
    int mScheduledNext;
};

struct Scr_SourcePos_t {
    int bufferIndex;
    int lineNum;
    unsigned int sourcePos;
};

struct ScreenPlacementGlob {
    vec2_t scaleVirtualToReal;
    vec2_t scaleVirtualToFull;
    vec2_t scaleRealToVirtual;
    float virtualScreenOffsetX;
    vec2_t virtualViewableMin;
    vec2_t virtualViewableMax;
    vec2_t realViewportSize;
    vec2_t realViewableMin;
    vec2_t realViewableMax;
    float subScreenLeft;
};

struct ShaderConstantRouting {
    const char *name;
    byte firstElem;
    byte elemCount;
    D3DXSHADER_CONSTANTINFO *constantInfo;
    LPCVOID (*typeInfo)[512];
};

struct ShadowCandidate {
    int entIndex;
    float weight;
};

struct ShadowCookieGlob {
    float weightCap;
    int lastTime;
};

struct SkinBuffers {
    DObjSkelMat matArrayBuf[128];
    union { GfxSModelCachedVertexDx7 dx7[512]; GfxSModelCachedVertex dx9[512]; } verts;
};

struct SkinRigidXModelCmd {
    surfaceType_t *surfs;
    int surfCount;
    GfxEntity *e;
    DObjAnimMat mat;
};

struct SkinStaticModelCachedCmd {
    GfxStaticModelSurfaceCached *cached;
    int smodelIndex;
    const Material *material;
};

struct SkinXModelCmd {
    surfaceType_t *surfs;
    GfxEntity *e;
    const DObjAnimMat *mat;
    int surfacePartBits[4];
    byte surfCount;
    byte boneCount;
    byte matOffset[64];
};

struct SndCurve {
    const char *filename;
    int knotCount;
    vec2_t knots[8];
};

struct SortedCluster {
    int clusterId;
    float distSq;
};

struct SortedEffect {
    Effect *effect;
    float distSq;
};

struct SoundDescription {
    long int descSize;
    long int dataFormat;
    long int resvd1;
    short int resvd2;
    short int dataRefIndex;
    short int version;
    short int revlevel;
    long int vendor;
    short int numChannels;
    short int sampleSize;
    short int compressionID;
    short int packetSize;
    UnsignedFixed sampleRate;
};

struct SoundFile {
    const char *soundName;
    struct MssSound *fileMem;
    byte isStreamFound;
    snd_alias_type_t type;
};

struct SoundFileInfo {
    int count;
    SoundFile *files;
};

struct SourceBufferInfo {
    const char *codePos;
    char *buf;
    const char *sourceBuf;
    int len;
    int sortedIndex;
    Bool archive;
};

struct SourceLookup {
    unsigned int sourcePos;
    int type;
};

struct SpawnVar {
    Bool spawnVarsValid;
    int numSpawnVars;
    char * spawnVars[64][2];
    int numSpawnVarChars;
    char spawnVarChars[2048];
};

struct SpeexBits {
    char *bytes;
    int nbBits;
    int bytePtr;
    int bitPtr;
    int owner;
    int overflow;
    int buf_size;
    int reserved1;
    void *reserved2;
};

#ifndef MAX_BYTES_PER_FRAME
#define MAX_BYTES_PER_FRAME 2000
#endif

struct SpeexCallback {
    int callback_id;
    speex_callback_func func;
    double *data;
    double *reserved1;
    int reserved2;
};

struct DecState {
    const SpeexMode *mode;
    int first;
    int count_lost;
    int frameSize;
    int subframeSize;
    int nbSubframes;
    int lpcSize;
    int min_pitch;
    int max_pitch;
    int sampling_rate;
    spx_word16_t last_ol_gain;
    char *stack;
    spx_sig_t *inBuf;
    spx_sig_t *frame;
    spx_sig_t *excBuf;
    spx_sig_t *exc;
    spx_sig_t *innov;
    spx_lsp_t *qlsp;
    spx_lsp_t *old_qlsp;
    spx_lsp_t *interp_qlsp;
    spx_coef_t *interp_qlpc;
    spx_mem_t *mem_sp;
    spx_word32_t *pi_gain;
    int last_pitch;
    spx_word16_t last_pitch_gain;
    spx_word16_t pitch_gain_buf[3];
    int pitch_gain_buf_idx;
    int encode_submode;
    const const SpeexSubmode * *submodes;
    int submodeID;
    int lpc_enh_enabled;
    CombFilterMem *comb_mem;
    SpeexCallback speex_callbacks[16];
    struct SpeexCallback user_callback;
    float voc_m1;
    float voc_m2;
    float voc_mean;
    int voc_offset;
    int dtx_enabled;
};

struct SpeexMode {
    const void *mode;
    mode_query_func query;
    const char *modeName;
    int modeID;
    int bitstream_version;
    encoder_init_func enc_init;
    encoder_destroy_func enc_destroy;
    encode_func enc;
    decoder_init_func dec_init;
    decoder_destroy_func dec_destroy;
    decode_func dec;
    encoder_ctl_func enc_ctl;
    decoder_ctl_func dec_ctl;
};

struct SpeexNBMode {
    int frameSize;
    int subframeSize;
    int lpcSize;
    int pitchStart;
    int pitchEnd;
    spx_word16_t gamma1;
    spx_word16_t gamma2;
    float lag_factor;
    float lpc_floor;
    const SpeexSubmode * submodes[16];
    int defaultSubmode;
    int quality_map[11];
};

struct SpeexSBMode {
    const SpeexMode *nb_mode;
    int frameSize;
    int subframeSize;
    int lpcSize;
    int bufSize;
    spx_word16_t gamma1;
    spx_word16_t gamma2;
    float lag_factor;
    float lpc_floor;
    float folding_gain;
    const SpeexSubmode * submodes[8];
    int defaultSubmode;
    int low_quality_map[11];
    int quality_map[11];
    const float (*vbr_thresh)[11];
    int nb_modes;
};

struct SpeexSubmode {
    int lbr_pitch;
    int forced_pitch_gain;
    int have_subframe_gain;
    int double_codebook;
    lsp_quant_func lsp_quant;
    lsp_unquant_func lsp_unquant;
    ltp_quant_func ltp_quant;
    ltp_unquant_func ltp_unquant;
    const int *ltp_params;
    innovation_quant_func innovation_quant;
    innovation_unquant_func innovation_unquant;
    const int *innovation_params;
    spx_word16_t lpc_enh_k1;
    spx_word16_t lpc_enh_k2;
    spx_word16_t lpc_enh_k3;
    spx_word16_t comb_gain;
    int bits_per_frame;
};

struct StColorConverter {
    int _placeholder;
};

struct StGWorldState {
    int _placeholder;
};

struct StMutexLock {
    int _placeholder;
};

struct StPortState {
    int _placeholder;
};

struct StSetDirectory {
    int _placeholder;
};

struct StShowCursor {
    int _placeholder;
};

struct StThreadLock {
    int _placeholder;
};

struct StdConverterABGR {
    int _placeholder;
};

struct StdConverterARGB {
    int _placeholder;
};

struct SunFlareDynamic {
    float flareIntensity;
    float currentBlind;
    float currentGlare;
    int lastTime;
    Bool error;
    float cumulVisibility;
    float lastVisibility;
    float lastDot;
    int hitNum;
    IDirect3DQuery9 * sunQuery[2];
    Bool sunQueryIssued[2];
};

struct SunLightParseParams {
    char name[64];
    float ambientScale;
    vec3_t ambientColor;
    float diffuseFraction;
    float sunLight;
    vec3_t sunColor;
    vec3_t diffuseColor;
    Bool diffuseColorHasBeenSet;
    vec3_t angles;
};

struct SysInfo {
    double cpuGHz;
    int sysMB;
    Bool SSE;
    char gpuDescription[512];
};

union TMediaElement {
    struct XModel *model;
    struct Material *material;
    union EffectTemplateLoadPtr u;
    void *data;
};

struct TMediaList {
    TMediaElement *elements;
    short unsigned int size;
    short unsigned int maxSize;
};

struct MediaHandles {
    struct TMediaList mMediaList;
};

struct PrimitiveTemplate {
    char mName[32];
    char mMaterialImpact[32];
    PrimType mType;
    int mParentPrimIndex;
    struct FxRange mSpawnDelay;
    FxRange mSpawnCount;
    FxRange mLife;
    FxRange mSpawnRange;
    struct MediaHandles mMediaHandles;
    MediaHandles mImpactFxHandles;
    MediaHandles mDeathFxHandles;
    MediaHandles mEmitterFxHandles;
    MediaHandles mPlayFxHandles;
    int mAttributeFlags;
    int mSpawnFlags;
    int mGroupFlags;
    Bool mNonUniformScale;
    Bool useLength;
    vec3_t mMin;
    vec3_t mMax;
    FxRange mOrigin1X;
    FxRange mOrigin1Y;
    FxRange mOrigin1Z;
    FxRange mOrigin2X;
    FxRange mOrigin2Y;
    FxRange mOrigin2Z;
    FxRange mRadius;
    FxRange mHeight;
    FxRange mWindModifier;
    FxChannel mFxChannels[24];
    FxRange mRotation;
    FxRange mAngle1;
    FxRange mAngle2;
    FxRange mAngle3;
    FxRange mAngle1Delta;
    FxRange mAngle2Delta;
    FxRange mAngle3Delta;
    FxRange mGravity;
    FxRange mDensity;
    FxRange mVariance;
    FxRange mTexCoordS;
    FxRange mTexCoordT;
    FxRange mElasticity;
    StartFrameMode mSequenceStartFrameMode;
    int mSequenceFixedFrameValue;
    PlayRateMode mSequencePlayRateMode;
    float mSequenceFixedFpsValue;
    LoopMode mSequenceLoopMode;
    int mSequenceLoopTimes;
    float spawnFrustumCullRadius;
};

struct TXNATSUIFeatures {
    ItemCount featureCount;
    ATSUFontFeatureType *featureTypes;
    ATSUFontFeatureSelector *featureSelectors;
};

struct TXNATSUIVariations {
    ItemCount variationCount;
    ATSUFontVariationAxis *variationAxis;
    ATSUFontVariationValue *variationValues;
};

union TXNAttributeData {
    inflate_blocks_statef *dataPtr;
    UInt32 dataValue;
    TXNATSUIFeatures *atsuFeatures;
    TXNATSUIVariations *atsuVariations;
    CFURLRef urlReference;
};

struct TXNTypeAttributes {
    TXNTypeRunAttributes tag;
    ByteCount size;
    union TXNAttributeData data;
};

struct Tail {

    struct Particle base;
    float endpoint[3];
    float tailLength;
    float lengthBlendFactor;
    FxChannelInstance lengthChannelInstance;
    FxChannelInstance lengthRandChannelInstance;

};

struct TestLod {
    Bool enabled;
    float dist;
};

struct TextPool {
    char *data;
    struct TextPool *next;
    int capacity;
    int used;
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

struct ThreadDebugInfo {
    const char * pos[32];
    int posSize;
    float varUsage;
    float endonUsage;
};

struct TraceCheckCount {
    int global;
    int *edges;
    int *verts;
    short unsigned int *partitions;
};

struct TraceExtents {
    vec3_t start;
    vec3_t end;
    vec3_t invDelta;
};

struct TraceThreadInfo {
    struct TraceCheckCount checkcount;
    cbrush_t *box_brush;
    cmodel_t *box_model;
};

struct Tuple {
    UINT16 v[3];
    UINT32 Score;
};

#if !COD2_FEATURE_SCRIPT_DEBUGGER
struct UI_Component {
    int _vptr$UI_Component;
    vec2_t size;
    int selectionParent;
};
#endif

struct UI_Component_data_t {
    float screenWidth;
    float screenHeight;
    float charWidth;
    float charHeight;
    float scrollBarSize;
    MaterialHandle cursor;
    vec2_t cursorPos;
    qboolean hideCursor;
    MaterialHandle filledCircle;
    int consoleReason;
    char findText[128];
};

#if !COD2_FEATURE_SCRIPT_DEBUGGER
struct UI_LinesComponent {
    int selectedLine;
    Bool focusOnSelectedLine;
    Bool focusOnSelectedLineUser;
    int numLines;
    vec2_t pos;
};

struct UI_ScrollPane {
    UI_LinesComponent *comp;
    Bool forceHorScoll;
    vec2_t mouseHeldScale;
    vec2_t mouseHeldPos;
    vec2_t mouseHeldCompPos;
    Bool mouseWasDown[2];
};

struct UI_VerticalDivider {
    UI_ScrollPane *topComp;
    UI_ScrollPane *bottomComp;
    float posY;
};
#endif

struct UTCDateTime {
    UInt16 highSeconds;
    UInt32 lowSeconds;
    UInt16 fraction;
};

struct FFItem {
    struct FSRef ref;
    char name[260];
    UTCDateTime modDate;
    UInt16 flags;
};

struct FSCatalogInfo {
    UInt16 nodeFlags;
    FSVolumeRefNum volume;
    UInt32 parentDirID;
    UInt32 nodeID;
    UInt8 sharingFlags;
    UInt8 userPrivileges;
    UInt8 reserved1;
    UInt8 reserved2;
    struct UTCDateTime createDate;
    UTCDateTime contentModDate;
    UTCDateTime attributeModDate;
    UTCDateTime accessDate;
    UTCDateTime backupDate;
    UInt32 permissions[4];
    UInt8 finderInfo[16];
    UInt8 extFinderInfo[16];
    UInt64 dataLogicalSize;
    UInt64 dataPhysicalSize;
    UInt64 rsrcLogicalSize;
    UInt64 rsrcPhysicalSize;
    UInt32 valence;
    TextEncoding textEncodingHint;
};

struct FSVolumeInfo {
    struct UTCDateTime createDate;
    UTCDateTime modifyDate;
    UTCDateTime backupDate;
    UTCDateTime checkedDate;
    UInt32 fileCount;
    UInt32 folderCount;
    UInt64 totalBytes;
    UInt64 freeBytes;
    UInt32 blockSize;
    UInt32 totalBlocks;
    UInt32 freeBlocks;
    UInt32 nextAllocation;
    UInt32 rsrcClumpSize;
    UInt32 dataClumpSize;
    UInt32 nextCatalogID;
    UInt8 finderInfo[32];
    UInt16 flags;
    UInt16 filesystemID;
    UInt16 signature;
    UInt16 driveNumber;
    short int driverRefNum;
};

struct RemoveDirectoryInfo {
    OSStatus error;
    ItemCount actualObjects;
    struct FSCatalogInfo catalogInfo;
};

struct UnsignedWide {
    UInt32 lo;
    UInt32 hi;
};

struct VBRState {
    float energy_alpha;
    float average_energy;
    float last_energy;
    float last_log_energy[5];
    float accum_sum;
    float last_pitch_coef;
    float soft_pitch;
    float last_quality;
    float noise_level;
    float noise_accum;
    float noise_accum_count;
    int consec_noise;
};

struct Variable {
    short unsigned int id;
    union { short unsigned int prev; short unsigned int prevSibling; } u;
};

struct VariableStackBuffer {
    const char *pos;
    short unsigned int size;
    short unsigned int bufLen;
    short unsigned int localId;
    byte time;
    char buf[1];
};

union VariableUnion {
    int intValue;
    float floatValue;
    unsigned int stringValue;
#if defined(__x86_64__) || defined(_M_X64)
    unsigned int vectorValue;
    unsigned int codePosValue;
#else
    const float *vectorValue;
    const char *codePosValue;
#endif
    unsigned int pointerValue;
#if defined(__x86_64__) || defined(_M_X64)
    unsigned int stackValue;
#else
    VariableStackBuffer *stackValue;
#endif
    unsigned int entityOffset;
};

extern unsigned char scrMemTreeGlob[];

extern void * const imp_scrVarPub;

#if defined(__x86_64__) || defined(_M_X64)

#define SCR_ARENA_PTR(off)    ((void *)(scrMemTreeGlob + (unsigned int)(off)))
#define SCR_ARENA_ENC(ptr)    ((unsigned int)((const unsigned char *)(const void *)(ptr) - scrMemTreeGlob))

#define SCR_PROGBUF_BASE()    (((struct scrVarPub_t *)imp_scrVarPub)->programBuffer)
#define SCR_CODEPOS_PTR(off)  ((const char *)((off) ? (SCR_PROGBUF_BASE() + (unsigned int)(off)) : (const char *)0))
#define SCR_CODEPOS_ENC(ptr)  ((unsigned int)((ptr) ? ((const char *)(ptr) - SCR_PROGBUF_BASE()) : 0))

#define SCR_VEC_TAG_PROG      0x80000000u
#define SCR_VEC_ENC_FROM(p) \
    ( ((const unsigned char *)(const void *)(p) >= scrMemTreeGlob && \
       (const unsigned char *)(const void *)(p) < scrMemTreeGlob + 0x80330) \
        ? SCR_ARENA_ENC(p) \
        : (SCR_VEC_TAG_PROG | SCR_CODEPOS_ENC(p)) )
#define SCR_VEC_PTR(u) \
    ((const float *)( ((u).vectorValue & SCR_VEC_TAG_PROG) \
        ? (SCR_PROGBUF_BASE() + ((u).vectorValue & ~SCR_VEC_TAG_PROG)) \
        : (const char *)SCR_ARENA_PTR((u).vectorValue) ))
#define SCR_VEC_ENC(p)        SCR_VEC_ENC_FROM(p)
#define SCR_STACK_PTR(u)      ((VariableStackBuffer *)SCR_ARENA_PTR((u).stackValue))
#define SCR_STACK_ENC(p)      SCR_ARENA_ENC(p)
#define SCR_CODEPOS_GET(u)    SCR_CODEPOS_PTR((u).codePosValue)
#define SCR_CODEPOS_SET(u,p)  ((u).codePosValue = SCR_CODEPOS_ENC(p))
#else
#define SCR_VEC_PTR(u)        ((u).vectorValue)
#define SCR_VEC_ENC(p)        ((const float *)(p))
#define SCR_STACK_PTR(u)      ((u).stackValue)
#define SCR_STACK_ENC(p)      ((VariableStackBuffer *)(p))
#define SCR_CODEPOS_GET(u)    ((u).codePosValue)
#define SCR_CODEPOS_SET(u,p)  ((u).codePosValue = (const char *)(p))
#endif

struct VariableValueInternal {
    struct Variable hash;
    union { short unsigned int next; union VariableUnion u; struct ObjectInfo o; } u;
    union { unsigned int status; unsigned int type; unsigned int name; unsigned int classnum; unsigned int notifyName; unsigned int waitTime; unsigned int parentLocalId; } w;
    union { short unsigned int next; short unsigned int index; } v;
    short unsigned int nextSibling;
};

struct VariableValue_s {
    union VariableUnion u;
    int type;
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

#ifdef _MSC_VER
#pragma pack(push, 1)
#endif
struct VoicePacket_t {
    byte talker;
    byte data[256];
    int dataSize;
} __attribute__((__packed__));
#ifdef _MSC_VER
#pragma pack(pop)
#endif

struct VolumeModGroup {
    char name[64];
    float value;
};

struct WDParam {
    QElemPtr qLink;
    short int qType;
    short int ioTrap;
    Ptr ioCmdAddr;
    IOCompletionUPP ioCompletion;
    volatile OSErr ioResult;
    StringPtr ioNamePtr;
    short int ioVRefNum;
    short int ioWDCreated;
    short int ioWDIndex;
    long int ioWDProcID;
    short int ioWDVRefNum;
    short int filler10;
    long int filler11;
    long int filler12;
    long int filler13;
    long int ioWDDirID;
};

union HParamBlockRec {
    struct HIOParam ioParam;
    struct HFileParam fileParam;
    struct HVolumeParam volumeParam;
    struct AccessParam accessParam;
    struct ObjParam objParam;
    struct CopyParam copyParam;
    struct WDParam wdParam;
    struct FIDParam fidParam;
    struct CSParam csParam;
    struct ForeignPrivParam foreignPrivParam;
};

struct WSAData {
    WORD wVersion;
    WORD wHighVersion;
    char szDescription[257];
    char szSystemStatus[129];
    short unsigned int iMaxSockets;
    short unsigned int iMaxUdpDg;
    char *lpVendorInfo;
};

struct WaterWritable {
    int frameCount;
};

struct WaveletDecode {
    short unsigned int value;
    short unsigned int bit;
    const byte *data;
    int width;
    int height;
    int channels;
    int bpp;
    int mipLevel;
    Bool dataInitialized;
};

struct WaveletHuffmanDecode {
    short int value;
    short int bits;
};

struct WeaponDef {
    const char *szInternalName;
    char *szDisplayName;
    char *szOverlayName;
    char *szGunXModel;
    char *szHandXModel;
    char * szXAnims[23];
    char *szModeName;
    int playerAnimType;
    weapType_t weapType;
    weapClass_t weapClass;
    weapSlot_t weapSlot;
    OffhandClass offhandClass;
    qboolean bSlotStackable;
    weapStance_t stance;
    char *szViewFlashEffect;
    char *szWorldFlashEffect;
    char *szPickupSound;
    char *szAmmoPickupSound;
    char *szProjectileSound;
    char *szPullbackSound;
    char *szFireSound;
    char *szFireSoundPlayer;
    char *szFireLoopSound;
    char *szFireLoopSoundPlayer;
    char *szFireStopSound;
    char *szFireStopSoundPlayer;
    char *szFireLastSound;
    char *szFireLastSoundPlayer;
    char *meleeSwipeSound;
    char *szRechamberSound;
    char *szRechamberSoundPlayer;
    char *szReloadSound;
    char *szReloadSoundPlayer;
    char *szReloadEmptySound;
    char *szReloadEmptySoundPlayer;
    char *szReloadStartSound;
    char *szReloadStartSoundPlayer;
    char *szReloadEndSound;
    char *szReloadEndSoundPlayer;
    char *szRaiseSound;
    char *szAltSwitchSound;
    char *szPutawaySound;
    char *szNoteTrackSoundA;
    char *szNoteTrackSoundB;
    char *szNoteTrackSoundC;
    char *szNoteTrackSoundD;
    char *szShellEjectEffect;
    char *szLastShotEjectEffect;
    char *szReticleCenter;
    char *szReticleSide;
    int iReticleCenterSize;
    int iReticleSideSize;
    int iReticleMinOfs;
    vec3_t vStandMove;
    vec3_t vStandRot;
    vec3_t vDuckedOfs;
    vec3_t vDuckedMove;
    vec3_t vDuckedRot;
    vec3_t vProneOfs;
    vec3_t vProneMove;
    vec3_t vProneRot;
    float fPosMoveRate;
    float fPosProneMoveRate;
    float fStandMoveMinSpeed;
    float fDuckedMoveMinSpeed;
    float fProneMoveMinSpeed;
    float fPosRotRate;
    float fPosProneRotRate;
    float fStandRotMinSpeed;
    float fDuckedRotMinSpeed;
    float fProneRotMinSpeed;
    char *szWorldModel;
    char *szHudIcon;
    char *szModeIcon;
    int iStartAmmo;
    char *szAmmoName;
    int iAmmoIndex;
    char *szClipName;
    int iClipIndex;
    int iMaxAmmo;
    int iClipSize;
    int shotCount;
    char *szSharedAmmoCapName;
    int iSharedAmmoCapIndex;
    int iSharedAmmoCap;
    int damage;
    int playerDamage;
    int iMeleeDamage;
    int iDamageType;
    int iFireDelay;
    int iMeleeDelay;
    int iFireTime;
    int iRechamberTime;
    int iRechamberBoltTime;
    int iHoldFireTime;
    int iMeleeTime;
    int iReloadTime;
    int iReloadEmptyTime;
    int iReloadAddTime;
    int iReloadStartTime;
    int iReloadStartAddTime;
    int iReloadEndTime;
    int iDropTime;
    int iRaiseTime;
    int iAltDropTime;
    int iAltRaiseTime;
    int quickDropTime;
    int quickRaiseTime;
    int iFuseTime;
    float autoAimRange;
    float slowdownAimRange;
    float slowdownAimRangeAds;
    float lockonAimRange;
    float lockonAimRangeAds;
    float enemyCrosshairRange;
    float fMoveSpeedScale;
    float fAdsZoomFov;
    float fAdsZoomInFrac;
    float fAdsZoomOutFrac;
    char *szOverlayMaterial;
    weapOverlayReticle_t overlayReticle;
    float overlayWidth;
    float overlayHeight;
    float fAdsBobFactor;
    float fAdsViewBobMult;
    float fHipSpreadStandMin;
    float fHipSpreadDuckedMin;
    float fHipSpreadProneMin;
    float hipSpreadStandMax;
    float hipSpreadDuckedMax;
    float hipSpreadProneMax;
    float fHipSpreadDecayRate;
    float fHipSpreadFireAdd;
    float fHipSpreadTurnAdd;
    float fHipSpreadMoveAdd;
    float fHipSpreadDuckedDecay;
    float fHipSpreadProneDecay;
    float fHipReticleSidePos;
    int iAdsTransInTime;
    int iAdsTransOutTime;
    float fAdsIdleAmount;
    float fHipIdleAmount;
    float adsIdleSpeed;
    float hipIdleSpeed;
    float fIdleCrouchFactor;
    float fIdleProneFactor;
    float fGunMaxPitch;
    float fGunMaxYaw;
    float swayMaxAngle;
    float swayLerpSpeed;
    float swayPitchScale;
    float swayYawScale;
    float swayHorizScale;
    float swayVertScale;
    float swayShellShockScale;
    float adsSwayMaxAngle;
    float adsSwayLerpSpeed;
    float adsSwayPitchScale;
    float adsSwayYawScale;
    float adsSwayHorizScale;
    float adsSwayVertScale;
    qboolean bRifleBullet;
    qboolean armorPiercing;
    qboolean bSemiAuto;
    qboolean bBoltAction;
    qboolean bADSPositionInfo;
    qboolean bRechamberWhileAds;
    float adsViewErrorMin;
    float adsViewErrorMax;
    qboolean bCookOffHold;
    qboolean bClipOnly;
    qboolean bWideListIcon;
    qboolean bADSFire;
    char *killIcon;
    qboolean wideKillIcon;
    qboolean flipKillIcon;
    qboolean bNoPartialReload;
    qboolean bSegmentedReload;
    int iReloadAmmoAdd;
    int iReloadStartAdd;
    char *szAltWeaponName;
    int iAltWeaponIndex;
    int iDropAmmoMin;
    int iDropAmmoMax;
    int iExplosionRadius;
    int iExplosionInnerDamage;
    int iExplosionOuterDamage;
    int iProjectileSpeed;
    int iProjectileSpeedUp;
    char *szProjectileModel;
    weapProjExposion_t projExplosion;
    char *szProjExplosionEffect;
    char *szProjExplosionSound;
    qboolean bProjImpactExplode;
    float parallelBounce[23];
    float perpendicularBounce[23];
    char *szProjTrailEffect;
    int iProjectileDLight;
    vec3_t vProjectileColor;
    float fAdsAimPitch;
    float fAdsCrosshairInFrac;
    float fAdsCrosshairOutFrac;
    int adsGunKickReducedKickBullets;
    float adsGunKickReducedKickPercent;
    float fAdsGunKickPitchMin;
    float fAdsGunKickPitchMax;
    float fAdsGunKickYawMin;
    float fAdsGunKickYawMax;
    float fAdsGunKickAccel;
    float fAdsGunKickSpeedMax;
    float fAdsGunKickSpeedDecay;
    float fAdsGunKickStaticDecay;
    float fAdsViewKickPitchMin;
    float fAdsViewKickPitchMax;
    float fAdsViewKickYawMin;
    float fAdsViewKickYawMax;
    float fAdsViewKickCenterSpeed;
    float fAdsViewScatterMin;
    float fAdsViewScatterMax;
    float fAdsSpread;
    int hipGunKickReducedKickBullets;
    float hipGunKickReducedKickPercent;
    float fHipGunKickPitchMin;
    float fHipGunKickPitchMax;
    float fHipGunKickYawMin;
    float fHipGunKickYawMax;
    float fHipGunKickAccel;
    float fHipGunKickSpeedMax;
    float fHipGunKickSpeedDecay;
    float fHipGunKickStaticDecay;
    float fHipViewKickPitchMin;
    float fHipViewKickPitchMax;
    float fHipViewKickYawMin;
    float fHipViewKickYawMax;
    float fHipViewKickCenterSpeed;
    float fHipViewScatterMin;
    float fHipViewScatterMax;
    float fightDist;
    float maxDist;
    char * accuracyGraphName[2];
    vec2_t * accuracyGraphKnots[2];
    int accuracyGraphKnotCount[2];
    int iPositionReloadTransTime;
    float leftArc;
    float rightArc;
    float topArc;
    float bottomArc;
    float accuracy;
    float aiSpread;
    float playerSpread;
    float minTurnSpeed[2];
    float maxTurnSpeed[2];
    float pitchConvergenceTime;
    float yawConvergenceTime;
    float suppressTime;
    float maxRange;
    float fAnimHorRotateInc;
    float fPlayerPositionDist;
    char *szUseHintString;
    char *dropHintString;
    int iUseHintStringIndex;
    int dropHintStringIndex;
    float horizViewJitter;
    float vertViewJitter;
    char *szScript;
    float fOOPosAnimLength[2];
    int minDamage;
    int minPlayerDamage;
    float fMaxDamageRange;
    float fMinDamageRange;
    float destabilizationBaseTime;
    float destabilizationTimeReductionRatio;
    float destabilizationAngleMax;
    int destabilizeDistance;
    float locationDamageMultipliers[19];
    char *fireRumble;
    char *meleeImpactRumble;
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

struct WindowAttributes {
    CGRect *start_pass;
    int decode_mcu;
    ITabHandle insufficient_data;
};

struct XAnimCalcAnimInfo {
    DObjAnimMat rotTransArray[512];
    int animPartBits[4];
    int ignorePartBits[4];
};

struct XAnimDeltaPart {
    XAnimDeltaPartTrans *trans;
    XAnimDeltaPartQuat *quat;
};

union XAnimDynamicIndices {
    byte _1[1];
    short unsigned int _2[1];
};

struct XAnimDeltaPartQuatDataFrames {
    XQuat2 *frames;
    XAnimDynamicIndices indices;
};

union XAnimDeltaPartQuatData {
    struct XAnimDeltaPartQuatDataFrames frames;
    XQuat2 frame0;
};

struct XAnimDeltaPartQuat {
    short unsigned int size;
    union XAnimDeltaPartQuatData u;
};

struct XAnimDeltaPartTransFrames {
    vec3_t *frames;
    XAnimDynamicIndices indices;
};

union XAnimDeltaPartTransData {
    struct XAnimDeltaPartTransFrames frames;
    vec3_t frame0;
};

struct XAnimDeltaPartTrans {
    short unsigned int size;
    union XAnimDeltaPartTransData u;
};

struct XAnimNotifyInfo {
    ScriptString name;
    float time;
};

struct XAnimNotify_s {
    const char *name;
    unsigned int type;
    float timeFrac;
};

struct XAnimParent {
    short unsigned int flags;
    short unsigned int children;
};

struct XAnimEntry {
    short unsigned int numAnims;
    short unsigned int parent;
    union { XAnimParts *parts; struct XAnimParent s; } u;
};

struct XAnimPart {
    XAnimPartTrans *trans;
    XAnimPartQuat *quat;
};

union XAnimPartQuatFrames {
    XQuat *frames;
    XQuat2 *frames2;
};

struct XAnimPartQuatDataFrames {
    union XAnimPartQuatFrames u;
    XAnimDynamicIndices indices;
};

union XAnimPartQuatData {
    struct XAnimPartQuatDataFrames frames;
    XQuat frame0;
    XQuat2 frame02;
};

struct XAnimPartQuat {
    short unsigned int size;
    union XAnimPartQuatData u;
};

struct XAnimPartTransFrames {
    vec3_t *frames;
    union XAnimDynamicIndices indices;
};

union XAnimPartTransData {
    struct XAnimPartTransFrames frames;
    vec3_t frame0;
};

struct XAnimPartTrans {
    short unsigned int size;
    union XAnimPartTransData u;
};

struct XAnimParts_s {
    short unsigned int numframes;
    Bool bLoop;
    Bool bDelta;
    float framerate;
    float frequency;
    byte notifyCount;
    short int boneCount;
    short unsigned int *names;
    char *simpleQuatBits;
    XAnimPart *parts;
    XAnimNotifyInfo *notify;
    XAnimDeltaPart *deltaPart;
    const char *name;
    Bool isDefault;
};

struct XAnimSimpleRotPos {
    vec2_t rot;
    float posWeight;
    vec3_t pos;
};

struct XAnimState {
    float time;
    float oldTime;
    short int timeCount;
    short int oldTimeCount;
    float goalTime;
    float goalWeight;
    float weight;
    float rate;
};

struct XAnimInfo {
    short unsigned int notifyChild;
    short int notifyIndex;
    short unsigned int notifyName;
    short unsigned int notifyType;
    short unsigned int prev;
    short unsigned int next;
    struct XAnimState s;
};

struct XAnimTime {
    float time;
    int frameCount;
    float frameFrac;
    int frameIndex;
};

struct XAnimToXModel {
    int partBits[4];
    unsigned char boneIndex[128];
};

struct XAnimTree_s {
    XAnim *anims;
    short unsigned int entnum;
    Bool bAbs;
    Bool bUseGoalWeight;
    short unsigned int infoArray[1];
};

struct XAnim_s {
    const char *debugName;
    unsigned int size;
    const char * *debugAnimNames;
    XAnimEntry entries[1];
};

union XAssetHeader {
    struct XAnimParts_s *parts;
    struct XModel *model;
    struct Material *material;
    struct GfxImage *image;
    struct snd_alias_list_s *sound;
    struct SndCurve *sndCurve;
    struct clipMap_t *clipMap;
    struct GfxWorld *world;
    struct GfxLightDef *lightDef;
    struct XAnimParts_s * (*font)[64];
    struct MenuList *menuList;
    struct menuDef_t *menu;
    LocalizeEntry *localize;
    struct WeaponDef *weapon;
    SndDriverGlobals *sndDriverGlobals;
    struct FxEffectDef *fx;
    struct FxImpactTable *impactFx;
    RawFile *rawfile;
    void *data;
};

struct XBlendInfo_s {
    vec3_t offset;
    short int boneOffset;
    short unsigned int boneWeight;
};

struct XBoneHierarchy {
    short unsigned int *names;
    unsigned char parentList[1];
};

struct XBoneInfo_s {
    vec3_t bounds[2];
    vec3_t offset;
    float radiusSquared;
};

struct XModelCollSurf_s {
    XModelCollTri *collTris;
    int numCollTris;
    vec3_t mins;
    vec3_t maxs;
    int boneIdx;
    int contents;
    int surfFlags;
};

struct XModelCollTri_s {
    vec4_t plane;
    vec4_t svec;
    vec4_t tvec;
};

struct XModelConfigEntry {
    char filename[1024];
    float dist;
};

struct XModelConfig {
    XModelConfigEntry entries[4];
    float mins[3];
    float maxs[3];
    int collLod;
    unsigned char flags;
};

struct XModelLodInfo {
    float dist;
    const char *filename;
    short int numsurfs;
    short unsigned int *surfNames;
    XModelSurfs *surfs;
};

struct XModel {
    void (*parts)();
    XModelLodInfo lodInfo[4];
    XModelCollSurf *collSurfs;
    int numCollSurfs;
    int contents;
    XBoneInfo_s *boneInfo;
    vec3_t mins;
    vec3_t maxs;
    short int numLods;
    short int collLod;
    struct trXSkin_t *xskins;
    int memUsage;
    const char *name;
    unsigned char flags;
    Bool bad;
};

struct XModelParts_s {
    short int numBones;
    short int numRootBones;
    XBoneHierarchy *hierarchy;
    short int *quats;
    float *trans;
    unsigned char *partClassification;
    DSkel skel;
};

struct XModelSurfs_s {
    void *surfs;
    XPartBits partBits;
};

struct XModelDefault {
    scr_string_t boneNames[1];
    XBoneHierarchy hierarchy;
    XModelParts modelParts;
    XModelSurfs modelSurf;
    XBoneInfo boneInfo;
    unsigned char partClassification[1];
    short unsigned int surfNames[1];
};

struct XRigidSurface {
    IDirect3DVertexBuffer9 *vb;
};

struct XSurface_s {
    byte tileMode;
    short int vertCount;
    short int triCount;
    short int boneOffset;
    int (*triIndices)[4];
    XVertexBuffer *verts;
    struct XRigidSurface surfRigid;
    IDirect3DIndexBuffer9 *indexBuffer;
};

struct XVertexInfo_s {
    vec3_t normal;
    byte color[4];
    vec3_t binormal;
    float texCoordX;
    vec3_t tangent;
    float texCoordY;
    vec3_t offset;
    byte numWeights;
    byte boneWeight;
    short int boneOffset;
};

struct XVertexBuffer {
    XVertexInfo v;
    XBlendInfo w[1];
};

struct _ADPCMDATATAG {
    long unsigned int blocksize;
    long unsigned int extrasamples;
    long unsigned int blockleft;
    long unsigned int step;
    long unsigned int savesrc;
    long unsigned int sample;
    long unsigned int destend;
    long unsigned int srcend;
    long unsigned int samplesL;
    long unsigned int samplesR;
    short unsigned int moresamples[16];
};

struct _AILSOUNDINFO {
    long int format;
    const void *data_ptr;
    long unsigned int data_len;
    long unsigned int rate;
    long int bits;
    long int channels;
    long unsigned int samples;
    long unsigned int block_size;
    const void *initial_ptr;
};

struct MssSound {
    AILSOUNDINFO info;
    char data[1];
};

struct _AILMIXINFO {
    AILSOUNDINFO Info;
    ADPCMDATA mss_adpcm;
    long unsigned int src_fract;
    long int left_val;
    long int right_val;
};

struct _Alloc_hider {
    char *_M_p;
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

struct _D3DDISPLAYMODE {
    UINT Width;
    UINT Height;
    UINT RefreshRate;
    D3DFORMAT Format;
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

struct _D3DMATRIX {
    union { struct { float _11; float _12; float _13; float _14; float _21; float _22; float _23; float _24; float _31; float _32; float _33; float _34; float _41; float _42; float _43; float _44; }; float m[4][4]; };
};

struct CRectList {
    MacRGBColor data[1];
};

struct GfxCmdSetShadowCookie {
    GfxCmdHeader header;
    D3DMATRIX lookupMatrix;
    float fade;
};

struct GfxCodeMatrix {
    D3DMATRIX matrix[4];
    Bool valid[4];
} COD2_ALIGNED(16);

struct GfxCodeMatrices {
    float worldScale;
    struct GfxCodeMatrix world;
    GfxCodeMatrix normalizedWorld;
    GfxCodeMatrix view;
    GfxCodeMatrix projection;
    GfxCodeMatrix worldView;
    GfxCodeMatrix normalizedWorldView;
    GfxCodeMatrix viewProjection;
    GfxCodeMatrix worldViewProjection;
    GfxCodeMatrix normalizedWorldViewProjection;
    GfxCodeMatrix shadowLookupMatrix;
    GfxCodeMatrix lightGridLookupMatrix;
    GfxCodeMatrix worldOutdoorLookup;
    GfxCodeMatrix OGLworldViewProjection;
};

union IDirect3DVertexBuffer9 {
    struct { float _11; float _12; float _13; float _14; float _21; float _22; float _23; float _24; float _31; float _32; float _33; float _34; float _41; float _42; float _43; float _44; };
    D3DMATRIX m[4];
};

struct ShadowCookie {
    int firstCasterDrawSurf;
    int firstReceiverDrawSurf;
    int firstUnusedDrawSurf;
    D3DMATRIX shadowLookupMatrix;
    vec3_t boxMin;
    vec3_t boxMax;
    GfxViewParms *shadowViewParms;
    float fade;
};

struct ShadowCookieList {
    ShadowCookie cookies[24];
    int cookieCount;
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

struct _D3DPSHADERCAPS2_0 {
    DWORD Caps;
    INT DynamicFlowControlDepth;
    INT NumTemps;
    INT StaticFlowControlDepth;
    INT NumInstructionSlots;
};

struct _D3DRASTER_STATUS {
    BOOL InVBlank;
    UINT ScanLine;
};

struct _D3DRECT {
    LONG x1;
    LONG y1;
    LONG x2;
    LONG y2;
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

struct _D3DVIEWPORT9 {
    DWORD X;
    DWORD Y;
    DWORD Width;
    DWORD Height;
    float MinZ;
    float MaxZ;
};

struct DxState {
    vec4_t vertexShaderConsts[256];
    vec4_t pixelShaderConsts[256];
    int refStateBits[2];
    int activeStateBits[2];
    int stencilRefValue;
    int refColorStageBits[8];
    int refAlphaStageBits[8];
    int activeColorStageBits[8];
    int activeAlphaStageBits[8];
    Bool gridLighting;
    byte genTexCoords[2];
    GfxRenderTargetId renderTargetId;
    int renderTargetWidth;
    int renderTargetHeight;
    GfxViewportBehavior viewportBehavior;
    IDirect3DSurface9 *renderTargetSurface;
    IDirect3DSurface9 *depthStencilSurface;
    D3DVIEWPORT9 viewport;
    Bool viewportIsNull;
    IDirect3DIndexBuffer9 *indexBuffer;
    struct { IDirect3DVertexBuffer9 *vb; int offset; int stride; } streams[1];
    volatile IDirect3DIndexBuffer9 *indexBufferDeselecting;
    volatile IDirect3DVertexBuffer9 *vertexBufferDeselecting;
    byte samplerState[16];
    const GfxImage * samplerImage[16];
    DWORD textureColorArg;
    IDirect3DPixelShader9 *pixelShader;
    IDirect3DVertexShader9 *vertexShader;
    IDirect3DVertexDeclaration9 *vertexDecl;
    DWORD fvf;
    byte alphaRef;
    D3DCOLOR textureFactor;
    D3DCOLOR ambientColor;
    int clipPlaneCount;
    struct { union GfxColor color; D3DFOGMODE mode; float start; float end; float density; } fog;
};

struct GfxViewParms {
    vec3_t origin;
    vec3_t axis[3];
    D3DVIEWPORT9 viewport;
    D3DMATRIX viewMatrix;
    D3DMATRIX projectionMatrix;
    D3DMATRIX viewProjectionMatrix;
    D3DMATRIX inverseViewProjectionMatrix;
    float depthHackNearClip;
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

struct _D3DVSHADERCAPS2_0 {
    DWORD Caps;
    INT DynamicFlowControlDepth;
    INT NumTemps;
    INT StaticFlowControlDepth;
};

struct _D3DCAPS9 {
    D3DDEVTYPE DeviceType;
    UINT AdapterOrdinal;
    DWORD Caps;
    DWORD Caps2;
    DWORD Caps3;
    DWORD PresentationIntervals;
    DWORD CursorCaps;
    DWORD DevCaps;
    DWORD PrimitiveMiscCaps;
    DWORD RasterCaps;
    DWORD ZCmpCaps;
    DWORD SrcBlendCaps;
    DWORD DestBlendCaps;
    DWORD AlphaCmpCaps;
    DWORD ShadeCaps;
    DWORD TextureCaps;
    DWORD TextureFilterCaps;
    DWORD CubeTextureFilterCaps;
    DWORD VolumeTextureFilterCaps;
    DWORD TextureAddressCaps;
    DWORD VolumeTextureAddressCaps;
    DWORD LineCaps;
    DWORD MaxTextureWidth;
    DWORD MaxTextureHeight;
    DWORD MaxVolumeExtent;
    DWORD MaxTextureRepeat;
    DWORD MaxTextureAspectRatio;
    DWORD MaxAnisotropy;
    float MaxVertexW;
    float GuardBandLeft;
    float GuardBandTop;
    float GuardBandRight;
    float GuardBandBottom;
    float ExtentsAdjust;
    DWORD StencilCaps;
    DWORD FVFCaps;
    DWORD TextureOpCaps;
    DWORD MaxTextureBlendStages;
    DWORD MaxSimultaneousTextures;
    DWORD VertexProcessingCaps;
    DWORD MaxActiveLights;
    DWORD MaxUserClipPlanes;
    DWORD MaxVertexBlendMatrices;
    DWORD MaxVertexBlendMatrixIndex;
    float MaxPointSize;
    DWORD MaxPrimitiveCount;
    DWORD MaxVertexIndex;
    DWORD MaxStreams;
    DWORD MaxStreamStride;
    DWORD VertexShaderVersion;
    DWORD MaxVertexShaderConst;
    DWORD PixelShaderVersion;
    float PixelShader1xMaxValue;
    DWORD DevCaps2;
    float MaxNpatchTessellationLevel;
    DWORD Reserved5;
    UINT MasterAdapterOrdinal;
    UINT AdapterOrdinalInGroup;
    UINT NumberOfAdaptersInGroup;
    DWORD DeclTypes;
    DWORD NumSimultaneousRTs;
    DWORD StretchRectFilterCaps;
    D3DVSHADERCAPS2_0 VS20Caps;
    D3DPSHADERCAPS2_0 PS20Caps;
    DWORD VertexTextureFilterCaps;
    DWORD MaxVShaderInstructionsExecuted;
    DWORD MaxPShaderInstructionsExecuted;
    DWORD MaxVertexShader30InstructionSlots;
    DWORD MaxPixelShader30InstructionSlots;
};

struct _D3DXCONSTANTTABLE_DESC {
    LPCSTR Creator;
    DWORD Version;
    UINT Constants;
};

struct _D3DXFONT_DESCA {
    INT Height;
    UINT Width;
    UINT Weight;
    UINT MipLevels;
    BOOL Italic;
    BYTE CharSet;
    BYTE OutputPrecision;
    BYTE Quality;
    BYTE PitchAndFamily;
    CHAR FaceName[32];
};

struct _D3DXFONT_DESCW {
    INT Height;
    UINT Width;
    UINT Weight;
    UINT MipLevels;
    BOOL Italic;
    BYTE CharSet;
    BYTE OutputPrecision;
    BYTE Quality;
    BYTE PitchAndFamily;
    WCHAR FaceName[32];
};

struct _D3DXIMAGE_INFO {
    UINT Width;
    UINT Height;
    UINT Depth;
    UINT MipLevels;
    D3DFORMAT Format;
    D3DRESOURCETYPE ResourceType;
    D3DXIMAGE_FILEFORMAT ImageFileFormat;
};

struct _D3DXMACRO {
    LPCSTR Name;
    LPCSTR Definition;
};

struct _D3DXSHADER_CONSTANTINFO {
    DWORD Name;
    WORD RegisterSet;
    WORD RegisterIndex;
    WORD RegisterCount;
    WORD Reserved;
    DWORD TypeInfo;
    DWORD DefaultValue;
};

struct _D3DXSHADER_CONSTANTTABLE {
    DWORD Size;
    DWORD Creator;
    DWORD Version;
    DWORD Constants;
    DWORD ConstantInfo;
    DWORD Flags;
    DWORD Target;
};

struct _D3DXSHADER_TYPEINFO {
    WORD Class;
    WORD Type;
    WORD Rows;
    WORD Columns;
    WORD Elements;
    WORD StructMembers;
    DWORD StructMemberInfo;
};

struct _DDCOLORKEY {
    DWORD dwColorSpaceLowValue;
    DWORD dwColorSpaceHighValue;
};

struct _DDPIXELFORMAT {
    DWORD dwSize;
    DWORD dwFlags;
    DWORD dwFourCC;
    union { DWORD dwRGBBitCount; DWORD dwYUVBitCount; DWORD dwZBufferBitDepth; DWORD dwAlphaBitDepth; DWORD dwLuminanceBitCount; DWORD dwBumpBitCount; DWORD dwPrivateFormatBitCount; };
    union { DWORD dwRBitMask; DWORD dwYBitMask; DWORD dwStencilBitDepth; DWORD dwLuminanceBitMask; DWORD dwBumpDuBitMask; DWORD dwOperations; };
    union { DWORD dwGBitMask; DWORD dwUBitMask; DWORD dwZBitMask; DWORD dwBumpDvBitMask; struct { WORD wFlipMSTypes; WORD wBltMSTypes; } MultiSampleCaps; };
    union { DWORD dwBBitMask; DWORD dwVBitMask; DWORD dwStencilBitMask; DWORD dwBumpLuminanceBitMask; };
    union { DWORD dwRGBAlphaBitMask; DWORD dwYUVAlphaBitMask; DWORD dwLuminanceAlphaBitMask; DWORD dwRGBZBitMask; DWORD dwYUVZBitMask; };
};

struct _DDSCAPS2 {
    DWORD dwCaps;
    DWORD dwCaps2;
    DWORD dwCaps3;
    union { DWORD dwCaps4; DWORD dwVolumeDepth; };
};

struct _DDSURFACEDESC2 {
    DWORD dwSize;
    DWORD dwFlags;
    DWORD dwHeight;
    DWORD dwWidth;
    union { LONG lPitch; DWORD dwLinearSize; };
    union { DWORD dwBackBufferCount; DWORD dwDepth; };
    union { DWORD dwMipMapCount; DWORD dwRefreshRate; DWORD dwSrcVBHandle; };
    DWORD dwAlphaBitDepth;
    DWORD dwReserved;
    void *lpSurface;
    union { DDCOLORKEY ddckCKDestOverlay; DWORD dwEmptyFaceColor; };
    DDCOLORKEY ddckCKDestBlt;
    DDCOLORKEY ddckCKSrcOverlay;
    DDCOLORKEY ddckCKSrcBlt;
    union { DDPIXELFORMAT ddpfPixelFormat; DWORD dwFVF; };
    DDSCAPS2 ddsCaps;
    DWORD dwTextureStage;
};

struct _Deque_iterator_UINT16_UINT16ref_UINT16ptr {
    UINT16 *_M_cur;
    UINT16 *_M_first;
    UINT16 *_M_last;
    UINT16 * *_M_node;
};

struct _Deque_impl {
    UINT16 * *_M_map;
    size_t _M_map_size;
    _Deque_iterator_UINT16_UINT16ref_UINT16ptr _M_start;
    _Deque_iterator_UINT16_UINT16ref_UINT16ptr _M_finish;
};

struct _FILETIME {
    DWORD dwLowDateTime;
    DWORD dwHighDateTime;
};

struct _GUID {
    long unsigned int Data1;
    short unsigned int Data2;
    short unsigned int Data3;
    unsigned char Data4[8];
};

union _LARGE_INTEGER {
    struct { DWORD LowPart; LONG HighPart; };
    struct { DWORD LowPart; LONG HighPart; } u;
    LONGLONG QuadPart;
};

struct _List_iterator_CFenceptr {
    _List_node_base *_M_node;
};

struct _List_iterator_GLuintptr {
    _List_node_base *_M_node;
};

struct _List_iterator_Tuple {
    _List_node_base *_M_node;
};

struct _List_node_CFenceptr {
    CFence *_M_data;
};

struct _List_node_GLuintptr {
    GLuint *_M_data;
};

struct _List_node_Tuple {
    Tuple _M_data;
};

struct _List_node_base {
    int _M_next;
    int _M_prev;
};

struct _List_impl {
    struct _List_node_base _M_node;
};

struct _MEMORYSTATUS {
    DWORD dwLength;
    DWORD dwMemoryLoad;
    SIZE_T dwTotalPhys;
    SIZE_T dwAvailPhys;
    SIZE_T dwTotalPageFile;
    SIZE_T dwAvailPageFile;
    SIZE_T dwTotalVirtual;
    SIZE_T dwAvailVirtual;
};

struct _OSVERSIONINFOA {
    DWORD dwOSVersionInfoSize;
    DWORD dwMajorVersion;
    DWORD dwMinorVersion;
    DWORD dwBuildNumber;
    DWORD dwPlatformId;
    CHAR szCSDVersion[128];
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

struct _SYSTEMTIME {
    WORD wYear;
    WORD wMonth;
    WORD wDayOfWeek;
    WORD wDay;
    WORD wHour;
    WORD wMinute;
    WORD wSecond;
    WORD wMilliseconds;
};

union _ULARGE_INTEGER {
    struct { DWORD LowPart; DWORD HighPart; };
    struct { DWORD LowPart; DWORD HighPart; } u;
    ULONGLONG QuadPart;
};

struct _Vector_impl {
    MacRect *_M_start;
    MacRect *_M_finish;
    MacRect *_M_end_of_storage;
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

struct __darwin_pthread_handler_rec {
    jpeg_alloc (*__routine)();
    jpeg_alloc *__arg;
    int __next;
};

struct __darwin_pthread_mutex_t {
    long int __sig;
    char __opaque[40];
};

struct DIR {
    int dd_fd;
    long int dd_loc;
    long int dd_size;
    char *dd_buf;
    int dd_len;
    long int dd_seek;
    long int dd_rewind;
    int dd_flags;
    struct __darwin_pthread_mutex_t dd_lock;
    _telldir *dd_td;
};

struct __false_type {
    int _placeholder;
};

struct __true_type {
    int _placeholder;
};

struct _opaque_pthread_mutex_t {
    long int __sig;
    char __opaque[40];
};

struct _opaque_pthread_mutexattr_t {
    long int __sig;
    char __opaque[8];
};

struct _opaque_pthread_t {
    long int __sig;
    __darwin_pthread_handler_rec *__cleanup_stack;
    char __opaque[596];
};

struct allocator_CCacheInfoBlock {
    int _placeholder;
};

struct allocator_CDirect3DSurfaceptr {
    int _placeholder;
};

struct allocator_CDirect3DVolumeptr {
    int _placeholder;
};

struct allocator_CDisplayInfo {
    int _placeholder;
};

struct allocator_CFenceptr {
    int _placeholder;
};

struct allocator_CResInfo {
    int _placeholder;
};

struct allocator_CSoundObjectptr {
    int _placeholder;
};

struct allocator_CStreamSoundptr {
    int _placeholder;
};

struct allocator_FFItem {
    int _placeholder;
};

struct allocator_FSRef {
    int _placeholder;
};

struct allocator_GLuint {
    int _placeholder;
};

struct allocator_GLuintptr {
    int _placeholder;
};

struct allocator_MacRect {
    int _placeholder;
};

struct allocator_Tuple {
    int _placeholder;
};

struct allocator_UINT16 {
    int _placeholder;
};

struct allocator_UINT16ptr {
    int _placeholder;
};

struct allocator_UINT32 {
    int _placeholder;
};

struct allocator_UINT8 {
    int _placeholder;
};

struct allocator_WinCursor {
    int _placeholder;
};

struct allocator_char {
    int _placeholder;
};

struct allocator_float {
    int _placeholder;
};

struct animConditionTable_t {
    animScriptConditionTypes_t type;
    animStringItem_t *values;
};

struct animScriptCommand_t {
    short int bodyPart[2];
    short int animIndex[2];
    short int animDuration[2];
    snd_alias_list_t *soundAlias;
};

struct animScriptCondition_t {
    int index;
    int value[2];
};

struct animScriptItem_t {
    int numConditions;
    animScriptCondition_t conditions[9];
    int numCommands;
    animScriptCommand_t commands[8];
};

struct animScript_t {
    int numItems;
    animScriptItem_t * items[128];
};

struct animStringItem_t {
    const char *string;
    int hash;
};

struct animation_s {
    char name[64];
    int initialLerp;
    float moveSpeed;
    int duration;
    int nameHash;
    int flags;
    long long int movetype;
    int noteType;
};

struct archivedEntityShared_t {
    int svFlags;
    int clientMask[2];
    vec3_t absmin;
    vec3_t absmax;
};

struct archivedSnapshot_s {
    int start;
    int size;
};

struct areaParms_t {
    const float *start;
    const float *mins;
    const float *maxs;
    int *list;
    int count;
    int maxcount;
    int contentmask;
    trace_t *results;
};

struct audioSample_t {
    unsigned char *buffer;
    int lengthInBytes;
    int lengthInSamples;
    int bytesPerSample;
    int frequency;
    Bool stereo;
    int channels;
    int sampleOffset;
};

struct backing_store_struct {
    void (*read_backing_store)();
    void (*write_backing_store)();
    void (*close_backing_store)();
    FILE *temp_file;
    char temp_name[64];
};

struct bidirectional_iterator_tag {
    int _placeholder;
};

struct binary_function_UINT32_UINT32_bool {
    int _placeholder;
};

struct bind_t {
    char *command;
    int defaultbind1;
    int defaultbind2;
    int bind1;
    int bind2;
};

struct bitread_perm_state {
    bit_buf_type get_buffer;
    int bits_left;
};

struct box {
    int c0min;
    int c0max;
    int c1min;
    int c1max;
    int c2min;
    int c2max;
    INT32 volume;
    long int colorcount;
};

struct builtin_s {
    char *string;
    int builtin;
};

struct cLeafBrushNodeChildren_t {
    float dist;
    float range;
    short unsigned int childOffset[2];
};

struct cLeafBrushNodeLeaf_t {
    short unsigned int *brushes;
};

union cLeafBrushNodeData_t {
    struct cLeafBrushNodeLeaf_t leaf;
    struct cLeafBrushNodeChildren_t children;
};

struct cLeafBrushNode_s {
    byte axis;
    short int leafBrushCount;
    int contents;
    union cLeafBrushNodeData_t data;
};

struct cLeaf_t {
    short unsigned int firstCollAabbIndex;
    short unsigned int collAabbCount;
    int brushContents;
    int terrainContents;
    vec3_t mins;
    vec3_t maxs;
    int leafBrushNode;
    short int cluster;
};

struct cNode_t {
    cplane_t *plane;
    short int children[2];
};

struct cStaticModelWritable {
    short unsigned int nextModelInWorldSector;
};

struct cStaticModel_s {
    struct cStaticModelWritable writable;
    struct XModel *xmodel;
    vec3_t origin;
    vec3_t invAxis[3];
    vec3_t absmin;
    vec3_t absmax;
};

struct c_derived_tbl {
    unsigned int ehufco[256];
    char ehufsi[256];
};

struct cachedSnapshot_t {
    int archivedFrame;
    int time;
    int num_entities;
    int first_entity;
    int num_clients;
    int first_client;
    int usesDelta;
};

struct cameraShake_t {
    int time;
    float scale;
    float length;
    float radius;
    vec3_t src;
    float size;
    float rumbleScale;
};

struct cbrush_t {
    vec3_t mins;
    vec3_t maxs;
    int contents;
    int numsides;
    cbrushside_t *sides;
    short int axialMaterialNum[2][3];
};

struct cbrushside_t {
    cplane_t *plane;
    int materialNum;
};

struct cgMedia_t {
    MaterialHandle whiteMaterial;
    MaterialHandle softLineMaterial;
    MaterialHandle softLineHMaterial;
    MaterialHandle teamStatusBar;
    MaterialHandle voiceChatMaterial;
    MaterialHandle balloonMaterial;
    MaterialHandle connectionMaterial;
    MaterialHandle youInKillCamMaterial;
    MaterialHandle tracerMaterial;
    MaterialHandle lagometerMaterial;
    MaterialHandle hintMaterials[133];
    MaterialHandle stanceMaterials[4];
    MaterialHandle objectiveMaterials[1];
    MaterialHandle friendMaterials[2];
    MaterialHandle damageMaterial;
    MaterialHandle mantleHint;
    FontHandle smallDevFont;
    FontHandle bigDevFont;
    snd_alias_list_t *noAmmoSound;
    snd_alias_list_t *landDmgSound;
    snd_alias_list_t *teleInSound;
    snd_alias_list_t * grenadeBounceSound[23];
    snd_alias_list_t * grenadeExplodeSound[23];
    snd_alias_list_t * rocketExplodeSound[23];
    snd_alias_list_t * bulletHitSmallSound[23];
    snd_alias_list_t * shotgunHitSound[23];
    snd_alias_list_t * bulletHitLargeSound[23];
    snd_alias_list_t * stepRunSound[23];
    snd_alias_list_t * stepRunSoundPlayer[23];
    snd_alias_list_t * stepWalkSound[23];
    snd_alias_list_t * stepWalkSoundPlayer[23];
    snd_alias_list_t * stepProneSound[23];
    snd_alias_list_t * stepProneSoundPlayer[23];
    snd_alias_list_t * landSound[23];
    snd_alias_list_t * landSoundPlayer[23];
    snd_alias_list_t *runningEquipmentSound;
    snd_alias_list_t *runningEquipmentSoundPlayer;
    snd_alias_list_t *walkingEquipmentSound;
    snd_alias_list_t *walkingEquipmentSoundPlayer;
    snd_alias_list_t *foliageMovement;
    snd_alias_list_t *bulletWhizby;
    snd_alias_list_t *meleeSwingLarge;
    snd_alias_list_t *meleeSwingSmall;
    snd_alias_list_t *meleeHit;
    snd_alias_list_t *meleeHitOther;
    snd_alias_list_t *playerHeartBeatSound;
    snd_alias_list_t *playerBreathInSound;
    snd_alias_list_t *playerBreathOutSound;
    snd_alias_list_t *playerBreathGaspSound;
    snd_alias_list_t *playerSwapOffhand;
    MaterialHandle checkbox_clear;
    MaterialHandle checkbox_checked;
    MaterialHandle checkbox_fail;
    MaterialHandle compassping_friendlyfiring;
    MaterialHandle compassping_friendlyyelling;
    MaterialHandle compassping_enemyfiring;
    MaterialHandle compassping_enemyyelling;
    MaterialHandle compassping_grenade;
    MaterialHandle compassping_explosion;
    MaterialHandle grenadeIcon;
    MaterialHandle grenadePointer;
    FxImpactTable *fx;
    struct FxEffectDef *fxNoBloodFleshHit;
};

struct cg_hudelem_t {
    float x;
    float y;
    float z;
    float width;
    float height;
    char hudElemLabel[256];
    float labelWidth;
    char hudElemText[256];
    float textWidth;
    FontHandle font;
    float fontScale;
    float fontHeight;
    vec4_t color;
};

struct cin_cache {
    char fileName[256];
    int CIN_WIDTH;
    int CIN_HEIGHT;
    int xpos;
    int ypos;
    int width;
    int height;
    qboolean looping;
    qboolean holdAtEnd;
    qboolean dirty;
    qboolean alterGameState;
    qboolean silent;
    qboolean material;
    qboolean letterBox;
    qboolean sound;
    fileHandle_t iFile;
    e_status status;
    unsigned int startTime;
    unsigned int lastFrameTime;
    unsigned int lastTime;
    long int tfps;
    long int RoQPlayed;
    long int ROQSize;
    unsigned int RoQFrameSize;
    long int onQuad;
    long int numQuads;
    long int samplesPerLine;
    unsigned int roq_id;
    long int screenDelta;
    void (*VQ0)(byte **status, unsigned char *data);
    void (*VQ1)(byte **status, unsigned char *data);
    void (*VQNormal)(byte **status, unsigned char *data);
    void (*VQBuffer)(byte **status, unsigned char *data);
    long int samplesPerPixel;
    byte *gray;
    unsigned int xsize;
    unsigned int ysize;
    unsigned int maxsize;
    unsigned int minsize;
    qboolean half;
    qboolean smootheddouble;
    qboolean inMemory;
    long int normalBuffer0;
    long int roq_flags;
    long int roqF0;
    long int roqF1;
    long int t[2];
    long int roqFPS;
    int playonwalls;
    byte *buf;
    connstate_t previousGameState;
};

struct cinematics_t {
    byte linbuf[2097152];
    byte file[65536];
    short int sqrTable[256];
    unsigned int mcomp[256];
    byte * qStatus[2][32768];
    long int oldXOff;
    long int oldYOff;
    long int oldysize;
    long int oldxsize;
    int currentHandle;
};

struct clientDebug_t {
    int maxStrings;
    int numStrings;
    trDebugString_t *strings;
    byte *stringFromServer;
    int maxLines;
    int numLines;
    trDebugLine_t *lines;
    byte *lineFromServer;
    int *lineDuration;
};

struct clientLogo_t {
    int startTime;
    int duration;
    int fadein;
    int fadeout;
    MaterialHandle material[2];
};

struct clientState_t {
    int clientIndex;
    team_t team;
    int modelindex;
    int attachModelIndex[6];
    int attachTagIndex[6];
    char name[32];
};

struct client_fields_s {
    const char *name;
    int ofs;
    fieldtype_t type;
    ScriptCallbackClient setter;
    ScriptCallbackClient getter;
};

struct cmd_function_s {
    /* really a cmd_function_t*; kept in an int-typed field
     * accessed via *(cmd_function_t**)&next. Use a pointer-sized int so it
     * survives LP64/LLP64 (4 on x86, 8 on x64). The old `int` truncated the
     * next-pointer on MSVC x64 -> crash in Cmd_AddCommand. */
    intptr_t next;
    char *name;
    const char *autoCompleteDir;
    const char *autoCompleteExt;
    xcommand_t function;
};

struct cmd_t {
    byte *data;
    int maxsize;
    int cmdsize;
};

struct cmodel_t {
    vec3_t mins;
    vec3_t maxs;
    float radius;
    struct cLeaf_t leaf;
};

struct clipMap_t {
    const char *name;
    int numStaticModels;
    cStaticModel_t *staticModelList;
    int numMaterials;
    dmaterial_t *materials;
    int numBrushSides;
    cbrushside_t *brushsides;
    int numNodes;
    cNode_t *nodes;
    int numLeafs;
    cLeaf_t *leafs;
    int leafbrushNodesCount;
    cLeafBrushNode_t *leafbrushNodes;
    int numLeafBrushes;
    short unsigned int *leafbrushes;
    int numLeafSurfaces;
    int *leafsurfaces;
    int vertCount;
    CollisionVertex *verts;
    int edgeCount;
    CollisionEdge *edges;
    int triCount;
    CollisionTriangle *tris;
    int borderCount;
    CollisionBorder *borders;
    int partitionCount;
    CollisionPartition *partitions;
    int aabbTreeCount;
    CollisionAabbTree *aabbTrees;
    int numSubModels;
    cmodel_t *cmodels;
    short unsigned int numBrushes;
    cbrush_t *brushes;
    int numClusters;
    int clusterBytes;
    byte *visibility;
    qboolean vised;
    int numEntityChars;
    char *entityString;
    cbrush_t *box_brush;
    cmodel_t box_model;
    int pathNodeCount;
    pathnode_t *pathNodes;
    int chainNodeCount;
    short unsigned int *chainNodes;
    short unsigned int *chainNodesRev;
    int visBytes;
    byte *pathVis;
    int nodeTreeCount;
    pathnode_tree_t *nodeTree;
    unsigned int checksum;
};

struct columnInfo_s {
    int pos;
    int width;
    int maxChars;
};

struct com_parse_mark_t {
    int lines;
    const char *text;
    qboolean ungetToken;
    int backup_lines;
    const char *backup_text;
};

struct commandDef_t {
    const char *name;
    void (*handler)();
};

struct compassactor_t {
    int iLastUpdate;
    vec2_t vLastPos;
    float fLastYaw;
    int pingTime;
    int beginFadeTime;
    Bool enemy;
};

struct complex_s {
    float real;
    float imag;
};

struct WaterGlob {
    complex_t H[16384];
    byte pixels[65536];
};

struct consoleCommand_t {
    char *cmd;
    void (*function)();
};

struct controller_info_t {
    vec3_t angles[6];
    vec3_t tag_origin_angles;
    vec3_t tag_origin_offset;
};

struct corpse_ent_t {
    int deathAnimStartTime;
};

struct cplane_s {
    vec3_t normal;
    float dist;
    byte type;
    byte signbits;
    byte pad[2];
};

struct cspField_t {
    const char *szName;
    int iOffset;
    int iFieldType;
};

struct d_derived_tbl {
    INT32 maxcode[18];
    INT32 valoffset[17];
    JHUFF_TBL *pub;
    int look_nbits[256];
    UINT8 look_sym[256];
};

struct dbrush_t {
    short int numSides;
    short int materialNum;
};

struct dbrushside_t {
    union { int planeNum; float bound; } u;
    int materialNum;
};

struct define_s {
    char *name;
    int flags;
    int builtin;
    int numparms;
    token_t *parms;
    token_t *tokens;
    intptr_t next;       /* pointer-as-int -> truncated on x64 (intptr_t==int on x86) */
    intptr_t hashnext;
};

struct directive_s {
    char *name;
    int (*func)();
};

struct directive_t {
    char *name;
    int (*func)(struct source_s *);   /* real handler sig (PC_Directive_*); byte-neutral fn-ptr */
};

struct directory_t {
    char path[256];
    char gamedir[256];
};

struct displayContextDef_s {
    float bias;
    int realTime;
    int frameTime;
    int cursorx;
    int cursory;
    qboolean debug;
    int screenWidth;
    int screenHeight;
    float screenAspect;
    float FPS;
    float blurRadiusOut;
    menuDef_t * Menus[128];
    int menuCount;
    menuDef_t * menuStack[16];
    int openMenuCount;
};

struct dleaf_t {
    int cluster;
    int area;
    int firstLeafSurface;
    int numLeafSurfaces;
    int firstLeafBrush;
    int numLeafBrushes;
    int cellNum;
    int firstLightIndex;
    int numLights;
};

struct dmaterial_t {
    char material[64];
    int surfaceFlags;
    int contentFlags;
};

struct dmodel_t {
    float mins[3];
    float maxs[3];
    int firstTriangle;
    int numTriangles;
    int firstSurface;
    int numSurfaces;
    int firstBrush;
    int numBrushes;
};

struct dnode_t {
    int planeNum;
    int children[2];
    int mins[3];
    int maxs[3];
};

struct dplane_t {
    float normal[3];
    float dist;
};

struct dsound_sample_t {
    struct IDirectSoundCaptureBuffer *DSCB;
    struct IDirectSoundBuffer *DSB;
    unsigned int dwBufferSize;
    unsigned int dwCaptureOffset;
    unsigned int currentOffset;
    unsigned int lastOffset;
    unsigned int currentBufferLength;
    int stopPosition;
    unsigned int lastPlayPos;
    int bytesBuffered;
    int mode;
    int frequency;
    int volume;
    int pan;
    int channels;
    Bool playing;
    int channel;
    byte playMode;
};

struct dvar_s {
    const char *name;
    short unsigned int flags;
    byte type;
    Bool modified;
    union DvarValue current;
    DvarValue latched;
    DvarValue reset;
    union DvarLimits domain;
    intptr_t next;
    intptr_t hashNext;
};

struct editFieldDef_s {
    float minVal;
    float maxVal;
    float defVal;
    float range;
    int maxChars;
    qboolean maxCharsGotoNext;
    int maxPaintChars;
    int paintOffset;
};

struct entityHandler_t {
    fn_think think;
    fn_reached reached;
    fn_blocked blocked;
    fn_touch touch;
    fn_use use;
    fn_pain pain;
    fn_die die;
    fn_controller controller;
    int methodOfDeath;
    int splashMethodOfDeath;
};

struct entityShared_t {
    byte linked;
    byte bmodel;
    byte svFlags;
    int clientMask[2];
    byte inuse;
    int broadcastTime;
    vec3_t mins;
    vec3_t maxs;
    int contents;
    vec3_t absmin;
    vec3_t absmax;
    vec3_t currentOrigin;
    vec3_t currentAngles;
    int ownerNum;
    int eventTime;
};

struct field_t {
    int cursor;
    int scroll;
    int drawWidth;
    int widthInPixels;
    float charHeight;
    qboolean fixedSize;
    char buffer[256];
};

struct fileData_s {
    void *data;
    intptr_t next;
    byte type;
    char name[1];
};

struct fileInPack_s {
    long unsigned int pos;
    char *name;
    intptr_t next;
};

struct forward_iterator_tag {
    int _placeholder;
};

struct function_stack_t {
    const char *pos;
    unsigned int localId;
    unsigned int localVarCount;
    VariableValue_s *top;
    VariableValue_s *startTop;
};

struct function_frame_t {
    function_stack_t fs;
    int topType;
};

struct gameState_t {
    int stringOffsets[2048];
    char stringData[MAX_GAMESTATE_CHARS];
    int dataCount;
};

struct gameTypeInfo {
    const char *gameType;
    const char *gameTypeName;
};

struct gameTypeScript_t {
    char pszScript[64];
    char pszName[64];
    qboolean bTeamBased;
};

struct game_hudelem_field_t {
    const char *name;
    int ofs;
    fieldtype_t type;
    int mask;
    int shift;
    ScriptCallbackHudElem setter;
    ScriptCallbackHudElem getter;
};

struct gitem_s {
    char *classname;
    char *pickup_sound;
    char * world_model[2];
    char *icon;
    char *pickup_name;
    int quantity;
    itemType_t giType;
    int giTag;
    int giAmmoIndex;
    int giClipIndex;
};

struct grenade_ent_t {
    float time;
};

union hudelem_color_t {
    struct { byte r; byte g; byte b; byte a; };
    int rgba;
};

struct hudelem_s {
    he_type_t type;
    float x;
    float y;
    float z;
    float fontScale;
    int font;
    int alignOrg;
    int alignScreen;
    union hudelem_color_t color;
    hudelem_color_t fromColor;
    int fadeStartTime;
    int fadeTime;
    int label;
    int width;
    int height;
    int materialIndex;
    int fromWidth;
    int fromHeight;
    int scaleStartTime;
    int scaleTime;
    float fromX;
    float fromY;
    int fromAlignOrg;
    int fromAlignScreen;
    int moveStartTime;
    int moveTime;
    int time;
    int duration;
    float value;
    int text;
    float sort;
    qboolean foreground;
};

struct XAnimTree {
    DSkel * *reset_marker_reader;
    const XAnimTime read_markers;
    const int read_restart_marker;
    const XAnimParts_s *saw_SOI;
    const XAnimParts_s *saw_SOF;
    XAnimPart next_restart_num;
    float discarded_bytes;
};

struct game_hudelem_s {
    hudelem_t elem;
    int clientNum;
    int team;
    qboolean archived;
};

struct hunkHeader_t {
    unsigned int magic;
    int size;
    int dummy[2];
};

struct hunkUsed_t {
    int permanent;
    int temp;
};

struct indent_s {
    int type;
    int skip;
    script_t *script;
    intptr_t next;       /* pointer-as-int -> truncated on x64 (intptr_t==int on x86) */
};

struct inflate_blocks_state {
    inflate_block_mode mode;
    union { uInt left; struct { uInt table; uInt index; uIntf *blens; uInt bb; inflate_huft *tb; } trees; struct { inflate_codes_statef *codes; } decode; } sub;
    uInt last;
    uInt bitk;
    uLong bitb;
    inflate_huft *hufts;
    Bytef *window;
    Bytef *end;
    Bytef *read;
    Bytef *write;
    check_func checkfn;
    uLong check;
};

struct inflate_codes_state {
    inflate_codes_mode mode;
    uInt len;
    union { struct { inflate_huft *tree; uInt need; } code; uInt lit; struct { uInt get; uInt dist; } copy; } sub;
    Byte lbits;
    Byte dbits;
    inflate_huft *ltree;
    inflate_huft *dtree;
};

struct inflate_huft_s {
    union { struct { Byte Exop; Byte Bits; } what; uInt pad; } word;
    uInt base;
};

struct infoParm_t {
    char *name;
    int clearSolid;
    int surfaceFlags;
    int contents;
    int toolFlags;
};

struct input_iterator_tag {
    int _placeholder;
};

struct internal_state {
    inflate_mode mode;
    union { uInt method; struct { uLong was; uLong need; } check; uInt marker; } sub;
    int nowrap;
    uInt wbits;
    inflate_blocks_statef *blocks;
};

struct ipFilter_s {
    unsigned int mask;
    unsigned int compare;
};

union itemDefData_t {
    listBoxDef_t *listBox;
    editFieldDef_t *editField;
    multiDef_t *multi;
    const char *enumDvarName;
    void *data;
};

struct itemInfo_t {
    qboolean registered;
    struct XModel * models[2];
    MaterialHandle icons[4];
    snd_alias_list_t *pickupSound;
    snd_alias_list_t *ammoPickupSound;
};

struct item_ent_t {
    int count2;
    short unsigned int index;
};

struct jpeg_alloc {
    void * (*malloc)();
    void (*free)();
};

struct _D3DXCONSTANT_DESC {
    LPCSTR Name;
    D3DXREGISTER_SET RegisterSet;
    UINT RegisterIndex;
    UINT RegisterCount;
    D3DXPARAMETER_CLASS Class;
    D3DXPARAMETER_TYPE Type;
    UINT Rows;
    UINT Columns;
    UINT Elements;
    UINT StructMembers;
    UINT Bytes;
    LPCVOID DefaultValue;
};

struct _MEMORY_BASIC_INFORMATION {
    PVOID BaseAddress;
    PVOID AllocationBase;
    DWORD AllocationProtect;
    SIZE_T RegionSize;
    DWORD State;
    DWORD Protect;
    DWORD Type;
};

struct _OVERLAPPED {
    ULONG_PTR Internal;
    ULONG_PTR InternalHigh;
    union { struct { DWORD Offset; DWORD OffsetHigh; }; PVOID Pointer; };
    HANDLE hEvent;
};

struct _SECURITY_ATTRIBUTES {
    DWORD nLength;
    LPVOID lpSecurityDescriptor;
    BOOL bInheritHandle;
};

struct jpeg_c_coef_controller {
    void (*start_pass)();
    boolean (*compress_data)();
};

struct jpeg_c_main_controller {
    void (*start_pass)();
    void (*process_data)();
};

struct jpeg_c_prep_controller {
    void (*start_pass)();
    void (*pre_process_data)();
};

struct jpeg_color_converter {
    void (*start_pass)();
    void (*color_convert)();
};

struct jpeg_color_deconverter {
    void (*start_pass)();
    void (*color_convert)();
};

struct jpeg_color_quantizer {
    void (*start_pass)();
    void (*color_quantize)();
    void (*finish_pass)();
    void (*new_color_map)();
};

struct jpeg_common_struct {
    jpeg_error_mgr *err;
    jpeg_memory_mgr *mem;
    jpeg_progress_mgr *progress;
    void *client_data;
    boolean is_decompressor;
    int global_state;
    struct jpeg_alloc alloc;
};

struct jpeg_comp_master {
    void (*prepare_for_pass)();
    void (*pass_startup)();
    void (*finish_pass)();
    boolean call_pass_startup;
    boolean is_last_pass;
};

struct jpeg_component_info {
    int component_id;
    int component_index;
    int h_samp_factor;
    int v_samp_factor;
    int quant_tbl_no;
    int dc_tbl_no;
    int ac_tbl_no;
    JDIMENSION width_in_blocks;
    JDIMENSION height_in_blocks;
    int DCT_scaled_size;
    JDIMENSION downsampled_width;
    JDIMENSION downsampled_height;
    boolean component_needed;
    int MCU_width;
    int MCU_height;
    int MCU_blocks;
    int MCU_sample_width;
    int last_col_width;
    int last_row_height;
    JQUANT_TBL *quant_table;
    void *dct_table;
};

struct jpeg_compress_struct {
    jpeg_error_mgr *err;
    jpeg_memory_mgr *mem;
    jpeg_progress_mgr *progress;
    void *client_data;
    boolean is_decompressor;
    int global_state;
    struct jpeg_alloc alloc;
    jpeg_destination_mgr *dest;
    JDIMENSION image_width;
    JDIMENSION image_height;
    int input_components;
    J_COLOR_SPACE in_color_space;
    double input_gamma;
    int data_precision;
    int num_components;
    J_COLOR_SPACE jpeg_color_space;
    jpeg_component_info *comp_info;
    JQUANT_TBL * quant_tbl_ptrs[4];
    JHUFF_TBL * dc_huff_tbl_ptrs[4];
    JHUFF_TBL * ac_huff_tbl_ptrs[4];
    UINT8 arith_dc_L[16];
    UINT8 arith_dc_U[16];
    UINT8 arith_ac_K[16];
    int num_scans;
    const jpeg_scan_info *scan_info;
    boolean raw_data_in;
    boolean arith_code;
    boolean optimize_coding;
    boolean CCIR601_sampling;
    int smoothing_factor;
    J_DCT_METHOD dct_method;
    unsigned int restart_interval;
    int restart_in_rows;
    boolean write_JFIF_header;
    UINT8 JFIF_major_version;
    UINT8 JFIF_minor_version;
    UINT8 density_unit;
    UINT16 X_density;
    UINT16 Y_density;
    boolean write_Adobe_marker;
    JDIMENSION next_scanline;
    boolean progressive_mode;
    int max_h_samp_factor;
    int max_v_samp_factor;
    JDIMENSION total_iMCU_rows;
    int comps_in_scan;
    jpeg_component_info * cur_comp_info[4];
    JDIMENSION MCUs_per_row;
    JDIMENSION MCU_rows_in_scan;
    int blocks_in_MCU;
    int MCU_membership[10];
    int Ss;
    int Se;
    int Ah;
    int Al;
    jpeg_comp_master *master;
    jpeg_c_main_controller *main;
    jpeg_c_prep_controller *prep;
    jpeg_c_coef_controller *coef;
    jpeg_marker_writer *marker;
    jpeg_color_converter *cconvert;
    jpeg_downsampler *downsample;
    jpeg_forward_dct *fdct;
    jpeg_entropy_encoder *entropy;
    jpeg_scan_info *script_space;
    int script_space_size;
};

struct jpeg_d_coef_controller {
    void (*start_input_pass)();
    int (*consume_data)();
    void (*start_output_pass)();
    int (*decompress_data)();
    void (*coef_arrays)();
};

struct jpeg_d_main_controller {
    void (*start_pass)();
    void (*process_data)();
};

struct jpeg_d_post_controller {
    void (*start_pass)();
    void (*post_process_data)();
};

struct jpeg_decomp_master {
    void (*prepare_for_output_pass)();
    void (*finish_output_pass)();
    boolean is_dummy_pass;
};

struct jpeg_destination_mgr {
    JOCTET *next_output_byte;
    size_t free_in_buffer;
    void (*init_destination)();
    boolean (*empty_output_buffer)();
    void (*term_destination)();
};

struct jpeg_downsampler {
    void (*start_pass)();
    void (*downsample)();
    boolean need_context_rows;
};

struct jpeg_entropy_decoder {
    void (*start_pass)();
    boolean (*decode_mcu)();
    boolean insufficient_data;
};

struct jpeg_entropy_encoder {
    void (*start_pass)();
    boolean (*encode_mcu)();
    void (*finish_pass)();
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

struct jpeg_error_mgr {
    void (*error_exit)();
    void (*emit_message)();
    void (*output_message)();
    void (*format_message)();
    void (*reset_error_mgr)();
    int msg_code;
    union { int i[8]; char s[80]; } msg_parm;
    int trace_level;
    long int num_warnings;
    const const char * *jpeg_message_table;
    int last_jpeg_message;
    const const char * *addon_message_table;
    int first_addon_message;
    int last_addon_message;
    void (*exit)();
    void (*printf)();
};

struct jpeg_forward_dct {
    void (*start_pass)();
    void (*forward_DCT)();
};

struct jpeg_input_controller {
    int (*consume_input)();
    void (*reset_input_controller)();
    void (*start_input_pass)();
    void (*finish_input_pass)();
    boolean has_multiple_scans;
    boolean eoi_reached;
};

struct jpeg_inverse_dct {
    void (*start_pass)();
    inverse_DCT_method_ptr inverse_DCT[10];
};

struct jpeg_marker_reader {
    void (*reset_marker_reader)();
    int (*read_markers)();
    jpeg_marker_parser_method read_restart_marker;
    boolean saw_SOI;
    boolean saw_SOF;
    int next_restart_num;
    unsigned int discarded_bytes;
};

struct jpeg_marker_struct {
    int next;
    UINT8 marker;
    unsigned int original_length;
    unsigned int data_length;
    JOCTET *data;
};

struct jpeg_decompress_struct {
    jpeg_error_mgr *err;
    jpeg_memory_mgr *mem;
    jpeg_progress_mgr *progress;
    void *client_data;
    boolean is_decompressor;
    int global_state;
    jpeg_alloc alloc;
    jpeg_source_mgr *src;
    JDIMENSION image_width;
    JDIMENSION image_height;
    int num_components;
    J_COLOR_SPACE jpeg_color_space;
    J_COLOR_SPACE out_color_space;
    unsigned int scale_num;
    unsigned int scale_denom;
    double output_gamma;
    boolean buffered_image;
    boolean raw_data_out;
    J_DCT_METHOD dct_method;
    boolean do_fancy_upsampling;
    boolean do_block_smoothing;
    boolean quantize_colors;
    J_DITHER_MODE dither_mode;
    boolean two_pass_quantize;
    int desired_number_of_colors;
    boolean enable_1pass_quant;
    boolean enable_external_quant;
    boolean enable_2pass_quant;
    JDIMENSION output_width;
    JDIMENSION output_height;
    int out_color_components;
    int output_components;
    int rec_outbuf_height;
    int actual_number_of_colors;
    JSAMPARRAY colormap;
    JDIMENSION output_scanline;
    int input_scan_number;
    JDIMENSION input_iMCU_row;
    int output_scan_number;
    JDIMENSION output_iMCU_row;
    int (*coef_bits)[64];
    JQUANT_TBL * quant_tbl_ptrs[4];
    JHUFF_TBL * dc_huff_tbl_ptrs[4];
    JHUFF_TBL * ac_huff_tbl_ptrs[4];
    int data_precision;
    jpeg_component_info *comp_info;
    boolean progressive_mode;
    boolean arith_code;
    UINT8 arith_dc_L[16];
    UINT8 arith_dc_U[16];
    UINT8 arith_ac_K[16];
    unsigned int restart_interval;
    boolean saw_JFIF_marker;
    UINT8 JFIF_major_version;
    UINT8 JFIF_minor_version;
    UINT8 density_unit;
    UINT16 X_density;
    UINT16 Y_density;
    boolean saw_Adobe_marker;
    UINT8 Adobe_transform;
    boolean CCIR601_sampling;
    jpeg_saved_marker_ptr marker_list;
    int max_h_samp_factor;
    int max_v_samp_factor;
    int min_DCT_scaled_size;
    JDIMENSION total_iMCU_rows;
    JSAMPLE *sample_range_limit;
    int comps_in_scan;
    jpeg_component_info * cur_comp_info[4];
    JDIMENSION MCUs_per_row;
    JDIMENSION MCU_rows_in_scan;
    int blocks_in_MCU;
    int MCU_membership[10];
    int Ss;
    int Se;
    int Ah;
    int Al;
    int unread_marker;
    jpeg_decomp_master *master;
    jpeg_d_main_controller *main;
    jpeg_d_coef_controller *coef;
    jpeg_d_post_controller *post;
    jpeg_input_controller *inputctl;
    jpeg_marker_reader *marker;
    jpeg_entropy_decoder *entropy;
    jpeg_inverse_dct *idct;
    jpeg_upsampler *upsample;
    jpeg_color_deconverter *cconvert;
    jpeg_color_quantizer *cquantize;
};

struct bitread_working_state {
    const JOCTET *next_input_byte;
    size_t bytes_in_buffer;
    bit_buf_type get_buffer;
    int bits_left;
    j_decompress_ptr cinfo;
};

struct jpeg_marker_writer {
    void (*write_file_header)();
    void (*write_frame_header)();
    void (*write_scan_header)();
    void (*write_file_trailer)();
    void (*write_tables_only)();
    void (*write_marker_header)();
    void (*write_marker_byte)();
};

struct jpeg_memory_mgr {
    void * (*alloc_small)();
    void * (*alloc_large)();
    JSAMPARRAY (*alloc_sarray)();
    JBLOCKARRAY (*alloc_barray)();
    jvirt_sarray_ptr (*request_virt_sarray)();
    jvirt_barray_ptr (*request_virt_barray)();
    void (*realize_virt_arrays)();
    JSAMPARRAY (*access_virt_sarray)();
    JBLOCKARRAY (*access_virt_barray)();
    void (*free_pool)();
    void (*self_destruct)();
    long int max_memory_to_use;
    long int max_alloc_chunk;
};

struct jpeg_progress_mgr {
    void (*progress_monitor)();
    long int pass_counter;
    long int pass_limit;
    int completed_passes;
    int total_passes;
};

struct jpeg_scan_info {
    int comps_in_scan;
    int component_index[4];
    int Ss;
    int Se;
    int Ah;
    int Al;
};

struct jpeg_source_mgr {
    const JOCTET *next_input_byte;
    size_t bytes_in_buffer;
    void (*init_source)();
    boolean (*fill_input_buffer)();
    void (*skip_input_data)();
    boolean (*resync_to_restart)();
    void (*term_source)();
};

struct jpeg_upsampler {
    void (*start_pass)();
    void (*upsample)();
    boolean need_context_rows;
};

struct jvirt_barray_control {
    JBLOCKARRAY mem_buffer;
    JDIMENSION rows_in_array;
    JDIMENSION blocksperrow;
    JDIMENSION maxaccess;
    JDIMENSION rows_in_mem;
    JDIMENSION rowsperchunk;
    JDIMENSION cur_start_row;
    JDIMENSION first_undef_row;
    boolean pre_zero;
    boolean dirty;
    boolean b_s_open;
    int next;
    backing_store_info b_s_info;
};

struct jvirt_sarray_control {
    JSAMPARRAY mem_buffer;
    JDIMENSION rows_in_array;
    JDIMENSION samplesperrow;
    JDIMENSION maxaccess;
    JDIMENSION rows_in_mem;
    JDIMENSION rowsperchunk;
    JDIMENSION cur_start_row;
    JDIMENSION first_undef_row;
    boolean pre_zero;
    boolean dirty;
    boolean b_s_open;
    int next;
    backing_store_info b_s_info;
};

struct kbutton_t {
    int down[2];
    unsigned int downtime;
    unsigned int msec;
    Bool active;
    Bool wasPressed;
};

struct keyname_t {
    char *name;
    int keynum;
};

struct keywordHash_s {
    char *keyword;
    qboolean (*func)();
    intptr_t next;       /* pointer-as-int -> truncated on x64 (intptr_t==int on x86) */
};

struct lagometer_t {
    int frameSamples[128];
    int frameCount;
    int snapshotFlags[128];
    int snapshotSamples[128];
    int snapshotCount;
};

struct languageInfo_t {
    const char *pszName;
    qboolean bPresent;
};

union large_pool_struct {
    int hdr;
    double dummy;
};

struct leafList_s {
    int count;
    int maxcount;
    qboolean overflowed;
    int *list;
    vec3_t bounds[2];
    int lastLeaf;
};

struct lerpFrame_t {
    float yawAngle;
    qboolean yawing;
    float pitchAngle;
    qboolean pitching;
    int animationNumber;
    animation_t *animation;
    int animationTime;
    vec3_t oldFramePos;
    float animSpeedScale;
    int oldFrameSnapshotTime;
};

struct clientInfo_s {
    qboolean infoValid;
    qboolean nextValid;
    int clientNum;
    char name[32];
    team_t team;
    team_t oldteam;
    int score;
    int location;
    int health;
    char model[64];
    char attachModelNames[6][64];
    char attachTagNames[6][64];
    struct lerpFrame_t legs;
    lerpFrame_t torso;
    float lerpMoveDir;
    float lerpLean;
    vec3_t playerAngles;
    qboolean leftHandGun;
    qboolean dobjDirty;
    vec3_t angles[6];
    vec3_t tag_origin_angles;
    vec3_t tag_origin_offset;
    int clientConditions[9][2];
    struct XAnimTree_s *pXAnimTree;
    int iDObjWeapon;
    int stanceTransitionTime;
    int turnAnimEndTime;
    char turnAnimType;
};

struct corpseInfo_t {
    struct XAnimTree_s *tree;
    int entnum;
    int time;
    struct clientInfo_s ci;
    Bool falling;
};

struct less_UINT32 {
    int _placeholder;
};

struct lightGlob_type {
    int defCount;
    GfxLightDef * defs[64];
};

struct listBoxDef_s {
    int startPos[4];
    int endPos[4];
    int drawPadding;
    int cursorPos[4];
    float elementWidth;
    float elementHeight;
    int elementStyle;
    int numColumns;
    columnInfo_t columnInfo[16];
    const char *doubleClick;
    qboolean notselectable;
    qboolean noScrollBars;
};

struct listColumnInfo_t {
    listColumnTypes_t type;
    float fWidth;
    const char *pszName;
    int iAlignment;
};

struct loadAssets_t {
    snd_alias_list_t *menuEnterSound;
    snd_alias_list_t *menuExitSound;
    snd_alias_list_t *menuBuzzSound;
    float fadeClamp;
    int fadeCycle;
    float fadeAmount;
    float fadeInAmount;
    float shadowX;
    float shadowY;
    vec4_t shadowColor;
    float shadowFadeClamp;
};

struct locTraceWork_t {
    int contents;
    TraceExtents extents;
};

struct loopmsg_t {
    byte data[1400];
    int datalen;
    int port;
};

struct loopback_t {
    loopmsg_t msgs[16];
    int get;
    int send;
};

struct ltp_params {
    const signed char *gain_cdbk;
    int gain_bits;
    int pitch_bits;
};

struct lump_t {
    int filelen;
    int fileofs;
};

struct dheader_s {
    int ident;
    int version;
    lump_t lumps[39];
};

struct mapInfo {
    const char *mapName;
    const char *mapLoadName;
    const char *imageName;
    const char *opponentName;
    int teamMembers;
    int typeBits;
    int cinematic;
    int timeToBeat[32];
    MaterialHandle levelShot;
    qboolean active;
};

struct materialCommands_t {
    union { union { GfxVertex generic[5450]; GfxWorldVertex world[5450]; } dx9; union { GfxVertexDx7 generic[5450]; GfxWorldVertexDx7 world[5450]; } dx7; } verts;
    byte reserved[8];
    r_index_t *indices;
    r_index_t *optimizedIndices;
    GfxOptimizedVertexSource optimizedVertexSource;
    const Material *material;
    MaterialTechniqueType techType;
    int lmapIndex;
    float materialTime;
    MaterialVertexDeclType declType;
    int indexCount;
    int vertexCount;
    int firstVertex;
    int lastVertex;
    int optimizedIndexCount;
    int optimizedVertexCount;
    int firstOptimizedVertex;
};

struct mnode_leaf_t {
    int cluster;
};

struct mnode_node_t {
    cplane_t *plane;
    mnode_t * children[2];
};

union mnode_info_t {
    struct mnode_node_t node;
    struct mnode_leaf_t leaf;
};

struct mnode_t {
    int contents;
    int parent;
    int cellIndex;
    union mnode_info_t u;
};

struct modInfo_t {
    const char *modName;
    const char *modDescr;
};

struct moveclip_t {
    vec3_t mins;
    vec3_t maxs;
    vec3_t outerSize;
    TraceExtents extents;
    int passEntityNum;
    int passOwnerNum;
    int contentmask;
};

struct mover_ent_t {
    float decelTime;
    float aDecelTime;
    float speed;
    float aSpeed;
    float midTime;
    float aMidTime;
    vec3_t pos1;
    vec3_t pos2;
    vec3_t pos3;
    vec3_t apos1;
    vec3_t apos2;
    vec3_t apos3;
};

struct msg_t {
    qboolean overflowed;
    byte *data;
    int maxsize;
    int cursize;
    int readcount;
    int bit;
};

struct multiDef_s {
    const char * dvarList[32];
    const char * dvarStr[32];
    float dvarValue[32];
    int count;
    qboolean strDef;
};

struct my_coef_controller {
    jpeg_d_coef_controller pub;
    JDIMENSION MCU_ctr;
    int MCU_vert_offset;
    int MCU_rows_per_iMCU_row;
    JBLOCKROW MCU_buffer[10];
    jvirt_barray_ptr whole_image[10];
    int *coef_bits_latch;
};

struct my_color_converter {
    jpeg_color_converter pub;
    INT32 *rgb_ycc_tab;
};

struct my_color_deconverter {
    jpeg_color_deconverter pub;
    int *Cr_r_tab;
    int *Cb_b_tab;
    void (*Cr_g_tab)();
    void (*Cb_g_tab)();
};

struct my_comp_master {
    jpeg_comp_master pub;
    c_pass_type pass_type;
    int pass_number;
    int total_passes;
    int scan_number;
};

struct my_cquantizer {
    jpeg_color_quantizer pub;
    JSAMPARRAY sv_colormap;
    int sv_actual;
    JSAMPARRAY colorindex;
    boolean is_padded;
    int Ncolors[4];
    int row_index;
    ODITHER_MATRIX_PTR odither[4];
    FSERRPTR fserrors[4];
    boolean on_odd_row;
};

struct my_decomp_master {
    jpeg_decomp_master pub;
    int pass_number;
    boolean using_merged_upsample;
    jpeg_color_quantizer *quantizer_1pass;
    jpeg_color_quantizer *quantizer_2pass;
};

struct my_destination_mgr {
    struct jpeg_destination_mgr pub;
    byte *outfile;
    int size;
};

struct my_downsampler {
    jpeg_downsampler pub;
    downsample1_ptr methods[10];
};

struct my_fdct_controller {
    jpeg_forward_dct pub;
    forward_DCT_method_ptr do_dct;
    jpeg_component_info * (*divisors[4])();
    float_DCT_method_ptr do_float_dct;
    float * float_divisors[4];
};

struct my_idct_controller {
    jpeg_inverse_dct pub;
    int cur_method[10];
};

struct my_input_controller {
    jpeg_input_controller pub;
    boolean inheaders;
};

struct my_main_controller {
    jpeg_d_main_controller pub;
    JSAMPARRAY buffer[10];
    boolean buffer_full;
    JDIMENSION rowgroup_ctr;
    JSAMPIMAGE xbuffer[2];
    int whichptr;
    int context_state;
    JDIMENSION rowgroups_avail;
    JDIMENSION iMCU_row_ctr;
};

struct my_marker_reader {
    jpeg_marker_reader pub;
    jpeg_marker_parser_method process_COM;
    jpeg_marker_parser_method process_APPn[16];
    unsigned int length_limit_COM;
    unsigned int length_limit_APPn[16];
    jpeg_saved_marker_ptr cur_marker;
    unsigned int bytes_read;
};

struct my_marker_writer {
    jpeg_marker_writer pub;
    unsigned int last_restart_interval;
};

struct my_post_controller {
    jpeg_d_post_controller pub;
    jvirt_sarray_ptr whole_image;
    JSAMPARRAY buffer;
    JDIMENSION strip_height;
    JDIMENSION starting_row;
    JDIMENSION next_row;
};

struct my_prep_controller {
    jpeg_c_prep_controller pub;
    JSAMPARRAY color_buf[10];
    JDIMENSION rows_to_go;
    int next_buf_row;
    int this_row_group;
    int next_buf_stop;
};

struct my_source_mgr {
    jpeg_source_mgr pub;
    boolean start_of_file;
};

struct my_upsampler {
    jpeg_upsampler pub;
    void (*upmethod)();
    int *Cr_r_tab;
    int *Cb_b_tab;
    J_DCT_METHOD (*Cr_g_tab)[8];
    J_DCT_METHOD (*Cb_g_tab)[8];
    JSAMPROW spare_row;
    boolean spare_full;
    JDIMENSION out_row_width;
    JDIMENSION rows_to_go;
};

struct ent_field_t {
    const char *name;
    int ofs;
    fieldtype_t type;
    ScriptCallbackEnt callback;
};

struct netProfilePacket_t {
    int iTime;
    int iSize;
    qboolean bFragment;
};

struct netProfileStream_t {
    netProfilePacket_t packets[60];
    int iCurrPacket;
    int iBytesPerSecond;
    int iLastBPSCalcTime;
    int iCountedPackets;
    int iCountedFragments;
    int iFragmentPercentage;
    int iLargestPacket;
    int iSmallestPacket;
};

struct IDirect3DBaseTexture9 {
    struct netProfileStream_t send;
    const D3DRECT recieve;
};

struct netProfileInfo_s {
    struct netProfileStream_t send;
    netProfileStream_t recieve;
};

struct netadr_t {
    netadrtype_t type;
    byte ip[4];
    short unsigned int port;
};

struct challenge_t {
    netadr_t adr;
    int challenge;
    int time;
    int pingTime;
    int firstTime;
    int firstPing;
    qboolean connected;
    int guid;
};

struct netchan_t {
    int outgoingSequence;
    netsrc_t sock;
    int incomingSequence;
    int dropped;
    struct netadr_t remoteAddress;
    int qport;
    int fragmentSequence;
    int fragmentLength;

    byte fragmentBuffer[MAX_MSGLEN];
    qboolean unsentFragments;
    int unsentFragmentStart;
    int unsentLength;
    byte unsentBuffer[MAX_MSGLEN];
    netProfileInfo_t *pProf;
};

struct clientConnection_t {
    connstate_t state;
    int qport;
    int clientNum;
    int lastPacketSentTime;
    int lastPacketTime;
    netadr_t serverAddress;
    int connectTime;
    int connectPacketCount;
    char serverMessage[256];
    int challenge;
    int checksumFeed;
    int reliableSequence;
    int reliableAcknowledge;
    char reliableCommands[128][1024];
    int serverMessageSequence;
    int serverCommandSequence;
    int lastExecutedServerCommand;
    char serverCommands[128][1024];
    fileHandle_t download;
    char downloadTempName[256];
    char downloadName[256];
    int downloadNumber;
    int downloadBlock;
    int downloadCount;
    int downloadSize;
    char downloadList[1024];
    qboolean downloadRestart;
    char demoName[64];
    qboolean demorecording;
    qboolean demoplaying;
    qboolean isTimeDemo;
    qboolean demowaiting;
    qboolean firstDemoFrameSkipped;
    fileHandle_t demofile;
    fileHandle_t timeDemoLog;
    int timeDemoFrames;
    int timeDemoStart;
    int timeDemoPrev;
    int timeDemoBaseTime;
    struct netchan_t netchan;
    netProfileInfo_t *pOOBProf;
};

struct new_allocator_CCacheInfoBlock {
    int _placeholder;
};

struct new_allocator_CDirect3DSurfaceptr {
    int _placeholder;
};

struct new_allocator_CDirect3DVolumeptr {
    int _placeholder;
};

struct new_allocator_CDisplayInfo {
    int _placeholder;
};

struct new_allocator_CFenceptr {
    int _placeholder;
};

struct new_allocator_CResInfo {
    int _placeholder;
};

struct new_allocator_CSoundObjectptr {
    int _placeholder;
};

struct new_allocator_CStreamSoundptr {
    int _placeholder;
};

struct new_allocator_FFItem {
    int _placeholder;
};

struct new_allocator_FSRef {
    int _placeholder;
};

struct new_allocator_GLuint {
    int _placeholder;
};

struct new_allocator_GLuintptr {
    int _placeholder;
};

struct new_allocator_MacRect {
    int _placeholder;
};

struct new_allocator_Tuple {
    int _placeholder;
};

struct new_allocator_UINT16 {
    int _placeholder;
};

struct new_allocator_UINT16ptr {
    int _placeholder;
};

struct new_allocator_UINT32 {
    int _placeholder;
};

struct new_allocator_UINT8 {
    int _placeholder;
};

struct new_allocator_WinCursor {
    int _placeholder;
};

struct new_allocator_char {
    int _placeholder;
};

struct new_allocator_float {
    int _placeholder;
};

struct nodetype {
    intptr_t left;
    intptr_t right;
    intptr_t parent;
    intptr_t next;
    intptr_t prev;
    intptr_t head;
    int weight;
    int symbol;
};

struct huff_t {
    int blocNode;
    int blocPtrs;
    node_t *tree;
    node_t *lhead;
    node_t *ltail;
    node_t * loc[257];
    node_t * *freelist;
    node_t nodeList[768];
    node_t * nodePtrs[768];
};

struct huffman_t {
    struct huff_t compressor;
    huff_t decompressor;
};

struct objective_t {
    objectiveState_t state;
    vec3_t origin;
    int entNum;
    int teamNum;
    int icon;
};

struct operator_s {
    int op;
    int priority;
    int parentheses;
    int prev;
    int next;
};

struct outPacket_t {
    int p_cmdNumber;
    int p_serverTime;
    int p_realtime;
};

struct pack_t {
    char iwdFilename[256];
    char iwdBasename[256];
    char iwdGamename[256];
    unzFile handle;
    int checksum;
    int pure_checksum;
    int numfiles;
    byte referenced;
    int hashSize;
    fileInPack_t * *hashTable;
    fileInPack_t *buildBuffer;
};

struct pair_UINT32_CVAOPacket {
    UINT32 first;
    CVAOPacket second;
};

struct pair_constUINT32_CVAOPacket {
    const UINT32 first;
    CVAOPacket second;
};

struct parseInfo_t {
    char token[1024];
    int lines;
    Bool ungetToken;
    Bool spaceDelimited;
    Bool keepStringQuotes;
    Bool csv;
    Bool negativeNumbers;
    const char *errorPrefix;
    const char *warningPrefix;
    int backup_lines;
    const char *backup_text;
    char parseFile[64];
};

struct ParseThreadInfo {
    parseInfo_t parseInfo[16];
    int parseInfoNum;
    const char *prevTokenPos;
    const char *tokenPos;
    char line[1024];
};

struct pc_token_s {
    int type;
    int subtype;
    int intvalue;
    float floatvalue;
    char string[1024];
};

struct pendingServer_t {
    char adrstr[64];
    char name[64];
    int startTime;
    int serverNum;
    qboolean valid;
};

struct pendingServerStatus_t {
    int num;
    pendingServer_t server[16];
};

struct phuff_entropy_encoder {
    jpeg_entropy_encoder pub;
    boolean gather_statistics;
    JOCTET *next_output_byte;
    size_t free_in_buffer;
    INT32 put_buffer;
    int put_bits;
    j_compress_ptr cinfo;
    int last_dc_val[4];
    int ac_tbl_no;
    unsigned int EOBRUN;
    unsigned int BE;
    char *bit_buffer;
    unsigned int restarts_to_go;
    int next_restart_num;
    void (*derived_tbls[4])();
    long int * count_ptrs[4];
};

struct ping_t {
    netadr_t adr;
    int start;
    int time;
    char info[1024];
};

struct pinglist_t {
    char adrstr[64];
    int start;
};

struct playerEntity_t {
    float fLastWeaponPosFrac;
    qboolean bPositionToADS;
    vec3_t vPositionLastOrg;
    float fLastIdleFactor;
    vec3_t vLastMoveOrg;
    vec3_t vLastMoveAng;
};

struct playerState_s {
    int commandTime;
    int pm_type;
    int bobCycle;
    int pm_flags;
    int pm_time;
    vec3_t origin;
    vec3_t velocity;
    vec2_t oldVelocity;
    int weaponTime;
    int weaponDelay;
    int grenadeTimeLeft;
    int weaponRestrictKickTime;
    int foliageSoundTime;
    int gravity;
    float leanf;
    int speed;
    int delta_angles[3];
    int groundEntityNum;
    vec3_t vLadderVec;
    int jumpTime;
    float jumpOriginZ;
    int legsTimer;
    int legsAnim;
    int torsoTimer;
    int torsoAnim;
    int legsAnimDuration;
    int torsoAnimDuration;
    int damageTimer;
    int damageDuration;
    int flinchYaw;
    int movementDir;
    int eFlags;
    int eventSequence;
    int events[4];
    int eventParms[4];
    int oldEventSequence;
    int clientNum;
    int offHandIndex;
    int weapon;
    int weaponstate;
    float fWeaponPosFrac;
    int adsDelayTime;
    int viewmodelIndex;
    vec3_t viewangles;
    int viewHeightTarget;
    float viewHeightCurrent;
    int viewHeightLerpTime;
    int viewHeightLerpTarget;
    qboolean viewHeightLerpDown;
    float viewHeightLerpPosAdj;
    vec2_t viewAngleClampBase;
    vec2_t viewAngleClampRange;
    int damageEvent;
    int damageYaw;
    int damagePitch;
    int damageCount;
    int stats[6];
    int ammo[128];
    int ammoclip[128];
    int weapons[4];
    char weaponslots[8];
    int weaponrechamber[4];
    vec3_t mins;
    vec3_t maxs;
    float proneDirection;
    float proneDirectionPitch;
    float proneTorsoPitch;
    int viewlocked;
    int viewlocked_entNum;
    int cursorHint;
    int cursorHintString;
    int cursorHintEntIndex;
    int iCompassFriendInfo;
    float fTorsoHeight;
    float fTorsoPitch;
    float fWaistPitch;
    float holdBreathScale;
    int holdBreathTimer;
    struct MantleState mantleState;
    int entityEventSequence;
    int weapAnim;
    float aimSpreadScale;
    int shellshockIndex;
    int shellshockTime;
    int shellshockDuration;
    objective_t objective[16];
    int deltaTime;
    struct { hudelem_t current[31]; hudelem_t archival[31]; } hud;
};

struct cachedClient_s {
    qboolean playerStateExists;
    clientState_t cs;
    playerState_t ps;
};

struct clSnapshot_t {
    qboolean valid;
    int snapFlags;
    int serverTime;
    int messageNum;
    int deltaNum;
    int ping;
    int cmdNum;
    playerState_t ps;
    int numEntities;
    int numClients;
    int parseEntitiesNum;
    int parseClientsNum;
    int serverCommandNum;
};

struct clientSnapshot_t {
    playerState_t ps;
    int num_entities;
    int num_clients;
    int first_entity;
    int first_client;
    int messageSent;
    int messageAcked;
    int messageSize;
};

struct playerTeamState_t {
    int location;
};

struct pmoveHandler_t {
    pmove_trace trace;
    pmove_pointcontents pointcontents;
    pmove_PlayerEvent playerEvent;
};

struct pointtrace_t {
    TraceExtents extents;
    int passEntityNum;
    int passOwnerNum;
    int contentmask;
    qboolean bLocational;
    unsigned char *priorityMap;
};

struct punctuation_s {
    char *p;
    int n;
    intptr_t next;   /* linked-list ptr (was int -> truncated on x64; intptr_t==int on x86) */
};

struct pushed_t {
    gentity_t *ent;
    vec3_t origin;
    vec3_t angles;
    float deltayaw;
};

union qfile_gus {
    FILE *o;
    unzFile z;
};

struct qfile_us {
    qfile_gut file;
    qboolean unique;
};

struct fileHandleData_t {
    qfile_ut handleFiles;
    qboolean handleSync;
    int fileSize;
    int zipFilePos;
    pack_t *zipFile;
    qboolean streamed;
    char name[256];
};

struct qkey_t {
    qboolean down;
    int repeats;
    char *binding;
};

struct PlayerKeyState {
    field_t chatField;
    qboolean chat_team;
    qboolean key_overstrikeMode;
    qboolean anykeydown;
    qkey_t keys[256];
};

struct qtime_s {
    int tm_sec;
    int tm_min;
    int tm_hour;
    int tm_mday;
    int tm_mon;
    int tm_year;
    int tm_wday;
    int tm_yday;
    int tm_isdst;
};

struct r_backEndGlobals_t {
    vec4_t codeConsts[59];
    int frameCount;
    int viewCount;
    GfxSceneDef sceneDef;
    const GfxViewParms *viewParms;
    GfxLodParms lodParms;
    GfxViewport sceneViewport;
    D3DMATRIX viewProjectionMatrix;
    int tileIndex;
    int tileCount;
    int width;
    int height;
    GfxEntity *currentEntity;
    GfxEntityLighting *currentEntityLighting;
    GfxEntity worldEntity;
    Bool viewportIsDirty;
    Bool projection2D;
    GfxSamplerState defaultSamplerState;
    GfxColor color_axis;
    GfxColor color_allies;
    GfxColor fogColor;
    int glowIndexFirst;
    int glowCount;
    GfxImage * glowImage[2];
    float texScrollAmountDx7;
    GfxCodeMatrices codeMatrixStack[3];
    int codeMatrixStackLevel;
    GfxRenderTargetId resolvedPostSunTarget;
    GfxRenderTargetId resolvedSceneTarget;
    GfxImage *currentFeedbackImage;
    GfxBackEndLightSettings light[2];
    GfxEntityLighting entityLighting[2046];
    D3DMATRIX shadowLookupMatrix;
    FontHandle debugFont;
};

struct r_global_permanent_t {
    qboolean materialLoaded;
    int materialCount;
    Material * sortedMaterials[1024];
    GfxImage *whiteImage;
    GfxImage *blackImage;
    GfxImage *identityNormalMapImage;
    GfxImage *specularityImage;
    GfxImage *lightmapWeightsImage;
    GfxImage * lightGridWeightsImage[2];
    GfxImage *outdoorImage;
    GfxLightDef *dlightDef;
    Material *defaultMaterial;
    Material *rawMaterial;
    Material *stencilPlaneMaterial;
    Material *whiteMaterial;
    Material *additiveMaterial;
    Material *pointMaterial;
    Material *lineMaterial;
    Material *clearAlphaStencilMaterial;
    Material *shadowClearMaterial;
    Material *shadowCookieOverlayMaterial;
    Material *shadowCookieBlurMaterial;
    Material *glareBlindMaterial;
    Material *normalFillMaterial;
    Material *alphaBlendFillMaterial;
    Material *alphaTestPassFillMaterial;
    Material *alphaTestFailFillMaterial;
    Material *additiveFillMaterial;
    Material *depthOnlyFillMaterial;
    Material *stencilOneSideKeepFillMaterial;
    Material *stencilOneSideRejectFillMaterial;
    Material *stencilTwoSideKeepFillMaterial;
    Material *stencilTwoSideRejectFillMaterial;
    Material *phongBumpFillMaterial;
    Material *phongBumpSpecFillMaterial;
    Material *colorChannelMixerMaterial;
    Material *frameColorDebugMaterial;
    Material *frameAlphaDebugMaterial;
    GfxImage *rawImage;
    GfxWorld *world;
    GfxImage *sunHalfAngleImage;
    GfxImage *waterColorImage;
    Material *feedbackReplaceMaterial;
    Material *feedbackBlendMaterial;
    Material * symmetricFilterMaterial[8];
    Material *shellShockMaterial;
    Material *glowSetupMaterial;
    Material *glowApplySkyBleedMaterial;
    Material *glowApplyBloomMaterial;
    int savedScreenTime;
    MaterialTextureDef rawTexdef;
};

struct r_globals_load_t {
    GfxOccluder * *occluderIndices;
    int *cullGroupIndices;
    GfxOccluder *occluders;
    vec3_t *portalVerts;
    GfxAabbTree *aabbTrees;
    int aabbTreeCount;
};

struct r_ib_state_t {
    int used;
    int total;
    IDirect3DIndexBuffer9 *buffer;
};

struct r_lightmapGroup_t {
    int wideCount;
    int highCount;
};

struct r_lightmapMerge_t {
    short int index;
    float shift[2];
    float scale[2];
};

struct GfxBspLoad {
    const dheader_t *header;
    const byte *fileBase;
    int fileSize;
    const dmaterial_t *materials;
    int materialCount;
    r_lightmapMerge_t lmapMergeInfo[32];
};

struct r_vb_state_t {
    int used;
    int total;
    void (*buffer)();
};

struct random_access_iterator_tag {
    int _placeholder;
};

struct rectDef_s {
    float x;
    float y;
    float w;
    float h;
    int horzAlign;
    int vertAlign;
};

struct Window {
    UiRectangle rect[4];
    UiRectangle rectClient[4];
    const char *name;
    const char *group;
    const char *cinematicName;
    int cinematic;
    int style;
    int border;
    int ownerDraw;
    int ownerDrawFlags;
    float borderSize;
    int staticFlags;
    int dynamicFlags[4];
    UiRectangle rectEffects0[4];
    UiRectangle rectEffects1[4];
    int offsetTime[4];
    int nextTime;
    vec4_t foreColor;
    vec4_t backColor;
    vec4_t borderColor;
    vec4_t outlineColor;
    MaterialHandle background;
};

struct itemDef_s {
    Window window;
    UiRectangle textRect[4];
    int type;
    int dataType;
    int alignment;
    int fontEnum;
    int textalignment;
    float textalignx;
    float textaligny;
    float textscale;
    int textStyle;
    const char *text;
    qboolean textSavegameInfo;
    menuDef_t *parent;
    const char *mouseEnterText;
    const char *mouseExitText;
    const char *mouseEnter;
    const char *mouseExit;
    const char *action;
    const char *onAccept;
    const char *onFocus;
    const char *leaveFocus;
    const char *dvar;
    const char *dvarTest;
    ItemKeyHandler *onKey;
    const char *enableDvar;
    int dvarFlags;
    snd_alias_list_t *focusSound;
    float special;
    int cursorPos[4];
    union itemDefData_t typeData;
    int imageTrack;
};

struct menuDef_t {
    struct Window window;
    const char *font;
    qboolean fullScreen;
    int itemCount;
    int fontIndex;
    int cursorItem[4];
    int fadeCycle;
    float fadeClamp;
    float fadeAmount;
    float fadeInAmount;
    float blurRadius;
    const char *onOpen;
    const char *onClose;
    const char *onESC;
    ItemKeyHandler *onKey;
    const char *soundName;
    int imageTrack;
    vec4_t focusColor;
    vec4_t disableColor;
    itemDef_t * *items;
};

struct refdef_s {
    int x;
    int y;
    int width;
    int height;
    float fov_x;
    float fov_y;
    vec3_t vieworg;
    vec3_t viewaxis[3];
    int time;
    float zNear;
    float blurRadius;
    int viewIndex;
};

struct refexport_t {
    void (*Shutdown)(void);
    void (*BeginRegistration)(void *vidConfigOut);
    struct XModel * (*RegisterModel)(const char *name);
    GfxBrushModel * (*RegisterInlineModel)(int modelIndex);
    MaterialHandle (*RegisterMaterial)(const char *name, int baseImageFlags, int imageTrack);
    MaterialHandle (*RegisterRawImage)(const char *name, int baseImageFlags, int imageTrack);
    Bool (*Material_IsDefault)(MaterialHandle handle);
    void (*LoadWorld)(const char *name, int *checksum);
    void (*GetWorldBounds)(vec_t *min, vec_t *max);
    void (*FinishLoadingModels)(void);
    void (*SetIgnorePrecacheErrors)(qboolean ignore);
    qboolean (*GetIgnorePrecacheErrors)(void);
    int (*GetMinSpecImageMemory)(void);
    const char * (*GetMaterialName)(MaterialHandle handle);
    int (*GetMaterialSubimageCount)(MaterialHandle handle);
    Bool (*IsMaterialRefractive)(MaterialHandle handle);
    float (*GetFarPlaneDist)(void);
    void (*EndRegistration)(void);
    void (*ClearScene)(void);
    void (*SkinGfxEntityDelayed)(GfxEntity *ent);
    GfxEntity * (*AddRefEntityToScene)(const GfxEntity *refEnt, GfxModel sceneModel, const centity_t *cent);
    void (*UpdateXModelBoundsDelayed)(GfxEntity *ent);
    void (*UpdateEffectsBolt)(void);
    void (*UpdateEffectsNonBolt)(void);
    void (*DefaultVertexFrames)(int vertCount, GfxWorldVertex *verts);
    void (*AddPolyToScene)(MaterialHandle materialHandle, unsigned short lmapIndex, unsigned short vertCount, const GfxWorldVertex *verts);
    void (*AddLightToScene)(const vec_t *org, float radius, float r, float g, float b);
    void (*InterpretSunLightParseParams)();
    void (*ResetSunLightParseParams)(void);
    void (*SetCullDist)(float dist);
    void (*SetFog)(int fogvar, float start, float end, int r, int g, int b, float density);
    void (*SwitchFog)(int fogvar, int startTime, int transitionTime);
    void (*ArchiveFogState)(void *memFile);
    void (*ClearFogs)(void);
    void (*SetSunLightOverride)(const vec_t *sunColor);
    void (*ResetSunLightOverride)(void);
    void (*RenderScene)(const refdef_t *refdef);
    int (*BeginDelayedDrawing)(void);
    void (*EndDelayedDrawing)(int marker);
    void (*IssueDelayedDrawing)(int marker);
    void (*ClearFlares)(void);
    void (*SetMaterialColor)(const vec_t *color);
    void (*DrawStretchPic)(float x, float y, float w, float h, float s0, float t0, float s1, float t1, const vec_t *color, MaterialHandle material);
    void (*DrawStretchPicRotate)(float x, float y, float w, float h, float s0, float t0, float s1, float t1, float angle, const vec_t *color, MaterialHandle material);
    void (*DrawStretchRaw)();
    void (*DrawQuadPic)(void *verts, const vec_t *color, MaterialHandle material);
    void (*DrawSprite)(MaterialHandle material, const unsigned char *rgbaColor, const vec_t *pos, float radius, float minScreenRadius, int renderFxFlags);
    void (*BeginFrame)(void);
    void (*EndFrame)(void);
    void (*BeginDebugFrame)(void);
    void (*EndDebugFrame)(void);
    void (*EndView)(int viewIndex);
    void (*DoneRenderingViews)(void);
    void (*SaveScreen)(void);
    void (*BlendSavedScreen)(int fadeMsec);
    void (*ClearScreen)(int whichToClear, const vec_t *color, float depth, int stencil);
    void (*SetViewport)(int x, int y, int width, int height);
    int (*MarkFragments)(void *points, const vec_t *origin, void *axis, float radius, int maxPoints, GfxWorldVertex *verts, int maxFragments, void *fragmentBuffer, MaterialHandle markMaterial);
    void (*ModelBounds)(GfxBrushModel *bmodel, vec_t *mins, vec_t *maxs);
    void (*TrackStatistics)(void *stats);
    qboolean (*PickMaterial)(const vec_t *org, const vec_t *dir, char *name, char *surfaceFlags, char *contents, int charLimit);
    FontHandle (*RegisterFont)(const char *fontName, int imageTrack);
    void (*ResetImageAllocations)(void);
    void (*FreeImageAllocations)(void);
    void (*BeginCubemapShot)(int pixelWidthHeight, int pixelBorder);
    void (*EndCubemapShot)(int shotIndex);
    void (*SaveCubemapShot)(const char *filename, int shotIndex, float n0, float n1);
    void (*LightingFromCubemapShots)(const vec_t *baseColor);
    void (*LocateDebugStrings)(void *strings, int stringCount, int maxStringCount);
    void (*LocateDebugLines)(void *lines, int lineCount, int maxLineCount);
    void (*AddPlume)(const vec_t *origin, int score, const vec_t *color, int duration);
    void (*ShutdownDebug)(void);
    void (*UpdateColor)(const vec_t *color_allies, const vec_t *color_axis);
    float (*NormalizedTextScale)(FontHandle font, float scale);
    int (*TextWidth)(const char *text, int maxChars, FontHandle font);
    int (*TextHeight)(FontHandle font);
    void (*DrawText)(const char *text, int maxChars, FontHandle font, float x, float y, float xScale, float yScale, const vec_t *color, int style);
    void (*DrawTextInSpace)(const char *text, FontHandle font, const vec_t *org, const vec_t *xPixelStep, const vec_t *yPixelStep, const vec_t *color);
    int (*ConsoleTextWidth)(const short int *string, int maxChars, FontHandle font);
    void (*DrawConsoleText)(const short int *string, int maxChars, FontHandle font, float x, float y, float xScale, float yScale, const vec_t *color, int style);
    void (*DrawTextWithCursor)(const char *text, int maxChars, FontHandle font, float x, float y, float xScale, float yScale, const vec_t *color, int style, int cursorPos, char cursor);
    void (*DObjGetSurfMaterials)(struct DObj_s *obj, int lod, MaterialHandle *matHandleArray);
    void (*DObjReplaceMaterial)(struct DObj_s *obj, int lod, int surfaceIndex, MaterialHandle material);
    const char * (*ParseSunLight)(void *params, const char *text);
    MaterialHandle (*Material_Duplicate)(MaterialHandle mtlCopy, const char *name);
    void (*DuplicateFont)(FontHandle fontCopy, const char *name);
    void *unknown86_95[10];
    struct XSurface_s * (*XModelReadSurface)(struct XModel *model, int *partBits, const unsigned char **pos, void *(*Alloc)(int));
    struct trXSkin_t * (*LoadXSkins)(struct XModel *model);
    void *unknown98_102[5];
    void (*SyncRenderThread)(void);
    void (*AbortRenderCommands)(void);
    void (*SyncGpu)(void);
    void *unknown106;
    void (*SetLodOrigin)(const refdef_t *refdef);
};

struct refimport_t {
    void (*Printf)(int print_level, const char *fmt, ...);
    void (*Error)(errorParm_t code, const char *fmt, ...);
    int (*Milliseconds)();
    void * (*Hunk_AllocInternal)(int size);
    void * (*Hunk_AllocateTempMemoryInternal)(int size);
    void * (*Z_MallocInternal)(int size);
    void (*Z_FreeInternal)(void *ptr);
    void * (*Hunk_AllocAlignInternal)(int size, int align);
    void * (*Z_VirtualReserveInternal)(int size);
    void (*Z_VirtualCommitInternal)(void *ptr, int size);
    void (*Z_VirtualDecommitInternal)(void *ptr, int size);
    void (*Z_VirtualFreeInternal)(void *ptr);
    void (*Hunk_FreeTempMemory)(void *buf);
    void (*Hunk_ClearTempMemory)();
    int (*Hunk_HideTempMemory)();
    void (*Hunk_ShowTempMemory)(void *mark);
    void * (*Hunk_AllocateTempMemoryHighInternal)(int size);
    void (*Hunk_ClearTempMemoryHigh)();
    void (*Sys_DirectXFatalError)();
    void (*Sys_ShowSplashWindow)();
    void (*Sys_HideSplashWindow)();
    void (*Sys_LoadingKeepAlive)();
    const dvar_t * (*Dvar_RegisterBool)(const char *dvarName, int value, int flags);
    const dvar_t * (*Dvar_RegisterInt)(const char *dvarName, int value, int min, int max, int flags);
    const dvar_t * (*Dvar_RegisterFloat)(const char *dvarName, float value, float min, float max, int flags);
    const dvar_t * (*Dvar_RegisterString)(const char *dvarName, const char *value, int flags);
    const dvar_t * (*Dvar_RegisterEnum)(const char *dvarName, const char **valueList, int defaultIndex, int flags);
    const dvar_t * (*Dvar_RegisterColor)(const char *dvarName, float r, float g, float b, float a, int flags);
    const dvar_t * (*Dvar_RegisterVec2)(const char *dvarName, float x, float y, float min, float max, int flags);
    const dvar_t * (*Dvar_RegisterVec3)(const char *dvarName, float x, float y, float z, float min, float max, int flags);
    const dvar_t * (*Dvar_RegisterVec4)(const char *dvarName, float x, float y, float z, float w, float min, float max, int flags);
    void (*Dvar_UnregisterSystem)(int sysFlag);
    void (*Dvar_ChangeResetValue)(const dvar_t *dvar, DvarValue value);
    Bool (*Dvar_IsAtDefaultValue)(const dvar_t *dvar);
    void (*Dvar_ClearModified)(const dvar_t *dvar);
    void (*Dvar_SetModified)(const dvar_t *dvar);
    void (*Dvar_UpdateEnumDomain)(const dvar_t *dvar, const char **stringTable);
    void (*Dvar_SetBool)(const dvar_t *dvar, int value);
    void (*Dvar_SetInt)(const dvar_t *dvar, int value);
    void (*Dvar_SetFloat)(const dvar_t *dvar, float value);
    void (*Dvar_SetString)(const dvar_t *dvar, const char *value);
    void (*Dvar_SetColor)(const dvar_t *dvar, float r, float g, float b, float a);
    void (*Dvar_SetVec2)(const dvar_t *dvar, float x, float y);
    void (*Dvar_SetVec3)(const dvar_t *dvar, float x, float y, float z);
    void (*Dvar_SetVec4)(const dvar_t *dvar, float x, float y, float z, float w);
    void (*Dvar_SetFromString)(const dvar_t *dvar, const char *string);
    void (*Dvar_SetBoolByName)(const char *name, int value);
    void (*Dvar_SetIntByName)(const char *name, int value);
    void (*Dvar_SetFloatByName)(const char *name, float value);
    void (*Dvar_SetStringByName)(const char *dvarName, const char *value);
    void (*Dvar_SetColorByName)(const char *dvarName, int r, int g, int b, int a);
    void (*Dvar_SetVec2ByName)(const char *dvarName, float x, float y);
    void (*Dvar_SetVec3ByName)(const char *dvarName, float x, float y, float z);
    void (*Dvar_SetVec4ByName)(const char *dvarName, float x, float y, float z, float w);
    void (*Dvar_SetFromStringByName)(const char *dvarName, const char *value);
    Bool (*Dvar_GetBool)(const char *dvarName);
    int (*Dvar_GetInt)(const char *dvarName);
    float (*Dvar_GetFloat)(const char *name);
    const char * (*Dvar_GetString)(const char *dvarName);
    const char * (*Dvar_GetVariantString)(const char *dvarName);
    const char * (*Dvar_EnumToString)(dvar_t *dvar);
    void (*Dvar_Reset)(void *dvar, int source);
    void (*Cmd_AddCommand)(const char *cmdName, xcommand_t function);
    void (*Cmd_RemoveCommand)(const char *cmdName);
    int (*Cmd_Argc)();
    char * (*Cmd_Argv)(int arg);
    void (*Cbuf_ExecuteText)(int exec_when, const char *text);
    qboolean (*Com_SaveDvarsToBuffer)(const char **dvar_names, int count, char *buf, int bufsize);
    qboolean (*Com_LoadDvarsFromBuffer)(const char **dvar_names, int count, const char *buf, const char *path);
    const dheader_s * (*Com_GetBsp)(int *fileSize, unsigned int *checksum);
    unsigned int (*SEH_ReadCharFromString)(const char **ppsText, qboolean *pbIsTrailingPunctuation);
    void (*CL_UpdateDebugData)();
    void (*CL_FlushDebugData)(qboolean fromServer);
    void (*StatMon_Warning)(int type, int duration, const char *materialName);
    int (*FS_ReadFile)(const char *qpath, void **buffer);
    void (*FS_FreeFile)(void *buffer);
    int (*FS_FOpenFileRead)(const char *filename, fileHandle_t *file, qboolean uniqueFILE);
    const char * * (*FS_ListFiles)(const char *dir, const char *ext, int flags, int *count, int);
    void (*FS_FreeFileList)(const char **list, int count);
    qboolean (*FS_FileExists)(const char *path);
    qboolean (*FS_WriteFile)(const char *path, const void *buffer, int size);
    int (*FS_FOpenFileByMode)(const char *qpath, fileHandle_t *f, fsMode_t mode);
    void (*FS_FCloseFile)(fileHandle_t h);
    int (*FS_Read)(void *buffer, int len, fileHandle_t h);
    int (*FS_Write)(const void *buf, int len, int fh);
    void (*CM_SaveLump)(int lumpnum, byte *newLump, int size, int *checksum);
    void (*CM_BoxTrace)(trace_t *results, const vec_t *start, const vec_t *end, const vec_t *mins, const vec_t *maxs, clipHandle_t model, int brushmask);
    int (*CM_BoxSightTrace)(int oldHitNum, const vec_t *start, const vec_t *end, const vec_t *mins, const vec_t *maxs, clipHandle_t model, int brushmask);
    Bool (*CM_RayTriangleIntersect)();
    struct XModel * (*XModelPrecache)(const char *name, Alloc_t Alloc, Alloc_t AllocColl);
    int (*XModelGetSurfaces)(const XModel *model, struct XSurface_s ***surfaces, int lod, int **partBits);
    int (*XModelBad)(const XModel *model);
    void (*Hunk_OverrideDataForFile)(int type, const char *name, void *data);
    int (*XModelGetNumLods)(const XModel *model);
    void (*XModelSetTestLods)(int lodLevel, float dist);
    int (*XModelGetLodForDist)(const XModel *model, float dist);
    float (*XModelGetLodOutDist)(const XModel *model);
    const char * (*XModelGetSurfaceName)(const XModel *model, int subMatIndex, int lod);
    const char * (*XModelGetName)(const XModel *model);
    unsigned char (*XModelGetFlags)(const XModel *model);
    int (*XModelNumBones)(const XModel *model);
    const trXSkin_t * (*XModelGetSkins)(const XModel *model);
    int (*XModelGetMemUsage)(const XModel *model);
    const char * (*XModelGetLodName)(const XModel *model, int lod);
    const DObjAnimMat * (*XModelGetBasePose)(const XModel *model);
    const DObjAnimMat * (*XModelGetBasePoseBone)(const XModel *model, int skelMatBoneOffset);
    int (*DObjBad)(const DObj *obj);
    void (*DB_EnumXAssets)(int type, void (*func)(union XAssetHeader, void *), void *data, int overrides);
    void (*DObjGetBounds)(const DObj *obj, vec_t *mins, vec_t *maxs);
    XSurface * (*DObjGetSurface)(const DObj *obj, int modelIndex, int subMatIndex, int lod);
    int (*DObjGetNumModels)(const DObj *obj);
    int (*DObjGetNumSurfaces)(const DObj *obj, char *lods);
    int (*DObjGetSurfaces)(const DObj *obj, DSurface *surfaces, int *partBits, char *lods);
    int * (*DObjGetPartBits)();
    DObjAnimMat * (*DObjGetRotTransArray)(const DObj *obj);
    int (*DObjSkelAreBonesUpToDate)(const DObj *obj, int *partBits);
    int (*DObjGetMatOffset)(const DObj *obj, int modelIndex);
    struct XModel * (*DObjGetModel)(const DObj *obj, int modelIndex);
    const char * (*DObjGetSurfaceName)(DObj *obj, int modelIndex, int subMatIndex, int lod);
    void (*DObjCreate)(DObjModel_s *dobjModels, unsigned int numModels, XAnimTree_s *tree, char *buf, unsigned int entnum);
    int (*DObjGetAllocSkelSize)(const DObj *obj);
    void (*DObjCreateSkel)(const DObj *obj, char *buf, int timeStamp);
    void (*DObjCalcAnim)(const DObj *obj, int *partBits);
    void (*DObjCalcSkel)(const DObj *obj, int *partBits);
    int (*DObjNumBones)(const DObj *obj);
    void (*DObjGetBoneInfo)(const DObj *obj, XBoneInfo **boneInfo);
    int (*DObjGetLodForDist)(const DObj *obj, int modelIndex, float dist);
    float (*DObjGetLodOutDist)(const DObj *obj);
    void (*DObjCompleteHierarchyBits)(const DObj *obj, int *partBits);
    void (*DObjSetModel)(DObj *obj, const XModel *model);
    void (*CIN_UploadCinematic)(int handle);
    int (*CIN_PlayCinematic)(const char *arg, int x, int y, int w, int h, int systemBits);
    e_status (*CIN_RunCinematic)(int handle);
    void (*CG_DObjCalcPose)(const centity_t *cent, const struct DObj_s *obj, int *partBits);
    const char * (*CL_GetHudMsgIconMaterialName)(int index);

#ifndef COD2_BYTEMATCH
    void * (*CM_GetPlaneNum)(int planeNum);
#endif
};

struct rlimit {
    rlim_t rlim_cur;
    rlim_t rlim_max;
};

struct saLoadObjGlob_type {
    snd_alias_build_t *tempAliases;
    int tempAliasCount;
    VolumeModGroup volumeModGroups[32];
    Bool volumeModGroupsInitialized;
    Bool refreshVolumeModGroupsCommandInitialized;
};

struct savable_state {
    INT32 put_buffer;
    int put_bits;
    int last_dc_val[4];
};

struct huff_entropy_decoder {
    jpeg_entropy_decoder pub;
    struct bitread_perm_state bitstate;
    struct savable_state saved;
    unsigned int restarts_to_go;
    d_derived_tbl * dc_derived_tbls[4];
    d_derived_tbl * ac_derived_tbls[4];
    d_derived_tbl * dc_cur_tbls[10];
    d_derived_tbl * ac_cur_tbls[10];
    boolean dc_needed[10];
    boolean ac_needed[10];
};

struct huff_entropy_encoder {
    jpeg_entropy_encoder pub;
    struct savable_state saved;
    unsigned int restarts_to_go;
    int next_restart_num;
    c_derived_tbl * dc_derived_tbls[4];
    c_derived_tbl * ac_derived_tbls[4];
    long int * dc_count_ptrs[4];
    long int * ac_count_ptrs[4];
};

struct phuff_entropy_decoder {
    jpeg_entropy_decoder pub;
    struct bitread_perm_state bitstate;
    struct savable_state saved;
    unsigned int restarts_to_go;
    d_derived_tbl * derived_tbls[4];
    d_derived_tbl *ac_derived_tbl;
};

struct sbpicinfo_t {
    float x;
    float y;
    float w;
    float h;
};

struct score_t {
    int client;
    int score;
    int ping;
    int deaths;
    int team;
    MaterialHandle hStatusIcon;
};

struct scrAnimGlob_t {
    const char *start;
    const char *pos;
    short unsigned int using_xanim_lookup[2][128];
    int bAnimCheck;
};

struct scrCompileDebugPub_t {
    Bool dummy;
};

struct scrCompilePub_t {
    int value_count;
    int far_function_count;
    unsigned int loadedscripts;
    unsigned int scripts;
    unsigned int builtinFunc;
    unsigned int builtinMeth;
    short unsigned int *canonicalStrings;
    const char *in_ptr;
    const char *parseBuf;
    Bool script_loading;
    Bool allowedBreakpoint;
    int developer_statement;
    byte *opcodePos;
    int programLen;
    int func_table_size;
    intptr_t func_table[1024];   /* holds BuiltinFunction/BuiltinMethod pointers; was int -> truncated on x64 */
};

struct scrMemTreeGlob_t {
    MemoryNode nodes[65536];
    unsigned char leftBits[256];
    unsigned char numBits[256];
    unsigned char logBits[256];
    short unsigned int head[17];
    int totalAlloc;
    int totalAllocBuckets;
};

struct scrMemTreePub_t {
    char *mt_buffer;
};

struct scrParserGlob_t {
    OpcodeLookup *opcodeLookup;
    unsigned int opcodeLookupMaxLen;
    unsigned int opcodeLookupLen;
    SourceLookup *sourcePosLookup;
    unsigned int sourcePosLookupMaxLen;
    unsigned int sourcePosLookupLen;
    unsigned int sourceBufferLookupMaxLen;
    const byte *currentCodePos;
    unsigned int currentSourcePosCount;
    SaveSourceBufferInfo *saveSourceBufferLookup;
    int saveSourceBufferLookupLen;
    int delayedSourceIndex;
    int threadStartSourceIndex;
};

struct scrParserPub_t {
    SourceBufferInfo *sourceBufferLookup;
    unsigned int sourceBufferLookupLen;
    const char *scriptfilename;
    const char *sourceBuf;
};

struct scrStringGlob_t {
    HashEntry hashTable[16384];
    Bool inited;
    HashEntry *nextFreeEntry;
};

struct scrVarDebugPub_t {
    Bool dummy;
};

struct scrVarGlob_t {
    VariableValueInternal variableList[65534];
};

struct scrVarPub_t {
    const char *fieldBuffer;
    int mark;
    short unsigned int canonicalStrCount;
    Bool developer;
    Bool developer_script;
    Bool evaluate;
    const char *error_message;
    int error_index;
    int time;
    unsigned int timeArrayId;
    unsigned int pauseArrayId;
    unsigned int levelId;
    unsigned int gameId;
    unsigned int animId;
    unsigned int freeEntList;
    unsigned int tempVariable;
    Bool bInited;
    short unsigned int savecount;
    unsigned int checksum;
    unsigned int entId;
    unsigned int entFieldName;
    const char *programBuffer;
    const char *endScriptBuffer;
    short unsigned int saveIdMap[65534];
    short unsigned int saveIdMapRev[65534];
};

struct scrVmDebugPub_t {
    int dummy;
};

struct scrVmGlob_t {
    VariableValue eval_stack[2];
    const char *dialog_error_message;
    int loading;
    unsigned int starttime;
    unsigned int localVarsStack[2048];
};

struct scrVmPub_t {
    unsigned int *localVars;
    VariableValue *maxstack;
    int function_count;
    function_frame_t *function_frame;
    VariableValue *top;
    Bool debugCode;
    Bool abort_on_error;
    Bool terminal_error;
    unsigned int inparamcount;
    unsigned int outparamcount;
    function_frame_t function_frame_start[32];
    VariableValue stack[2048];
};

struct scr_anim_s {
    short unsigned int index;
    short unsigned int tree;
};

struct loadAnim_t {
    scr_anim_t anim;
    int iNameHash;
    char szAnimName[64];
};

struct scr_animtree_t {
    struct XAnim_s *anims;
};

struct animScriptData_t {
    animation_t animations[512];
    int numAnimations;
    animScript_t scriptAnims[4][41];
    animScript_t scriptCannedAnims[4][41];
    animScript_t scriptStateChange[4][4];
    animScript_t scriptEvents[19];
    animScriptItem_t scriptItems[2048];
    int numScriptItems;
    struct scr_animtree_t animTree;
    short unsigned int torsoAnim;
    short unsigned int legsAnim;
    short unsigned int turningAnim;
    snd_alias_list_t * (*soundAlias)(const char *);                 /* real sigs; byte-neutral fn-ptrs */
    int (*playSoundAlias)(int, snd_alias_list_t *);
};

struct bgs_s {
    struct animScriptData_t animScriptData;
    struct { scr_animtree_t tree; scr_anim_t root; scr_anim_t torso; scr_anim_t legs; scr_anim_t turning; } generic_human;
    int time;
    int latestSnapshotTime;
    int frametime;
    int anim_user;
    struct XModel * (*GetXModel)(const char *);                                  /* real bgs handler sigs; byte-neutral fn-ptrs */
    void (*CreateDObj)(void *, unsigned short, void *, int, void *);
    void (*SafeDObjFree)(int);
    void * (*AllocXAnim)();
    clientInfo_t clientinfo[64];
};

struct scrAnimPub_t {
    unsigned int animtrees;
    unsigned int animtree_node;
    unsigned int animTreeNames;
    scr_animtree_t xanim_lookup[2][128];
    int xanim_num[2];
    int animTreeIndex;
    Bool animtree_loading;
};

struct scr_classStruct_t {
    short unsigned int id;
    short unsigned int entArrayId;
    char charId;
    const char *name;
};

struct scr_const_t {
    scr_string_t _;
    scr_string_t allies;
    scr_string_t axis;
    scr_string_t current;
    scr_string_t damage;
    scr_string_t death;
    scr_string_t dlight;
    scr_string_t done;
    scr_string_t empty;
    scr_string_t entity;
    scr_string_t failed;
    scr_string_t fraction;
    scr_string_t goal;
    scr_string_t grenade;
    scr_string_t info_notnull;
    scr_string_t invisible;
    scr_string_t key1;
    scr_string_t key2;
    scr_string_t killanimscript;
    scr_string_t left;
    scr_string_t movedone;
    scr_string_t noclass;
    scr_string_t normal;
    scr_string_t pistol;
    scr_string_t plane_waypoint;
    scr_string_t player;
    scr_string_t position;
    scr_string_t primary;
    scr_string_t primaryb;
    scr_string_t prone;
    scr_string_t right;
    scr_string_t rocket;
    scr_string_t rotatedone;
    scr_string_t script_brushmodel;
    scr_string_t script_model;
    scr_string_t script_origin;
    scr_string_t spectator;
    scr_string_t stand;
    scr_string_t surfacetype;
    scr_string_t target_script_trigger;
    scr_string_t tempEntity;
    scr_string_t touch;
    scr_string_t trigger;
    scr_string_t trigger_use;
    scr_string_t trigger_use_touch;
    scr_string_t trigger_damage;
    scr_string_t trigger_lookat;
    scr_string_t truck_cam;
    scr_string_t worldspawn;
    scr_string_t binocular_enter;
    scr_string_t binocular_exit;
    scr_string_t binocular_fire;
    scr_string_t binocular_release;
    scr_string_t binocular_drop;
    scr_string_t begin;
    scr_string_t intermission;
    scr_string_t menuresponse;
    scr_string_t playing;
    scr_string_t none;
    scr_string_t dead;
    scr_string_t auto_change;
    scr_string_t manual_change;
    scr_string_t freelook;
    scr_string_t call_vote;
    scr_string_t vote;
    scr_string_t snd_enveffectsprio_level;
    scr_string_t snd_enveffectsprio_shellshock;
    scr_string_t snd_channelvolprio_holdbreath;
    scr_string_t snd_channelvolprio_pain;
    scr_string_t snd_channelvolprio_shellshock;
    scr_string_t tag_flash;
    scr_string_t tag_flash_11;
    scr_string_t tag_flash_2;
    scr_string_t tag_flash_22;
    scr_string_t tag_brass;
    scr_string_t j_head;
    scr_string_t tag_weapon;
    scr_string_t tag_player;
    scr_string_t tag_camera;
    scr_string_t tag_aim;
    scr_string_t tag_aim_animated;
    scr_string_t tag_origin;
    scr_string_t tag_butt;
    scr_string_t tag_weapon_right;
    scr_string_t back_low;
    scr_string_t back_mid;
    scr_string_t back_up;
    scr_string_t neck;
    scr_string_t head;
    scr_string_t pelvis;
};

struct scr_data_t {
    scr_func_t levelscript;
    scr_func_t gametypescript;
    struct { scr_func_t main; scr_func_t startupgametype; scr_func_t playerconnect; scr_func_t playerdisconnect; scr_func_t playerdamage; scr_func_t playerkilled; scr_func_t votecalled; scr_func_t playervote; int iNumGameTypes; gameTypeScript_t list[32]; } gametype;
    scr_func_t delete_;
    scr_func_t initstructs;
    scr_func_t createstruct;
    corpseInfo_t playerCorpseInfo[8];
};

struct scr_entref_t {
    short unsigned int entnum;
    short unsigned int classnum;
};

struct scr_localVar_t {
    unsigned int name;
};

struct scr_block_s {
    int abortLevel;
    int localVarsCreateCount;
    int localVarsPublicCount;
    int localVarsCount;
    byte localVarsInitBits[8];
    scr_localVar_t localVars[64];
};

struct scrollInfo_s {
    int nextScrollTime;
    int nextAdjustTime;
    int adjustValue;
    int scrollKey;
    float xStart;
    float yStart;
    itemDef_t *item;
    qboolean scrollDir;
};

struct searchpath_s {
    intptr_t next;
    pack_t *pack;
    directory_t *dir;
    qboolean bLocalized;
    int language;
};

struct serverAddress_t {
    byte ip[4];
    short unsigned int port;
};

struct serverFilter_s {
    const char *description;
    const char *basedir;
};

struct serverInfo_t {
    netadr_t adr;
    byte netType;
    byte clients;
    byte maxClients;
    byte dirty;
    byte allowAnonymous;
    byte bPassword;
    byte pure;
    char friendlyfire;
    char killcam;
    char consoleDisabled;
    byte hardware;
    byte mod;
    byte voice;
    byte requestCount;
    short int minPing;
    short int maxPing;
    short int ping;
    char hostName[32];
    char mapName[32];
    char game[24];
    char gameType[16];
};

struct serverStatusDvar_t {
    const char *name;
    const char *altName;
    sscType_t type;
};

struct serverStatusInfo_t {
    char address[64];
    const char * lines[128][4];
    char text[1024];
    char pings[192];
    int numLines;
};

struct serverStatus_s {
    pinglist_t pingList[16];
    int numqueriedservers;
    int currentping;
    int nextpingtime;
    int maxservers;
    int refreshtime;
    int numServers;
    int sortKey;
    int sortDir;
    int lastCount;
    qboolean refreshActive;
    int currentServer;
    int displayServers[20000];
    int numDisplayServers;
    int serverCount;
    int numPlayersOnServers;
    int nextDisplayRefresh;
    int nextSortTime;
    MaterialHandle currentServerPreview;
    int currentServerCinematic;
    int motdLen;
    int motdWidth;
    int motdPaintX;
    int motdPaintX2;
    int motdOffset;
    int motdTime;
    char motd[1024];
};

struct sharedUiInfo_t {
    struct CachedAssets_t assets;
    int playerCount;
    char playerNames[64][32];
    char teamNames[64][32];
    int playerClientNums[64];
    int numGameTypes;
    gameTypeInfo gameTypes[32];
    int numJoinGameTypes;
    gameTypeInfo joinGameTypes[32];
    int mapCount;
    mapInfo mapList[128];
    MaterialHandle serverHardwareIconList[8];
    modInfo_t modList[64];
    int modCount;
    int modIndex;
    const char * movieList[256];
    int movieCount;
    int movieIndex;
    int previewMovie;
    serverStatus_t serverStatus;
    char serverStatusAddress[64];
    serverStatusInfo_t serverStatusInfo;
    int nextServerStatusRefresh;
    struct pendingServerStatus_t pendingServerStatus;
};

struct shellshock_parms_t {
    struct { int fadeTime; float kickRate; float kickRadius; } view;
    struct { int fadeTime; int effectTime; } screenBlend;
    struct { qboolean use; int fadeInTime; int fadeOutTime; float drylevel; float wetlevel; char roomtype[16]; float channelvolume[11]; int modEndDelay; int loopFadeTime; int loopEndDelay; } sound;
    struct { qboolean use; int fadeTime; float sensitivity; float maxPitchSpeed; float maxYawSpeed; } mouse;
};

struct cgs_t {
    struct gameState_t gameState;
    int viewX;
    int viewY;
    int viewWidth;
    int viewHeight;
    float viewAspect;
    int serverCommandSequence;
    int processedSnapshotNum;
    qboolean localServer;
    char gametype[32];
    char szHostName[256];
    int maxclients;
    char mapname[64];
    char redTeam[64];
    char blueTeam[64];
    int voteTime;
    int voteYes;
    int voteNo;
    char voteString[256];
    int teamVoteTime[2];
    int teamVoteYes[2];
    int teamVoteNo[2];
    qboolean teamVoteModified[2];
    char teamVoteString[2][256];
    int levelStartTime;
    int teamScores[2];
    struct XModel * gameModels[256];
    struct FxEffectDef * fxs[64];
    struct FxEffectDef *smokeGrenadeFx;
    shellshock_parms_t shellshockParms[16];
    struct shellshock_parms_t holdBreathParams;
    int numInlineModels;
    struct GfxBrushModel * inlineDrawModel[1023];
    vec3_t inlineModelMidpoints[1023];
    char teamChatMsgs[8][271];
    int teamChatMsgTimes[8];
    int teamChatPos;
    int teamLastChatPos;
    struct cgMedia_t media;
    float compassWidth;
    float compassHeight;
    float compassY;
    clientInfo_t corpseinfo[8];
};

struct shellshock_t {
    const shellshock_parms_t *parms;
    int startTime;
    int duration;
    int loopEndTime;
    float sensitivity;
    vec2_t viewDelta;
    qboolean hasSavedScreen;
};

struct sightclip_t {
    vec3_t mins;
    vec3_t maxs;
    vec3_t outerSize;
    vec3_t start;
    vec3_t end;
    int passEntityNum[2];
    int contentmask;
};

struct sightpointtrace_t {
    vec3_t start;
    vec3_t end;
    int passEntityNum[2];
    int contentmask;
    qboolean locational;
};

union small_pool_struct {
    int hdr;
    double dummy;
};

struct my_memory_mgr {
    jpeg_memory_mgr pub;
    small_pool_ptr small_list[2];
    large_pool_ptr large_list[2];
    jvirt_sarray_ptr virt_sarray_list;
    jvirt_barray_ptr virt_barray_list;
    long int total_space_allocated;
    JDIMENSION last_rowsperchunk;
};

struct snapshotEntityNumbers_t {
    int numSnapshotEntities;
    int snapshotEntities[1024];
};

struct snd_alias_build_s {
    char szSourceFile[64];
    char szAliasName[64];
    char szSecondaryAliasName[64];
    char *subtitleText;
    int iSequence;
    char szSoundFile[64];
    SoundFile *permSoundFile;
    float fVolMin;
    float fVolMax;
    float fVolMod;
    float fPitchMin;
    float fPitchMax;
    float fDistMin;
    float fDistMax;
    int iChannel;
    snd_alias_type_t eType;
    SndCurve *volumeFalloffCurve;
    float fSlavePercentage;
    float fProbability;
    float fLfePercentage;
    int startDelay;
    byte bLooping;
    byte bMaster;
    byte bSlave;
    byte bFullDryLevel;
    byte bNoWetLevel;
    Bool error;
    Bool keep;
    intptr_t pSameSoundFile;
    intptr_t pNext;
};

struct snd_alias_list_s {
    const char *aliasName;
    snd_alias_t *head;
    int count;
    intptr_t pHashNext;
};

struct g_sa_type {
    byte initialized[3];
    int randSeed;
    snd_alias_list_t * pHash[1024];
    snd_alias_list_t aliasInfo[3];
    SoundFileInfo soundFileInfo[3];
    char szLoadSpec[64];
    Bool curvesInitialized;
    SndCurve volumeFalloffCurves[16];
    char volumeFalloffCurveNames[16][64];
};

struct snd_alias_t {
    const char *pszAliasName;
    const char *pszSubtitle;
    const char *pszSecondaryAliasName;
    SoundFile *soundFile;
    int iSequence;
    float fVolMin;
    float fVolMax;
    float fPitchMin;
    float fPitchMax;
    float fDistMin;
    float fDistMax;
    int flags;
    float fSlavePercentage;
    float fProbability;
    float fLfePercentage;
    int startDelay;
    SndCurve *volumeFalloffCurve;
};

struct snd_background_info_t {
    float goalvolume;
    float goalrate;
};

struct snd_channel_info_t {
    int entnum;
    int entchannel;
    int startDelay;
    int looptime;
    int endtime;
    float basevolume;
    int baserate;
    float pitch;
    int srcChannelCount;
    const snd_alias_t *pAlias0;
    const snd_alias_t *pAlias1;
    float lerp;
    vec3_t org;
    vec3_t offset;
    Bool paused;
    Bool master;
    snd_alias_system_t system;
};

struct snd_enveffect {
    int roomtype;
    float drylevel;
    float drygoal;
    float dryrate;
    float wetlevel;
    float wetgoal;
    float wetrate;
    Bool active;
};

struct snd_listener {
    struct orientation_t orient;
    int entnum;
    Bool active;
};

struct snd_overlay_info_t {
    const char *pszSampleName;
    float fBaseVolume;
    float fCurVolume;
    int dist;
    float fPitch;
};

struct snd_save_2D_sample_t {
    float fraction;
    float pitch;
    float volume;
    float pan;
};

struct snd_save_3D_sample_t {
    float fraction;
    float pitch;
    float volume;
    vec3_t org;
};

struct snd_save_stream_t {
    float fraction;
    int rate;
    float basevolume;
    float volume;
    float pan;
    vec3_t org;
};

struct snd_volume_info_t {
    float volume;
    float goalvolume;
    float goalrate;
};

struct snd_channelvolgroup {
    snd_volume_info_t channelvol[11];
    Bool active;
};

struct snd_local_t {
    Bool Initialized2d;
    Bool Initialized3d;
    Bool paused;
    int playback_rate;
    int playback_bits;
    int playback_channels;
    float timescale;
    int pausetime;
    int cpu;
    struct { byte *buffer; int size; } restore;
    float volume;
    snd_volume_info_t mastervol;
    snd_channelvolgroup channelVolGroups[4];
    snd_channelvolgroup *channelvol;
    snd_background_info_t background[3];
    int ambient_track;
    float slaveLerp;
    snd_enveffect envEffects[3];
    snd_enveffect *effect;
    Bool defaultPauseSettings[11];
    Bool pauseSettings[11];
    snd_listener listeners[1];
    int time;
    int looptime;
    snd_channel_info_t chaninfo[53];
    int max_2D_channels;
    int max_3D_channels;
    int max_stream_channels;
};

struct spawn_t {
    const char *name;
    void (*spawn)(gentity_t *ent);
};

struct split_cb_params {
    int subvect_size;
    int nb_subvect;
    const signed char *shape_cb;
    int shape_bits;
    int have_sign;
};

struct srfPoly_t {
    surfaceType_t surfaceType;
    MaterialHandle material;
    short unsigned int lmapIndex;
    short unsigned int vertCount;
    GfxWorldVertex *verts;
};

struct GfxBackEndData {
    int frameCount;
    int drawSurfCount;
    GfxDrawSurf drawSurfs[65536];
    int surfPos;
    byte surfsBuffer[131072];
    int entityCount;
    GfxEntity entities[8184];
    int polyCount;
    srfPoly_t polys[2048];
    int polyVertCount;
    GfxWorldVertex polyVerts[8192];
    r_vb_state_t *skinnedCacheVb;
    int viewParmCount;
    GfxViewParms viewParms[25];
    struct GfxFog fogSettings;
    struct GfxCmdArray commands;
    DebugGlobals debugGlobals;
};

struct GfxDebugFrameGlob {
    GfxCmdArray *restoreCmdList;
    GfxBackEndData *restoreFrontEndDataOut;
    Bool restoreSkinnedCache;
    GfxBackEndData frontEndDataOut;
};

struct srfTriangles_t {
    surfaceType_t surfaceType;
    vec3_t bounds[2];
    int firstVertex;
    short unsigned int vertexCount;
    short unsigned int indexCount;
    int (*indices)[4];
};

struct static_model_node_list_t {
    intptr_t prev;   /* hold pointers (was int -> truncated on x64; intptr_t==int on x86) */
    intptr_t next;
};

union static_model_leaf_t {
    GfxStaticModelSurfaceCached surf;
    struct static_model_node_list_t freenode;
};

struct static_model_node_t {
    short int usedVerts;
    Bool inuse;
    byte reserved;
};

struct static_model_tree_list_t {
    intptr_t prev;   /* hold pointers (was int -> truncated on x64; intptr_t==int on x86) */
    intptr_t next;
};

struct static_model_tree_t {
    struct static_model_tree_list_t usedlist;
    int frameCount;
    static_model_node_t nodes[31];
    static_model_leaf_t leafs[16];
};

struct static_model_cache_t {
    static_model_tree_t trees[128];
    static_model_node_list_t freelist[5];
    static_model_tree_list_t usedlist;
    struct { int allocatedVerts; int usedVerts; } stats;
};

struct staticmodeltrace_t {
    struct TraceExtents extents;
    int contents;
};

struct statmonitor_s {
    int endtime;
    MaterialHandle material;
};

struct stream_dest_info_t {
    BYTE Usage;
    BYTE UsageIndex;
};

struct stream_source_info_t {
    BYTE Stream;
    BYTE Offset;
    BYTE Type;
};

struct stringDef_s {
    intptr_t next;   /* linked-list ptr (was int -> truncated on x64; intptr_t==int on x86) */
    const char *str;
};

struct sunflare_t {
    Bool hasValidData;
    Material *spriteMaterial;
    Material *flareMaterial;
    float spriteSize;
    float flareMinSize;
    float flareMinDot;
    float flareMaxSize;
    float flareMaxDot;
    float flareMaxAlpha;
    int flareFadeInTime;
    int flareFadeOutTime;
    float blindMinDot;
    float blindMaxDot;
    float blindMaxDarken;
    int blindFadeInTime;
    int blindFadeOutTime;
    float glareMinDot;
    float glareMaxDot;
    float glareMaxLighten;
    int glareFadeInTime;
    int glareFadeOutTime;
    vec3_t sunFxPosition;
};

struct GfxWorld {
    const char *name;
    const char *baseName;
    int nodeCount;
    mnode_t *nodes;
    int surfaceCount;
    GfxSurface *surfaces;
    int skySurfCount;
    int *skyStartSurfs;
    GfxImage *skyImage;
    GfxSamplerState skySamplerState;
    int vertexCount;
    struct GfxWorldVertexData vd;
    struct SunLightParseParams sunParse;
    struct GfxLight sunLight;
    vec3_t sunColorFromBsp;
    int cullGroupCount;
    GfxCullGroup *cullGroups;
    int smodelCount;
    GfxStaticModelInstance *smodelInsts;
    int cellCount;
    GfxCell *cells;
    int lightmapCount;
    GfxLightmapArray *lightmaps;
    GfxImage *smodelLightingImage;
    vec3_t smodelLightingLookupScale;
    struct GfxLightGrid lightGrid;
    vec4_t (*smodelLightingColorTable)[6];
    float *smodelLightingSunVisTable;
    int modelCount;
    GfxBrushModel *models;
    vec3_t mins;
    vec3_t maxs;
    unsigned int checksum;
    int materialMemoryCount;
    MaterialMemory *materialMemory;
    struct sunflare_t sun;
    vec4_t outdoorLookupMatrix[4];
    GfxImage *outdoorImage;
};

union sval_u {
    int type;
    unsigned int stringValue;
    unsigned int idValue;
    float floatValue;
    int intValue;
    intptr_t node;
    unsigned int sourcePosValue;
    const char *codePosValue;
    const char *debugString;
    scr_block_s *block;
};

struct VariableCompileValue {
    VariableValue value;
    sval_t sourcePos;
};

struct scrCompileGlob_t {
    byte *codePos;
    byte *prevOpcodePos;
    unsigned int fileId;
    unsigned int threadId;
    int cumulOffset;
    int maxOffset;
    int maxCallOffset;
    Bool bConstRefCount;
    Bool in_developer_thread;
    unsigned int developer_thread_sourcePos;
    Bool firstThread[2];
    Bool bCanIgnoreCase;
    CaseStatementInfo *currentCaseStatement;
    Bool bCanBreak;
    Bool bCanIgnoreBreak;
    BreakStatementInfo *currentBreakStatement;
    Bool bCanContinue;
    Bool bCanIgnoreContinue;
    ContinueStatementInfo *currentContinueStatement;
    scr_block_t * *breakChildBlocks;
    int *breakChildCount;
    scr_block_t *breakBlock;
    scr_block_t * *continueChildBlocks;
    int *continueChildCount;
    Bool forceNotCreate;
    PrecacheEntry *precachescriptList;
    PrecacheEntry *precachescriptListHead;
    VariableCompileValue value_start[32];
};

struct stype_t {
    sval_t val;
    unsigned int pos;
};

struct svscmd_info_t {
    char cmd[1024];
    int time;
    int type;
};

struct sysEvent_t {
    int evTime;
    sysEventType_t evType;
    int evValue;
    int evValue2;
    int evPtrLength;
    void *evPtr;
};

struct tagBITMAPFILEHEADER {
    WORD bfType;
    DWORD bfSize;
    WORD bfReserved1;
    WORD bfReserved2;
    DWORD bfOffBits;
};

struct tagBITMAPINFOHEADER {
    DWORD biSize;
    LONG biWidth;
    LONG biHeight;
    WORD biPlanes;
    WORD biBitCount;
    DWORD biCompression;
    DWORD biSizeImage;
    LONG biXPelsPerMeter;
    LONG biYPelsPerMeter;
    DWORD biClrUsed;
    DWORD biClrImportant;
};

struct tagInfo_s {
    gentity_t *parent;
    gentity_t *next;
    scr_string_t name;
    int index;
    vec3_t axis[4];
    vec3_t parentInvAxis[4];
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

struct tagRECT {
    LONG left;
    LONG top;
    LONG right;
    LONG bottom;
};

struct LARGE_INTEGER {
    void (*Format)();
    netProfilePacket_t Type;
    D3DRESOURCETYPE Usage;
    _D3DSTATEBLOCKTYPE *Pool;
    void (*MultiSampleType)();
    D3DRESOURCETYPE MultiSampleQuality;
    IDirect3DBaseTexture9 *Width;
    IDirect3DBaseTexture9 *Height;
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

struct tagRGBQUAD {
    BYTE rgbBlue;
    BYTE rgbGreen;
    BYTE rgbRed;
    BYTE rgbReserved;
};

struct tagBITMAPINFO {
    BITMAPINFOHEADER bmiHeader;
    RGBQUAD bmiColors[1];
};

struct tagTEXTMETRICA {
    LONG tmHeight;
    LONG tmAscent;
    LONG tmDescent;
    LONG tmInternalLeading;
    LONG tmExternalLeading;
    LONG tmAveCharWidth;
    LONG tmMaxCharWidth;
    LONG tmWeight;
    LONG tmOverhang;
    LONG tmDigitizedAspectX;
    LONG tmDigitizedAspectY;
    BYTE tmFirstChar;
    BYTE tmLastChar;
    BYTE tmDefaultChar;
    BYTE tmBreakChar;
    BYTE tmItalic;
    BYTE tmUnderlined;
    BYTE tmStruckOut;
    BYTE tmPitchAndFamily;
    BYTE tmCharSet;
};

struct tagTEXTMETRICW {
    LONG tmHeight;
    LONG tmAscent;
    LONG tmDescent;
    LONG tmInternalLeading;
    LONG tmExternalLeading;
    LONG tmAveCharWidth;
    LONG tmMaxCharWidth;
    LONG tmWeight;
    LONG tmOverhang;
    LONG tmDigitizedAspectX;
    LONG tmDigitizedAspectY;
    WCHAR tmFirstChar;
    WCHAR tmLastChar;
    WCHAR tmDefaultChar;
    WCHAR tmBreakChar;
    BYTE tmItalic;
    BYTE tmUnderlined;
    BYTE tmStruckOut;
    BYTE tmPitchAndFamily;
    BYTE tmCharSet;
};

struct tempBanSlot_t {
    int guid;
    int banTime;
};

struct serverStatic_t {
    qboolean initialized;
    int time;
    int snapFlagServerBit;
    client_t *clients;
    int numSnapshotEntities;
    int numSnapshotClients;
    int nextSnapshotEntities;
    int nextSnapshotClients;
    entityState_t *snapshotEntities;
    char * (*snapshotClients)();
    qboolean archiveEnabled;
    int nextArchivedSnapshotFrames;
    char * (*archivedSnapshotFrames)();
    byte *archivedSnapshotBuffer;
    int nextArchivedSnapshotBuffer;
    int nextCachedSnapshotEntities;
    int nextCachedSnapshotClients;
    int nextCachedSnapshotFrames;
    fileHandle_t (*cachedSnapshotEntities)();
    client_t * (*cachedSnapshotClients)();
    cachedSnapshot_t *cachedSnapshotFrames;
    int nextHeartbeatTime;
    int nextStatusResponseTime;
    challenge_t challenges[1024];
    netadr_t redirectAddress;
    netadr_t authorizeAddress;
    netProfileInfo_t *pOOBProf;
    tempBanSlot_t tempBans[16];
};

struct tm_unz_s {
    uInt tm_sec;
    uInt tm_min;
    uInt tm_hour;
    uInt tm_mday;
    uInt tm_mon;
    uInt tm_year;
};

struct token_s {
    char string[1024];
    int type;
    int subtype;
    long unsigned int intvalue;
    int _pad_fv;
    long double floatvalue;
    char *whitespace_p;
    char *endwhitespace_p;
    int line;
    int linescrossed;
    struct token_s *next;
};

struct script_s {
    char filename[64];
    char *buffer;
    char *script_p;
    char *end_p;
    char *lastscript_p;
    char *whitespace_p;
    char *endwhitespace_p;
    int length;
    int line;
    int lastline;
    int tokenavailable;
    int flags;
    punctuation_t *punctuations;
    punctuation_t * *punctuationtable;
    token_t token;
    struct script_s *next;
};

struct source_s {
    char filename[64];
    char includepath[64];
    punctuation_t *punctuations;
    script_t *scriptstack;
    token_t *tokens;
    define_t *defines;
    define_t * *definehash;
    indent_t *indentstack;
    int skip;
    token_t token;
};

struct trDebugLine_t {
    vec3_t start;
    vec3_t end;
    vec4_t color;
    qboolean depthTest;
};

struct trDebugString_t {
    vec3_t xyz;
    vec4_t color;
    float scale;
    char text[96];
};

struct trStatistics_t {
    int c_indexes;
    int c_fxIndexes;
    int c_viewIndexes;
    int c_shadowIndexes;
    int c_vertexes;
    int c_batches;
    int c_refents;
    float dc;
    struct Image_MemUsage c_imageUsage;
};

struct trXSkin_t {
    XVertexBuffer (*materialHandles)();
};

struct traceWork_t {
    struct TraceExtents extents;
    vec3_t delta;
    vec_t deltaLen;
    vec_t deltaLenSq;
    vec3_t midpoint;
    vec3_t halfDelta;
    vec3_t halfDeltaAbs;
    vec3_t size;
    vec3_t bounds[2];
    int contents;
    qboolean isPoint;
    qboolean axialCullOnly;
    float radius;
    float offsetZ;
    vec3_t radiusOffset;
    struct TraceThreadInfo threadInfo;
};

struct trace_t {
    float fraction;
    vec3_t normal;
    int surfaceFlags;
    int contents;
    const char *material;
    short unsigned int entityNum;
    short unsigned int partName;
    short unsigned int partGroup;
    byte allsolid;
    byte startsolid;
};

struct pml_t {
    vec3_t forward;
    vec3_t right;
    vec3_t up;
    float frametime;
    int msec;
    qboolean walking;
    qboolean groundPlane;
    qboolean almostGroundPlane;
    trace_t groundTrace;
    float impactSpeed;
    vec3_t previous_origin;
    vec3_t previous_velocity;
    int previous_waterlevel;
};

struct trajectory_t {
    trType_t trType;
    int trTime;
    int trDuration;
    vec3_t trBase;
    vec3_t trDelta;
};

struct entityState_s {
    int number;
    int eType;
    int eFlags;
    struct trajectory_t pos;
    trajectory_t apos;
    int time;
    int time2;
    vec3_t origin2;
    vec3_t angles2;
    int otherEntityNum;
    int attackerEntityNum;
    int groundEntityNum;
    int constantLight;
    int loopSound;
    int surfType;
    union { int brushmodel; int item; int xmodel; } index;
    int clientNum;
    int iHeadIcon;
    int iHeadIconTeam;
    int solid;
    int eventParm;
    int eventSequence;
    int events[4];
    int eventParms[4];
    int weapon;
    int legsAnim;
    int torsoAnim;
    float leanf;
    union { int scale; int eventParm2; };
    int dmgFlags;
    int animMovetype;
    float fTorsoHeight;
    float fTorsoPitch;
    float fWaistPitch;
};

struct archivedEntity_s {
    entityState_t s;
    struct archivedEntityShared_t r;
};

struct centity_s {
    entityState_t currentState;
    entityState_t nextState;
    Bool nextValid;
    byte cullIn;
    Bool bMuzzleFlash;
    Bool bTrailMade;
    int previousEventSequence;
    int miscTime;
    vec3_t lerpOrigin;
    vec3_t lerpAngles;
    vec3_t lightingOrigin;
    int dl_time;
    MaterialHandle voiceChatSprite;
    int voiceChatSpriteTime;
    struct XAnimTree_s *tree;
    int localClientNum;
};

struct localEntity_s {
    struct localEntity_s *prev;
    struct localEntity_s *next;
    leType_t leType;
    int endTime;
    trajectory_t pos;
    float color[4];
    float tracerClipDist;
    struct GfxEntity refEntity;
};

struct snapshot_t {
    int snapFlags;
    int ping;
    int serverTime;
    playerState_t ps;
    int numEntities;
    int numClients;
    entityState_t entities[256];
    clientState_t clients[64];
    int serverCommandSequence;
};

struct svEntity_s {
    short unsigned int worldSector;
    short unsigned int nextEntityInWorldSector;
    archivedEntity_t baseline;
    int numClusters;
    int clusternums[16];
    int lastCluster;
    int linkcontents;
    vec2_t linkmin;
    vec2_t linkmax;
};

struct server_t {
    serverState_t state;
    qboolean restarting;
    int start_frameTime;
    int checksumFeed;
    int timeResidual;
    int nextFrameTime;
    struct cmodel_t * models[256];
    char * configstrings[2048];
    svEntity_t svEntities[1024];
    const char *entityParsePoint;
    gentity_t *gentities;
    int gentitySize;
    int num_entities;
    playerState_t *gameClients;
    int gameClientSize;
    int skelTimeStamp;
    int skelMemPos;
    int bpsWindow[20];
    int bpsWindowSteps;
    int bpsTotalBytes;
    int bpsMaxBytes;
    int ubpsWindow[20];
    int ubpsTotalBytes;
    int ubpsMaxBytes;
    float ucompAve;
    int ucompNum;
    char gametype[64];
};

struct trigger_ent_t {
    int threshold;
    int accumulate;
    int timestamp;
    int singleUserEntIndex;
};

struct gentity_s {
    entityState_t s;
    struct entityShared_t r;
    gclient_t *client;
    turretInfo_t *pTurretInfo;
    byte physicsObject;
    byte takedamage;
    byte active;
    byte nopickup;
    byte model;
    byte attachIgnoreCollision;
    byte handler;
    byte team;
    scr_string_t classname;
    scr_string_t target;
    scr_string_t targetname;
    int spawnflags;
    int flags;
    int eventTime;
    qboolean freeAfterEvent;
    qboolean unlinkAfterEvent;
    int clipmask;
    int processedFrame;
    int parent;
    int nextthink;
    int health;
    int maxHealth;
    int damage;
    int count;
    int chain;
    union { struct item_ent_t item; struct trigger_ent_t trigger; struct mover_ent_t mover; struct corpse_ent_t corpse; struct grenade_ent_t grenade; };
    int tagInfo;
    int tagChildren;
    byte attachModelNames[7];
    scr_string_t attachTagNames[7];
    int useCount;
    int nextFree;
};

int COD2_GEntityHandle(const gentity_t *ent);
gentity_t *COD2_GEntityFromHandle(int handle);
int COD2_TagInfoHandle(const tagInfo_t *tagInfo);
tagInfo_t *COD2_TagInfoFromHandle(int handle);

struct trigger_info_t {
    short unsigned int entnum;
    short unsigned int otherEntnum;
    int useCount;
    int otherUseCount;
};

struct level_locals_t {
    gclient_s *clients;
    gentity_s *gentities;
    int gentitySize;
    int num_entities;
    gentity_s *firstFreeEnt;
    gentity_s *lastFreeEnt;
    fileHandle_t logFile;
    qboolean initializing;
    qboolean clientIsSpawning;
    objective_t objectives[16];
    int maxclients;
    int framenum;
    int time;
    int previousTime;
    int frametime;
    int startTime;
    int teamScores[4];
    int lastTeammateHealthTime;
    qboolean bUpdateScoresForIntermission;
    qboolean manualNameChange;
    int numConnectedClients;
    int sortedClients[64];
    char voteString[1024];
    char voteDisplayString[1024];
    int voteTime;
    int voteExecuteTime;
    int voteYes;
    int voteNo;
    int numVotingClients;
    char teamVoteString[2][1024];
    int teamVoteTime[2];
    int teamVoteYes[2];
    int teamVoteNo[2];
    struct SpawnVar spawnVar;
    qboolean savepersist;
    gentity_t * droppedWeaponCue[32];
    float fFogOpaqueDist;
    float fFogOpaqueDistSqrd;
    int remapCount;
    int currentPlayerClone;
    trigger_info_t pendingTriggerList[256];
    trigger_info_t currentTriggerList[256];
    int pendingTriggerListSize;
    int currentTriggerListSize;
    int finished;
    qboolean bPlayerIgnoreRadiusDamage;
    qboolean bPlayerIgnoreRadiusDamageLatched;
    qboolean registerWeapons;
    qboolean bRegisterItems;
    int currentEntityThink;
    fileHandle_t openScriptIOFileHandles[1];
    char * openScriptIOFileBuffers[1];
    com_parse_mark_t currentScriptIOLineMark[1];
};

struct turretInfo_s {
    qboolean inuse;
    int flags;
    int fireTime;
    float arcmin[2];
    float arcmax[2];
    float dropPitch;
    int stance;
    int prevStance;
    int fireSndDelay;
    vec3_t userOrigin;
    float playerSpread;
    float pitchCap;
    SoundAlias fireSnd;
    SoundAlias fireSndPlayer;
    SoundAlias stopSnd;
    SoundAlias stopSndPlayer;
};

struct D3DDISPLAYMODE {
    UINT Width;
    UINT Height;
    UINT RefreshRate;
    D3DFORMAT Format;
};

struct DxGlobals {
    HINSTANCE hinst;
    IDirect3D9 *d3d9;
    void (*device)();
    UINT adapterIndex;
    D3DFORMAT backBufferFormat;
    int displayModeCount;
    D3DDISPLAYMODE displayModes[256];
    const char * resolutionNameTable[257];
    const char * refreshRateNameTable[257];
    char modeText[5120];
    int gpuSync;
    D3DMULTISAMPLE_TYPE multiSampleType;
    DWORD multiSampleQuality;
    int sunSpriteSamples;
    GfxRenderTarget renderTargets[13];
    IDirect3DSurface9 *singleSampleDepthStencilSurface;
    short unsigned int cubemapShotRes;
    short unsigned int cubemapShotPixelBorder;
    Bool deviceLost;
    Bool inScene;
    short unsigned int feedbackWidth;
    short unsigned int feedbackHeight;
    int targetWindowIndex;
    int windowCount;
    GfxWindowTarget windows[1];
    IDirect3DQuery9 *flushGpuQuery;
    int gpuSyncDelay;
    int gpuSyncAlreadyWaited;
    Bool flushGpuQueryIssued;
    int anisotropy;
    int maxAnisotropy;
    int maxClipPlanes;
    Bool stencilTwoSided;
    Bool stencilWrap;
    Bool slopeScaleDepthBias;
    Bool canMipCubemaps;
    Bool hasBlendOp;
    Bool hasSeparateAlphaBlend;
    Bool hasTransparencyMsaa;
    r_ib_state_t dynamicIndexBufferPool[1];
    r_ib_state_t *dynamicIndexBuffer;
    r_vb_state_t skinnedCacheVbPool[2];
    r_vb_state_t dynamicVertexBufferPool[1];
    r_vb_state_t *dynamicVertexBuffer;
    IDirect3DVertexBuffer9 *particleCloudVertexBuffer;
    IDirect3DIndexBuffer9 *particleCloudIndexBuffer;
    byte *skinnedCacheLockAddr;
    IDirect3DVertexBuffer9 *smodelCacheVb;
    r_index_t *smodelCacheIndices;
    int dynamicBufferFrame;
    byte *tempSkinBuf;
    int tempSkinPos;
};

struct IDirect3DPixelShader9 {
    MaterialShaderArgument *adr;
    MaterialPassDx9 netType;
    MaterialPassDx9 clients;
    MaterialPassDx9 maxClients;
    MaterialPassDx9 dirty;
    MaterialPassDx9 allowAnonymous;
    MaterialPassDx9 bPassword;
    MaterialPassDx9 pure;
    int friendlyfire[2];
    int killcam[2];
    int consoleDisabled[2];
    MaterialPassDx9 hardware;
    MaterialPassDx9 mod;
    MaterialPassDx9 voice;
    MaterialPassDx9 requestCount;
    IDirect3D9 *minPing;
    IDirect3D9 *maxPing;
    IDirect3D9 *ping;
    WaterWritable hostName;
    WaterWritable mapName;
    D3DCAPS9 game;
    D3DDISPLAYMODE gameType;
};

struct IDirect3DVolume9 {
    const char *next;
    const TextureInfoType *marker;
    const COpenGLTexture *original_length;
    const COpenGLTexture *data_length;
    unsigned char *data;
};

struct ucmd_t {
    char *name;
    void (*func)();
};

struct uiClientState_t {
    connstate_t connState;
    int connectPacketCount;
    int clientNum;
    char servername[1024];
    char updateInfoString[1024];
    char messageString[1024];
};

struct uiInfo_t {
    displayContextDef_t uiDC;
    int myTeamCount;
    int playerRefresh;
    int playerIndex;
    int playerProfileCount;
    const char * playerProfileName[64];
    struct PlayerProfileStatus playerProfileStatus;
    int timeIndex;
    int previousTimes[4];
    uiMenuCommand_t currentMenuType;
    char findPlayerName[960];
    char foundPlayerServerAddresses[16][64];
    char foundPlayerServerNames[17][64];
    int numFoundPlayerServers;
    int nextFindPlayerRefresh;
};

struct unz_file_info_internal_s {
    uLong offset_curfile;
};

struct unz_file_info_s {
    uLong version;
    uLong version_needed;
    uLong flag;
    uLong compression_method;
    uLong dosDate;
    uLong crc;
    uLong compressed_size;
    uLong uncompressed_size;
    uLong size_filename;
    uLong size_file_extra;
    uLong size_file_comment;
    uLong disk_num_start;
    uLong internal_fa;
    uLong external_fa;
    tm_unz tmu_date;
};

struct unz_global_info_s {
    uLong number_entry;
    uLong size_comment;
};

struct unz_s {
    FILE *file;
    unz_global_info gi;
    uLong byte_before_the_zipfile;
    uLong num_file;
    uLong pos_in_central_dir;
    uLong current_file_ok;
    uLong central_pos;
    uLong size_central_dir;
    uLong offset_central_dir;
    unz_file_info cur_file_info;
    unz_file_info_internal cur_file_info_internal;
    file_in_zip_read_info_s *pfile_in_zip_read;
};

struct useList_t {
    gentity_t *ent;
    float score;
};

struct usercmd_s {
    int serverTime;
    int buttons;
    byte weapon;
    byte offHandIndex;
    int angles[3];
    signed char forwardmove;
    signed char rightmove;
};

struct clientActive_t {
    Bool active;
    int keyCatchers;
    Bool displayHUDWithKeycatchUI;
    Bool cgameInitialized;
    Bool cgameInitCalled;
    Bool usingAds;
    int frameActiveClientCount;
    int frameClientIndex;
    int timeoutcount;
    struct clSnapshot_t snap;
    int serverTime;
    int oldServerTime;
    int oldFrameServerTime;
    int serverTimeDelta;
    int oldSnapServerTime;
    qboolean extrapolatedSnapshot;
    qboolean newSnapshots;
    struct gameState_t gameState;
    char mapname[64];
    int parseEntitiesNum;
    int parseClientsNum;
    int mouseDx[2];
    int mouseDy[2];
    int mouseIndex;
    Bool stanceHeld;
    int stancePosition;
    int stanceTime;
    int cgameUserCmdValue;
    int cgameUserHoldableValue;
    qboolean cgameInShellshock;
    float cgameSensitivity;
    float cgameMaxPitchSpeed;
    float cgameMaxYawSpeed;
    vec3_t cgameKickAngles;
    vec3_t viewangles;
    int serverId;
    vec4_t color_allies;
    vec4_t color_axis;
    int skelTimeStamp;
    int skelMemPos;
    char skelMemory[262144];
    char *skelMemoryStart;
    Bool allowedAllocSkel;
    usercmd_t cmds[128];
    int cmdNumber;
    outPacket_t outPackets[32];
    clSnapshot_t snapshots[32];
    entityState_t entityBaselines[1024];
    entityState_t parseEntities[2048];
    clientState_t parseClients[2048];
    qboolean corruptedTranslationFile;
    char translationVersion[256];
    ClientVoicePacket_t voicePackets[10];
    int voicePacketCount;
    int voicePacketLastTransmit;
};

struct clientSession_t {
    sessionState_t sessionState;
    int forceSpectatorClient;
    int status_icon;
    int archiveTime;
    int score;
    int deaths;
    short unsigned int scriptPersId;
    clientConnected_t connected;
    usercmd_t cmd;
    usercmd_t oldcmd;
    qboolean localClient;
    qboolean predictItemPickup;
    char newnetname[32];
    int maxHealth;
    int enterTime;
    struct playerTeamState_t teamState;
    int voteCount;
    int teamVoteCount;
    int viewmodelIndex;
    int noSpectate;
    qboolean teamInfo;
    clientState_t cs;
    int psOffsetTime;
};

struct client_s {
    int state;
    qboolean sendAsActive;
    const char *dropReason;
    char userinfo[1024];
    svscmd_info_t reliableCommandInfo[128];
    int reliableSequence;
    int reliableAcknowledge;
    int reliableSent;
    int messageAcknowledge;
    int gamestateMessageNum;
    int challenge;
    usercmd_t lastUsercmd;
    int lastClientCommand;
    char lastClientCommandString[1024];
    unsigned char * (*gentity)[16];
    char name[32];
    char downloadName[64];
    fileHandle_t download;
    int downloadSize;
    int downloadCount;
    int downloadClientBlock;
    int downloadCurrentBlock;
    int downloadXmitBlock;
    unsigned char * downloadBlocks[8];
    int downloadBlockSize[8];
    qboolean downloadEOF;
    int downloadSendTime;
#if COD2_IS_PATCH_13

    char downloadURL[256];
    qboolean wwwOk;
    qboolean downloadingWWW;
    qboolean clientDownloadingWWW;
    qboolean wwwFallback;
#endif
    int deltaMessage;
    int nextReliableTime;
    int lastPacketTime;
    int lastConnectTime;
    int nextSnapshotTime;
    qboolean rateDelayed;
    int timeoutCount;
    clientSnapshot_t frames[32];
    int ping;
    int rate;
    int snapshotMsec;
    int pureAuthentic;
    struct netchan_t netchan;
    int guid;
    short unsigned int scriptId;
    qboolean bIsTestClient;
    int serverId;
    VoicePacket_t voicePackets[40];
    int voicePacketCount;
    Bool muteList[64];
    Bool sendVoice;
};

struct gclient_s {
    playerState_t ps;
    struct clientSession_t sess;
    int spectatorClient;
    qboolean noclip;
    qboolean ufo;
    qboolean bFrozen;
    int lastCmdTime;
    int buttons;
    int oldbuttons;
    int latched_buttons;
    int buttonsSinceLastFrame;
    vec3_t oldOrigin;
    float fGunPitch;
    float fGunYaw;
    int damage_blood;
    vec3_t damage_from;
    qboolean damage_fromWorld;
    int accurateCount;
    int accuracy_shots;
    int accuracy_hits;
    int inactivityTime;
    qboolean inactivityWarning;
    int lastVoiceTime;
    int switchTeamTime;
    float currentAimSpreadScale;
    gentity_t *persistantPowerup;
    int portalID;
    int dropWeaponTime;
    int sniperRifleFiredTime;
    float sniperRifleMuzzleYaw;
    int PCSpecialPickedUpCount;
    gentity_t *pLookatEnt;
    int useHoldEntity;
    int useHoldTime;
    int iLastCompassFriendlyInfoEnt;
    int compassPingTime;
    int damageTime;
    float v_dmg_roll;
    float v_dmg_pitch;
    vec3_t swayViewAngles;
    vec3_t swayOffset;
    vec3_t swayAngles;
    vec3_t vLastMoveAng;
    float fLastIdleFactor;
    vec3_t vGunOffset;
    vec3_t vGunSpeed;
    int weapIdleTime;
    int lastServerTime;
    int lastSpawnTime;
};

struct pmove_t {
    playerState_t *ps;
    usercmd_t cmd;
    usercmd_t oldcmd;
    int tracemask;
    int numtouch;
    int touchents[32];
    vec3_t mins;
    vec3_t maxs;
    float xyspeed;
    qboolean proneChange;
    byte handler;
    Bool mantleStarted;
    vec3_t mantleEndPos;
    int mantleDuration;
};

struct va_info_t {
    char va_string[2][1024];
    int index;
};

#pragma pack(push, 4)
struct value_s {
    long int intvalue;
    double floatvalue;
    int parentheses;
    int prev;
    int next;
};
#pragma pack(pop)

struct vidConfig_t {
    int width;
    int height;
    int displayFrequency;
    qboolean isFullscreen;
    float aspectRatioWindow;
    float aspectRatioPixel;
    int maxTextureSize;
    int maxTextureMaps;
    int maxTextureCoords;
    int maxActiveLights;
    Bool deviceSupportsGamma;
};

struct clientStatic_t {
    qboolean quit;
    qboolean hunkUsersStarted;
    char servername[256];
    qboolean rendererStarted;
    qboolean soundStarted;
    qboolean uiStarted;
    int frametime;
    int realtime;
    int realFrametime;
    struct clientLogo_t logo;
    int numlocalservers;
    serverInfo_t localServers[128];
    qboolean waitglobalserverresponse;
    int numglobalservers;
    serverInfo_t globalServers[20000];
    int numfavoriteservers;
    serverInfo_t favoriteServers[128];
    int pingUpdateSource;
    netadr_t updateServer;
    char updateChallenge[1024];
    char updateInfoString[1024];
    netadr_t authorizeServer;
    MaterialHandle whiteMaterial;
    MaterialHandle consoleMaterial;
    FontHandle consoleFont;
    struct vidConfig_t vidConfig;
    struct clientDebug_t debug;
#if COD2_IS_PATCH_13

    qboolean wwwDlDisconnected;
    char downloadName[256];
    char downloadTempName[256];
    char originalDownloadName[64];
    qboolean downloadRestart;
    int downloadFlags;
    qboolean wwwDlInProgress;
    fileHandle_t download;
    int downloadBlock;
    int downloadSize;
    char downloadList[1024];
#endif
};

struct viewDamage_t {
    int time;
    int duration;
    vec_t yaw;
};

struct cg_t {
    int clientFrame;
    int clientNum;
    DemoType demoType;
    CubemapShot cubemapShot;
    int cubemapSize;
    qboolean renderScreen;
    int latestSnapshotNum;
    int latestSnapshotTime;
    snapshot_t *snap;
    snapshot_t *nextSnap;
    snapshot_t activeSnapshots[2];
    float frameInterpolation;
    int frametime;
    int time;
    int oldTime;
    int physicsTime;
    qboolean mapRestart;
    qboolean renderingThirdPerson;
    playerState_t predictedPlayerState;
    centity_t predictedPlayerEntity;
    struct playerEntity_t playerEntity;
    int predictedErrorTime;
    vec3_t predictedError;
    float stepChange;
    int stepTime;
    float landChange;
    int landTime;
    vec3_t autoAnglesSlow;
    vec3_t autoAxisSlow[3];
    vec3_t autoAngles;
    vec3_t autoAxis[3];
    vec3_t autoAnglesFast;
    vec3_t autoAxisFast[3];
    refdef_t refdef;
    vec3_t refdefViewAngles;
    vec3_t swayViewAngles;
    vec3_t swayAngles;
    vec3_t swayOffset;
    int iEntityLastType[1024];
    struct XModel * pEntityLastXModel[1024];
    float zoomSensitivity;
    char infoScreenText[1024];
    char objectiveText[1024];
    char scriptMainMenu[256];
    int scoresRequestTime;
    int numScores;
    int teamScores[4];
    int teamPings[4];
    int teamPlayers[4];
    score_t scores[64];
    qboolean showScores;
    int scoreFadeTime;
    int scoresTop;
    qboolean scoresOffBottom;
    int firstLineVisible;
    int lastLineVisible;
    char killerName[32];
    char spectatorList[1024];
    int spectatorLen;
    float spectatorWidth;
    int spectatorTime;
    int spectatorPaintX;
    int spectatorPaintX2;
    int spectatorOffset;
    int spectatorPaintLen;
    qboolean showItems;
    int itemFadeTime;
    int centerPrintTime;
    int centerPrintCharWidth;
    char centerPrint[1024];
    int centerPrintLines;
    int centerPrintPriority;
    int fadeTime;
    float fadeRate;
    vec4_t fadeColor1;
    vec4_t fadeColor2;
    int drawHud;
    int crosshairClientNum;
    int crosshairClientTime;
    int crosshairPowerupNum;
    int crosshairPowerupTime;
    int identifyClientNum;
    int identifyClientHealth;
    int identifyNextTime;
    int cursorHintIcon;
    int cursorHintTime;
    int cursorHintFade;
    int cursorHintString;
    int lastClipFlashTime;
    int lastHealthPulseTime;
    int lastHealthLerpDelay;
    int lastHealthClient;
    float lastHealth;
    float healthOverlayFromAlpha;
    float healthOverlayToAlpha;
    int healthOverlayPulseTime;
    int healthOverlayPulseDuration;
    int healthOverlayPulsePhase;
    Bool healthOverlayHurt;
    int healthOverlayLastHitTime;
    float healthOverlayOldHealth;
    int healthOverlayPulseIndex;
    int proneBlockedEndTime;
    int lastStance;
    int lastStanceChangeTime;
    int lastStanceFlashTime;
    int powerupActive;
    int powerupTime;
    int attackerTime;
    int voiceTime;
    int weaponSelect;
    int weaponSelectTime;
    int weaponAnimation;
    int weaponAnimationTime;
    float fWeapSelectFrac[3];
    int iWeapSelectLastDrawTime;
    int equippedOffHand;
    viewDamage_t viewDamage[8];
    int damageTime;
    float damageX;
    float damageY;
    float damageValue;
    float viewFade;
    qboolean nomarks;
    int weapIdleTime;
    float headYaw;
    float headEndPitch;
    float headEndYaw;
    int headEndTime;
    float headStartPitch;
    float headStartYaw;
    int headStartTime;
    int v_dmg_time;
    float v_dmg_pitch;
    float v_dmg_roll;
    float fBobCycle;
    float xyspeed;
    int nextOrbitTime;
    struct GfxEntity testModelEntity;
    char testModelName[64];
    qboolean testGun;
    char testFxName[64];
    vec3_t testFxPos;
    int testFxTime;
    int testFxRespawnTime;
    vec3_t kickAVel;
    vec3_t kickAngles;
    vec3_t offsetAngles;
    vec2_t vKickScatter;
    vec2_t vNewKickScatterChange;
    float vKickScatterLerp;
    float vKickScatterFrac;
    int iLastKickScatterDir;
    vec2_t vLastCompensationViewAngles;
    vec2_t vKickCompensation;
    vec2_t vKickCompensationCentering;
    qboolean bManualKickCompensation[2];
    float gunPitch;
    float gunYaw;
    float gunXOfs;
    float gunYOfs;
    float gunZOfs;
    vec3_t vGunOffset;
    vec3_t vGunSpeed;
    vec3_t viewModelOrigin;
    vec3_t viewModelAxis[3];
    vec3_t viewModelAngles;
    int oidTeam;
    int oidPrintTime;
    int oidPrintCharWidth;
    int oidPrintY;
    char oidPrint[1024];
    int oidPrintLines;
    cameraShake_t cameraShake[4];
    float cameraShakePhase;
    float rumbleScale;
    qboolean latchVictorySound;
    int compassLastTime;
    float compassNorthYaw;
    float compassYaw;
    float compassSpeed;
    float compPointerYaw;
    float compPointerSpeed;
    int compassFadeTime;
    int healthFadeTime;
    int ammoFadeTime;
    int stanceFadeTime;
    int offhandFadeTime;
    int offhandFlashTime;
    compassactor_t compassActors[64];
    struct shellshock_t shellshock;
    struct { int time; int duration; } testShock;
    int holdBreathTime;
    int holdBreathInTime;
    int holdBreathDelay;
    float holdBreathFrac;
    qboolean adsViewErrorDone;
    qboolean inKillCam;
    struct bgs_s bgs;
    struct MarkPoly activeMarkPolys;
};

struct viewLerpWaypoint_s {
    int iFrac;
    float fViewHeight;
    int iOffset;
};

struct viewState_t {
    playerState_t *ps;
    int damageTime;
    int time;
    float v_dmg_pitch;
    float v_dmg_roll;
    float xyspeed;
    float frametime;
    float fLastIdleFactor;
    int *weapIdleTime;
};

struct water_t {
    struct WaterWritable writable;
    complex_t *H0;
    float *wTerm;
    int M;
    int N;
    float Lx;
    float Lz;
    float gravity;
    float windvel;
    vec2_t winddir;
    float amplitude;
    vec4_t codeConstant;
    GfxImage *image;
};

struct r_globals_t {
    Bool registered;
    Bool forbidDelayLoadImages;
    Bool ignorePrecacheErrors;
    vec3_t viewOrg;
    vec3_t viewDir;
    int frameCountInternal;
    int markCount;
    int totalImageMemory;
    Material * materialHashTable[1024];
    int sceneWaterMapSetupsCount;
    water_t sceneWaterMapSetups[16];
    GfxFog fogSettings[5];
    int fogIndex;
    float sinTable[1024];
    complex_t fftTrigTable[256];
    int fftBitswap[256];
    struct DObj_s *modelDObj;
    char modelDObjBuf[100];
    trStatistics_t *stats;
    struct GfxLodParms lodParms;
    const GfxViewParms *debugViewParms;
    GfxStaticModelDynamic *smodelDyncs;
    GfxSurfaceDynamic *surfaces;
    GfxCullGroupDynamic *cullGroups;
};

struct weaponInfo_s {
    struct DObj_s *viewModelDObj;
    float viewModelAnimRates[23];
    char handModel[64];
    int iPrevAnim;
    struct XAnimTree_s *tree;
    qboolean registered;
    const gitem_t *item;
    const char *pszTranslatedDisplayName;
    const char *pszTranslatedModename;
    const char *pszTranslatedAIOverlayDescription;
    struct XModel *worldSurfModel;
    struct FxEffectDef *viewFlashEffect;
    struct FxEffectDef *worldFlashEffect;
    vec3_t weaponMidpoint;
    snd_alias_list_t *missileSound;
    snd_alias_list_t *pullbackSound;
    snd_alias_list_t *flashSound;
    snd_alias_list_t *flashSoundPlayer;
    snd_alias_list_t *lastShotSound;
    snd_alias_list_t *lastShotSoundPlayer;
    snd_alias_list_t *meleeSwipeSound;
    snd_alias_list_t *rechamberSound;
    snd_alias_list_t *rechamberSoundPlayer;
    snd_alias_list_t *reloadSound;
    snd_alias_list_t *reloadSoundPlayer;
    snd_alias_list_t *reloadEmptySound;
    snd_alias_list_t *reloadEmptySoundPlayer;
    snd_alias_list_t *reloadStartSound;
    snd_alias_list_t *reloadStartSoundPlayer;
    snd_alias_list_t *reloadEndSound;
    snd_alias_list_t *reloadEndSoundPlayer;
    snd_alias_list_t *raiseSound;
    snd_alias_list_t *altSwitchSound;
    snd_alias_list_t *putawaySound;
    snd_alias_list_t * noteTrackSound[4];
    MaterialHandle ammoIcon;
    MaterialHandle hHudIcon;
    MaterialHandle hModeIcon;
    struct XModel *missileSurfModel;
    void (*missileTrailFunc)();
    float missileDlight;
    vec3_t missileDlightColor;
    int missileRenderfx;
    struct FxEffectDef *shellEjectEffect;
    struct FxEffectDef *lastShotEjectEffect;
    struct FxEffectDef *projExplosionEffect;
    snd_alias_list_t *projExplosionSound;
    struct FxEffectDef *projTrailEffect;
    float trailRadius;
    float wiTrailTime;
    MaterialHandle hReticleCenter;
    MaterialHandle hReticleSide;
    MaterialHandle hADSOverlay;
    vec3_t vPositionOrg[2];
    vec3_t vPositionAng[2];
};

struct weaponParms {
    vec3_t forward;
    vec3_t right;
    vec3_t up;
    vec3_t muzzleTrace;
    vec3_t gunForward;
    WeaponDef *weapDef;
};

struct weaponState_t {
    playerState_t *ps;
    float xyspeed;
    float frametime;
    vec3_t vLastMoveAng;
    float fLastIdleFactor;
    int time;
    int damageTime;
    float v_dmg_pitch;
    float v_dmg_roll;
    vec3_t vGunOffset;
    vec3_t vGunSpeed;
    vec3_t swayAngles;
    int *weapIdleTime;
};

struct windowDef_t {
    UiRectangle rect[4];
    UiRectangle rectClient[4];
    const char *name;
    const char *group;
    const char *cinematicName;
    int cinematic;
    int style;
    int border;
    int ownerDraw;
    int ownerDrawFlags;
    float borderSize;
    int staticFlags;
    int dynamicFlags[4];
    UiRectangle rectEffects0[4];
    UiRectangle rectEffects1[4];
    int offsetTime[4];
    int nextTime;
    vec4_t foreColor;
    vec4_t backColor;
    vec4_t borderColor;
    vec4_t outlineColor;
    MaterialHandle background;
};

struct working_state {
    JOCTET *next_output_byte;
    size_t free_in_buffer;
    savable_state cur;
    j_compress_ptr cinfo;
};

struct worldContents_s {
    int contentsStaticModels;
    int contentsEntities;
    short unsigned int entities;
    short unsigned int staticModels;
};

struct worldTree_s {
    float dist;
    short unsigned int axis;
    union { short unsigned int parent; short unsigned int nextFree; } u;
    short unsigned int child[2];
};

struct worldSector_s {
    worldContents_t contents;
    worldTree_t tree;
};

struct cm_world_t {
    vec3_t mins;
    vec3_t maxs;
    Bool lockTree;
    short unsigned int freeHead;
    worldSector_t sectors[1024];
};

struct yy_buffer_state {
    FILE *yy_input_file;
    char *yy_ch_buf;
    char *yy_buf_pos;
    yy_size_t yy_buf_size;
    int yy_n_chars;
    int yy_is_our_buffer;
    int yy_is_interactive;
    int yy_at_bol;
    int yy_fill_buffer;
    int yy_buffer_status;
};

struct z_stream_s {
    Bytef *next_in;
    uInt avail_in;
    uLong total_in;
    Bytef *next_out;
    uInt avail_out;
    uLong total_out;
    char *msg;
    struct internal_state *state;
    alloc_func zalloc;
    free_func zfree;
    voidpf opaque;
    int data_type;
    uLong adler;
    uLong reserved;
};

struct file_in_zip_read_info_s {
    char *read_buffer;
    z_stream stream;
    uLong pos_in_zipfile;
    uLong stream_initialised;
    uLong offset_local_extrafield;
    uInt size_local_extrafield;
    uLong pos_local_extrafield;
    uLong rest_read_compressed;
    uLong rest_read_uncompressed;
    FILE *file;
    uLong compression_method;
    uLong byte_before_the_zipfile;
};

typedef StdConverterARGB (*PUINT8)[4];
typedef jpeg_decompress_struct (*hist2d)[64];
typedef jpeg_d_post_controller (*histptr)[4];
typedef jpeg_component_info * (*my_fdct_ptr)[10];

#endif
#endif
