#!/usr/bin/env python

"""
    The MIT License (MIT)

    Copyright (c) 2017 Joel Hoener <athre0z@zyantific.com>
    
    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:
    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.
    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
"""

from __future__ import absolute_import, division, print_function, unicode_literals

import os
import errno
import argparse
import glob

from subprocess import Popen, PIPE
from distutils.spawn import find_executable


def get_build_cmd():
    return {'posix': 'make', 'nt': 'MSBuild'}[os.name]

def get_cmake_gen(target_version):
    if os.name == 'posix':
        return 'Unix Makefiles'
    elif os.name == 'nt':
        gen = 'Visual Studio ' + (
            '10' if target_version[0] <= 6 and target_version[1] <= 8 else '14'
        )
        return (gen + ' Win64') if target_version >= (7, 0) else gen
    else:
        assert False

def get_build_solution_arguments(build_dir):
    build_bin = get_build_cmd()
    if os.name == 'nt':
        sln, = glob.glob(os.path.join(build_dir, '*.sln'))
        sln = os.path.basename(sln)
        return [build_bin, sln, '/p:Configuration=Release']
    elif os.name == 'posix':
        # Speed things up a little.
        from multiprocessing import cpu_count
        return [build_bin, '-j%d' % cpu_count()]
    else:
        assert False

def get_install_solution_arguments():
    build_bin = get_build_cmd()
    if os.name == 'nt':
        return [build_bin, 'INSTALL.vcxproj', '/p:Configuration=Release']
    elif os.name == 'posix':
        return [build_bin, 'install', 'VERBOSE=1']
    else:
        assert False


if __name__ == '__main__':
    #
    # Parse arguments
    #
    parser = argparse.ArgumentParser(
        description='Build script compiling and installing the plugin.'
    )
    
    target_args = parser.add_argument_group('target configuration')
    target_args.add_argument(
        '--ida-sdk', '-i', type=str, required=True,
        help='Path to the IDA SDK'
    )
    target_args.add_argument(
        '--target-version', '-t', required=True,
        help='IDA versions to build for (e.g. 6.9).'
    )
    target_args.add_argument(
        '--idaq-path', type=str, required=False,
        help='Path with idaq binary, used for installing the plugin. '
             'On unix-like platforms, also required for linkage.'

    )
    target_args.add_argument(
        '--ea', required=False, choices=[32, 64], type=int,
        help='The IDA variant (ida/ida64, sizeof(ea_t) == 4/8) to build for. '
             'If omitted, build both.'
    )

    parser.add_argument(
        '--skip-install', action='store_true', default=False,
        help='Do not execute install target'
    )
    parser.add_argument(
        'cmake_args', default='', type=str, nargs=argparse.REMAINDER,
        help='Additional arguments passed to CMake'
    )
    args = parser.parse_args()

    def print_usage(error=None):
        parser.print_usage()
        if error:
            print(error)
        exit()

    # Parse target version
    target_version = args.target_version.strip().split('.')
    try:
        target_version = int(target_version[0]), int(target_version[1])
    except (ValueError, IndexError):
        print_usage('[-] Invalid version format, expected something like "6.5"')

    # Supported platform?
    if os.name not in ('nt', 'posix'):
        print('[-] Unsupported platform')

    # Unix specific sanity checks
    if os.name == 'posix' and not args.idaq_path:
        print_usage('[-] On unix-like platforms, --idaq-path is required.')

    #
    # Find tools
    #
    cmake_bin = find_executable('cmake')
    build_bin = find_executable(get_build_cmd())
    if not cmake_bin:
        print_usage('[-] Unable to find CMake binary')
    if not build_bin:
        print_usage('[-] Unable to find build (please use Visual Studio MSBuild CMD or make)')

    #
    # Build targets
    #
    for ea in (args.ea,) if args.ea else (32, 64):
        build_dir = 'build-{}.{}-{}'.format(*(target_version + (ea,)))
        try:
            os.mkdir(build_dir)
        except OSError as e:
            if e.errno != errno.EEXIST:
                raise

        # Run cmake
        cmake_cmd = [
            cmake_bin,
            '-DIDA_SDK=' + args.ida_sdk,
            '-G', get_cmake_gen(target_version),
            '-DIDA_VERSION={}{:02}'.format(*target_version),
            '-DIDA_BINARY_64=' + ('ON' if target_version >= (7, 0) else 'OFF')
        ]

        if args.idaq_path:
            cmake_cmd.append('-DIDA_INSTALL_DIR=' + args.idaq_path)
            cmake_cmd.append('-DCMAKE_INSTALL_PREFIX=' + args.idaq_path)

        if ea == 64:
            cmake_cmd.append('-DIDA_EA_64=TRUE')

        cmake_cmd.append('..')

        print('CMake command:')
        print(' '.join("'%s'" % x if ' ' in x else x for x in cmake_cmd))

        proc = Popen(cmake_cmd, cwd=build_dir)
        if proc.wait() != 0:
            print('[-] CMake failed, giving up.')
            exit()

        # Build plugin
        proc = Popen(get_build_solution_arguments(build_dir), cwd=build_dir)
        if proc.wait() != 0:
            print('[-] Build failed, giving up.')
            exit()

        if not args.skip_install and args.idaq_path:
            # Install plugin
            proc = Popen(get_install_solution_arguments(), cwd=build_dir)
            if proc.wait() != 0:
                print('[-] Install failed, giving up.')
                exit()

    print('[+] Done!')
