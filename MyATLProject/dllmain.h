// dllmain.h: 模块类的声明。

class CMyATLProjectModule : public ATL::CAtlDllModuleT< CMyATLProjectModule >
{
public :
	DECLARE_LIBID(LIBID_MyATLProjectLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_MYATLPROJECT, "{10c74815-1ff0-477c-9aa6-5f16e2551a19}")
};

extern class CMyATLProjectModule _AtlModule;
