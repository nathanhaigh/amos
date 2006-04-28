#!/bin/sh

if (( $# == 1 )); then
    UIC=$1
else
    UIC=uic
fi

echo Running `which $UIC`
for ui in `ls *.ui`; do
    base=${ui%.ui}
    echo + Writing ../$base.hh
    $UIC $ui | grep -v '\*\*' > $base.hh || exit 1;
    mv $base.hh ..
    echo + Writing ../$base.cc
    $UIC -impl $base.hh $ui | grep -v '\*\*' > $base.cc || exit 1;
    mv $base.cc ..
done
