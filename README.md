# DOSLib
***LISP Library for CAD Applications***

<img width="160" height="160" src="https://github.com/dalefugier/DOSLib/raw/master/resources/doslib.png">

This repository contains the source code to DOSLib, a library of LISP-callable functions that provide functionality not available in LISP interpreters included with AutoCAD and BricsCAD.

## Prerequisites

The following tools are required to build DOSLib:

- [Microsoft Visual Studio 2017](https://visualstudio.microsoft.com/). Visual Studio 2017 comes in three editions: Community (free), Professional, and Enterprise. All of these editions will work.
- [ObjectARX for AutoCAD 2019](https://www.autodesk.com/developer-network/platform-technologies/autocad/objectarx-license-download).
- [BRX (BricsCAD Runtime eXtension) SDK for BricsCAD V19](https://www.bricsys.com/en-eu/applications/developers/).

Note, to build DOSLib for version of AutoCAD and/or BricsCAD earlier than what is listed above, you may need an earlier version of Visual Studio. Check the documentation that comes with each of these SDK for their Visual Studio requirement.

The DOSLib build solution looks for ObjectARX and BRX SDK install in this folder structure:

```
\ acad
  \ arx
    - 19 // ObjectARX 19
    - 20 // ObjectARX 20
    - 21 // ObjectARX 21
    - 22 // ObjectARX 22
    - 23 // ObjectARX 23
  \ brx
    - 13 // BRXSDK 13
    - 14 // BRXSDK 14
    - 15 // BRXSDK 15
    - 16 // BRXSDK 16
    - 17 // BRXSDK 17
    - 18 // BRXSDK 18
    - 19 // BRXSDK 19
```

###Compiling

- Clone the repository. At a command prompt, enter the following command:

```
git clone https://github.com/dalefugier/doslib
```

- Open the `DOSLib.sln` solution, found in the `doslib\source` folder, in Visual Studio.
- Select either the `Release_ARX23` or `Release_BRX19` solution configuration.
- Press <kbd>F7</kbd>, or click *Build > Build Solution*  to build the solution.

Note, you can target DOSLib for other versions of AutoCAD as long as:

1. The correct ObjectARX or BRXSDK installed.
2. The correct Visual Studio platform toolset is installed.

