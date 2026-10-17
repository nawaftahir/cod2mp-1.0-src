/* MSS_auto_cleanup: Miles mss.h startup hook, a plain (non-LTCG) object inside mss32.lib. */
#include <stdlib.h>

void __stdcall AIL_shutdown(void);

int __cdecl MSS_auto_cleanup(void)
{
	atexit((void (__cdecl *)(void))AIL_shutdown);
	return 0;
}
