#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <EASTL/internal/config.h>


void* operator new[](size_t size, const char* pName, int flags, unsigned debugFlags, const char* file, int line)
{
	(void)pName;
	(void)flags;
	(void)debugFlags;
	(void)file;
	(void)line;

	return std::malloc(size);
}
void* operator new[](size_t size, size_t alignment, size_t alignmentOffset, const char* pName, int flags, unsigned debugFlags, const char* file, int line)
{
	(void)pName;
	(void)alignment;
	(void)alignmentOffset;
	(void)flags;
	(void)debugFlags;
	(void)file;
	(void)line;

	return std::malloc(size);
}

int Vsnprintf8(char8_t* pDestination, size_t n, const char8_t* pFormat, va_list arguments)
{
#ifdef _MSC_VER
	return _vsnprintf(pDestination, n, pFormat, arguments);
#else
	return vsnprintf(pDestination, n, pFormat, arguments);
#endif
}
