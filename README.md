
# Simple Process Tracker
A simple, lightweight Qt-based app that tracks how long user-selected processes have been running on the system.

# Build instructions:

# Linux:
1. Install the following dependencies for your distribution: `qt6-base qt6-svg cmake`
2. Execute the following:
```bash
git clone https://github.com/kik4444/simple-process-tracker
cd simple-process-tracker
mkdir build-dir && cd build-dir
cmake -S .. -B . -DCMAKE_BUILD_TYPE:STRING=Release
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
cd simple-process-tracker
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
cd simple-process-tracker-master
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
## Adding new process entry
![1](https://user-images.githubusercontent.com/7779637/140617125-652b62c4-c48d-4b44-a0f5-cd24a591c844.PNG)
![1](https://user-images.githubusercontent.com/7779637/140617162-afa14cdf-bf6b-40d5-9e2b-295c2ae49873.png)
![1](https://user-images.githubusercontent.com/7779637/140617163-525c74be-9ad2-43e5-b1cf-e13bb0a00287.png)

## Choosing which process to track
![2](https://user-images.githubusercontent.com/7779637/140617343-652c7825-0c06-4dbd-ba97-aa23e48ef28c.PNG)
![2](https://user-images.githubusercontent.com/7779637/140617344-8e179e22-372f-42c5-8db1-5cd36e703025.png)
![2](https://user-images.githubusercontent.com/7779637/140617346-6bcd4b59-2bbd-4d74-854f-0b52f7dcb9da.png)

## Tracking several processes
![3](https://user-images.githubusercontent.com/7779637/140617373-365c09c0-a3e3-4874-ba2c-f9727b9a0d5e.PNG)
![3](https://user-images.githubusercontent.com/7779637/140617374-66a8d18b-2293-4e70-af37-51538adeac7d.png)
![3](https://user-images.githubusercontent.com/7779637/140617377-038f6a80-59b1-48f7-a6fa-47f6df13b74e.png)

## Hiding a process
![4](https://user-images.githubusercontent.com/7779637/140617383-b2024a53-6da5-4b9d-b286-b63bf16016db.PNG)
![4](https://user-images.githubusercontent.com/7779637/140617384-fed8d57b-4d08-4e2a-a2f5-780f832c8a75.png)
![4](https://user-images.githubusercontent.com/7779637/140617386-a419081c-7456-46f9-a41e-eaadef488735.png)

## Options
![5](https://user-images.githubusercontent.com/7779637/140617398-65620483-59a1-4721-abb9-d110b65445c2.PNG)
![5](https://user-images.githubusercontent.com/7779637/140617400-c8246161-e11f-46fd-965a-38521777dc7e.png)
![5](https://user-images.githubusercontent.com/7779637/140617401-83781aeb-aa42-4af5-a265-8cdd636211c9.png)