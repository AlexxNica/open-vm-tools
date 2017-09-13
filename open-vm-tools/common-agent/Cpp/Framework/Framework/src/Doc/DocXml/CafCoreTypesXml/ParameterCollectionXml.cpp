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
#include "ParameterCollectionXml.h"

using namespace Caf;

void ParameterCollectionXml::add(
	const SmartPtrCParameterCollectionDoc parameterCollectionDoc,
	const SmartPtrCXmlElement thisXml) {
	CAF_CM_STATIC_FUNC_VALIDATE("ParameterCollectionXml", "add");

	CAF_CM_ENTER {
		CAF_CM_VALIDATE_SMARTPTR(parameterCollectionDoc);
		CAF_CM_VALIDATE_SMARTPTR(thisXml);

		const std::deque<SmartPtrCRequestParameterDoc> parameterVal =
			parameterCollectionDoc->getParameter();
		if (! parameterVal.empty()) {
			for (TConstIterator<std::deque<SmartPtrCRequestParameterDoc> > parameterIter(parameterVal);
				parameterIter; parameterIter++) {
				const SmartPtrCXmlElement parameterXml =
					thisXml->createAndAddElement("parameter");
				RequestParameterXml::add(*parameterIter, parameterXml);
			}
		}

		const std::deque<SmartPtrCRequestInstanceParameterDoc> instanceParameterVal =
			parameterCollectionDoc->getInstanceParameter();
		if (! instanceParameterVal.empty()) {
			for (TConstIterator<std::deque<SmartPtrCRequestInstanceParameterDoc> > instanceParameterIter(instanceParameterVal);
				instanceParameterIter; instanceParameterIter++) {
				const SmartPtrCXmlElement instanceParameterXml =
					thisXml->createAndAddElement("instanceParameter");
				RequestInstanceParameterXml::add(*instanceParameterIter, instanceParameterXml);
			}
		}
	}
	CAF_CM_EXIT;
}

SmartPtrCParameterCollectionDoc ParameterCollectionXml::parse(
	const SmartPtrCXmlElement thisXml) {
	CAF_CM_STATIC_FUNC_VALIDATE("ParameterCollectionXml", "parse");

	SmartPtrCParameterCollectionDoc parameterCollectionDoc;

	CAF_CM_ENTER {
		CAF_CM_VALIDATE_SMARTPTR(thisXml);

		const CXmlElement::SmartPtrCElementCollection parameterChildrenXml =
			thisXml->findOptionalChildren("parameter");

		std::deque<SmartPtrCRequestParameterDoc> parameterVal;
		if (! parameterChildrenXml.IsNull() && ! parameterChildrenXml->empty()) {
			for (TConstIterator<CXmlElement::CElementCollection> parameterXmlIter(*parameterChildrenXml);
				parameterXmlIter; parameterXmlIter++) {
				const SmartPtrCXmlElement parameterXml = parameterXmlIter->second;
				const SmartPtrCRequestParameterDoc parameterDoc =
					RequestParameterXml::parse(parameterXml);
				parameterVal.push_back(parameterDoc);
			}
		}

		const CXmlElement::SmartPtrCElementCollection instanceParameterChildrenXml =
			thisXml->findOptionalChildren("instanceParameter");

		std::deque<SmartPtrCRequestInstanceParameterDoc> instanceParameterVal;
		if (! instanceParameterChildrenXml.IsNull() && ! instanceParameterChildrenXml->empty()) {
			for (TConstIterator<CXmlElement::CElementCollection> instanceParameterXmlIter(*instanceParameterChildrenXml);
				instanceParameterXmlIter; instanceParameterXmlIter++) {
				const SmartPtrCXmlElement instanceParameterXml = instanceParameterXmlIter->second;
				const SmartPtrCRequestInstanceParameterDoc instanceParameterDoc =
					RequestInstanceParameterXml::parse(instanceParameterXml);
				instanceParameterVal.push_back(instanceParameterDoc);
			}
		}

		parameterCollectionDoc.CreateInstance();
		parameterCollectionDoc->initialize(
			parameterVal,
			instanceParameterVal);
	}
	CAF_CM_EXIT;

	return parameterCollectionDoc;
}

