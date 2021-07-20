#!/bin/bash

docker run --rm --privileged -v $PWD:/share coderus/sailfishos-platform-sdk:$RELEASE /bin/bash -c "
  set -x
  export RELEASE="$RELEASE"
  export SFOS_MIN=$SFOS_MIN
  export SFOS_MAX=$SFOS_MAX
  mkdir -p tree build
  cp -r /share/* tree
  cd build
  for ARCH in $ARCHS
  do
    mb2 -t SailfishOS-$RELEASE-\$ARCH build -d ../tree
    sudo cp RPMS/*.rpm /share/RPMS
  done
"
