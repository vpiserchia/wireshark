/* ws_diag_control.h
 * Turn compiler diagnostic messages on and off.
 *
 * From FreeRADIUS build.h.
 *
 * @copyright 2013 The FreeRADIUS server project
 *
 * That project is covered by the GPLv2, so:
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef __WS_DIAG_CONTROL_H__
#define __WS_DIAG_CONTROL_H__

#include "ws_compiler_tests.h"

#ifdef __cplusplus
extern "C" {
#endif

#define XSTRINGIFY(x) #x

/*
 *	Macros for controlling warnings in various compilers.
 */
#define DIAG_JOINSTR(x,y) XSTRINGIFY(x ## y)

/*
 * XXX - this is only for GCC or GCC-compatible compilers, and we only use
 * it to have a macro that takes a warning as an argument and turns it
 * off in the appropriate fashion for Clang and GCC; it should only be
 * used internally in this header.
 */
#define DIAG_DO_PRAGMA(x) _Pragma (#x)

#if defined(__clang__)
  /*
   * Clang, so we'd use _Pragma("clang diagnostic XXX"), if it's
   * supported.
   */
  #if WS_IS_AT_LEAST_CLANG_VERSION(2,8)
    /*
     * This is Clang 2.8 or later: we can use "clang diagnostic ignored -Wxxx"
     * and "clang diagnostic push/pop".
     *
     * The good news is that this can be used for diagnostics that
     * might be errors or might just be warnings.  The bad news is
     * that DIAG_ON() just undoes the results of the most recent
     * DIAG_OFF(), independent of its argument, so you have to be
     * careful in how you nest them.
     */
    #define DIAG_PRAGMA(x) DIAG_DO_PRAGMA(clang diagnostic x)
    #define DIAG_OFF(x) DIAG_PRAGMA(push) DIAG_PRAGMA(ignored DIAG_JOINSTR(-W,x))
    #define DIAG_ON(x) DIAG_PRAGMA(pop)
  #endif

  /*
   * Not all versions of Clang understand -Wpedantic.
   */
  #if WS_IS_AT_LEAST_CLANG_VERSION(4,0)
    /*
     * This is Clang 4.0, which appears to be the first version to
     * support -Wpedantic.
     */
    #define DIAG_OFF_PEDANTIC DIAG_OFF(pedantic)
    #define DIAG_ON_PEDANTIC DIAG_ON(pedantic)
  #else
    /*
     * -Wpedantic isn't supported, so there's nothing to turn on or
     * off, and we can't turn it on or off in any case.
     */
    #define DIAG_OFF_PEDANTIC
    #define DIAG_ON_PEDANTIC
  #endif
#elif defined(__GNUC__)
  /*
   * GCC, or a compiler (other than Clang) that claims to be GCC.
   * We assume that the compiler accepts _Pragma("GCC diagnostic xxx")
   * even if it's only claiming to be GCC.
   */
  #if WS_IS_AT_LEAST_GNUC_VERSION(4,2)
    /*
     * This is GCC 4.2 or later, or a compiler claiming to be that.
     * We can use "GCC diagnostic ignored -Wxxx"; we may or may
     * not be able to use "GCC diagnostic push/pop" (introduced in 4.6).
     */
    #define DIAG_PRAGMA(x) DIAG_DO_PRAGMA(GCC diagnostic x)
    #if WS_IS_AT_LEAST_GNUC_VERSION(4,6)
      /*
       * This is GCC 4.6 or later, or a compiler claiming to be that.
       * We can use "GCC diagnostic push/pop".
       *
       * The good news is that this can be used for diagnostics that
       * might be errors or might just be warnings.  The bad news is
       * that DIAG_ON() just undoes the results of the most recent
       * DIAG_OFF(), independent of its argument, so you have to be
       * careful in how you nest them.
       */
      #define DIAG_OFF(x) DIAG_PRAGMA(push) DIAG_PRAGMA(ignored DIAG_JOINSTR(-W,x))
      #define DIAG_ON(x) DIAG_PRAGMA(pop)

      #if WS_IS_AT_LEAST_GNUC_VERSION(4,8)
        /*
         * This is GCC 4.8, which supports -Wpedantic.
         */
        #define DIAG_OFF_PEDANTIC DIAG_OFF(pedantic)
        #define DIAG_ON_PEDANTIC DIAG_ON(pedantic)
      #else /* WS_IS_AT_LEAST_GNUC_VERSION(4,8) */
        /*
         * -Wpedantic isn't supported, so there's nothing to turn on or
         * off, and we can't turn it on or off in any case.
         */
        #define DIAG_OFF_PEDANTIC
        #define DIAG_ON_PEDANTIC
      #endif /* WS_IS_AT_LEAST_GNUC_VERSION(4,8) */
    #else /* WS_IS_AT_LEAST_GNUC_VERSION(4,6) */
      /*
       * This is GCC 4.2 through 4.5; we can't use push and pop to turn
       * diagnostics on or off, but we can, at least, use "error" to
       * turn a diagnostic back on.
       *
       * The good news is that DIAG_ON() really *does* turn the
       * diagnostic in question back on, so you don't have to worry
       * about nesting them correctly.  The bad news is that if you
       * use it on a diagnostic that's not supposed to be an error,
       * it'll become an error anyway.
       */
      #define DIAG_OFF(x) DIAG_PRAGMA(ignored DIAG_JOINSTR(-W,x))
      #define DIAG_ON(x) DIAG_PRAGMA(error DIAG_JOINSTR(-W,x))

      /*
       * -Wpedantic isn't supported, so there's nothing to turn on or
       * off, and we can't turn it on or off in any case.
       */
      #define DIAG_OFF_PEDANTIC
      #define DIAG_ON_PEDANTIC
    #endif /* WS_IS_AT_LEAST_GNUC_VERSION(4,6) */
  #else /* WS_IS_AT_LEAST_GNUC_VERSION(4,2) */
    /*
     * This is GCC prior to 4.2 -Wpedantic isn't supported, so there's
     * nothing to turn on or off, and we can't turn it on or off in any
     * case.
     */
    #define DIAG_OFF_PEDANTIC
    #define DIAG_ON_PEDANTIC
  #endif
#endif

#ifndef DIAG_OFF
  /*
   * This is none of the above; we don't have any way to turn diagnostics
   * on or off.
   *
   * XXX - you can do that in MSVC, but it's done differently; we'd
   * have to have macros for *particular* diagnostics, using the
   * warning flag for GCC and Clang and the error number for MSVC.
   */
  #define DIAG_OFF(x)
  #define DIAG_ON(x)
  #define DIAG_OFF_PEDANTIC
  #define DIAG_ON_PEDANTIC
#endif

/* Use for clang specific pragmas, so we can keep -Wpragmas enabled */
#ifdef __clang__
#  define DIAG_OFF_CLANG(x) DIAG_OFF(x)
#  define DIAG_ON_CLANG(x)  DIAG_ON(x)
#else
#  define DIAG_OFF_CLANG(x)
#  define DIAG_ON_CLANG(x)
#endif

/*
 * Suppress complaints about narrowing converstions and about signed vs.
 * unsigned comparison.
 *
 * XXX - this is done solely to squelch complaints from code generated
 * by Flex, but newer versions of Flex might fix the code; can we
 * check the version of Flex and suppress only the checks that code
 * generated by that version of Flex triggers?
 */
#if defined(_MSC_VER)
  /*
   * Suppress:
   *
   *   warning C4018: signed/unsigned mismatch
   *   warning C4244: 'initializing' : conversion from '__int64' to 'int', possible loss of data
   *   warning C4267: 'argument' : conversion from 'size_t' to 'int', possible loss of data
   *
   * as well as Visual Studio Code Analyzer warnings:
   *
   *   warning C6011: Dereferencing NULL pointer
   *   warning C6308: 'realloc' might return null pointer
   *   warning C6386: Buffer overrun
   *   warning C6387: 'XXX' could be '0'
   *   warning C28182: Dereferencing NULL pointer
   */
  #define DIAG_OFF_FLEX \
    __pragma(warning(push)) \
    __pragma(warning(disable:4018)) \
    __pragma(warning(disable:4244)) \
    __pragma(warning(disable:4267)) \
    __pragma(warning(disable:6011)) \
    __pragma(warning(disable:6308)) \
    __pragma(warning(disable:6386)) \
    __pragma(warning(disable:6387)) \
    __pragma(warning(disable:28182))
  #define DIAG_ON_FLEX	__pragma(warning(pop))

  /*
   * XXX - is there an issue with shadowed definitions with MSVC if
   * somebody were to happen to use Berkeley YACC rather than Bison?
   */
  #define DIAG_OFF_BYACC
  #define DIAG_ON_BYACC
#else
  /*
   * Suppress:
   *
   *   -Wsigned-compare warnings
   *   -Wshorten-64-to-32 warnings, if the compiler *has* -Wshorten-64-to-32
   *   -Wunreachable-code warnings
   *
   * We use DIAG_OFF() and DIAG_ON(), so we only use features that the
   * compiler supports.
   *
   * We disable -Wshorten-64-to-32 if we're using Clang, or if __APPLE__
   * is defined; that option was originally added to an Apple version of
   * GCC, and at least some versions of Clang support it - given that
   * the Clang work started at Apple, it may be in all versions of Clang.
   *
   * (Does no version of GCC or Clang support the same generic "you're
   * narrowing a value, and you didn't throw in a cast to assert that
   * you know what you're doing" warning that MSVC does?)
   */
  #if defined(__clang__) || defined(__APPLE__)
    #define DIAG_OFF_FLEX \
      DIAG_OFF(sign-compare) \
      DIAG_OFF(shorten-64-to-32) \
      DIAG_OFF(unreachable-code)
    #define DIAG_ON_FLEX \
      DIAG_ON(unreachable-code) \
      DIAG_ON(shorten-64-to-32) \
      DIAG_ON(sign-compare)
  #else
    #define DIAG_OFF_FLEX \
      DIAG_OFF(sign-compare)
    #define DIAG_ON_FLEX \
      DIAG_ON(sign-compare)
  #endif

  /*
   * Berkeley YACC and, apparently, some versions of Bison, such as the
   * one in Fedora 21, generate a global declaration of yylval, or the
   * appropriately prefixed version of yylval, in grammar.h, *even
   * though it's been told to generate a pure parser, meaning it
   * doesn't have any global variables*.  Other versions of Bison, such
   * as the one in macOS Sierra don't do that.
   *
   * That causes a warning due to the local declaration in the parser
   * shadowing the global declaration.
   *
   * So, if we have _Pragma, and have pragmas to suppress diagnostics,
   * we use it to turn off -Wshadow warnings.
   *
   * XXX - do this for Bison only in versions of Bison with this
   * problem?
   */
  #define DIAG_OFF_BYACC \
    DIAG_OFF(shadow)
  #define DIAG_ON_BYACC \
    DIAG_ON(shadow)
#endif

/*
 *	For dealing with APIs which are only deprecated in macOS (like the
 *	OpenSSL and MIT/Heimdal Kerberos APIs).
 *
 *	Dear Apple: this is a cross-platform program, and we're not
 *	going to use your Shiny New Frameworks on macOS unless there's
 *	a sufficiently clear benefit to make it worth our while to have
 *	both macOS and non-macOS versions of the code.
 */
#ifdef __APPLE__
#  define USES_APPLE_DEPRECATED_API DIAG_OFF(deprecated-declarations)
#  define USES_APPLE_RST DIAG_ON(deprecated-declarations)
#else
#  define USES_APPLE_DEPRECATED_API
#  define USES_APPLE_RST
#endif

#ifdef __cplusplus
}
#endif
#endif /* __WS_DIAG_CONTROL_H__ */
