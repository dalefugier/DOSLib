set configuration=%1
set platform=%2
set buildconfig=%configuration%"|"%platform%
set build_option=%3
set msdev="C:\Program Files (x86)\Microsoft Visual Studio\2017\Professional\Common7\IDE\devenv.exe"
set slnfile=make_doslib.sln
set batchdir=.
set slnfullpath=%batchdir%\%slnfile%
set logfile=%batchdir%\LOG_DETAILS_%build_option%_%configuration%_%platform%.txt
set summaryfile=%batchdir%\LOG_%build_option%.txt
set log=/out %logfile%

cd %batchdir%
if exist %logfile% del /F %logfile%

echo %build_option% %buildconfig%
%msdev% %slnfullpath% /%build_option% "%configuration%|%platform%" /project DOSLib %log%
if ERRORLEVEL 1 goto build_summary

:build_summary

echo. >> %summaryfile%
echo %buildconfig% >> %summaryfile%
type %logfile% | find "succeeded," >> %summaryfile%
type %logfile% | find "error(s)" >> %summaryfile%
type %logfile% | find "errors" >> %summaryfile%
type %logfile% | find " error" >> %summaryfile%
type %logfile% | find " warning" >> %summaryfile%

if exist %logfile% del /F %logfile%
