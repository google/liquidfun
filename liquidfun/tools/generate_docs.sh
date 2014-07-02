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

declare -r doc_dirs="API-Ref Building Programmers-Guide Readme ReleaseNotes SWIG"

declare -r footer_file="${root_dir}/Box2D/Box2D/Documentation/footer.html"

#liquidfun.js paths
declare -r lfjs_dir=${root_dir}/Box2D/lfjs
declare -r testbed_output_dir=${docs_output_dir}/testbed


usage() {
  echo "
Build documentation from markdown using doxygen.

Usage: $(basename $0) [-h]

-h: Display this help text.
" >&2
  exit 1
}

main() {
  while getopts "h" options; do
    case ${options} in
      h) usage;;
    esac
  done

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
<a href="'"${source_dir}"'/html/index.html">Click here if you are not
redirected.</a>
' > "${docs_output_dir}/${source_dir}.html"
    cat "${footer_file}" >> "${docs_output_dir}/${source_dir}.html"
    pushd "${docs_source_dir}/${source_dir}" >/dev/null
    doxygen
    popd >/dev/null
    # Clean up some of the output html.
    index_html="${docs_output_dir}/${source_dir}/html/index.html"
    awk '/ Documentation<\/div>/ { sub(/ Documentation/, ""); } { print $0 }' \
      "${index_html}" > "${index_html}.new" && \
      mv "${index_html}.new" "${index_html}"
  done

  # Check built documentation.
  if [[ "$(which linklint)" != "" ]]; then
    results="${root_dir}/linklint_results"
    # NOTE: -no_anchors is used since linklint generates spurious errors about
    # missing named anchors where they're present.
    linklint -no_anchors -orphan -root "${docs_output_dir}" /@ -doc "${results}"
    if grep -q ERROR "${results}/index.html"; then
      echo -e "\nlinklint detected errors: results are available for"\
              "inspection in" \
              "\n  ${results}" >&2
      exit 1
    fi
    # Report an error if an https link has been added.
    # github does not support https links
    if grep -r https "${results}"; then
      echo -e "\nERROR linklint detected https links: "\
              "github does not support https." >&2
      exit 1
    fi
  else
    echo "Unable to find linklint, install to validate the" \
         "generated documentation." >&2
    exit 1
  fi

  # transfer liquidfun.js to output
  mkdir ${testbed_output_dir} >/dev/null
  mkdir ${testbed_output_dir}/testbed >/dev/null
  mkdir ${testbed_output_dir}/testbed/tests >/dev/null

  cp ${lfjs_dir}/index.html ${testbed_output_dir}/ >/dev/null
  cp ${lfjs_dir}/liquidfun.js ${testbed_output_dir}/ >/dev/null
  for file in $(find ${lfjs_dir}/testbed -maxdepth 1 -name "*.js") ; do
     cp $file ${testbed_output_dir}/testbed/ >/dev/null
  done

  for file in $(find ${lfjs_dir}/testbed/tests -name "*.js") ; do
     cp $file ${testbed_output_dir}/testbed/tests/ >/dev/null
  done

}

main "${@}"
