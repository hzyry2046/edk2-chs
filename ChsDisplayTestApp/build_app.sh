#!/bin/bash
# Build script for Chinese Display Test Application

echo "Building Chinese Display Test Application..."
cd /home/hzyry2046/edk2/edk2
. edksetup.sh
build -p OvmfPkg/OvmfPkgX64.dsc -m ChsDisplayTestApp/ChsDisplayTestApp.inf -a X64 -b DEBUG

if [ $? -eq 0 ]; then
    echo "Build successful!"
    echo "Generated EFI file location:"
    find Build -name "ChsDisplayTestApp.efi" -type f
else
    echo "Build failed!"
    exit 1
fi