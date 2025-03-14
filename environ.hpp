/*! \file    environ.hpp
 *  \brief   Defines the compilation and the target environments.
 *  \author  Peter Chapin <spicacality@kelseymountain.org>
 *
 * This file contains settings that define the environment in which the program was compiled and
 * the environment where it runs. This file should be included into source files that need to
 * distinguish one environment from another. Conditional compilation directives can then be used
 * to select appropriate code for each environment.
 *
 * I find it easier to use the symbols defined in this file than it is to use the symbols
 * defined by the various individual compilers. The symbols here are more consistent and more
 * natural. However, it is my intention for this file to use the compiler specific symbols to
 * automatically set appropriate values for the symbols defined here whenever possible.
 *
 * Each symbol defined here is prefixed with 'e' (for "environment"). This is done because
 * several of the symbol names are fairly generic and they tend to collide with similarly named
 * symbols in other libraries.
*/

#ifndef ENVIRON_HPP
#define ENVIRON_HPP

// Make sure we are using C++
#if !defined(__cplusplus)
#error C++ compiler required!
#endif

//-----------------------------
//           Compiler
//-----------------------------

// The following are the allowed values of eCOMPILER.
#define eVANILLA     1  // Generic, Standard C++ 2020 only
#define eCLANG       2  // clang++
#define eCOMPAQ      3  // Compaq C++ (remove?)
#define eGCC         4  // g++
#define eIBM         5  // IBM's Visual Age C++ (remove?)
#define eMETROWERKS  6  // Metrowerks CodeWarrior (remove?)
#define eMICROSOFT   7  // Microsoft Visual C++
#define eOPENWATCOM  8  // Open Watcom C++
// TODO: What about Intel's compiler?

// Choose your compiler! This file can autodetect all of the compilers mentioned above. If the
// compiler can't be automatically detected it will default to eVANILLA.

#if defined(__clang__)
#define eCOMPILER eCLANG
#endif

#if defined(__DECCXX)
#define eCOMPILER eCOMPAQ
#endif

// Clang also defines __GNUC__. Make sure we don't get confused.
#if defined(__GNUC__) && !defined(__clang__)
#define eCOMPILER eGCC
#endif

#if defined(__IBMCPP__)
#define eCOMPILER eIBM
#endif

#if defined(__MWERKS__)
#define eCOMPILER eMETROWERKS
#endif

#if defined(_MSC_VER)
#define eCOMPILER eMICROSOFT
#endif

#if defined(__WATCOMC__)
#define eCOMPILER eOPENWATCOM
#endif

#if !defined(eCOMPILER)
#define eCOMPILER eVANILLA
#endif

// TODO: It might make sense to encode the compiler version also.

//-------------------------------------
//           Operating System
//-------------------------------------

// The following are the allowed values of eOPSYS.
#define eDOS     1  // DOS and its variations.
#define eMAC     2  // macOS (modern system).
#define eNETWARE 3  // NetWare NLM. Assume v4.x or higher (NDS support).
#define eOS2     4  // OS/2 (32 bit only).
#define ePOSIX   5  // POSIX is intended to support all Unix flavors including macOS.
#define eVMS     6  // DEC's VMS operating system.
#define eWINDOWS 7  // Windows NT+ only. Win95/98/Me are obsolete.
#define eWIN32   7  // Same as "eWINDOWS". This macro is for compatibility. It is deprecated.

// Choose your operating system! In most cases this file can autodetect the operating system
// from the compiler that is being used. If that is not the case, you will have to specify the
// operating system if it matters. There is no default.

// Assume that clang++ is on a Unix-like system.
#if eCOMPILER == eCLANG
#define eOPSYS ePOSIX
#endif

// Assume that Compaq C++ is on a Unix-like system. Is this assumption safe? (What of VMS?)
// TODO: Is Compaq C++ still supported? I doubt it.
#if eCOMPILER == eCOMPAQ
#define eOPSYS ePOSIX
#endif

// Assume g++ is on a Unix-like system.
#if eCOMPILER == eGCC
#define eOPSYS ePOSIX
#endif

// Visual Age C++ supports OS/2 and Win32 programming.
// TODO: Is Visual Age C++ still supported? IBM appears to be using "IBM OpenXL C/C++."
#if eCOMPILER == eIBM
#if defined(__TOS_OS2__)
#define eOPSYS eOS2
#elif defined(__TOS_WIN__)
#define eOPSYS eWINDOWS
#endif
#endif

// CodeWarrior supports macOS and Win32 programming.
// TODO: CodeWarrior no longer supports either of the above platforms. What now?
#if eCOMPILER == eMETROWERKS
#if defined(macintosh)
#define eOPSYS eMAC
#elif defined(__INTEL__)
#define eOPSYS eWINDOWS
#endif
#endif

// Visual C++ supports Windows programming.
// TODO: What about 32 vs 64-bit?
#if eCOMPILER == eMICROSOFT
#if defined(_WIN32)
#define eOPSYS eWINDOWS
#endif
#endif

// Open Watcom supports a variety of different systems.
#if eCOMPILER == eOPENWATCOM
#if defined (__DOS__)
#define eOPSYS eDOS
#elif defined(__OS2__)
#define eOPSYS eOS2
#elif defined(__NT__)
#define eOPSYS eWINDOWS
#elif defined(__NETWARE__)
#define eOPSYS eNETWARE
#elif defined(__LINUX__)
#define eOPSYS ePOSIX
#endif
#endif

//---------------------------------------------
//           Graphical User Interface
//---------------------------------------------

// The following are the allowed values of eGUI.
#define eNONE   1   // Text mode application.
#define ePM     2   // The OS/2 graphical interface. This also implies WPS.
#define eWIN    3   // Windows NT+ only. (Consider renaming; too similar to eWINDOWS)
#define eXWIN   4   // X Windows.

// Choose your GUI. This file does not currently autodetect any GUI. The default GUI is eNONE.

#if !defined(eGUI)
#define eGUI eNONE
#endif

// Do a few checks to make sure the GUI selection makes sense.

#if eGUI == eWIN && eOPSYS != eWINDOWS
#error Can not specify the Windows GUI without the Windows operating system!
#endif

#if eGUI == ePM && eOPSYS != eOS2
#error Can not specify the PM GUI without the OS/2 operating system!
#endif

//-----------------------------------
//           Multithreaded
//-----------------------------------

// When writing a multithreaded program, there are additional issues that must be considered.
// The symbol eMULTITHREADED will be defined in all such cases. This file can auto-detect this
// feature in some but not all cases.
//
// TODO: Eliminate the need for eMULTITHREADED? Isn't it just the default now?
//
// Note that if eMULTITHREADED is defined when eOPSYS is ePOSIX, Posix threads are implied. If
// eOPSYS is eWIN32, Windows threads are implied. If eOPSYS is eOS2, OS/2 threads are implied.
//

#if eCOMPILER == eClang
#define eMULTITHREADED
#endif

#if eCOMPILER == eCOMPAQ && defined(_REENTRANT)
#define eMULTITHREADED
#endif

#if eCOMPILER == eIBM && defined(_MT)
#define eMULTITHREADED
#endif

#if eCOMPILER == eMICROSOFT && defined(_MT)
#define eMULTITHREADED
#endif

#if eCOMPILER == eOPENWATCOM && defined(_MT)
#define eMULTITHREADED
#endif

#endif
