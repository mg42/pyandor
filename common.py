#!/usr/bin/python
from pyandor import *

class camera:
  def __init__(self, cameraHandle):
    self.versions = GetVersionInfo()
    print("Versions:", self.versions)
    self.handle = cameraHandle
    print("Camera handle# %d initializing ..." %self.handle)
    Initialize()
    self.head = GetHeadModel()
    print("Identified head: %s" %self.head)
    self.serial = GetCameraSerialNumber()
    print("Head serial number: %d" %self.serial)
    self.capabilities = GetCapabilities()
    print("Capabilities:", self.capabilities)
    self.internalshutter = IsInternalMechanicalShutter()
    print("Internal Mechanical Shutter?: %i" %self.internalshutter)
    self.readoutcapabilities = GetReadoutCapabilities()
    print("Readout Capabilities:", self.readoutcapabilities)
    self.verticalshiftcapabilities = GetVSCapabilities()
    print("Vertical Shift Capabilities:", self.verticalshiftcapabilities)
  
  def __repr__(self):
    return "<Camera Handle %s>" %self.handle

  def shutdown(self):
    print("Camera handle# %d Shutting down ..." %self.handle)
    ShutDown()
