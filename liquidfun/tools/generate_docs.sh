#!/bin/bash -eu
#
# Copyright (c) 2013 Google, Inc.
#
# This software is provided 'as-is', without any express or implied
# warranty.  In no event will the authors be held liable for any damages
# arising from the use of this software.
# Permission is granted to anyone to use this software for any purpose,
# including commercial applications, and to alter it and redistribute it
# freely, subject to the following restrictions:
# 1. The origin of this software must not be misrepresented; you must not
# claim that you wrote the original software. If you use this software
# in a product, an acknowledgment in the product documentation would be
# appreciated but is not required.
# 2. Altered source versions must be plainly marked as such, and must not be
# misrepresented as being the original software.
# 3. This notice may not be removed or altered from any source distribution.
#

# Generate html documentation from markdown and doxygen comments in the source.
# This requires at doxygen version 1.8.5 or above.

declare -r root_dir="$(cd $(dirname $0)/.. ; pwd)"
declare -r docs_source_dir=${root_dir}/Box2D/Box2D/Documentation
declare -r docs_output_dir=${root_dir}/Box2D/Documentation

declare -r doc_dirs="API-Ref Building Programmers-Guide Readme"

# Clean output directory.
for output_dir in ${doc_dirs}; do
  rm -rf ${docs_output_dir}/${output_dir}/html/*
done

# Build documentation.
for source_dir in ${doc_dirs}; do
  echo '
<html>
<head>
<meta http-equiv="refresh" content="0;url='"${source_dir}"'/html/index.html">
</head>
<body>
<a href="'"${source_dir}"'/html/index.html">click here if you are not
redirected</a>
</body>
</html>' > "${docs_output_dir}/${source_dir}.html"


  pushd "${docs_source_dir}/${source_dir}" >/dev/null
  doxygen
  popd >/dev/null
done
