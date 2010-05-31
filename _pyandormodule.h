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

#ifndef __PYANDOR_H__
#define __PYANDOR_H__

#include <Python.h>
#include "structmember.h"
#include "atmcdLXd.h"

#ifdef _LP64
#define T_AT_32 T_INT
#define T_AT_u32 T_UINT
#else
#define T_AT_32 T_LONG
#define T_AT_u32 T_ULONG
#endif /* #ifdef _LP64 */

/* --- Macros -------------------------------------------------------------- */
#define HANDLE(err) if (err != DRV_SUCCESS) { \
  fprintf(stderr, "Return code %u instead of DRV_SUCCESS at %s/%s:%d\n", \
                  err, __FILE__, __FUNCTION__, __LINE__); \
  return NULL; \
}
#define CHK if (self->_status != DRV_SUCCESS) { \
  fprintf(stderr, "Return code %u instead of DRV_SUCCESS at %s/%s:%d\n", \
                  self->_status, __FILE__, __FUNCTION__, __LINE__); \
  return NULL; \
}
#define ETC_DIR "/usr/local/etc/andor/"
#define MAX_PATH 12 /* TODO: `12' was found be trial and error.
                     *   What should MAX_PATH actually be?
                     *   The PDF manual function description mentions
                     *   MAX_PATH constant, but it does not exist in
                     *   atmcdLXd.h */

/* --- Type objects -------------------------------------------------------- */
typedef struct {
  PyObject_HEAD
  at_32 _handle;
  PyObject *_model;
  float exposure_time;
  unsigned long _image_size_bytes;
  PyObject *active_chip_window;
  PyObject *readout_mode;
  PyObject *readout_vertical_speed;
  PyObject *shutter_state;
  PyObject *shutter_trigger_mode;
  PyObject *shutter_timing;
  PyObject *camera_trigger_mode;
  float delay_after_readout;
  float cooling_temperature_target;
  float cooling_temperature_actual;
  unsigned long images_to_acquire;
  float _time_readout;
  float _frame_rate;
  unsigned int _status;
} Camera;

static PyMemberDef Camera_members[] = {
  {"_handle", T_AT_32, offsetof(Camera, _handle), 0,
   "Session unique camera handle"},
  {"_model", T_OBJECT_EX, offsetof(Camera, _model), 0,
   "Head model tuple with (name=, head_serial=, pixels_width=, \
    pixels_height=)"},
  {"exposure_time", T_FLOAT, offsetof(Camera, exposure_time), 0,
   "Exposure time"},
  {"_image_size_bytes", T_ULONG, offsetof(Camera, _image_size_bytes), 0,
   "Calculated length of required buffer size in bytes to store data \
    for one frame.  This will be affected by the image size, binning, and \
    whether metadata is appended to the data stream"},
  {"active_chip_window", T_OBJECT_EX, offsetof(Camera, active_chip_window), 0,
   "Sensor configuration of image pixels to read [width=, height=, \
    x_offset=, y_offset=, bin_x=, bin_y=]"},
  {"readout_mode", T_OBJECT_EX, offsetof(Camera, readout_mode), 0,
   "Sensor readout compound option(s) of {digitizer_bit_depth + \
    amplifier_type + horizontal speed + post_readout_gain}"},
  {"readout_vertical_speed", T_OBJECT_EX, offsetof(Camera, 
                                                   readout_vertical_speed), 0,
   "Sensor vertical clock shift speed option(s) of {shift time #1 in us \
    <, etc>}"},
  {"shutter_state", T_OBJECT_EX, offsetof(Camera, shutter_state), 0,
   "Internal or External shutter state options of {open, close, auto}"},
  {"shutter_trigger_mode", T_OBJECT_EX, offsetof(Camera,
                                                 shutter_trigger_mode), 0,
   "Select shutter control options of {internal, external}"},
  {"shutter_timing", T_OBJECT_EX, offsetof(Camera, shutter_timing), 0,
   "Set manufacturer's specification for shutter [opening=, closing=]"},
  {"camera_trigger_mode", T_OBJECT_EX, offsetof(Camera,
                                                camera_trigger_mode), 0,
   "Acquire by triggering camera {internal, external}"},
  {"delay_after_readout", T_FLOAT, offsetof(Camera, delay_after_readout), 0,
   "Delay after readout before the next acquisition in a series"},
  {"cooling_temperature_target", T_FLOAT,
   offsetof(Camera, cooling_temperature_target), 0,
   "Target temperature for thermal electric cooler"},
  {"cooling_temperature_actual", T_FLOAT,
   offsetof(Camera, cooling_temperature_target), 0,
   "Sensor temperature"},
  {"images_to_acquire", T_ULONG, offsetof(Camera, images_to_acquire), 0,
   "Total number of acquisitions to be carried out in a series"},
  {"_time_readout", T_FLOAT, offsetof(Camera, _time_readout), 0,
   "Calculated readout time from image settings, readout mode, vertical \
    shift speed, and (if applicable) readout overlapping"},
  {"_frame_rate", T_FLOAT, offsetof(Camera, _frame_rate), 0,
   "Calculated frame rate based on readout time, exposure and delays"},
  {"_status", T_UINT, offsetof(Camera, _status), 0,
   "Return code from last Andor SDK command"},
  {NULL} /* sentinel */
};

static void
Camera_dealloc(Camera* self) {
  Py_XDECREF(self->_model);
  Py_XDECREF(self->active_chip_window);
  Py_XDECREF(self->readout_mode);
  Py_XDECREF(self->readout_vertical_speed);
  Py_XDECREF(self->shutter_state);
  Py_XDECREF(self->shutter_trigger_mode);
  Py_XDECREF(self->shutter_timing);
  Py_XDECREF(self->camera_trigger_mode);
  Py_TYPE(self)->tp_free((PyObject*)self);
}

static PyObject *
Camera_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
  Camera *self;
  
  //self = (Camera *)type->tp_alloc(type,0);
  self = (Camera *)PyType_GenericNew(type, args, kwds);
  if (self != NULL) {
    self->_handle = -1;
    self->exposure_time = -1;
    self->delay_after_readout = -1;
    self->_time_readout = -1;
    self->_frame_rate = -1;
    self->cooling_temperature_target = -999;
    self->cooling_temperature_actual = -999;
    self->_image_size_bytes = 0;
    self->images_to_acquire = 0;
    self->_status = 0;
    self->_model = PyDict_New();
    if (self->_model == NULL)
      goto error;
    else {
      if (PyDict_SetItem(self->_model, PyUnicode_FromString("name"),
                         PyUnicode_FromString("")) != 0) goto error;
      if (PyDict_SetItem(self->_model, PyUnicode_FromString("head_serial"),
                         PyLong_FromLong(0)) != 0) goto error;
      if (PyDict_SetItem(self->_model, PyUnicode_FromString("pixels_width"),
                         PyLong_FromLong(0)) != 0) goto error;
      if (PyDict_SetItem(self->_model, PyUnicode_FromString("pixels_height"),
                         PyLong_FromLong(0)) != 0) goto error;
    }
    self->active_chip_window = PyDict_New();
    if (self->active_chip_window == NULL)
      goto error;
    else {
      if (PyDict_SetItem(self->active_chip_window,
                         PyUnicode_FromString("width"),
                         PyLong_FromLong(-1)) != 0) goto error;
      if (PyDict_SetItem(self->active_chip_window,
                         PyUnicode_FromString("height"),
                         PyLong_FromLong(-1)) != 0) goto error;
      if (PyDict_SetItem(self->active_chip_window,
                         PyUnicode_FromString("x_offset"),
                         PyLong_FromLong(-1)) != 0) goto error;
      if (PyDict_SetItem(self->active_chip_window,
                         PyUnicode_FromString("y_offset"),
                         PyLong_FromLong(-1)) != 0) goto error;
      if (PyDict_SetItem(self->active_chip_window,
                         PyUnicode_FromString("bin_x"),
                         PyLong_FromLong(-1)) != 0) goto error;
      if (PyDict_SetItem(self->active_chip_window,
                         PyUnicode_FromString("bin_y"),
                         PyLong_FromLong(-1)) != 0) goto error;
    }
  }

  return (PyObject *)self;

  error:
    Py_DECREF(self);
    return NULL;
}

static int Camera_init(Camera *self, PyObject *args, PyObject *kwds);

/* Method address prototypes */
static PyObject* PyGetCameraHandles(PyObject *self);
static PyObject* PyShutDown(PyObject *self);

/* Method table listing method names and addresses */
static PyMethodDef Camera_methods[] = {
  {"GetCameraHandles",  (PyCFunction)PyGetCameraHandles, METH_NOARGS,
   "Get available camera handles"},
  {"ShutDown",  (PyCFunction)PyShutDown, METH_NOARGS,
   "ShutDown current camera (not necessary for USB cameras)"},
  {NULL, NULL, 0, NULL} /* sentinel */
};

static PyTypeObject CameraType = {
  PyVarObject_HEAD_INIT(NULL, 0)
  "pyandor.Camera",             /* tp_name */
  sizeof(Camera),               /* tp_basicsize */
  0,                            /* tp_itemsize */
  (destructor)Camera_dealloc,   /* tp_dealloc */
  0,                            /* tp_print */
  0,                            /* tp_getattr */
  0,                            /* tp_setattr */
  0,                            /* tp_reserved */
  0,                            /* tp_repr */
  0,                            /* tp_as_number */
  0,                            /* tp_as_sequence */
  0,                            /* tp_as_mapping */
  0,                            /* tp_hash  */
  0,                            /* tp_call */
  0,                            /* tp_str */
  0,                            /* tp_getattro */
  0,                            /* tp_setattro */
  0,                            /* tp_as_buffer */
  Py_TPFLAGS_DEFAULT |
    Py_TPFLAGS_BASETYPE,        /* tp_flags */
  "Andor Camera object",        /* tp_doc */
  0,		                        /* tp_traverse */
  0,		                        /* tp_clear */
  0,		                        /* tp_richcompare */
  0,		                        /* tp_weaklistoffset */
  0,		                        /* tp_iter */
  0,		                        /* tp_iternext */
  Camera_methods,               /* tp_methods */
  Camera_members,               /* tp_members */
  0,                            /* tp_getset */
  0,                            /* tp_base */
  0,                            /* tp_dict */
  0,                            /* tp_descr_get */
  0,                            /* tp_descr_set */
  0,                            /* tp_dictoffset */
  (initproc)Camera_init,        /* tp_init */
  0,                            /* tp_alloc */
  Camera_new,                   /* tp_new */
};

/* --- Exception name unique to this module -------------------------------- */
static PyObject* AndorError;
/* TODO: set exceptions for the functions and map the return codes
 *   and meanings to the exception message. */

#endif /* #ifndef __PYANDOR_H__ */
