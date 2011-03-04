#include <cstdlib>

/*
 * copy of new operator overload in "EASTL/example/example.cpp"
 */


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
