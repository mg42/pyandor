from distutils.core import setup, Extension

module1 = Extension('pyandor',
                    libraries = ['andor'],
                    sources = ['_pyandormodule.c'])

setup (name = 'pyandor',
       version = '0.1a',
       description = 'Wrapper for Andor SDK functions',
       ext_modules = [module1])
