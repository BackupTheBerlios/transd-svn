#!/bin/sh

set -x

aclocal
automake --foreign --add-missing --copy
autoconf
./configure
