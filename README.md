# Neptan, a chatting application
Neptan is a chatting app written in C++/ASIO.

## Building
1. Download the repository: `git clone https://github.com/ralphrbergman/neptan.git`
2. Install Conan for dependencies: `pip install conan` (You must have `python` and `python-pip` installed)
3. `cd neptan/`
4. `mkdir build`
5. Install Conan dependencies: `conan install . --output-folder=build --build=missing` (this must be run only once, if **build** directory gets deleted, you need to run this command again)
6. Build the project: `cmake -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release && cmake --build build/ -- -j$(nproc)`
