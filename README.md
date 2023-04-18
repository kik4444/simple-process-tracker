
# Simple Process Tracker
A simple, lightweight Qt-based app that tracks how long user-selected processes have been running on the system.

# Build instructions:

# Linux:
1. Install the following dependencies for your distribution: `qt6-base qt6-svg cmake`
2. Execute the following:
```bash
git clone https://github.com/kik4444/simple-process-tracker
cd simple-process-tracker/src
mkdir build-dir && cd build-dir
cmake -S .. -B . -DCMAKE_BUILD_TYPE:STRING=Release
make -j$(nproc)
```

## Alternative Linux Qt5
```bash
git clone https://github.com/kik4444/simple-process-tracker
cd simple-process-tracker/src
mkdir build-dir && cd build-dir
qmake -o Makefile ../simple-process-tracker.pro -spec linux-g++
make -j$(nproc)
```

# Mac OS X
1. Download the open source version of Qt from https://www.qt.io/download-qt-installer.
2. Register a Qt account to log into the installer.
3. Start the installer and accept the request to install the commandline developer tools. Open the subdirectory `Qt/Qt 6.2.3`. From there choose the component:
- macOS
4. Now go to the directory `Develop and Designer Tools` and select:
- CMake
- Ninja
5. Continue with the installation and finish it.
6. Execute the following from a terminal:
```bash
git clone https://github.com/kik4444/simple-process-tracker
cd simple-process-tracker/src
mkdir build-dir && cd build-dir
export PATH=/Users/$USER/Qt/Tools/Ninja:$PATH
/Users/$USER/Qt/Tools/CMake/CMake.app/Contents/bin/cmake -S .. -B . -GNinja -DCMAKE_BUILD_TYPE:STRING=Release "-DCMAKE_PROJECT_INCLUDE_BEFORE:PATH=/Users/$USER/Qt/Qt Creator.app/Contents/Resources/package-manager/auto-setup.cmake" -DQT_QMAKE_EXECUTABLE:STRING=/Users/$USER/Qt/6.2.3/macos/bin/qmake -DCMAKE_PREFIX_PATH:STRING=/Users/$USER/Qt/6.2.3/macos -DCMAKE_C_COMPILER:STRING=/usr/bin/clang -DCMAKE_CXX_COMPILER:STRING=/usr/bin/clang++
/Users/$USER/Qt/Tools/CMake/CMake.app/Contents/bin/cmake --build . --target all -- -j$(sysctl -n hw.ncpu)
/Users/$USER/Qt/6.2.3/macos/bin/macdeployqt simple-process-tracker.app -dmg
```

You will now have a dmg file containing the application that you can drag and drop into the applications folder in Finder.

# Windows
1. Download the open source version of Qt from https://www.qt.io/download-qt-installer.
2. Register a Qt account to log into the installer.
3. Start the installer, when you need to select which components to install, open the subdirectory `Qt/Qt 6.2.3`. From there choose the components:
- MinGW 64-bit
4. Now go to the directory `Develop and Designer Tools` and select:
- CMake 64-bit
- Ninja
5. Continue with the installation and finish it. You should now have Qt and Qt Creator installed on your computer and the main Qt SDK folder should be located in `C:\Qt`.
6. Open powershell and execute the following script which will download and compile the program in a folder on your desktop:
```bash
cd C:\Users\$env:UserName\Desktop
Start-BitsTransfer -Source https://github.com/kik4444/simple-process-tracker/archive/refs/heads/master.zip -Destination .
Expand-Archive -Path .\master.zip -DestinationPath .
cd simple-process-tracker-master\src
mkdir build-dir
cd build-dir
$env:Path = "C:\Qt\Tools\mingw900_64\bin;C:\Windows\system32;C:\Windows;C:\Windows\System32\Wbem;C:\Windows\System32\WindowsPowerShell\v1.0\;C:\Windows\System32\OpenSSH\;C:\Qt\Tools\Ninja;"
C:\Qt\Tools\CMake_64\bin\cmake.exe -S .. -B . -GNinja "-DCMAKE_BUILD_TYPE:STRING=Release" "-DCMAKE_PROJECT_INCLUDE_BEFORE:PATH=C:/Qt/Tools/QtCreator/share/qtcreator/package-manager/auto-setup.cmake" "-DQT_QMAKE_EXECUTABLE:STRING=C:/Qt/6.2.3/mingw_64/bin/qmake.exe" "-DCMAKE_PREFIX_PATH:STRING=C:/Qt/6.2.3/mingw_64" "-DCMAKE_C_COMPILER:STRING=C:/Qt/Tools/mingw900_64/bin/gcc.exe" "-DCMAKE_CXX_COMPILER:STRING=C:/Qt/Tools/mingw900_64/bin/g++.exe"
C:\Qt\Tools\CMake_64\bin\cmake.exe --build . --target all -- -j $((Get-CimInstance Win32_ComputerSystem).NumberOfLogicalProcessors)
mv .\simple-process-tracker.exe ..
rm -Recurse *
mv ..\simple-process-tracker.exe .
C:\Qt\6.2.3\mingw_64\bin\windeployqt.exe --no-translations .\simple-process-tracker.exe
```

You will now have a working portable program in the directory `C:\Users\YOUR_USER_NAME\Desktop\simple-process-tracker-master\build-dir`.

# Screenshots for Windows / Mac OS X / Linux
## Main window tracking 3 processes
![Windows-1](https://user-images.githubusercontent.com/7779637/154840366-d9162191-7fec-4602-9b91-9a7bf7c655f3.png)
![MacOS-1](https://user-images.githubusercontent.com/7779637/154840547-9c1db673-4f05-4432-9261-f47081982aa3.png)
![Linux-1](https://user-images.githubusercontent.com/7779637/154840364-a5d16ae5-435d-4862-8bf2-ea258adab2d0.png)

## Adding a new process to track
![Windows-2](https://user-images.githubusercontent.com/7779637/154819189-f41babfe-c96a-4138-b38d-0182a9ae3dc5.png)
![MacOS-2](https://user-images.githubusercontent.com/7779637/154819230-c1855d3e-6dde-447e-9dbb-c0b02e407f4c.png)
![Linux-2](https://user-images.githubusercontent.com/7779637/154819240-e4aac09d-377c-442e-b0fa-dfb34fe9de09.png)

## Options menu
![Windows-3](https://user-images.githubusercontent.com/7779637/154819196-e541133e-9367-4beb-9a44-7073224a7aec.png)
![MacOS-3](https://user-images.githubusercontent.com/7779637/154819232-b59a940a-a836-41e0-a847-99868e2dbb5a.png)
![Linux-3](https://user-images.githubusercontent.com/7779637/154819241-a95c4a08-df8b-40f3-b950-362702af2619.png)
