@echo off

set dir_curr=%cd%
echo #==================================================================
echo 当前目录：%dir_curr%
echo #==================================================================

##程序运行时，会输出QTDIR目录，拷贝过来即可
set qt_package_tool_dir=C:\Qt\Qt5.14.1\5.14.1\msvc2017\bin

set /p dir_clear=请输入Qt打包工具windeployqt.exe目录（比如%qt_package_tool_dir%）：

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