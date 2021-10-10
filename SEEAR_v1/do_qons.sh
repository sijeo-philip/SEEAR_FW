#!/bin/sh
export APPS_OUTFILE=${IMAGEDIR}/${PROJECT_NAME}.out
if [ ! -d "qons" ]; then
  mkdir qons
fi
cp tunable_input.txt qons
../../tool/qonstruct.sh --qons ./qons

