#!/usr/bin/python

def info(object, spacing=10, collapse=1):
  """Print members and values.

  Takes object, module, class, list, dictionary, or string."""
  memberList = [member for member in dir(object)]
  for member in memberList:
    if not member.endswith("__"): #Ignore built-ins
      try:
        print("%s %s" %(member.ljust(spacing),
                        getattr(object, member)))
      except AttributeError:
        print("%s <AttributeError>" %(member))

from pyandor import *
try:
  c = Camera()
except NoCameraError:
  print("No cameras found")
  import sys
  sys.exit()

info(c)
