#pragma once

/*
#if (_MSC_VER == 1600)        // Visual Studio 2010
#define ARX19
#define BRX13
#define BRX14
#define BRX15
#elif (_MSC_VER == 1700)      // Visual Studio 2012
#define ARX20
#elif (_MSC_VER == 1800)      // Visual Studio 2013
#define BRX16
#define BRX17
#define BRX18
#elif (_MSC_VER == 1900)      // Visual Studio 2015
#define ARX21
#define ARX22
#endif
*/

#if defined(ARX19)
#define _ARX_VER 19
#elif defined(ARX20)
#define _ARX_VER 20
#elif defined(ARX21)
#define _ARX_VER 21
#elif defined(ARX22)
#define _ARX_VER 22
#elif defined(ARX23)
#define _ARX_VER 23
#else
#define _ARX_VER 0
#endif

#if defined(BRX13)
#define _BRX_VER 13
#elif defined(BRX14)
#define _BRX_VER 14
#elif defined(BRX15)
#define _BRX_VER 15
#elif defined(BRX16)
#define _BRX_VER 16
#elif defined(BRX17)
#define _BRX_VER 17
#elif defined(BRX18)
#define _BRX_VER 18
#elif defined(BRX19)
#define _BRX_VER 19
#else
#define _BRX_VER 0
#endif
