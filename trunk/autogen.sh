#!/bin/sh

set -x

aclocal
automake --foreign --missing --copy
autoconf
./configure
