#!/bin/bash
set -e
cd "$(dirname "$0")/.."
ROOT_DIR=$(pwd)

echo "initializing benchware macos environment"

# download libtorch if missing
if [ ! -d "$ROOT_DIR/libtorch" ]; then
    echo "downloading prebuilt libtorch for macos arm64"
    curl -L https://download.pytorch.org/libtorch/cpu/libtorch-macos-arm64-2.5.1.zip -o libtorch.zip
    unzip -q libtorch.zip
    rm libtorch.zip
    echo "libtorch ready"
else
    echo "libtorch already present"
fi

rm -rf build
cmake -B build -DCMAKE_BUILD_TYPE=Release -DTORCH_INSTALL_PREFIX="$ROOT_DIR/libtorch"

echo "-------------------------------------------------------"
echo "success benchware is ready to build with prebuilt libtorch"
echo "-------------------------------------------------------"
echo "run cmake --build build -j$(sysctl -n hw.ncpu)"
echo "-------------------------------------------------------"