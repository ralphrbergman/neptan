# Neptan, a chatting application
Neptan is a chatting app written in C++/ASIO.

## Building
1. Download the repository: `git clone https://github.com/ralphrbergman/neptan.git`
2. `cd neptan/`
3. `mkdir build`
4. Build the project: `cmake -DCMAKE_BUILD_TYPE=Relese && cmake --build build/ -- -j$(nproc)`
