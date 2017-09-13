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
#include "OperationXml.h"

using namespace Caf;

void OperationXml::add(
	const SmartPtrCOperationDoc operationDoc,
	const SmartPtrCXmlElement thisXml) {
	CAF_CM_STATIC_FUNC_VALIDATE("OperationXml", "add");

	CAF_CM_ENTER {
		CAF_CM_VALIDATE_SMARTPTR(operationDoc);
		CAF_CM_VALIDATE_SMARTPTR(thisXml);

		const std::string nameVal = operationDoc->getName();
		CAF_CM_VALIDATE_STRING(nameVal);
		thisXml->addAttribute("name", nameVal);

		const SmartPtrCParameterCollectionDoc parameterCollectionVal =
			operationDoc->getParameterCollection();
		if (! parameterCollectionVal.IsNull()) {
			const SmartPtrCXmlElement parameterCollectionXml =
				thisXml->createAndAddElement("parameterCollection");
			ParameterCollectionXml::add(parameterCollectionVal, parameterCollectionXml);
		}
	}
	CAF_CM_EXIT;
}

SmartPtrCOperationDoc OperationXml::parse(
	const SmartPtrCXmlElement thisXml) {
	CAF_CM_STATIC_FUNC_VALIDATE("OperationXml", "parse");

	SmartPtrCOperationDoc operationDoc;

	CAF_CM_ENTER {
		CAF_CM_VALIDATE_SMARTPTR(thisXml);

		const std::string nameStrVal =
			thisXml->findRequiredAttribute("name");
		const std::string nameVal = nameStrVal;

		const SmartPtrCXmlElement parameterCollectionXml =
			thisXml->findOptionalChild("parameterCollection");

		SmartPtrCParameterCollectionDoc parameterCollectionVal;
		if (! parameterCollectionXml.IsNull()) {
			parameterCollectionVal = ParameterCollectionXml::parse(parameterCollectionXml);
		}

		operationDoc.CreateInstance();
		operationDoc->initialize(
			nameVal,
			parameterCollectionVal);
	}
	CAF_CM_EXIT;

	return operationDoc;
}

