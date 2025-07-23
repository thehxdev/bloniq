#!/usr/bin/env bash

set -xe

SELF_PATH=$(dirname "$(realpath $0)")
LIBBLOOM_PATH="$SELF_PATH/libbloom"
EXE_NAME=bloniq

if [[ -z "$CC" ]]; then
    CC=cc
fi

CFLAGS=(-std=c99 -O2 -DNDEBUG -Wall -Wextra -s)

make -j$(nproc) -C "$LIBBLOOM_PATH"

$CC "${CFLAGS[@]}" \
    -I"$LIBBLOOM_PATH" -L"$LIBBLOOM_PATH/build" \
    -o "$SELF_PATH/$EXE_NAME" \
    "$SELF_PATH/main.c" \
    -l:libbloom.a -lm
