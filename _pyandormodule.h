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
#define N(err) if (err != DRV_SUCCESS) { \
  fprintf(stderr, "Error %u instead of DRV_SUCCESS at %s/%s:%d\n", \
                  err, __FILE__, __FUNCTION__, __LINE__); \
  return NULL; \
}

/* --- Constants ----------------------------------------------------------- */
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
