`pyandor` is a Python wrapper around the accompanying C Python extension
`_pyandor`, which is a wrapper around the SDK from Andor Technology.

Build `pyandor` by running::

    $ python setup.py build

Note that it won't override an existing build, so if you have built it once before you would need to run::

    $ rm -rf build/ && python setup.py build

To use the example `test.py`, create a symlink named pyandor.so to the build object
similar to::

    $ ln -s build/lib.linux-x86_64-3.1/pyandor.so pyandor.so

Example output with Luca-S::

    $ ./test.py
    GetCameraHandles <built-in method GetCameraHandles of pyandor.Camera object at 0x7f3a0e99fc00>
    ShutDown   <built-in method ShutDown of pyandor.Camera object at 0x7f3a0e99fc00>
    _frame_rate -1.0
    _handle    0
    _image_size_bytes 0
    _model     {'head_serial': 598, 'pixels_height': 1002, 'name': 'Luc285_MONO\x00', 'pixels_width': 1004}
    _status    0
    _time_readout 0.0806499272585
    active_chip_window {'height': 1002, 'width': 1004, 'y_offset': 1, 'x_offset': 1, 'bin_y': 1, 'bin_x': 1}
    camera_trigger_mode <AttributeError>
    cooling_temperature_actual -999.0
    cooling_temperature_target -999.0
    delay_after_readout -1.0
    exposure_time 9.99999974738e-06
    images_to_acquire 0
    readout_mode <AttributeError>
    readout_vertical_speed <AttributeError>
    shutter_state <AttributeError>
    shutter_timing <AttributeError>
    shutter_trigger_mode <AttributeError>

You have to install the Andor SDK package separately, and have your compiler
and linker find the include files and libraries.  You also need to have the
latest pre-libusb:1 compatibility library installed if you are using a
USB camera, else have the Andor kernel module andordrvlx.ko loaded.

Class structure
===============

Since Python cannot and should not implement call-by-reference wrapping of
the the Andor SDK C functions, the planned object layer is written in
`uml.zargo`

IRC
===

omsai at #friendly-coders on irc.freenode.net

Change Log
==========

- Camera Class
- Camera identification and capability functions
- USB camera support started: Luca-S
