
#if !defined( CUSTOM_SYSTEM_HEADER_H )

#if WINDOWS

/* NOTE simon: from shlwapi.h */
//
//  Association APIs
//
//  these APIs are to assist in accessing the data in HKCR
//  getting the Command strings and exe paths
//  for different verbs and extensions are simplified this way
//

enum
{
    ASSOCF_NONE                        = 0x00000000,
    ASSOCF_INIT_NOREMAPCLSID           = 0x00000001,  //  do not remap clsids to progids
    ASSOCF_INIT_BYEXENAME              = 0x00000002,  //  executable is being passed in
    ASSOCF_OPEN_BYEXENAME              = 0x00000002,  //  executable is being passed in
    ASSOCF_INIT_DEFAULTTOSTAR          = 0x00000004,  //  treat "*" as the BaseClass
    ASSOCF_INIT_DEFAULTTOFOLDER        = 0x00000008,  //  treat "Folder" as the BaseClass
    ASSOCF_NOUSERSETTINGS              = 0x00000010,  //  dont use HKCU
    ASSOCF_NOTRUNCATE                  = 0x00000020,  //  dont truncate the return string
    ASSOCF_VERIFY                      = 0x00000040,  //  verify data is accurate (DISK HITS)
    ASSOCF_REMAPRUNDLL                 = 0x00000080,  //  actually gets info about rundlls target if applicable
    ASSOCF_NOFIXUPS                    = 0x00000100,  //  attempt to fix errors if found
    ASSOCF_IGNOREBASECLASS             = 0x00000200,  //  dont recurse into the baseclass
    ASSOCF_INIT_IGNOREUNKNOWN          = 0x00000400,  //  dont use the "Unknown" progid, instead fail
#if (NTDDI_VERSION >= NTDDI_WIN8)
    ASSOCF_INIT_FIXED_PROGID           = 0x00000800,  //  the Init() pszAssoc value is a ProgId that should not be mapped using the current user defaults
    ASSOCF_IS_PROTOCOL                 = 0x00001000,  //  the Init() pszAssoc value is an uri scheme (not including the ":") that should be mapped using the current user defaults
    ASSOCF_INIT_FOR_FILE               = 0x00002000,  //  use this flag when specifying ASSOCF_INIT_FIXED_PROGID if the ProgId corresponds with a file extension based association
#endif
#if (NTDDI_VERSION >= NTDDI_WIN10_RS1)
    ASSOCF_IS_FULL_URI                 = 0x00004000,  //  Used to specify that full http/https URI is being passed for target resolution
    //  Only one of ASSOCF_INIT_FIXED_PROGID, ASSOCF_IS_PROTOCOL or ASSOCF_IS_FULL_URI can be specified at a time.
    ASSOCF_PER_MACHINE_ONLY            = 0x00008000,  //  Enforces per-machine association look-up only and avoid HKCU.
#endif
};
typedef DWORD ASSOCF;

typedef enum
{
    ASSOCSTR_COMMAND      = 1,  //  shell\verb\command string
    ASSOCSTR_EXECUTABLE,        //  the executable part of command string
    ASSOCSTR_FRIENDLYDOCNAME,   //  friendly name of the document type
    ASSOCSTR_FRIENDLYAPPNAME,   //  friendly name of executable
    ASSOCSTR_NOOPEN,            //  noopen value
    ASSOCSTR_SHELLNEWVALUE,     //  query values under the shellnew key
    ASSOCSTR_DDECOMMAND,        //  template for DDE commands
    ASSOCSTR_DDEIFEXEC,         //  DDECOMMAND to use if just create a process
    ASSOCSTR_DDEAPPLICATION,    //  Application name in DDE broadcast
    ASSOCSTR_DDETOPIC,          //  Topic Name in DDE broadcast
    ASSOCSTR_INFOTIP,           //  info tip for an item, or list of properties to create info tip from
#if (_WIN32_IE >= _WIN32_IE_IE60)
    ASSOCSTR_QUICKTIP,          //  same as ASSOCSTR_INFOTIP, except, this list contains only quickly retrievable properties
    ASSOCSTR_TILEINFO,          //  similar to ASSOCSTR_INFOTIP - lists important properties for tileview
    ASSOCSTR_CONTENTTYPE,       //  MIME Content type
    ASSOCSTR_DEFAULTICON,       //  Default icon source
    ASSOCSTR_SHELLEXTENSION,    //  Guid string pointing to the Shellex\Shellextensionhandler value.
#endif // _WIN32_IE_IE60
#if (_WIN32_IE >= _WIN32_IE_IE80)
    ASSOCSTR_DROPTARGET,        //  The CLSID of DropTarget
    ASSOCSTR_DELEGATEEXECUTE,   //  The CLSID of DelegateExecute
#endif // _WIN32_IE_IE80
    // a string value of the uri protocol schemes, for example "http:https:ftp:file:" or "*" indicating all
    ASSOCSTR_SUPPORTED_URI_PROTOCOLS,
#if (NTDDI_VERSION >= NTDDI_WIN10)
    ASSOCSTR_PROGID,            // The ProgId provided by the app associated with the file type or uri scheme based on user default settings.
    ASSOCSTR_APPID,             // The AppUserModelID of the app associated with the file type or uri scheme based on user default settings.
    ASSOCSTR_APPPUBLISHER,      // THe publisher of the app associated with the file type or uri scheme based on user default settings.
    ASSOCSTR_APPICONREFERENCE,  // The icon reference of the app associated with the file type or uri scheme based on user default settings.
#endif // NTDDI_WIN10
    ASSOCSTR_MAX                //  last item in enum...
} ASSOCSTR;

/*LWSTDAPI*/ HRESULT AssocQueryStringA( ASSOCF flags, ASSOCSTR str, LPCSTR pszAssoc, LPCSTR pszExtra, LPSTR pszOut, DWORD *pcchOut );
/*LWSTDAPI*/ HRESULT AssocQueryStringW( ASSOCF flags, ASSOCSTR str, LPCWSTR pszAssoc, LPCWSTR pszExtra, LPWSTR pszOut, DWORD *pcchOut);

/* NOTE simon: end of shlwapi.h */

/* NOTE simon: How to get GUID https://hero.handmade.network/forums/code-discussion/t/7350-waveout_sound_api#22896

__uiidof(X) returns GUID from type X.
If you look how MMDeviceEnumerator is defined, you'll see this:

[code]
MIDL_INTERFACE("A95664D2-9614-4F35-A746-DE8DB63617E6")
IMMDeviceEnumerator : public IUnknown
{
...
}
[/code]

__uiidof(MMDeviceEnumerator) will produce A95664D2-9614-4F35-A746-DE8DB63617E6 as GUID type.

To fix this in C, you should include #include <initguid.h> header first (before all the Windows stuff) or #define INITGUID before all header includes. This will make CLSID/IID guids to be defined. Without it they are only declared extern. 
*/

/* NOTE simon: from guiddef.h (modified) */

#if 0

/*
GUIDs are horrible.
They are given in the doc and sources in this format.
xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx

In practice they are a 16 bytes value. In C they are defined as a structure:
*/

struct GUID {
    long data1; // 4 bytes
    short data2; // 2 bytes
    short data3; // 2 bytes
    char data4[ 8 ]; // 8 bytes
};

/*
01234567-0123-0123-0123-456789abcdef

The first numbers is data1, if its 01234567, we write 0x1234567 in data1;
The second number is data2, if its 0123, we write 0x0123 in data2;
The third number is data3, if tis 0123, we write 0x0123 in data3;

Starting from the 4th number the rest are bytes value that should be written
left to right, the left side being the first number in the data4 array.
*/

data4[ 0 ] = 0x01;
data4[ 1 ] = 0x23;
data4[ 2 ] = 0x45;
data4[ 3 ] = 0x67;
data4[ 4 ] = 0x89;
data4[ 5 ] = 0xab;
data4[ 6 ] = 0xcd;
data4[ 7 ] = 0xef;

#endif

typedef struct csh_guid_t {
    long data1; // 4 bytes
    short data2; // 2 bytes
    short data3; // 2 bytes
    char unsigned data4[ 8 ]; // 8 bytes
} csh_guid_t;

#define csh_define_guid( name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8 ) const csh_guid_t csh_##name = { l, w1, w2, { b1, b2, b3, b4, b5, b6, b7, b8 } }

#define csh_define_guid_2( name, data_1, data_2, data_3, data_4, data_5 ) \
const csh_guid_t csh_##name = { data_1, data_2, data_3, {\
( char unsigned ) ( ( data_4 >> 8 ) & 0xff ),\
( char unsigned ) ( ( data_4 >> 0 ) & 0xff ),\
( char unsigned ) ( ( data_5 >> 40 ) & 0xff ),\
( char unsigned ) ( ( data_5 >> 32 ) & 0xff ),\
( char unsigned ) ( ( data_5 >> 24 ) & 0xff ),\
( char unsigned ) ( ( data_5 >> 16 ) & 0xff ),\
( char unsigned ) ( ( data_5 >> 8 ) & 0xff ),\
( char unsigned ) ( ( data_5 >> 0 ) & 0xff ) } }

/* NOTE simon: end of guiddef.h */

/* NOTE simon: from combaseapi.h */
typedef struct csh_tagPROPVARIANT csh_PROPVARIANT;
#define csh_CLSCTX_ALL ( 0x1 | 0x2 | 0x4 | 0x10 )

#define WINOLEAPI_(type) EXTERN_C DECLSPEC_IMPORT type STDAPICALLTYPE

WINOLEAPI_(void) CoTaskMemFree( _Frees_ptr_opt_ LPVOID pv );
/* NOTE simon: end of combaseapi.h */

/* NOTE simon: From mmreg.h */

#pragma pack( push, 1 )

typedef struct csh_WAVEFORMATEX {
    
    WORD    wFormatTag;        /* format type */
    WORD    nChannels;         /* number of channels (i.e. mono, stereo...) */
    DWORD   nSamplesPerSec;    /* sample rate */
    DWORD   nAvgBytesPerSec;   /* for buffer estimation */
    WORD    nBlockAlign;       /* block size of data */
    WORD    wBitsPerSample;    /* Number of bits per sample of mono data */
    WORD    cbSize;            /* The count in bytes of the size of
extra information (after cbSize) */
    
} csh_WAVEFORMATEX;

typedef struct csh_WAVEFORMATEXTENSIBLE {
    
    csh_WAVEFORMATEX    Format;
    union {
        WORD wValidBitsPerSample;       /* bits of precision  */
        WORD wSamplesPerBlock;          /* valid if wBitsPerSample==0 */
        WORD wReserved;                 /* If neither applies, set to zero. */
    } Samples;
    DWORD           dwChannelMask;      /* which channels are */
    /* present in stream  */
    csh_guid_t            SubFormat;
} csh_WAVEFORMATEXTENSIBLE;

#pragma pack( pop )  /* Assume byte packing throughout */

/* NOTE simon: End of mmreg.h*/

/* NOTE simon: audiosessiontypes.h */
typedef enum csh_AUDCLNT_SHAREMODE {
    csh_AUDCLNT_SHAREMODE_SHARED,
    csh_AUDCLNT_SHAREMODE_EXCLUSIVE
} csh_AUDCLNT_SHAREMODE;

#define csh_AUDCLNT_STREAMFLAGS_CROSSPROCESS             0x00010000
#define csh_AUDCLNT_STREAMFLAGS_LOOPBACK                 0x00020000
#define csh_AUDCLNT_STREAMFLAGS_EVENTCALLBACK            0x00040000
#define csh_AUDCLNT_STREAMFLAGS_NOPERSIST                0x00080000
#define csh_AUDCLNT_STREAMFLAGS_RATEADJUST               0x00100000
#define csh_AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY      0x08000000
#define csh_AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM           0x80000000
#define csh_AUDCLNT_SESSIONFLAGS_EXPIREWHENUNOWNED       0x10000000
#define csh_AUDCLNT_SESSIONFLAGS_DISPLAY_HIDE            0x20000000
#define csh_AUDCLNT_SESSIONFLAGS_DISPLAY_HIDEWHENEXPIRED 0x40000000

/* NOTE simon: end of audiosessiontypes.h */

/* NOTE simon: from propsys.h */
typedef struct csh_IPropertyStore csh_IPropertyStore;
/* NOTE simon: end of propsys.h */

/* NOTE simon (13/10/23 14:07:24): from wtypes.h */
typedef struct csh_tagpropertykey {
    GUID fmtid;
    DWORD pid;
} csh_PROPERTYKEY;
/* NOTE simon (13/10/23 14:07:31): end of wtypes.h */

/* NOTE simon: from mmdeviceapi.h */

csh_define_guid( CLSID_MMDeviceEnumerator, 0xBCDE0395, 0xE52F, 0x467C, 0x8E, 0x3D, 0xC4, 0x57, 0x92, 0x91, 0x69, 0x2E );
csh_define_guid( IID_IMMDeviceEnumerator, 0xA95664D2, 0x9614, 0x4F35, 0xA7, 0x46, 0xDE, 0x8D, 0xB6, 0x36, 0x17, 0xE6 );

typedef struct csh_IMMNotificationClient csh_IMMNotificationClient;
typedef struct csh_IMMDeviceEnumerator csh_IMMDeviceEnumerator;
typedef struct csh_IMMDeviceCollection csh_IMMDeviceCollection;
typedef struct csh_IMMDevice csh_IMMDevice;

typedef enum {
    csh_eConsole = 0,
    csh_eMultimedia = ( csh_eConsole + 1 ),
    csh_eCommunications = ( csh_eMultimedia + 1 ),
    csh_ERole_enum_count = ( csh_eCommunications + 1 )
} csh_ERole;

typedef enum {
    csh_eRender = 0,
    csh_eCapture = ( csh_eRender + 1 ) ,
    csh_eAll = ( csh_eCapture + 1 ) ,
    csh_EDataFlow_enum_count = ( csh_eAll + 1 )
} csh_EDataFlow;


typedef struct csh_IMMNotificationClientVtbl {
    
    HRESULT ( *QueryInterface )( csh_IMMNotificationClient* This, REFIID riid, void **ppvObject );
    ULONG ( *AddRef )( csh_IMMNotificationClient* This );
    ULONG ( *Release )( csh_IMMNotificationClient* This );
    HRESULT ( *OnDeviceStateChanged )( csh_IMMNotificationClient* This, LPCWSTR pwstrDeviceId, DWORD dwNewState );
    HRESULT ( *OnDeviceAdded )( csh_IMMNotificationClient* This, LPCWSTR pwstrDeviceId );
    HRESULT ( *OnDeviceRemoved )( csh_IMMNotificationClient* This, LPCWSTR pwstrDeviceId );
    HRESULT ( *OnDefaultDeviceChanged )( csh_IMMNotificationClient* This, csh_EDataFlow flow, csh_ERole role, LPCWSTR pwstrDefaultDeviceId );
    HRESULT ( *OnPropertyValueChanged )( csh_IMMNotificationClient* This, LPCWSTR pwstrDeviceId, const csh_PROPERTYKEY key );
    
} csh_IMMNotificationClientVtbl;

typedef struct csh_IMMNotificationClient {
    csh_IMMNotificationClientVtbl *lpVtbl;
} csh_IMMNotificationClient;

#if 0

#define csh_IMMNotificationClient_QueryInterface( This, riid, ppvObject )	\
( ( This )->lpVtbl->QueryInterface( This, riid, ppvObject ) )

#define csh_IMMNotificationClient_AddRef( This )	\
( ( This )->lpVtbl->AddRef( This ) )

#define csh_IMMNotificationClient_Release( This )	\
( ( This )->lpVtbl->Release( This ) )

#define csh_IMMNotificationClient_OnDeviceStateChanged( This, pwstrDeviceId, dwNewState )	\
( ( This )->lpVtbl->OnDeviceStateChanged( This, pwstrDeviceId, dwNewState ) )

#define csh_IMMNotificationClient_OnDeviceAdded( This, pwstrDeviceId )	\
( ( This )->lpVtbl->OnDeviceAdded( This, pwstrDeviceId ) )

#define csh_IMMNotificationClient_OnDeviceRemoved( This, pwstrDeviceId )	\
( ( This )->lpVtbl->OnDeviceRemoved( This, pwstrDeviceId ) )

#define csh_IMMNotificationClient_OnDefaultDeviceChanged( This, flow, role, pwstrDefaultDeviceId )	\
( ( This )->lpVtbl->OnDefaultDeviceChanged( This, flow, role, pwstrDefaultDeviceId ) )

#define csh_IMMNotificationClient_OnPropertyValueChanged( This, pwstrDeviceId, key )	\
( ( This )->lpVtbl->OnPropertyValueChanged( This, pwstrDeviceId, key ) )

#endif

/* NOTE simon: It seems that iid is the same as guid and refiid is just *iid. */

typedef struct csh_IMMDeviceVtbl {
    
    HRESULT ( *QueryInterface ) ( csh_IMMDevice* This, REFIID riid, void** ppvObject );
    ULONG ( *AddRef )( csh_IMMDevice* This );
    ULONG ( *Release )( csh_IMMDevice* This );
    HRESULT ( *Activate )( csh_IMMDevice* This, const IID* iid, DWORD dwClsCtx, csh_PROPVARIANT* pActivationParams, void** ppInterface );
    HRESULT ( *OpenPropertyStore )( csh_IMMDevice* This, DWORD stgmAccess, csh_IPropertyStore** ppProperties );
    HRESULT ( *GetId )( csh_IMMDevice* This, LPWSTR* ppstrId );
    HRESULT ( *GetState )( csh_IMMDevice* This, DWORD* pdwState );
    
} csh_IMMDeviceVtbl;

typedef struct csh_IMMDevice {
    csh_IMMDeviceVtbl* lpVtbl;
} csh_IMMDevice;

#define csh_IMMDevice_QueryInterface( This, riid, ppvObject )	\
( ( This )->lpVtbl->QueryInterface( This, riid, ppvObject ) )

#define csh_IMMDevice_AddRef( This )	\
( ( This )->lpVtbl->AddRef( This ) )

#define csh_IMMDevice_Release( This )	\
( ( This )->lpVtbl->Release( This ) )

#define csh_IMMDevice_Activate( This, iid, dwClsCtx, pActivationParams, ppInterface )	\
( ( This )->lpVtbl->Activate( This, iid, dwClsCtx, pActivationParams, ppInterface ) )

#define csh_IMMDevice_OpenPropertyStore( This, stgmAccess, ppProperties )	\
( ( This )->lpVtbl->OpenPropertyStore( This, stgmAccess, ppProperties ) )

#define csh_IMMDevice_GetId( This, ppstrId )	\
( ( This )->lpVtbl->GetId( This, ppstrId ) )

#define csh_IMMDevice_GetState( This, pdwState )	\
( ( This )->lpVtbl->GetState( This, pdwState ) )

typedef struct csh_IMMDeviceEnumeratorVtbl {
    
    HRESULT ( *QueryInterface )( csh_IMMDeviceEnumerator* This, REFIID riid, void **ppvObject );
    ULONG ( *AddRef )( csh_IMMDeviceEnumerator* This );
    ULONG ( *Release )( csh_IMMDeviceEnumerator* This );
    HRESULT ( *EnumAudioEndpoints )( csh_IMMDeviceEnumerator* This, csh_EDataFlow dataFlow, DWORD dwStateMask, csh_IMMDeviceCollection** ppDevices );
    HRESULT ( *GetDefaultAudioEndpoint )( csh_IMMDeviceEnumerator* This, csh_EDataFlow dataFlow, csh_ERole role, csh_IMMDevice** ppEndpoint );
    HRESULT ( *GetDevice )( csh_IMMDeviceEnumerator* This, LPCWSTR pwstrId, csh_IMMDevice** ppDevice );
    HRESULT ( *RegisterEndpointNotificationCallback )( csh_IMMDeviceEnumerator* This, csh_IMMNotificationClient* pClient );
    HRESULT ( *UnregisterEndpointNotificationCallback )( csh_IMMDeviceEnumerator* This, csh_IMMNotificationClient* pClient );
    
} csh_IMMDeviceEnumeratorVtbl;

typedef struct csh_IMMDeviceEnumerator {
    csh_IMMDeviceEnumeratorVtbl* lpVtbl;
} csh_IMMDeviceEnumerator;

#define csh_IMMDeviceEnumerator_QueryInterface( This, riid, ppvObject )	\
( ( This )->lpVtbl->QueryInterface( This, riid, ppvObject ) )

#define csh_IMMDeviceEnumerator_AddRef( This )	\
( ( This )->lpVtbl->AddRef( This ) )

#define csh_IMMDeviceEnumerator_Release( This )	\
( ( This )->lpVtbl->Release( This ) )

#define csh_IMMDeviceEnumerator_EnumAudioEndpoints( This, dataFlow, dwStateMask, ppDevices )	\
( ( This )->lpVtbl->EnumAudioEndpoints( This, dataFlow, dwStateMask, ppDevices ) )

#define csh_IMMDeviceEnumerator_GetDefaultAudioEndpoint( This, dataFlow, role, ppEndpoint )	\
( ( This )->lpVtbl->GetDefaultAudioEndpoint( This, dataFlow, role, ppEndpoint ) )

#define csh_IMMDeviceEnumerator_GetDevice( This, pwstrId, ppDevice )	\
( ( This )->lpVtbl->GetDevice( This, pwstrId, ppDevice ) )

#define csh_IMMDeviceEnumerator_RegisterEndpointNotificationCallback( This, pClient )	\
( ( This )->lpVtbl->RegisterEndpointNotificationCallback( This, pClient ) )

#define csh_IMMDeviceEnumerator_UnregisterEndpointNotificationCallback( This, pClient )	\
( ( This )->lpVtbl->UnregisterEndpointNotificationCallback( This, pClient ) )

/* NOTE simon: end of mmdeviceapi.h */

/* NOTE simon: audioclient.h */
csh_define_guid( IID_IAudioClient, 0x1CB9AD4C, 0xDBFA, 0x4c32, 0xB1, 0x78, 0xC2, 0xF5, 0x68, 0xA7, 0x03, 0xB2 );
csh_define_guid( IID_IAudioRenderClient, 0xF294ACFC, 0x3146, 0x4483, 0xA7, 0xBF, 0xAD, 0xDC, 0xA7, 0xC2, 0x60, 0xE2 );

typedef u64 REFERENCE_TIME;
typedef struct csh_IAudioClient csh_IAudioClient;

enum csh_AUDCLNT_BUFFERFLAGS {
    csh_AUDCLNT_BUFFERFLAGS_DATA_DISCONTINUITY	= 0x1,
    csh_AUDCLNT_BUFFERFLAGS_SILENT	= 0x2,
    csh_AUDCLNT_BUFFERFLAGS_TIMESTAMP_ERROR	= 0x4
};

#if 0
#define AUDCLNT_ERR(n) MAKE_HRESULT(SEVERITY_ERROR, FACILITY_AUDCLNT, n)
#define AUDCLNT_SUCCESS(n) MAKE_SCODE(SEVERITY_SUCCESS, FACILITY_AUDCLNT, n)
#define AUDCLNT_E_NOT_INITIALIZED              AUDCLNT_ERR(0x001)
#define AUDCLNT_E_ALREADY_INITIALIZED          AUDCLNT_ERR(0x002)
#define AUDCLNT_E_WRONG_ENDPOINT_TYPE          AUDCLNT_ERR(0x003)
#define AUDCLNT_E_DEVICE_INVALIDATED           AUDCLNT_ERR(0x004)
#define AUDCLNT_E_NOT_STOPPED                  AUDCLNT_ERR(0x005)
#define AUDCLNT_E_BUFFER_TOO_LARGE             AUDCLNT_ERR(0x006)
#define AUDCLNT_E_OUT_OF_ORDER                 AUDCLNT_ERR(0x007)
#define AUDCLNT_E_UNSUPPORTED_FORMAT           AUDCLNT_ERR(0x008)
#define AUDCLNT_E_INVALID_SIZE                 AUDCLNT_ERR(0x009)
#define AUDCLNT_E_DEVICE_IN_USE                AUDCLNT_ERR(0x00a)
#define AUDCLNT_E_BUFFER_OPERATION_PENDING     AUDCLNT_ERR(0x00b)
#define AUDCLNT_E_THREAD_NOT_REGISTERED        AUDCLNT_ERR(0x00c)
#define AUDCLNT_E_EXCLUSIVE_MODE_NOT_ALLOWED   AUDCLNT_ERR(0x00e)
#define AUDCLNT_E_ENDPOINT_CREATE_FAILED       AUDCLNT_ERR(0x00f)
#define AUDCLNT_E_SERVICE_NOT_RUNNING          AUDCLNT_ERR(0x010)
#define AUDCLNT_E_EVENTHANDLE_NOT_EXPECTED     AUDCLNT_ERR(0x011)
#define AUDCLNT_E_EXCLUSIVE_MODE_ONLY          AUDCLNT_ERR(0x012)
#define AUDCLNT_E_BUFDURATION_PERIOD_NOT_EQUAL AUDCLNT_ERR(0x013)
#define AUDCLNT_E_EVENTHANDLE_NOT_SET          AUDCLNT_ERR(0x014)
#define AUDCLNT_E_INCORRECT_BUFFER_SIZE        AUDCLNT_ERR(0x015)
#define AUDCLNT_E_BUFFER_SIZE_ERROR            AUDCLNT_ERR(0x016)
#define AUDCLNT_E_CPUUSAGE_EXCEEDED            AUDCLNT_ERR(0x017)
#define AUDCLNT_E_BUFFER_ERROR                 AUDCLNT_ERR(0x018)
#define AUDCLNT_E_BUFFER_SIZE_NOT_ALIGNED      AUDCLNT_ERR(0x019)
#define AUDCLNT_E_INVALID_DEVICE_PERIOD        AUDCLNT_ERR(0x020)
#define AUDCLNT_E_INVALID_STREAM_FLAG          AUDCLNT_ERR(0x021)
#define AUDCLNT_E_ENDPOINT_OFFLOAD_NOT_CAPABLE AUDCLNT_ERR(0x022)
#define AUDCLNT_E_OUT_OF_OFFLOAD_RESOURCES     AUDCLNT_ERR(0x023)
#define AUDCLNT_E_OFFLOAD_MODE_ONLY            AUDCLNT_ERR(0x024)
#define AUDCLNT_E_NONOFFLOAD_MODE_ONLY         AUDCLNT_ERR(0x025)
#define AUDCLNT_E_RESOURCES_INVALIDATED        AUDCLNT_ERR(0x026)
#define AUDCLNT_E_RAW_MODE_UNSUPPORTED         AUDCLNT_ERR(0x027)
#define AUDCLNT_E_ENGINE_PERIODICITY_LOCKED    AUDCLNT_ERR(0x028)
#define AUDCLNT_E_ENGINE_FORMAT_LOCKED         AUDCLNT_ERR(0x029)
#define AUDCLNT_E_HEADTRACKING_ENABLED         AUDCLNT_ERR(0x030)
#define AUDCLNT_E_HEADTRACKING_UNSUPPORTED     AUDCLNT_ERR(0x040)
#define AUDCLNT_E_EFFECT_NOT_AVAILABLE         AUDCLNT_ERR(0x041)
#define AUDCLNT_E_EFFECT_STATE_READ_ONLY       AUDCLNT_ERR(0x042)
#define AUDCLNT_S_BUFFER_EMPTY                 AUDCLNT_SUCCESS(0x001)
#define AUDCLNT_S_THREAD_ALREADY_REGISTERED    AUDCLNT_SUCCESS(0x002)
#define AUDCLNT_S_POSITION_STALLED             AUDCLNT_SUCCESS(0x003)
#endif

typedef struct csh_IAudioClientVtbl {
    
    HRESULT ( *QueryInterface )( csh_IAudioClient* This, REFIID riid, void** ppvObject );
    ULONG ( *AddRef )( csh_IAudioClient* This );
    ULONG ( *Release )( csh_IAudioClient* This );
    HRESULT ( *Initialize )( csh_IAudioClient* This, csh_AUDCLNT_SHAREMODE ShareMode, DWORD StreamFlags, REFERENCE_TIME hnsBufferDuration,   REFERENCE_TIME hnsPeriodicity, const csh_WAVEFORMATEX* pFormat, LPCGUID AudioSessionGuid );
    HRESULT ( *GetBufferSize )( csh_IAudioClient* This, UINT32* pNumBufferFrames );
    HRESULT ( *GetStreamLatency )( csh_IAudioClient* This, REFERENCE_TIME* phnsLatency );
    HRESULT ( *GetCurrentPadding )( csh_IAudioClient* This, UINT32* pNumPaddingFrames );
    HRESULT ( *IsFormatSupported )( csh_IAudioClient* This, csh_AUDCLNT_SHAREMODE ShareMode, const csh_WAVEFORMATEX* pFormat, csh_WAVEFORMATEX** ppClosestMatch );
    HRESULT ( *GetMixFormat )( csh_IAudioClient* This, csh_WAVEFORMATEX** ppDeviceFormat );
    HRESULT ( *GetDevicePeriod )( csh_IAudioClient* This, REFERENCE_TIME* phnsDefaultDevicePeriod, REFERENCE_TIME* phnsMinimumDevicePeriod );
    HRESULT ( *Start )( csh_IAudioClient* This );
    HRESULT ( *Stop )( csh_IAudioClient* This );
    HRESULT ( *Reset )( csh_IAudioClient* This );
    HRESULT ( *SetEventHandle )( csh_IAudioClient* This, HANDLE eventHandle );
    HRESULT ( *GetService )( csh_IAudioClient* This, const IID* riid, void** ppv );
    
} csh_IAudioClientVtbl;

typedef struct csh_IAudioClient {
    csh_IAudioClientVtbl* lpVtbl;
} csh_IAudioClient;

#define csh_IAudioClient_QueryInterface( This, riid, ppvObject )	\
( ( This )->lpVtbl->QueryInterface( This, riid, ppvObject ) )

#define csh_IAudioClient_AddRef( This )	\
( ( This )->lpVtbl->AddRef( This ) )

#define csh_IAudioClient_Release( This )	\
( ( This )->lpVtbl->Release( This ) )

#define csh_IAudioClient_Initialize( This, ShareMode, StreamFlags, hnsBufferDuration, hnsPeriodicity, pFormat, AudioSessionGuid )	\
( ( This )->lpVtbl->Initialize( This, ShareMode, StreamFlags, hnsBufferDuration, hnsPeriodicity, pFormat, AudioSessionGuid ) )

#define csh_IAudioClient_GetBufferSize( This, pNumBufferFrames )	\
( ( This )->lpVtbl->GetBufferSize( This, pNumBufferFrames ) )

#define csh_IAudioClient_GetStreamLatency( This, phnsLatency )	\
( ( This )->lpVtbl->GetStreamLatency( This, phnsLatency ) )

#define csh_IAudioClient_GetCurrentPadding( This, pNumPaddingFrames )	\
( ( This )->lpVtbl->GetCurrentPadding( This, pNumPaddingFrames ) )

#define csh_IAudioClient_IsFormatSupported( This, ShareMode, pFormat, ppClosestMatch )	\
( ( This )->lpVtbl->IsFormatSupported( This, ShareMode, pFormat, ppClosestMatch ) )

#define csh_IAudioClient_GetMixFormat( This, ppDeviceFormat )	\
( ( This )->lpVtbl->GetMixFormat( This, ppDeviceFormat ) )

#define csh_IAudioClient_GetDevicePeriod( This, phnsDefaultDevicePeriod, phnsMinimumDevicePeriod )	\
( ( This )->lpVtbl->GetDevicePeriod( This, phnsDefaultDevicePeriod, phnsMinimumDevicePeriod ) )

#define csh_IAudioClient_Start( This )	\
( ( This )->lpVtbl->Start( This ) )

#define csh_IAudioClient_Stop( This )	\
( ( This )->lpVtbl->Stop( This ) )

#define csh_IAudioClient_Reset( This )	\
( ( This )->lpVtbl->Reset( This ) )

#define csh_IAudioClient_SetEventHandle( This, eventHandle )	\
( ( This )->lpVtbl->SetEventHandle( This, eventHandle ) )

#define csh_IAudioClient_GetService( This, riid, ppv )	\
( ( This )->lpVtbl->GetService( This, riid, ppv ) )

typedef struct csh_IAudioRenderClient csh_IAudioRenderClient;

typedef struct csh_IAudioRenderClientVtbl {
    
    HRESULT ( *QueryInterface )( csh_IAudioRenderClient* This, REFIID riid, void** ppvObject );
    ULONG ( *AddRef )( csh_IAudioRenderClient* This );
    ULONG ( *Release )( csh_IAudioRenderClient* This );
    HRESULT ( *GetBuffer )( csh_IAudioRenderClient* This, UINT32 NumFramesRequested, BYTE** ppData );
    HRESULT ( *ReleaseBuffer )( csh_IAudioRenderClient* This, UINT32 NumFramesWritten, DWORD dwFlags );
    
} csh_IAudioRenderClientVtbl;

typedef struct csh_IAudioRenderClient {
    csh_IAudioRenderClientVtbl* lpVtbl;
} csh_IAudioRenderClient;

#define csh_IAudioRenderClient_QueryInterface( This, riid, ppvObject )	\
( ( This )->lpVtbl->QueryInterface( This, riid, ppvObject ) )

#define csh_IAudioRenderClient_AddRef( This )	\
( ( This )->lpVtbl->AddRef( This ) )

#define csh_IAudioRenderClient_Release( This )	\
( ( This )->lpVtbl->Release( This ) )

#define csh_IAudioRenderClient_GetBuffer( This, NumFramesRequested, ppData )	\
( ( This )->lpVtbl->GetBuffer( This, NumFramesRequested, ppData ) )

#define csh_IAudioRenderClient_ReleaseBuffer( This, NumFramesWritten, dwFlags )	\
( ( This )->lpVtbl->ReleaseBuffer( This, NumFramesWritten, dwFlags ) )

/* NOTE simon: end of audioclient.h */

/* NOTE simon: From dwmapi.h */
EXTERN_C DECLSPEC_IMPORT HRESULT STDAPICALLTYPE DwmIsCompositionEnabled( BOOL* pfEnabled );
/* NOTE simon: end of dwmapi.h */

/* NOTE simon: From shtypes.h */
typedef GUID KNOWNFOLDERID;
#define REFKNOWNFOLDERID const KNOWNFOLDERID* const

/* NOTE simon: From shlobj_core.h */
STDAPI SHGetKnownFolderPath(_In_ REFKNOWNFOLDERID rfid,
                            _In_ DWORD /* KNOWN_FOLDER_FLAG */ dwFlags,
                            _In_opt_ HANDLE hToken,
                            _Outptr_ PWSTR *ppszPath); // free *ppszPath with CoTaskMemFree
/* NOTE simon: end of shlobj_core.h */

#endif

#define CUSTOM_SYSTEM_HEADER_H
#endif
