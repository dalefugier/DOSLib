# DOSLib: LISP Library for CAD Applications

<img width="128" height="128" src="https://github.com/dalefugier/DOSLib/raw/master/resources/doslib.png">

This repository contains the source code to DOSLib, a library of LISP-callable functions that provide functionality not available in CAD-base LISP interpreters, such as those included with AutoCAD, BricsCAD and ZWCAD.

## Prerequisites

The following tools are required to build DOSLib:

- [Microsoft Visual Studio 2022](https://visualstudio.microsoft.com/)
- [ObjectARX SDK for AutoCAD](https://www.autodesk.com/developer-network/platform-technologies/autocad/objectarx-license-download)
- [BRX SDK for BricsCAD](https://www.bricsys.com/en-eu/applications/developers/)
- [ZRX SDK for ZWCAD](https://www.zwsoft.com/support/zwcad-devdoc/) (Requires Visual Studio 2017*)

When installing Visual Studio 2022, make sure to install the Visual Studio 2019 (v142) and Visual Studio 2017 (v141) C++ x64/x86 build tools and libraries, including ATL and MFC. 

Note, to build DOSLib for version of AutoCAD, BricsCAD and/or ZWCAD earlier than what is listed above may also require an earlier version of Visual Studio. Check the documentation that comes with each of these SDK for their Visual Studio requirement.

The DOSLib build solution looks for ObjectARX, BRXSDK and ZRXSDK installations in this folder structure:

```
\acad
  \arx
    \23    - ObjectARX for AutoCAD 2019
    \24    - ObjectARX for AutoCAD 2021
    \25    - ObjectARX for AutoCAD 2025
  \brx
    \21    - BRXSDK for BricsCAD Pro V21
    \22    - BRXSDK for BricsCAD Pro V22
    \23    - BRXSDK for BricsCAD Pro V23
    \24    - BRXSDK for BricsCAD Pro V24
    \25    - BRXSDK for BricsCAD Pro V25
    \26    - BRXSDK for BricsCAD Pro V26
   \zrx
    \21    - ZRXSDK for ZWCAD 2021/2022/2023/2024
    \25    - ZRXSDK for ZWCAD 2025
```

### Compiling

1. Clone the repository. At a command prompt, enter the following command:
```
git clone https://github.com/dalefugier/doslib
```
2. Open the `DOSLib.sln` solution file, found in the `doslib\source` folder, in Visual Studio.
3. Select either the `Release_ARX25`, `Release_BRX26` or `Release_ZRX25` solution configuration.
4. Press <kbd>F7</kbd>, or click *Build > Build Solution*  to build the solution.

Note, you can target DOSLib for other versions of AutoCAD, BricsCAD or ZWCAD as long as:

1. The correct ObjectARX, BRXSDK or ZRXSDK is installed in above mentioned location.
2. The correct Visual Studio platform toolset is installed.

### More Information

[DOSLib home page](https://wiki.mcneel.com/doslib/home)

### License
Code licensed under the [MIT License](https://github.com/dalefugier/DOSLib/blob/master/LICENSE).
