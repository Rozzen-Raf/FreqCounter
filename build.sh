git submodule init
git submodule update
cd external/Catch2
cmake -Bbuild -H. -DBUILD_TESTING=OFF
sudo cmake --build build/ --target install
cd ../..
mkdir build
cmake -DCMAKE_BUILD_TYPE=Release -Bbuild -H. -DTESTING=ON
cd build
cmake --build .
