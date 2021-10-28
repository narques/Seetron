// The following ifdef block is the standard way of creating macros which make exporting
// from a DLL simpler. All files within this DLL are compiled with the LXCORE_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see
// LXCORE_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef LXCORE_EXPORTS
#define LXCORE_API __declspec(dllexport)
#else
#define LXCORE_API __declspec(dllimport)
#endif

// This class is exported from the dll
class LXCORE_API CLXCore {
public:
	CLXCore(void);
	// TODO: add your methods here.
};

extern LXCORE_API int nLXCore;

LXCORE_API int fnLXCore(void);
