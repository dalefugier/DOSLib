# DOSLib: LISP Library for CAD Applications

<img width="128" height="128" src="https://github.com/dalefugier/DOSLib/raw/master/resources/doslib.png">

This repository contains the source code to DOSLib, a library of LISP-callable functions that provide functionality not available in CAD-base LISP interpreters, such as those included with AutoCAD and BricsCAD.

## Prerequisites

The following tools are required to build DOSLib:

- [Microsoft Visual Studio 2019](https://visualstudio.microsoft.com/). Visual Studio 2019 comes in three editions: Community (free), Professional, and Enterprise. All of these editions will work.
- [ObjectARX SDK for AutoCAD 2021](https://www.autodesk.com/developer-network/platform-technologies/autocad/objectarx-license-download).
- [BRX SDK for BricsCAD V20](https://www.bricsys.com/en-eu/applications/developers/).
- [ZRX SDK for ZWCAD 2021](https://www.zwsoft.com/zwcad/developer/).

Note, to build DOSLib for version of AutoCAD, BricsCAD and/or ZWCAD earlier than what is listed above may also require an earlier version of Visual Studio. Check the documentation that comes with each of these SDK for their Visual Studio requirement.

The DOSLib build solution looks for ObjectARX, BRXSDK and ZRXSDK installations in this folder structure:

```
\acad
  \arx
    \19    - ObjectARX for AutoCAD 2013
    \20    - ObjectARX for AutoCAD 2015 
    \21    - ObjectARX for AutoCAD 2017 
    \22    - ObjectARX for AutoCAD 2018
    \23    - ObjectARX for AutoCAD 2019
    \24    - ObjectARX for AutoCAD 2021
  \brx
    \13    - BRXSDK for BricsCAD Pro V13 
    \14    - BRXSDK for BricsCAD Pro V14
    \15    - BRXSDK for BricsCAD Pro V15 
    \16    - BRXSDK for BricsCAD Pro V16 
    \17    - BRXSDK for BricsCAD Pro V17 
    \18    - BRXSDK for BricsCAD Pro V18 
    \19    - BRXSDK for BricsCAD Pro V19 
    \20    - BRXSDK for BricsCAD Pro V20
\zwcad
  \zrx
    \20    - ZRXSDK for ZWCAD 2018/2019/2020
    \21    - ZRXSDK for ZWCAD 2021
```

### Compiling

1. Clone the repository. At a command prompt, enter the following command:
```
git clone https://github.com/dalefugier/doslib
```
2. Open the `DOSLib.sln` solution file, found in the `doslib\source` folder, in Visual Studio.
3. Select either the `Release_ARX24`, `Release_BRX20` or `Release_ZRX21` solution configuration.
4. Press <kbd>F7</kbd>, or click *Build > Build Solution*  to build the solution.

Note, you can target DOSLib for other versions of AutoCAD, BricsCAD or ZWCAD as long as:

1. The correct ObjectARX, BRXSDK or ZRXSDK is installed in above mentioned location.
2. The correct Visual Studio platform toolset is installed.

### More Information

[DOSLib home page](https://wiki.mcneel.com/doslib/home)

### License
Code licensed under the [MIT License](https://github.com/dalefugier/DOSLib/blob/master/LICENSE).
