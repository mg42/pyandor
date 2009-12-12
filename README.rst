`pyandor` is a Python wrapper around the accompanying C Python extension
`_pyandor`, which is a wrapper around the SDK from Andor Technology.

Build `pyandor` by running:

    $ python setup.py build

To use the example, create a symlink named pyandor.so to the build object
similar to:

    $ ln -sv build/lib.linux-x86_64-3.1/pyandor.so pyandor.so
    `pyandor.so' -> `build/lib.linux-x86_64-3.1/pyandor.so'

You have to install the Andor SDK package separately, and have your compiler
and linker find the include files and libraries.  You also need to have the
latest pre-libusb:1 compatibility library installed if you are using a
USB camera, else have the Andor kernel module andordrvlx.ko loaded.

IRC
===

omsai at #friendly-coders on irc.freenode.net

Change Log
==========

- Camera identification and capability functions
- USB camera support started: Luca-S
