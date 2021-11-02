
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
cmake -DCMAKE_INSTALL_PREFIX:PATH=build-dir -DCMAKE_BUILD_TYPE=Release ../
make -j$(nproc)
```

# Mac OS X
**TBA**

# Windows
**TBA**