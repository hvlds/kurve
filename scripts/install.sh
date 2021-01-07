#!/bin/sh

DIRECTORY="./Release"

if [ -d "$DIRECTORY" ]; then
  sudo rm -rf Release
fi

mkdir $DIRECTORY
cd $DIRECTORY
cmake .. -DCMAKE_BUILD_TYPE=Release
sudo make install

cd ..
sudo rm -rf Release