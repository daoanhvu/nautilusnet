mkdir build
cd build
cmake -DCUDA_TOOLKIT_ROOT_DIR="C:\\Program Files\\NVIDIA GPU Computing Toolkit\\CUDA\\v11.6" -DCMAKE_PREFIX_PATH=C:\\programs\\libtorch ..
cmake --build . --config Release