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
    Error 20066 instead of DRV_SUCCESS at _pyandormodule.c/PyGetVersionInfo:260
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

Class structure
===============

Since Python cannot and should not implement call-by-reference wrapping of
the the Andor SDK C functions, the following is the planned object layer::

    GetCameraHandles()

    class Camera(Handle=NULL, shutter=open):
      # Automatic handling of GetCurrentCamera / SetCurrentCamera(Handle)
      # when calling class instance for all functions, e.g.:
      # >>> camera[0].SnapEM()
      # >>> camera[1].SnapConv()
      # ...will take care of calling SetCurrentCamera

      # class instance constructor
      __init__:
        Initialize()       # SDK function automatically called
        # self properties populated
        .Handle
        .HeadModel
        .HeadSerial
        .Exposure        
        .Binning
        .Cooling
        .height
        .width
        .left
        .right
        # properties populated if > 1
        .HorzReadout[]     # Bits + Amplifier + HSSpeed + PreAmp settable list
        .VertReadout[]     # VSSpeed settable list
        .VertVoltage[]     # Voltages settable list
        # properties populated if they exist
        .EMGain            # Warn if non-linear
        .EMGainInfo[Min, Max, IsLinear]
        .BaselineCompensation
        .BaseLineOffsetLevels[]
        .Fan[]             # on, low, off
        .SetTemperature
        .TemperatureInfo[Min, Max]
        .Overlap           # Frame transfer
        .HasInternalShutter
        .RingExposure[]    # iCam
        .Crop
        # custom status properties
        .Busy
        .Temperature
        .TemperatureLocked 
      
      # member functions
      Temperature()        # Returns if on, current value, and if stabilized
      Shutter(state=['open', 'close', 'auto'], open=0.04, close=0.04)
                            # set timings, and behavior for internal shutter
      ShutterOutput()       # set timings, and behavior for shutter output
      Snap(images=1, Mode=['EM', 'conv'], spool=NULL, ...)
      SnapEM()              # synonym for Snap(Mode='EM') allowing same arguments
      SnapConv()            # synonym for Snap(Mode='conv')
      SnapRing(images=20, exposure=[0.1, 0.35, ...], ...)
      Live()                # RunTillAbort
      Abort()               # aborts snap or live
     
      # class instance destructor
      __destroy__:
        Conf.Write()
        Temperature(state='Off')
        ShutDown()

    # Default settings such as EM gain, conventional HSSpeed, VS, exposure, ...
    class Conf():
      Apply(file="/etc/andor/allcameras.conf")  # Reads in and applies the file
      Write(file)

    class Troubleshoot():
      .SDKVersion
      .EEPROM              # Support only *reading* from EEPROM
      .LogFile

IRC
===

omsai at #friendly-coders on irc.freenode.net

Change Log
==========

- Camera Class
- Camera identification and capability functions
- USB camera support started: Luca-S
