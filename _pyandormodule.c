/*-
 * Copyright (c) 2009-2010 Pariksheet Nanda
 * 
 * Written by Pariksheet Nanda <p.nanda@andor.com>
 * 
 * This file is part of pyandor
 * 
 * pyandor is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 * 
 * pyandor is distributed in the hope that it will be useful, 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public 
 * License along with pyandor.  If not, see http://www.gnu.org/licenses/
 */

#include "_pyandormodule.h"

/* --- Python methods wrapping Andor SDK functions ------------------------- */
// unsigned int GetCameraHandle(at_32 cameraIndex, at_32 * cameraHandle);
// unsigned int GetAvailableCameras(at_32 * totalCameras);
static PyObject*
PyGetCameraHandles(PyObject *self) {
  unsigned int err;
  PyObject *t;
  at_32 totalCameras, cameraHandle;
  int i;
  err= GetAvailableCameras(&totalCameras); N(err)
  if (totalCameras > 0) {
    t = PyTuple_New(totalCameras);
    for (i=0; i < totalCameras; ++i) {
      err= GetCameraHandle(i, &cameraHandle); N(err)
      PyTuple_SetItem(t, i, PyLong_FromLong((long)cameraHandle));
    }
    return t;
  }
  Py_INCREF(Py_None);
  return Py_None;
}
// unsigned int SetCurrentCamera(at_32 cameraHandle);
static PyObject*
PySetCurrentCamera(PyObject *self,
                   PyObject *args) {
  unsigned int err;
  at_32 cameraHandle;
  PyArg_ParseTuple(args, "i",  &cameraHandle);
  err= SetCurrentCamera(cameraHandle);
	                                         if (err != DRV_SUCCESS) return NULL;
  Py_INCREF(Py_None);
  return Py_None;
}
// unsigned int Initialize(char * dir);
// unsigned int SetCurrentCamera(at_32 cameraHandle);
static PyObject*
PyInitialize(PyObject *self,
             PyObject *args) {
  unsigned int err;
  at_32 cameraHandle = 0;
  PyArg_ParseTuple(args, "|i",  &cameraHandle);
  if (cameraHandle) {
    err= SetCurrentCamera(cameraHandle); N(err)
  }
  err= Initialize(ETC_DIR);
  if (err != DRV_SUCCESS) {
    fprintf(stderr, "Initialization exception %u\n", err);
    return NULL;
  }
  /* The win32 example program mentioned a 2 second wait is required
   * from the time of initialization until before *capturing an image*.
   * So it may be unnecessary to include waiting in the initialize
   * function itself.
  sleep(2); /* wait 2 seconds for initialize to complete */
  Py_INCREF(Py_None);
  return Py_None;
}
// unsigned int GetHeadModel(char * name);
static PyObject*
PyGetHeadModel(PyObject *self) {
  unsigned int err;
  char name[MAX_PATH];
  err= GetHeadModel(name); N(err)
  return Py_BuildValue("z#", &name, MAX_PATH);
  /* TODO: with a dictionary make this more human readable, e.g.:
   *   Luc247_MONO = Luca-S */
}
// unsigned int GetCameraSerialNumber(int * number);
static PyObject*
PyGetCameraSerialNumber(PyObject *self) {
  unsigned int err;
  int number;
  err= GetCameraSerialNumber(&number); N(err)
  return Py_BuildValue("i", number);
}
// unsigned int GetCapabilities(AndorCapabilities * caps);
static PyObject*
PyGetCapabilities(PyObject *self) {
  unsigned int err;
  PyObject *d;
  AndorCapabilities caps;
  caps.ulSize = sizeof(caps);
  err= GetCapabilities(&caps); N(err)
  d = PyDict_New();
  PyDict_SetItem(d, PyUnicode_FromString("AcqModes"),
                 PyLong_FromLong((long)caps.ulAcqModes));
  PyDict_SetItem(d, PyUnicode_FromString("ReadModes"),
                 PyLong_FromLong((long)caps.ulReadModes));
  PyDict_SetItem(d, PyUnicode_FromString("TriggerModes"),
                 PyLong_FromLong((long)caps.ulTriggerModes));
  PyDict_SetItem(d, PyUnicode_FromString("CameraType"),
                 PyLong_FromLong((long)caps.ulCameraType));
  PyDict_SetItem(d, PyUnicode_FromString("PixelMode"),
                 PyLong_FromLong((long)caps.ulPixelMode));
  PyDict_SetItem(d, PyUnicode_FromString("SetFunctions"),
                 PyLong_FromLong((long)caps.ulSetFunctions));
  PyDict_SetItem(d, PyUnicode_FromString("GetFunctions"),
                 PyLong_FromLong((long)caps.ulGetFunctions));
  PyDict_SetItem(d, PyUnicode_FromString("Features"),
                 PyLong_FromLong((long)caps.ulFeatures));
  PyDict_SetItem(d, PyUnicode_FromString("PCICard"),
                 PyLong_FromLong((long)caps.ulPCICard));
  PyDict_SetItem(d, PyUnicode_FromString("EMGainCapability"),
                 PyLong_FromLong((long)caps.ulEMGainCapability));
  PyDict_SetItem(d, PyUnicode_FromString("FTReadModes"),
                 PyLong_FromLong((long)caps.ulFTReadModes));
  return d;
}
//unsigned int IsInternalMechanicalShutter(int * InternalShutter);
static PyObject*
PyIsInternalMechanicalShutter(PyObject *self) {
	unsigned int err;
	int InternalShutter;
	err= IsInternalMechanicalShutter(&InternalShutter); N(err)
  return Py_BuildValue("i", InternalShutter);
}
//unsigned int GetNumberADChannels(int * channels);
//unsigned int GetNumberAmp(int * amp);
//unsigned int GetNumberPreAmpGains(int * noGains);
//unsigned int GetBitDepth(int channel, int * depth);
//unsigned int GetNumberHSSpeeds(int channel, int typ, int * speeds);
//unsigned int GetHSSpeed(int channel, int typ, int index, float * speed);
//unsigned int GetPreAmpGain(int index, float * gain);
//unsigned int GetAmpDesc(int index, char * name, int length);
//unsigned int IsPreAmpGainAvailable(int channel, int amplifier, int index, int pa, int * status);
static PyObject*
PyGetReadoutCapabilities(PyObject *self) {
  unsigned int err;
  int NumChannels, NumAmp, NumHSpeeds, NumPreAmpGains, IsAvailable;
  int BitDepth;
  float HSSpeed, PreAmpGain;
  char AmplifierName[22];
  PyObject *c, *ADChannelBitDepths, *AmplifierTypes, *HSSpeeds, *PreAmpGains;
  PyObject *AvailableMatrix;
  int i,j,k,m;
  err= GetNumberADChannels(&NumChannels); N(err)
  err= GetNumberAmp(&NumAmp); N(err)
  err= GetNumberPreAmpGains(&NumPreAmpGains); N(err)
  ADChannelBitDepths = PyList_New(NumChannels);
  AmplifierTypes = PyList_New(NumAmp);
  HSSpeeds = PyList_New(0);
  PreAmpGains = PyList_New(NumPreAmpGains);
  AvailableMatrix = PyList_New(0);
  
  for (i=0; i < NumChannels; ++i) {
    err= GetBitDepth(i, &BitDepth); N(err)
    PyList_SetItem(ADChannelBitDepths, i,
                   PyLong_FromLong((long)BitDepth));
    for (j=0; j < NumAmp; ++j) {
		  err= GetNumberHSSpeeds(i,j, &NumHSpeeds); N(err)
      err= GetAmpDesc(j, AmplifierName, sizeof(AmplifierName));
      PyList_SetItem(AmplifierTypes, j, PyUnicode_FromString(AmplifierName));
      for (k=0; k < NumHSpeeds; ++k) {
			  err= GetHSSpeed(i,j,k, &HSSpeed); N(err)
        PyList_Append(HSSpeeds,
                      PyLong_FromLong((long)HSSpeed));
			  for (m=0; m < NumPreAmpGains; ++m) {
				  err= GetPreAmpGain(m, &PreAmpGain); N(err)
          PyList_SetItem(PreAmpGains, m, PyLong_FromLong((long)PreAmpGain));
          err= IsPreAmpGainAvailable(i,j,k,m, &IsAvailable); N(err)
          if (IsAvailable)
            PyList_Append(AvailableMatrix, Py_BuildValue("[i,i,i,i]",i,j,k,m));
				}
			}
		}
	}
  c = PyDict_New();
  PyDict_SetItem(c, PyUnicode_FromString("ADChannelBitDepths"),
                                       ADChannelBitDepths);
  PyDict_SetItem(c, PyUnicode_FromString("AmplifierTypes"), AmplifierTypes);
  PyDict_SetItem(c, PyUnicode_FromString("HSSpeeds"), HSSpeeds);
  PyDict_SetItem(c, PyUnicode_FromString("PreAmpGains"), PreAmpGains);
  PyDict_SetItem(c, PyUnicode_FromString("AvailableMatrix"), AvailableMatrix);
  return c;
}
//unsigned int GetNumberVSSpeeds(int * speeds);
//unsigned int GetVSSpeed(int index, float * speed);
//unsigned int GetFastestRecommendedVSSpeed(int * index, float * speed);
//unsigned int GetNumberVSAmplitudes(int * number);
static PyObject*
PyGetVSCapabilities(PyObject *self) {
  unsigned int err;
  int NumSpeeds, NumAmplitudes;
  PyObject *v, *speed, *amplitudes;
  float tempSpeed;
  //float speed[MAX_VS]={0}, recommended={0};
  int i;
  err= GetNumberVSSpeeds(&NumSpeeds); N(err)
  speed = PyList_New(NumSpeeds);
  for (i=0; i < NumSpeeds; ++i) {
    err= GetVSSpeed(i, &tempSpeed); N(err)
    PyList_SetItem(speed, i, PyFloat_FromDouble((double)tempSpeed));
  }
  err= GetFastestRecommendedVSSpeed(&i, &tempSpeed); N(err)
  err= GetNumberVSAmplitudes(&NumAmplitudes);
  if (err == DRV_NOT_AVAILABLE) {
    Py_INCREF(Py_None);
    amplitudes = Py_None;
  }
  else {
    if (err != DRV_SUCCESS) return NULL;
    else amplitudes = PyFloat_FromDouble((double)NumAmplitudes);
  }
  v = PyDict_New();
  PyDict_SetItem(v, PyUnicode_FromString("VSSpeeds"), speed);
  PyDict_SetItem(v, PyUnicode_FromString("OkayWithoutVSAmp"),
                 PyFloat_FromDouble((double)tempSpeed));
  PyDict_SetItem(v, PyUnicode_FromString("VSAmplitudes"),
                 amplitudes);
  return v;
}
//unsigned int ShutDown();
static PyObject*
PyShutDown(PyObject *self,
           PyObject *args) {
  unsigned int err;
  at_32 cameraHandle = 0;
  PyArg_ParseTuple(args, "|i",  &cameraHandle);
  if (cameraHandle) {
    err= SetCurrentCamera(cameraHandle); N(err)
  }
  err= ShutDown(); N(err)
  Py_INCREF(Py_None);
  return Py_None;
}
//unsigned int GetVersionInfo(AT_VersionInfoId arr, char * szVersionInfo, at_u32 ui32BufferLen);
static PyObject*
PyGetVersionInfo(PyObject *self) {
  unsigned int err;
  PyObject *v;
  AT_VersionInfoId ver;
  char VersionInfo[256];
  at_u32 size_t = sizeof(VersionInfo);
  err= GetVersionInfo(ver, VersionInfo, size_t);
  if (err != DRV_SUCCESS) {
    fprintf(stderr, "Error %u instead of DRV_SUCCESS at %s/%s:%d\n", \
                  err, __FILE__, __FUNCTION__, __LINE__); \
    Py_INCREF(Py_None);
    return Py_None;
  }
  v = PyDict_New();/*
  PyDict_SetItem(v, PyUnicode_FromString("SDKVersion"),
                 PyLong_FromLong((long)ver->AT_SDKVersion));
  PyDict_SetItem(v, PyUnicode_FromString("DeviceDriverVersion"),
                 PyLong_FromLong((long)ver->AT_DeviceDriverVersion)); */
  PyDict_SetItem(v, PyUnicode_FromString("VersionInfo"),
                 PyUnicode_FromString(VersionInfo));
  return v;
}

/* --- Method table listing method names and addresses --------------------- */
static PyMethodDef AndorMethods[] = {
  {"GetCameraHandles",  (PyCFunction)PyGetCameraHandles, METH_NOARGS,
   "Get camera handles. Required for multiple cameras"},
  {"Initialize",  (PyCFunction)PyInitialize, METH_VARARGS,
   "Initialize current camera or provided camera handle"},
  {"SetCurrentCamera",  (PyCFunction)PySetCurrentCamera, METH_VARARGS,
   "When there are multiple cameras, select active camera by handle"},
  {"GetHeadModel",  (PyCFunction)PyGetHeadModel, METH_NOARGS,
   "Get head model string"},
  {"GetCameraSerialNumber",  (PyCFunction)PyGetCameraSerialNumber, METH_NOARGS,
   "Get head serial number"},
  {"GetCapabilities",  (PyCFunction)PyGetCapabilities, METH_NOARGS,
   "Camera supported modes"},
  {"IsInternalMechanicalShutter",  (PyCFunction)PyIsInternalMechanicalShutter, METH_NOARGS,
   "Internal Shutter Available?"},
  {"GetReadoutCapabilities",  (PyCFunction)PyGetReadoutCapabilities, METH_NOARGS,
   "Cameras supported Readout modes"},
  {"GetVSCapabilities",  (PyCFunction)PyGetVSCapabilities, METH_NOARGS,
   "Cameras supported Vertical shift modes"},
  {"GetCameraHandles",  (PyCFunction)PyGetCameraHandles, METH_NOARGS,
   "Get camera handles. Required for multiple cameras"},
  {"ShutDown",  (PyCFunction)PyShutDown, METH_VARARGS,
   "ShutDown current camera or provided camera handle"},
  {"GetVersionInfo",  (PyCFunction)PyGetVersionInfo, METH_NOARGS,
   "Get SDK and driver versions"},
  {NULL, NULL, 0, NULL} /* sentinel */
};

/* --- Module definition --------------------------------------------------- */
static struct PyModuleDef pyandormodule = {
  PyModuleDef_HEAD_INIT,
  "pyandor", /* name of module */
  NULL,      /* module documentation, may be NULL */
  -1,        /* size of per-interpreter state of the module,
                or -1 if the module keeps state in global variables. */
  AndorMethods
};

/* --- Module initialization ----------------------------------------------- */
PyMODINIT_FUNC
PyInit_pyandor(void) {
  PyObject *m;

  m = PyModule_Create(&pyandormodule);
  if (m == NULL) {
    return NULL; /* module cannot be initiailized satisfactorily */
  }

  /* boilerplate to handle exceptions */
  AndorError = PyErr_NewException("pyandor.error", NULL, NULL);
  Py_INCREF(AndorError);
  PyModule_AddObject(m, "error", AndorError);
  return m;
}
