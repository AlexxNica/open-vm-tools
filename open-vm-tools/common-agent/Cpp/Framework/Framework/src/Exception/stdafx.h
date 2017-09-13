/*
 *	 Author: mdonahue
 *  Created: Jan 13, 2011
 *
 *	Copyright (c) 2011 Vmware, Inc.  All rights reserved.
 *	-- VMware Confidential
 */

#ifndef STDAFX_H_
#define STDAFX_H_

#ifdef WIN32
	#define EXCEPTION_LINKAGE __declspec(dllexport)
	#define COMMONAGGREGATOR_LINKAGE __declspec(dllexport)
#else
	#define EXCEPTION_LINKAGE
	#define COMMONAGGREGATOR_LINKAGE
#endif

#include <BaseDefines.h>
#include "../Globals/CommonGlobalsLink.h"
#include "CCafException.h"
#include "ClassMacros.h"
#include "ExceptionMacros.h"

#endif /* STDAFX_H_ */
