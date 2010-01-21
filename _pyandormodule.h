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

/* --- Type objects -------------------------------------------------------- */
typedef struct {
    PyObject_HEAD
    /* Type-specific fields go here. */
} Camera;

static PyTypeObject CameraType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "pyandor.Camera",             /* tp_name */
    sizeof(Camera),               /* tp_basicsize */
    0,                            /* tp_itemsize */
    0,                            /* tp_dealloc */
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
    Py_TPFLAGS_DEFAULT,           /* tp_flags */
    "pyandor Camera object",      /* tp_doc */
};

/* --- Macros -------------------------------------------------------------- */
#define HANDLE(err) if (err != DRV_SUCCESS) { \
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

/* --- Exception name unique to this module -------------------------------- */
static PyObject* AndorError;
/* TODO: set exceptions for the functions and map the return codes
 *   and meanings to the exception message. */

#endif /* def __PYANDOR_H__ */
