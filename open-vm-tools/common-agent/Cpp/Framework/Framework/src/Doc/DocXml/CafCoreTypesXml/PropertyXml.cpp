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
#include "PropertyXml.h"

using namespace Caf;

void PropertyXml::add(
	const SmartPtrCPropertyDoc propertyDoc,
	const SmartPtrCXmlElement thisXml) {
	CAF_CM_STATIC_FUNC_VALIDATE("PropertyXml", "add");

	CAF_CM_ENTER {
		CAF_CM_VALIDATE_SMARTPTR(propertyDoc);
		CAF_CM_VALIDATE_SMARTPTR(thisXml);

		const std::string nameVal = propertyDoc->getName();
		CAF_CM_VALIDATE_STRING(nameVal);
		thisXml->addAttribute("name", nameVal);

		const std::string typeVal =
			EnumConvertersXml::convertPropertyTypeToString(propertyDoc->getType());
		CAF_CM_VALIDATE_STRING(typeVal);
		thisXml->addAttribute("type", typeVal);

		const std::deque<std::string> valueVal =
			propertyDoc->getValue();
		CAF_CM_VALIDATE_STL(valueVal);

		if (! valueVal.empty()) {
			for (TConstIterator<std::deque<std::string> > valueIter(valueVal);
				valueIter; valueIter++) {
				const SmartPtrCXmlElement valueXml =
					thisXml->createAndAddElement("value");
				valueXml->setValue(*valueIter);
			}
		}
	}
	CAF_CM_EXIT;
}

SmartPtrCPropertyDoc PropertyXml::parse(
	const SmartPtrCXmlElement thisXml) {
	CAF_CM_STATIC_FUNC_VALIDATE("PropertyXml", "parse");

	SmartPtrCPropertyDoc propertyDoc;

	CAF_CM_ENTER {
		CAF_CM_VALIDATE_SMARTPTR(thisXml);

		const std::string nameStrVal =
			thisXml->findRequiredAttribute("name");
		const std::string nameVal = nameStrVal;

		const std::string typeStrVal =
			thisXml->findRequiredAttribute("type");
		PROPERTY_TYPE typeVal = PROPERTY_NONE;
		if (! typeStrVal.empty()) {
			typeVal = EnumConvertersXml::convertStringToPropertyType(typeStrVal);
		}

		const CXmlElement::SmartPtrCElementCollection valueChildrenXml =
			thisXml->findRequiredChildren("value");

		std::deque<std::string> valueVal;
		if (! valueChildrenXml.IsNull() && ! valueChildrenXml->empty()) {
			for (TConstIterator<CXmlElement::CElementCollection> valueXmlIter(*valueChildrenXml);
				valueXmlIter; valueXmlIter++) {
				const SmartPtrCXmlElement valueXml = valueXmlIter->second;
				const std::string valueDoc = valueXml->getValue();
				valueVal.push_back(valueDoc);
			}
		}

		propertyDoc.CreateInstance();
		propertyDoc->initialize(
			nameVal,
			typeVal,
			valueVal);
	}
	CAF_CM_EXIT;

	return propertyDoc;
}

