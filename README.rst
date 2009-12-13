`pyandor` is a Python wrapper around the accompanying C Python extension
`_pyandor`, which is a wrapper around the SDK from Andor Technology.

Build `pyandor` by running::

    $ python setup.py build

To use the example, create a symlink named pyandor.so to the build object
similar to::

    $ ln -s build/lib.linux-x86_64-3.1/pyandor.so pyandor.so

Example output with Luca-S::

    $ ./test-image-example.py
    Cameras found: (100,)
    GetVersionInfo exception 20066
    Versions: None
    Camera handle# 100 initializing ...
    Identified head: Luc247_MONO
    Head serial number: 358
    Capabilities: {'TriggerModes': 59, 'PixelMode': 2, 'Features': 139909, 'CameraType': 11, 'EMGainCapability': 1, 'SetFunctions': 20500, 'PCICard': 0, 'AcqModes': 63, 'FTReadModes': 7, 'GetFunctions': 45, 'ReadModes': 7}
    Internal Mechanical Shutter?: 0
    Readout Capabilities: {'ADChannelBitDepths': [14], 'AmplifierTypes': ['Electron Multiplying'], 'AvailableMatrix': [[0, 0, 0, 0]], 'PreAmpGains': [1], 'HSSpeeds': [12]}
    Vertical Shift Capabilities: {'VSAmplitudes': None, 'VSSpeeds': [0.6172999739646912], 'OkayWithoutVSAmp': 0.6172999739646912}
    Camera handle# 100 Shutting down ...

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
