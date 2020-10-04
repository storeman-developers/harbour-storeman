#!/bin/bash

docker run --rm --privileged -v $PWD:/share coderus/sailfishos-platform-sdk:$RELEASE /bin/bash -c "
  mkdir -p build;
  cd build;
  cp -r /share/* .;
  mb2 -t SailfishOS-$RELEASE-$ARCH build -d;
  sudo cp -r RPMS/*.rpm /share/RPMS"

