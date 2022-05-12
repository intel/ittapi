// Apply the definition of `strnlen_s` committed to mingw-w64 but not yet
// available (somehow) in the version of mingw available in the GitHub runner.
// This change is pulled from:
// https://github.com/mingw-w64/mingw-w64/commit/8a63e4c9f7252fded035c004dfe3f84e11677bf6

#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

__forceinline size_t __cdecl strnlen_s(const char* _src, size_t _count) {
  return _src ? strnlen(_src, _count) : 0;
}

#ifdef __cplusplus
}
#endif
