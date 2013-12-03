#!/bin/bash -eu
rm -f ../Box2D/Documentation/Building/html/*
rm -f ../Box2D/Documentation/Programmers-Guide/html/*
rm -rf ../Box2D/Documentation/API-Ref/html/*
cp ../Box2D/Box2D/Documentation/image_* ../Box2D/Documentation/Programmers-Guide/html
pushd ../Box2D/Box2D/Documentation
doxygen
pushd Building
doxygen
popd
pushd API-Ref
doxygen
popd
popd

