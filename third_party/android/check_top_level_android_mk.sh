#!/bin/sh

SCRIPT_PATH=$(dirname $(readlink -e $0))

$SCRIPT_PATH/generate_android_mk.py --output Android.mk.autogen

if diff Android.mk.autogen $SCRIPT_PATH/../../Android.mk ; then
  exit 0
else
  echo \
    "ERROR: The top level Android.mk does not appear to have been" \
    "automatically generated. Changes to Android.mk should be done" \
    "through the scripts in $SCRIPT_PATH."
  exit 1
fi
