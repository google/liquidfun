#!/bin/bash -e
#
# Really simple test of each build target.  This doesn't validate the
# build artifacts it simply verifies the build succeeded and writes a summary
# to build_summary.log

declare -r this="$(readlink -f ${0})"
declare -r project_dir="$(readlink -f $(dirname ${0})/../Box2D)"
declare -r project_name="$(basename ${project_dir})"
declare -r android_root="$(readlink -f ${project_dir}/../../../)"
# TODO(smiles): Select this based upon uname.
declare -r cmake="$(readlink -f \
${android_root}/prebuilts/cmake/linux-x86/current/bin/cmake)"
declare -r build_targets_android="
libBox2D
libBox2D_static
"
declare -r build_targets_ndk="
Box2D CMAKE=${cmake}
Box2D_static CMAKE=${cmake}
"
declare -r build_configs="
BOX2D_BUILD_USING_CMAKE=1
BOX2D_BUILD_USING_CMAKE=0
"
declare -r build_log="${project_dir}/build.log"
declare -r build_summary="${project_dir}/build_summary.log"

rm -f "${build_log}" "${build_summary}"

(
  pushd "${project_dir}"
  IFS=$'\n'
  for target in ${build_targets_ndk}; do
    for config in ${build_configs}; do
      ndk-build clean | tee -a "${build_log}"
	  echo "ndk-build ${config} ${target}" | tee -a "${build_summary}"
	  if ! eval "ndk-build ${config} ${target} ${@}"; then
        echo "Failed: $?" | tee "${build_summary}"
	  fi | tee -a "${build_log}"
      find -L obj libs -type f -name '*.a*' -o -name '*.so*' | \
        xargs ls -l | tee -a "${build_summary}"
    done
  done
  popd
)

(
  pushd "${android_root}"
  IFS=$'\n'
  source build/envsetup.sh
  tapas null | tee -a "${build_log}"
  for target in ${build_targets_android}; do
    for config in ${build_configs}; do
      find out -type f -ipath "*${project_name}*" | xargs rm -rf
      echo "make ${config} ${target}" | tee -a "${build_summary}"
      if ! eval "make ${config} ${target} ${@}"; then
	    echo "Failed: $?" | tee -a "${build_summary}"
      fi | tee -a "${build_log}";
      find -L out -type f -iname "*${project_name}*.a*" -o \
                          -iname "*${project_name}*.so*" | \
        xargs ls -l | tee -a "${build_summary}"
    done
  done
  popd
)
