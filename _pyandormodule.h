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
#include "atmcdLXd.h"

/* --- Macros -------------------------------------------------------------- */
#define HANDLE(err) if (err != DRV_SUCCESS) { \
  fprintf(stderr, "Error %u instead of DRV_SUCCESS at %s/%s:%d\n", \
                  err, __FILE__, __FUNCTION__, __LINE__-2); \
  return NULL; \
}
#define CHK if (self->Status != DRV_SUCCESS) { \
  fprintf(stderr, "Error %u instead of DRV_SUCCESS at %s/%s:%d\n", \
                  err, __FILE__, __FUNCTION__, __LINE__-2); \
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
    /* Type-specific fields go here. */
} Camera;

typedef struct {
  PyObject_HEAD
  /* Type-specific fields */
  at_32 *_handle;       /* session-unique camera handle */
  PyObject *_model;     /* head model tuple with
                         *   (name=, head_serial=, pixels_width=,
                         *    pixels_height=) */
  float *exposure_time;
  unsigned long int
    *_image_size_bytes; /* module calculated length of required buffer size
                         *   in bytes to store data for one frame.  This will
                         *   be affected by the image size, binning, and
                         *   whether metadata is appended to the data stream */
  PyObject *image;      /* list of [width=, height=, x=, y=, bin_x=, bin_y=] */
  PyObject *readout_mode;          /* option(s) = {digitizer_bit_depth + 
                                    *              amplifier_type +
                                    *              horizontal speed +
                                    *              post_readout_gain} */
  PyObject *readout_vertical_speed; /* option(s) = {0.45us <, etc>} */
  PyObject *shutter_state;          /* options = {open, close, auto} */
  PyObject *shutter_trigger_mode;   /* options = {internal, external} */
  PyObject *shutter_timing;         /* list of [opening=, closing=] */
  PyObject *camera_trigger_mode;    /* options = {internal, external} */
  float *delay_after_readout;
  float *cooling_temperature_target;
  float *cooling_temperature_actual;
  unsigned long int *images_to_acquire;
  float *_time_readout; /* total time for readout stage only */
  float *_frame_rate;   /* total time for capture and delays */

  unsigned int _status;
} Camera;

static void
Camera_dealloc(Camera* self) {
  Py_XDECREF(self->_handle);
  Py_XDECREF(self->_model);
  Py_XDECREF(self->binning);
  Py_XDECREF(self->_image_size_bytes);
  Py_XDECREF(self->readout_mode);
  Py_XDECREF(self->readout_vertical_speed);
  Py_XDECREF(self->shutter_state);
  Py_XDECREF(self->shutter_trigger_mode);
  Py_XDECREF(self->shutter_timing);
  Py_XDECREF(self->camera_trigger_mode);
  Py_XDECREF(self->delay_after_readout);
  Py_XDECREF(self->cooling_temperature_target);
  Py_XDECREF(self->cooling_temperature_actual);
  Py_XDECREF(self->images_to_acquire);
  Py_XDECREF(self->_time_readout);
  Py_XDECREF(self->_frame_rate);
  Py_XDECREF(self->_status);
  Py_TYPE(self)->tp_free((PyObject*)self);
}

static int
Camera_init(Camera *self, PyObject *args, PyObject *kwds) {
  PyObject *_user_handle=NULL;
  static char *kwlist[] = {"_handle", NULL};
  if (! PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, 
                                    &_user_handle))
    return -1; 

  /* TODO: would be nice to have a way to match serial number to camera.
   *       e.g. Confocal = Camera(HeadSerial=3193)
   *       ...but this would probably need some juggling of handles.
   */
  //if(_handle != NULL) {
  //  self->_status = SetCameraHandle(_handle); CHK
  //}
  //else
  self->_status = GetCurrentCamera(self->_handle); CHK
  self->_status = Initialize(ETC_DIR); CHK
  self->_model = PyDict_New();
  char name[MAX_PATH];
  self->_status = GetHeadModel(name); CHK
  PyDict_SetItem(self->_model, PyUnicode_FromString("name"),
                 PyUnicode_FromString(Py_BuildValue("z#", &name, MAX_PATH));
  Py_DECREF(name);
  int *number;
  self->_status = GetCameraSerialNumber(number); CHK
  PyDict_SetItem(self->_model, PyUnicode_FromString("head_serial"),
                 PyLong_FromLong((long)number);
  Py_DECREF(number);
  int *xpixels, *ypixels;
  self->_status = GetDetector(xpixels, ypixels); CHK
  PyDict_SetItem(self->_model, PyUnicode_FromString("pixels_width"),
                 PyLong_FromLong((long)xpixels);
  Py_DECREF(xpixels);
  PyDict_SetItem(self->_model, PyUnicode_FromString("pixels_height"),
                 PyLong_FromLong((long)ypixels);
  Py_DECREF(ypixels);
  int *mintemp, *maxtemp;
  self->_status = GetTemperatureRange(mintemp, maxtemp); CHK
  if (mintemp == maxtemp) { /* Temperature is not user editable */
    printf("Temperature is not user editable");
  }
  Py_DECREF(mintemp); Py_DECREF(maxtemp);
  float *devnull1, *devnull2;
  self->_status = GetAcquisitionTimings(self->exposure_time, devnull1,
                                        devnull2); CHK
  Py_DECREF(devnull1); Py_DECREF(devnull2);
  self->_status = GetReadOutTime(self->_time_readout); CHK
  self->image = PyDict_New();
  PyDict_SetItem(self->image, PyUnicode_FromString("width"),
                 PyDict_GetItem(self->_model, pixels_width));
  PyDict_SetItem(self->image, PyUnicode_FromString("height"),
                 PyDict_GetItem(self->_model, pixels_height));
  PyDict_SetItem(self->image, PyUnicode_FromString("x"),
                 PyLong_FromLong(1);
  PyDict_SetItem(self->image, PyUnicode_FromString("y"),
                 PyLong_FromLong(1);
  PyDict_SetItem(self->image, PyUnicode_FromString("bin_x"),
                 PyLong_FromLong(1);
  PyDict_SetItem(self->image, PyUnicode_FromString("bin_y"),
                 PyLong_FromLong(1);
  return 0;
}

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
  "pyandor Camera object",      /* tp_doc */
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
  0,                            /* tp_new */
};

/* --- Exception name unique to this module -------------------------------- */
static PyObject* AndorError;
/* TODO: set exceptions for the functions and map the return codes
 *   and meanings to the exception message. */

#endif /* define __PYANDOR_H__ */
