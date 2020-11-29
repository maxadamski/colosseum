#!/bin/bash
rm -f judge
make
pushd ../player
rm -f player
make
popd
rm -f f1 f2 f3 f4
mkfifo f1 f2 f3 f4
./judge f1 f2 f3 f4 6 30 &
../player/player f1 f2 &
../player/player f3 f4 &
