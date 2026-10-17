/* =============================================================================
 * msvc_seam_aliases.h -- engine-global seam aliases for the MSVC build
 *
 * Some engine globals are reached through a second name (usually
 * an `<x>_ptr` byte-array base) that must resolve to the real symbol. The GNU /
 * web build binds these with linker `--defsym` (see the alias list in
 * src/blobs/literals.S); `/alternatename` is the exact MSVC equivalent. A
 * directive is a no-op unless `<lhs>` is actually referenced-but-undefined, so
 * this list holds ONLY seams that are live in the cl.exe link -- dead `--defsym`
 * entries from the GNU build are intentionally omitted.
 *
 * Hand-maintained (unlike the auto-generated msvc_alias_pragmas.h, which covers
 * the data-blob/stub alias pointers). Kept here rather than in the CMake link
 * flags so every /alternatename binding lives in source, in one place. x86 C
 * symbols carry one leading '_'.
 *
 * Included by msvc_link_glue.c so the directives reach the linker.
 * ===========================================================================*/
#ifdef _MSC_VER

/* game: level_locals + entity array bases (used as `<x>_ptr + n*stride`) */
COD2_ALT("level_ptr", "level")
COD2_ALT("g_entities_ptr", "g_entities")
COD2_ALT("g_time", "imp_level_bgs")
COD2_ALT("g_time_ptr", "imp_bgs")

/* server globals */
COD2_ALT("sv_ptr", "sv")
COD2_ALT("svs_ptr", "svs")

/* script VM */
COD2_ALT("scr_const_ptr", "scr_const")

/* renderer globals (rg = frontend data, vidConfig = limits) */
COD2_ALT("r_frontEndData_ptr", "rg")
COD2_ALT("r_limits_ptr", "vidConfig")

/* cgame UI globals; builtin-method table (address-suffixed blob symbol) */
COD2_ALT("cg_globUI", "legacyHacks")

#endif /* _MSC_VER */
