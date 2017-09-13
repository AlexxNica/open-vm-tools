/*
 *  Author: bwilliams
 *  Created: April 6, 2012
 *
 *  Copyright (c) 2012 Vmware, Inc.  All rights reserved.
 *  -- VMware Confidential
 *
 *  This code was generated by the script "build/dev/codeGen/genCppXml". Please
 *  speak to Brian W. before modifying it by hand.
 *
 */

#include "stdafx.h"
#include "GetInventoryJobXml.h"

using namespace Caf;

void GetInventoryJobXml::add(
	const SmartPtrCGetInventoryJobDoc getInventoryJobDoc,
	const SmartPtrCXmlElement thisXml) {
	CAF_CM_STATIC_FUNC_VALIDATE("GetInventoryJobXml", "add");

	CAF_CM_ENTER {
		CAF_CM_VALIDATE_SMARTPTR(getInventoryJobDoc);
		CAF_CM_VALIDATE_SMARTPTR(thisXml);

		const std::string jobIdVal =
			BasePlatform::UuidToString(getInventoryJobDoc->getJobId());
		CAF_CM_VALIDATE_STRING(jobIdVal);
		thisXml->addAttribute("jobId", jobIdVal);
	}
	CAF_CM_EXIT;
}

SmartPtrCGetInventoryJobDoc GetInventoryJobXml::parse(
	const SmartPtrCXmlElement thisXml) {
	CAF_CM_STATIC_FUNC_VALIDATE("GetInventoryJobXml", "parse");

	SmartPtrCGetInventoryJobDoc getInventoryJobDoc;

	CAF_CM_ENTER {
		CAF_CM_VALIDATE_SMARTPTR(thisXml);

		const std::string jobIdStrVal =
			thisXml->findRequiredAttribute("jobId");
		UUID jobIdVal = CAFCOMMON_GUID_NULL;
		if (! jobIdStrVal.empty()) {
			BasePlatform::UuidFromString(jobIdStrVal.c_str(), jobIdVal);
		}

		getInventoryJobDoc.CreateInstance();
		getInventoryJobDoc->initialize(
			jobIdVal);
	}
	CAF_CM_EXIT;

	return getInventoryJobDoc;
}

