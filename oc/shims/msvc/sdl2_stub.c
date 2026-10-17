/* PLACEHOLDER SDL2 stubs for the MSVC client link.
 *
 * SDL2 is a USER-SUPPLIED external dependency (see README: drop SDL2 dev libs
 * under src/win32/sdl2/lib/). The repo ships none, and the bundled .a libs are
 * MinGW-format anyway. These cdecl stubs let cod2_win32.exe LINK end-to-end so
 * the engine's own symbols are proven resolved; replace with a real
 * MSVC SDL2.lib for a runnable client (the build drops these when SDL2 is found).
 *
 * Link only matches the (cdecl) symbol name, so bodyless stubs suffice; they are
 * deliberately NOT declared against the engine's SDL.h (avoids signature clashes).
 */
#ifdef _MSC_VER
#define COD2_SDL_STUB(name) int name(void) { return 0; }
COD2_SDL_STUB(SDL_AddEventWatch)
COD2_SDL_STUB(SDL_CreateWindow)
COD2_SDL_STUB(SDL_CreateWindowFrom)
COD2_SDL_STUB(SDL_GetError)
COD2_SDL_STUB(SDL_GetWindowWMInfo)
COD2_SDL_STUB(SDL_GL_CreateContext)
COD2_SDL_STUB(SDL_GL_GetAttribute)
COD2_SDL_STUB(SDL_GL_GetProcAddress)
COD2_SDL_STUB(SDL_GL_SetAttribute)
COD2_SDL_STUB(SDL_GL_SwapWindow)
COD2_SDL_STUB(SDL_IsTextInputActive)
COD2_SDL_STUB(SDL_PollEvent)
COD2_SDL_STUB(SDL_RaiseWindow)
COD2_SDL_STUB(SDL_SetHint)
COD2_SDL_STUB(SDL_SetRelativeMouseMode)
COD2_SDL_STUB(SDL_SetWindowGrab)
COD2_SDL_STUB(SDL_SetWindowInputFocus)
COD2_SDL_STUB(SDL_ShowWindow)
COD2_SDL_STUB(SDL_StartTextInput)
COD2_SDL_STUB(SDL_WarpMouseInWindow)
#endif
