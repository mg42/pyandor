#!/usr/bin/python
from common import *
import sys

cameraHandles = GetCameraHandles()
print("Cameras found:", cameraHandles)
if (cameraHandles == None):
  sys.exit()
"""
for i in range(0, cameras):
  camera(i)
"""
Luca_S = camera(cameraHandles[0])
Luca_S.shutdown()
