#!/usr/bin/env bash

OBJFILE=${1:?please specify object file}
BINFILE=${2:?please specify object file}

ld -o $BINFILE \
  /usr/lib/crt1.o \
  /usr/lib/crti.o \
  $OBJFILE \
  -lc \
  /usr/lib/crtn.o \
  -dynamic-linker /lib64/ld-linux-x86-64.so.2
