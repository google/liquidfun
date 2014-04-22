#!/usr/bin/python
# Copyright (c) 2014 Google, Inc.
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
# Build, deploy, debug / execute a native Android package based upon
# NativeActivity.

import argparse
import os
import re
import shutil
import subprocess
import sys
import xml.dom.minidom as minidom

MANIFEST_NAME = 'AndroidManifest.xml'

def get_number_of_devices_connected():
  """Gets the numer of connected android devices.

  Returns:
    The number of connected android devices.
  """
  process = subprocess.Popen(
      'adb devices -l', shell=True,
      stdout=subprocess.PIPE, stderr=subprocess.PIPE)

  out = process.communicate()[0]
  return len(
      re.sub(r'^(List of devices attached\s*\n)|(\n)$', r'', out).splitlines())


def get_package_name_from_manifest(name):
  """Gets the name of the apk package to profile from the Manifest.

  Args:
    name: The name of the AndroidManifest file to search through.

  Returns:
    The name of the apk package.
  """
  xml = minidom.parse(name)
  return xml.getElementsByTagName('manifest')[0].getAttribute('package')


def execute_remote_command(command_str, adb_device=''):
  """Execute a command on the connected android device using adb.

  Args:
    command_str: The command to be executed.
    adb_device: The device to execute the command on.

  Returns:
    The resulting output, err, and returncode of the command.
  """
  process = subprocess.Popen(
      'adb %s shell %s' % (adb_device, command_str),
      shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

  out, err = process.communicate()
  return (out, err, process.returncode)


def get_pid(process_name, adb_device=''):
  """Gets the process id of the given process.

  Args:
    process_name: The name of the process to get the pid of.
    adb_device: The device to look for the process on.

  Returns:
    The process id of the given process if found or None if not.
  """
  out = execute_remote_command('ps', adb_device)[0]
  for line in out.splitlines():
    if re.search(process_name, line):
      return int(line.split()[1])
  return None


def get_perf_data_file(package_name, adb_device, output_file):
  """Get the perf data file from the remote host.

  Args:
    package_name: The name of the running package.
    adb_device: The device that the package is running on.
    output_file: The directory to store the output in.
  """
  execute_remote_command(
      'run-as %s chmod 666 /data/data/%s/perf.data' %
      (package_name, package_name), adb_device)

  process = subprocess.Popen(
      'adb %s pull /data/data/%s/perf.data' % (adb_device, package_name),
      shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
  process.wait()

  execute_remote_command(
      'run-as %s rm /data/data/%s/perf.data' % (package_name, package_name),
      adb_device)

  if output_file:
    shutil.move('perf.data', output_file)

def run_perf_remotely(adb_device, apk_directory, perf_args, output_file):
  """Run perf remotely.

  Args:
    adb_device: The device that perf is run on
    apk_directory: The directory of the apk file to profile
    perf_args: The arguments to run perf with
    output_file: The destination perf file to save the data to

  Returns:
    1 for error, 0 for success.
  """
  android_perf = os.path.dirname(os.path.realpath(__file__)) + '/perf'
  android_perf_remote = '/data/local/tmp/perf'
  perf_command = perf_args[0]

  process = subprocess.Popen(
      'adb %s push %s %s' % (adb_device, android_perf, android_perf_remote),
      shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
  process.wait()

  name = MANIFEST_NAME
  if apk_directory:
    name = apk_directory + '/' + MANIFEST_NAME
  package_name = get_package_name_from_manifest(name)

  # Deal with starting the package and retrieving the data
  if perf_command == 'record' or perf_command == 'stat':
    execute_remote_command(
        'am start -n %s/android.app.NativeActivity' %
        package_name, adb_device)
    pid = get_pid(package_name, adb_device)
    if pid is None:
      print  >> sys.stderr, 'Cannot start %s' % package_name
      return 1

    # Use -m 4 due to certain devices not having mmap data pages
    record_args = ''
    if perf_command == 'record':
      record_args = '-m 4'
    out = execute_remote_command(
        'run-as %s %s %s -p %d %s %s' %
        (package_name, android_perf_remote, perf_command, pid, record_args,
         ' '.join(perf_args[1:])),
        adb_device)[0]

    if perf_command == 'record':
      get_perf_data_file(package_name, adb_device, output_file);
    else:
      print out
  else:
    print execute_remote_command(
        '%s %s' % (android_perf_remote, ' '.join(perf_args)), adb_device)[0]
  return 0

#TODO(mlentine): Add Windows/Mac Support
def main():
  parser = argparse.ArgumentParser(
      description=('Run Perf for the Android package in the current '
                   'directory on a connected device.'))
  parser.add_argument(
      '-s', '--adb-device',
      help=('adb-device specifies the serial_number of the device to deploy '
            'the built apk to if multiple Android devices are connected to '
            'the host'))
  parser.add_argument(
      '--apk-directory',
      help='apk-directory specifies the directory of the package to profile')
  # This should hijack the -o option of perf record
  parser.add_argument('-o', '--output-file')

  args, perf_args = parser.parse_known_args()
  perf_command = perf_args[0]
  # Define the adb device string that is needed for passing to adb shell
  adb_device = ''
  if args.adb_device:
    adb_device = '-s '+args.adb_device

  # Do nothing if no device is specified or can be implicitly found
  if adb_device == '' and get_number_of_devices_connected() > 1:
    print (
        'Multiple Android devices are connected to this host. Please specify '
        'a device using --adb-device <serial>. The devices connected are: ')
    subprocess.Popen('adb devices -l', shell=True)
    return 1

  # Run perf remotely
  if (perf_command == 'record' or perf_command == 'stat'
      or perf_command == 'top'):
      return run_perf_remotely(
        adb_device, args.apk_directory, perf_args, args.output_file);
  #Run perf locally
  else:
    perf_host = os.path.dirname(os.path.realpath(__file__)) + '/perfhost_linux'
    process = subprocess.Popen(
        '%s %s' % (perf_host, ' '.join(perf_args)), shell=True)
    process.wait()
    return process.returncode
  return 0

if __name__ == '__main__':
  sys.exit(main())
