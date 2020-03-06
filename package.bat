@echo off

set dir_curr=%cd%
echo #==================================================================
echo 当前目录：%dir_curr%
echo #==================================================================

cd bin

set /p dir_clear=请输入Qt打包工具windeployqt.exe目录（比如C:\Qt\Qt5.7.1\5.7\msvc2013\bin）：

if not exist "%dir_clear%" (
    echo 输入的目录不存在：%dir_clear%
    goto bat_ended
)
:end_input

:package_begin

echo 开始执行打包。。。
echo #==================================================================
%dir_clear%\windeployqt.exe FileSearch.exe
echo #==================================================================
echo 打包结束。
ls -ltr
:package_ended

:bat_ended
echo #==================================================================
pause