#!/bin/sh

DIRECTORY="./Debug"

if [ -d "$DIRECTORY" ]; then
  rm -rf Debug
fi

mkdir $DIRECTORY
cd $DIRECTORY
cmake .. -DCMAKE_BUILD_TYPE=Debug
make
./kurve