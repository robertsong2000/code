/*
 * Copyright (C) 2000, 2001 Robert Fitzsimons
 *
 * This file is subject to the terms and conditions of the GNU General
 * Public License.  See the file "COPYING" in the main directory of
 * this archive for more details.
 */

#ifndef _kimplementation_h
#define _kimplementation_h

#define KERNEL_IMPLEMENTATION_VERSION 0

#include <stdarg.h>

#define KNULL		((void*)0)
#define KPAGESIZE	(4096)
#define kfunction

typedef signed long int		kint;
typedef signed char		kint8;
typedef signed short int	kint16;
typedef signed long int		kint32;
typedef signed long long int	kint64;

typedef unsigned long int	kuint;
typedef unsigned char		kuint8;
typedef unsigned short int	kuint16;
typedef unsigned long int	kuint32;
typedef unsigned long long int	kuint64;

typedef long double		kfloat;
typedef float			kfloat32;
typedef double			kfloat64;

typedef struct kmutex kmutex;
typedef struct kcondition kcondition;

#endif

