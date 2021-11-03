
# Simple Process Tracker
A simple, lightweight Qt-based app that tracks how long user-selected processes have been running on the system.

# Build instructions:

# Linux:
1. Install the dependencies `qt6-base` and `cmake` for your distribution
2. Execute the following:
```bash
git clone https://github.com/kik4444/simple-process-tracker
cd simple-process-tracker
mkdir build-dir && cd build-dir
cmake -DCMAKE_BUILD_TYPE=Release ../
make -j$(nproc)
```

# Mac OS X
**TBA**

# Windows
1. Download the open source version of Qt from https://www.qt.io/download-qt-installer.
2. Register a Qt account to log into the installer.
3. Start the installer, when you need to select which components to install, open the subdirectory `Qt/Qt 6.2.1`. From there choose the components:
- MinGW 8.1.0 64-bit
4. Now go to the directory `Develop and Designer Tools` and select:
- CMake 64-bit
5. Continue with the installation and finish it. You should now have Qt and Qt Creator installed on your computer and the main Qt SDK folder should be located in `C:\Qt`.
6. Open powershell and execute the following script which will download and compile the program in a folder on your desktop:
```bash
cd C:\Users\$env:UserName\Desktop
Start-BitsTransfer -Source https://github.com/kik4444/simple-process-tracker/archive/refs/heads/master.zip -Destination .
Expand-Archive -Path .\master.zip -DestinationPath .
cd simple-process-tracker-master
mkdir build-dir
cd build-dir
$env:Path = "C:\Qt\Tools\mingw810_64\bin;C:\Windows\system32;C:\Windows;C:\Windows\System32\Wbem;C:\Windows\System32\WindowsPowerShell\v1.0\;C:\Windows\System32\OpenSSH\;C:\Qt\Tools\Ninja;"
C:\Qt\Tools\CMake_64\bin\cmake.exe -S ..\ -B . -GNinja "-DCMAKE_BUILD_TYPE:STRING=Release" "-DCMAKE_PROJECT_INCLUDE_BEFORE:PATH=C:/Qt/Tools/QtCreator/share/qtcreator/package-manager/auto-setup.cmake" "-DQT_QMAKE_EXECUTABLE:STRING=C:/Qt/6.2.1/mingw81_64/bin/qmake.exe" "-DCMAKE_PREFIX_PATH:STRING=C:/Qt/6.2.1/mingw81_64" "-DCMAKE_C_COMPILER:STRING=C:/Qt/Tools/mingw810_64/bin/gcc.exe" "-DCMAKE_CXX_COMPILER:STRING=C:/Qt/Tools/mingw810_64/bin/g++.exe"
C:\Qt\Tools\CMake_64\bin\cmake.exe --build . --target all
mv .\simple-process-tracker.exe ..
rm -Recurse *
mv ..\simple-process-tracker.exe .
C:\Qt\6.2.1\mingw81_64\bin\windeployqt.exe --no-translations .\simple-process-tracker.exe
```

You will now have a working portable program in the directory `C:\Users\YOUR_USER_NAME\Desktop\simple-process-tracker-master\build-dir`.