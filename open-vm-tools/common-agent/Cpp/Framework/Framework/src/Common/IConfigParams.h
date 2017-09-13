/*
 *	 Author: mdonahue
 *  Created: Jan 17, 2011
 *
 *	Copyright (c) 2011 Vmware, Inc.  All rights reserved.
 *	-- VMware Confidential
 */

#ifndef ICONFIGPARAMS_H_
#define ICONFIGPARAMS_H_

namespace Caf {

struct __declspec(novtable)
IConfigParams : public ICafObject
{
	typedef enum {
		PARAM_REQUIRED,
		PARAM_OPTIONAL
	} EParamDisposition;

	virtual GVariant* lookup(
		const char* key,
		const EParamDisposition disposition = PARAM_REQUIRED) const = 0;

	virtual std::string getSectionName() const = 0;

	virtual void insert(const char* key, GVariant* value) = 0;
};

CAF_DECLARE_SMART_INTERFACE_POINTER(IConfigParams);

}

#endif /* ICONFIGPARAMS_H_ */
