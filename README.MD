# Installation
## MacOS
```
git clone https://github.com/opencv/opencv.git
git clone https://github.com/opencv/opencv_contrib.git
```

CMake
```
mkdir build_opencv
    cd build_opencv
    cmake -DCMAKE_BUILD_TYPE=Release \
          -DOPENCV_EXTRA_MODULES_PATH=../opencv_contrib/modules \
          -DBUILD_EXAMPLES=OFF \
          -DBUILD_opencv_python3=ON \
          -DBUILD_opencv_python2=OFF \
          -DINSTALL_PYTHON_EXAMPLES=OFF \
          -DINSTALL_C_EXAMPLES=OFF \
          -DPYTHON3_EXECUTABLE=$(which python3) \
          -DPYTHON3_INCLUDE_DIR=$(python3 -c "from distutils.sysconfig import get_python_inc; print(get_python_inc())") \
          -DPYTHON3_PACKAGES_PATH=$(python3 -c "from distutils.sysconfig import get_python_lib; print(get_python_lib())") \
          ../opencv
```

```
make -j$(sysctl -n hw.ncpu) # Use all available CPU cores for faster compilation
sudo make install
```