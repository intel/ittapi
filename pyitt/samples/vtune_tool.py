from os import environ, path
from sys import platform
from subprocess import run


class VTuneTool:
    _ONEAPI_ROOT_ENV = 'ONEAPI_ROOT'
    _VTUNE_PROFILER_DIR_ENV = 'VTUNE_PROFILER_DIR'

    def __init__(self):
        if environ.get(self._VTUNE_PROFILER_DIR_ENV, None):
            self.path = path.join(environ[self._VTUNE_PROFILER_DIR_ENV])
        elif environ.get(self._ONEAPI_ROOT_ENV, None):
            self.path = path.join(environ[self._ONEAPI_ROOT_ENV], 'vtune', 'latest')
        else:
            self.path = None

        if not self.path or not path.exists(self.path):
            if platform == 'win32':
                amplxe_vars_script_command = '<vtune_install_dir>\\amplxe-vars.bat'
                export_command = 'set VTUNE_PROFILER_DIR=<vtune_install_dir>'
            else:
                amplxe_vars_script_command = '<vtune_install_dir>/amplxe-vars.sh'
                export_command = 'export VTUNE_PROFILER_DIR=<vtune_install_dir>'

            raise ValueError(f'VTune Profiler installation directory is not found.\n'
                             f'Use {amplxe_vars_script_command} to prepare the environment or specify VTune Profiler'
                             f' installation directory using VTUNE_PROFILER_DIR environment variable:\n'
                             f'{export_command}')
        self._tool_path = path.join(self.path, 'bin64', 'vtune.exe' if platform == 'win32' else 'vtune')

    def run_hotspot_collection(self, app_args, additional_collection_args=None):
        collection_args = [self._tool_path, '-collect', 'hotspots', '-knob', 'enable-characterization-insights=false']

        if isinstance(additional_collection_args, list):
            collection_args.extend(additional_collection_args)
        elif isinstance(additional_collection_args, str):
            collection_args.extend(additional_collection_args.split(' '))
        elif additional_collection_args is not None:
            raise TypeError('additional_collection_args argument must be a list or str')

        collection_args.append('--')

        if isinstance(app_args, list):
            collection_args.extend(app_args)
        elif isinstance(app_args, str):
            collection_args.extend(app_args.split(' '))
        else:
            raise TypeError('app_args argument must be a list or str')

        return run(collection_args, check=True)


def run_vtune_hotspot_collection(app_args, additional_collection_args=None):
    return VTuneTool().run_hotspot_collection(app_args, additional_collection_args)
