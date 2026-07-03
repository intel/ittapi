#!/usr/bin/env python
#
# Copyright (C) 2005-2026 Intel Corporation
#
# SPDX-License-Identifier: GPL-2.0-only OR BSD-3-Clause
#

import os
import sys
import shutil
import subprocess


def run_shell(cmd):
    print(f"\n>> {cmd}")
    try:
        subprocess.run(cmd, shell=True, check=True)
    except subprocess.CalledProcessError:
        sys.exit(f">> failed to run shell command: {cmd}")


if sys.platform == 'win32':
    def read_registry(path, depth=0xFFFFFFFF, statics={}):
        try:
            import _winreg
        except ImportError:
            import winreg as _winreg
        parts = path.split('\\')
        hub = parts[0]
        path = '\\'.join(parts[1:])
        if not statics:
            statics['hubs'] = {
                'HKLM': _winreg.HKEY_LOCAL_MACHINE, 'HKCL': _winreg.HKEY_CLASSES_ROOT}

        def enum_nodes(curpath, level):
            if level < 1:
                return {}
            res = {}
            try:
                aKey = _winreg.OpenKey(
                    statics['hubs'][hub], curpath, 0, _winreg.KEY_READ | _winreg.KEY_WOW64_64KEY)
            except WindowsError:
                return res

            try:
                i = 0
                while True:
                    name, value, _ = _winreg.EnumValue(aKey, i)
                    i += 1
                    res[name] = value
            except WindowsError:
                pass

            keys = []
            try:
                i = 0
                while True:
                    key = _winreg.EnumKey(aKey, i)
                    i += 1
                    keys.append(key)
            except WindowsError:
                pass

            _winreg.CloseKey(aKey)

            for key in keys:
                res[key] = enum_nodes(curpath + '\\' + key, level - 1)

            return res

        return enum_nodes(path, depth)


def get_vs_versions():  # https://www.mztools.com/articles/2008/MZ2008003.aspx
    if sys.platform != 'win32':
        return []
    versions = []

    hkcl = read_registry(r'HKCL', 1)
    for key in hkcl:
        if 'VisualStudio.DTE.' in key:
            version = key.split('.')[2]
            if int(version) >= 12:
                versions.append(version)

    if not versions:
        print("No Visual Studio version found")
    return sorted(versions)


def detect_cmake():
    if sys.platform == 'darwin':
        cmake_path = shutil.which('cmake')
        if cmake_path:
            return 'cmake'
        xcrun_path = shutil.which('xcrun')
        if xcrun_path:
            return 'xcrun cmake'
        print("No cmake and no XCode found...")
        return None
    return 'cmake'


def main():
    import argparse
    parser = argparse.ArgumentParser()
    vs_versions = get_vs_versions()
    parser.add_argument(
        "-d", "--debug", help="specify debug build configuration (release by default)", action="store_true")
    parser.add_argument(
        "-c", "--clean", help="delete any intermediate and output files", action="store_true")
    parser.add_argument(
        "-v", "--verbose", help="enable verbose output from build process", action="store_true")
    parser.add_argument(
        "-pt", "--ptmark", help="enable anomaly detection support", action="store_true")
    parser.add_argument(
        "-ft", "--fortran", help="enable fortran support", action="store_true")
    parser.add_argument(
        "-cpp", "--cpp", help="enable C++ wrapper support", action="store_true")
    parser.add_argument(
        "--refcol", help="enable reference collector build", action="store_true")
    if sys.platform == 'win32':
        if vs_versions:
            parser.add_argument(
                "--vs", help="specify visual studio version {default}", choices=vs_versions, default=vs_versions[0])
        parser.add_argument(
            "--cmake_gen", choices=["vs", "ninja"], help="specify cmake build generator")
    args = parser.parse_args()

    work_dir = os.getcwd()
    if args.clean:
        bin_dir = os.path.join(work_dir, 'bin')
        if os.path.exists(bin_dir):
            shutil.rmtree(bin_dir)
        work_folder = os.path.join(
            work_dir, "build_" + sys.platform.replace('32', ""))
        if os.path.exists(work_folder):
            shutil.rmtree(work_folder)
        return

    # Build folder: build_linux, build_win, build_darwin
    work_folder = os.path.join(
        work_dir, "build_" + sys.platform.replace('32', ""))
    if not os.path.exists(work_folder):
        os.makedirs(work_folder)
    print("work_folder: ", work_folder)
    os.chdir(work_folder)

    cmake = detect_cmake()
    if not cmake:
        print("Error: cmake is not found")
        return

    if sys.platform == 'win32':
        vs_year = {
            '12': '2013', '14': '2015', '15': '2017',
            '16': '2019', '17': '2022', '18': '2025',
        }
        use_ninja = getattr(args, 'cmake_gen', None) == 'ninja'
        if vs_versions and not use_ninja:
            year = vs_year.get(args.vs, '')
            generator = f'Visual Studio {args.vs} {year}'.strip()
            generator_args = '-A x64'
        else:
            generator = 'Ninja'
            generator_args = ''
    else:
        generator = 'Unix Makefiles'
        generator_args = ''

    cmake_options = [f'-G"{generator}"', generator_args]
    if args.debug:
        cmake_options.append('-DCMAKE_BUILD_TYPE=Debug')
    if args.verbose:
        cmake_options.append('-DCMAKE_VERBOSE_MAKEFILE:BOOL=ON')
    if args.ptmark:
        cmake_options.append('-DITT_API_IPT_SUPPORT=1')
    if args.fortran:
        cmake_options.append('-DITT_API_FORTRAN_SUPPORT=1')
    if args.cpp:
        cmake_options.append('-DITT_API_CPP_SUPPORT=ON')
    if args.refcol:
        cmake_options.append('-DITT_API_REFERENCE_COLLECTOR=ON')

    run_shell(f'{cmake} "{work_dir}" {" ".join(opt for opt in cmake_options if opt)}')

    if sys.platform == 'win32':
        target = 'ALL_BUILD' if not use_ninja else 'all'
        config = 'Debug' if args.debug else 'Release'
        run_shell(f'{cmake} --build . --config {config} --target {target}')
    else:
        config = 'Debug' if args.debug else 'Release'
        run_shell(f'{cmake} --build . --config {config}')


if __name__ == "__main__":
    main()
