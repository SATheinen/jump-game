from setuptools import setup, Extension
import pybind11
import subprocess

# Get SDL2 flags
sdl_cflags = subprocess.check_output(['/opt/homebrew/bin/sdl2-config', '--cflags']).decode().strip().split()
sdl_libs = subprocess.check_output(['/opt/homebrew/bin/sdl2-config', '--libs']).decode().strip().split()

# Define extension
ext_modules = [
    Extension(
        'jumpgame',
        sources=[
            'bindings.cpp',
            '../src/Game.cpp',
            '../src/Player.cpp',
            '../src/PlayerManager.cpp',
            '../src/PlatformManager.cpp',
            '../src/TextWindow.cpp',
        ],
        include_dirs=[
            pybind11.get_include(),
            '../include',
        ],
        extra_compile_args=['-std=c++17'] + sdl_cflags,
        extra_link_args=sdl_libs + ['-lSDL2_ttf'],
        language='c++'
    ),
]

setup(
    name='jumpgame',
    version='0.1.0',
    author='Silas',
    description='Jump Game RL Environment',
    ext_modules=ext_modules,
)