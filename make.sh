#!/bin/sh

cd frontend/code/
make -f Makefile

cd ../../backend/code/
make -f Makefile

cd ../../processor/ASM/code/
make -f Makefile1

cd ../../../processor/CPU/code/
make -f Makefile2