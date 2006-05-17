
// === Including of libs: ============================================================================
#include "../../../FCConfig.h"
#ifdef FC_OS_WIN32
#	pragma comment(lib,"TKernel.lib")
#	pragma comment(lib,"TKMath.lib")
#	pragma comment(lib,"TKService.lib")
#	pragma comment(lib,"TKGeomAlgo.lib")
#	pragma comment(lib,"TKGeomBase.lib")
#	pragma comment(lib,"TKG2d.lib")
#	pragma comment(lib,"TKG3d.lib")
#	pragma comment(lib,"TKBRep.lib")
#	pragma comment(lib,"TKTopAlgo.lib")
#	pragma comment(lib,"TKPrim.lib")
	// OpenCasCade Std IO
//#	pragma comment(lib,"TKIGESStd.lib")
//#	pragma comment(lib,"TKShHealingStd.lib")
//#	pragma comment(lib,"TKSTEPStd.lib")
	// OpenCascade View
#	pragma comment(lib,"TKV2d.lib")
#	pragma comment(lib,"TKV3d.lib")
	// Application Framwork OCAF
#	pragma comment(lib,"TKCAF.lib")
#	pragma comment(lib,"TKCDF.lib")
	// QT extension
#	pragma comment(lib,QTLIBNAME)

#else
//#	error "Dont compile this file on UNIX!"
#endif

