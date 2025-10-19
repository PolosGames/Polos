#!/bin/bash

POLOS_DIR=$(dirname "$(dirname "$(readlink -f "$0")")")

cd $POLOS_DIR

cmake --preset $1
cmake --build --preset $1

cd $POLOS_DIR/out/bin/$1

./dummy
