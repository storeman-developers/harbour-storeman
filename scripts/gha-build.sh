#!/bin/bash

docker run --rm --privileged -v $PWD:/share coderus/sailfishos-platform-sdk:$RELEASE /bin/bash -c "
  set -x
  export LATEST_RELEASE=$LATEST_RELEASE
  mkdir -p tree
  cd tree
  cp -r /share/* .
  cd ..
  mkdir -p build
  cd build
  mb2 -t SailfishOS-$RELEASE-$ARCH build -d ../tree
  sudo cp -r RPMS/*.rpm /share/RPMS"
