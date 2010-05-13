#!/bin/bash

BOOST_DIR=~/src/boost

DEPS=`cat deps.txt | xargs`
DIR=`pwd`

pushd $BOOST_DIR
bcp $DEPS $DIR
popd

for lib in `ls libs`; do
	for it in `ls libs/$lib`; do
		if [ "$it" != "src" ]; then
			rm -rf libs/$lib/$it
		fi
	done
done

rm -rf doc