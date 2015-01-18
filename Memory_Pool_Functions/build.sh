#!/bin/sh

if [ ! -d ./Debug ]; then
  mkdir Debug
  cd Debug
  cmake ..
  make
fi

