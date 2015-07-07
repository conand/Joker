# Joker
Windows Minifilter driver that redirects any IRP of mp3 files to a target file.

## Building Using Visual Studio

* Open the driver project or solution in Visual Studio (`Joker.sln` or `Joker.vcxproj`).
* Right-click the solution in the Solutions Explorer and select Configuration Manager.
* From the Configuration Manager, select the Active Solution Configuration (for example, Windows 8.1 Debug or Windows 8.1 Release) and the Active Solution Platform (for example, Win32) that correspond to the type of build you are interested in.
* From the Build menu, click Build Solution (Ctrl+Shift+B).

## Building Using the Command Line (MSBuild)

* Open a Visual Studio Command Prompt window at the Start screen. From this window you can use MsBuild.exe to build any Visual Studio project by specifying the project (.VcxProj) or solutions (.Sln) file.
* Navigate to the project directory and enter the MSbuild command for your target. For example, to perform a clean build, navigate to the project directory and enter the following MSBuild command: `msbuild /t:clean /t:build .\Joker.vcxproj`.

## Run

### Installation

The Joker driver comes with an INF file that will install the minifilter. To install the minifilter, do the following:
* Make sure that `Joker.sys` and `Joker.inf` are in the same directory.
* In Windows Explorer, right-click filtername.inf, and click Install.
* To load the minifilter, run `fltmc load Joker` or `net start filtername`.
