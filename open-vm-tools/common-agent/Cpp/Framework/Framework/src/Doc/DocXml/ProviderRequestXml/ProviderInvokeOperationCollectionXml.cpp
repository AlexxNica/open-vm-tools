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
#include "ProviderInvokeOperationCollectionXml.h"

using namespace Caf;

void ProviderInvokeOperationCollectionXml::add(
	const SmartPtrCProviderInvokeOperationCollectionDoc providerInvokeOperationCollectionDoc,
	const SmartPtrCXmlElement thisXml) {
	CAF_CM_STATIC_FUNC_VALIDATE("ProviderInvokeOperationCollectionXml", "add");

	CAF_CM_ENTER {
		CAF_CM_VALIDATE_SMARTPTR(providerInvokeOperationCollectionDoc);
		CAF_CM_VALIDATE_SMARTPTR(thisXml);

		const std::deque<SmartPtrCProviderInvokeOperationDoc> invokeOperationVal =
			providerInvokeOperationCollectionDoc->getInvokeOperation();
		CAF_CM_VALIDATE_STL(invokeOperationVal);

		if (! invokeOperationVal.empty()) {
			for (TConstIterator<std::deque<SmartPtrCProviderInvokeOperationDoc> > invokeOperationIter(invokeOperationVal);
				invokeOperationIter; invokeOperationIter++) {
				const SmartPtrCXmlElement invokeOperationXml =
					thisXml->createAndAddElement("invokeOperation");
				ProviderInvokeOperationXml::add(*invokeOperationIter, invokeOperationXml);
			}
		}
	}
	CAF_CM_EXIT;
}

SmartPtrCProviderInvokeOperationCollectionDoc ProviderInvokeOperationCollectionXml::parse(
	const SmartPtrCXmlElement thisXml) {
	CAF_CM_STATIC_FUNC_VALIDATE("ProviderInvokeOperationCollectionXml", "parse");

	SmartPtrCProviderInvokeOperationCollectionDoc providerInvokeOperationCollectionDoc;

	CAF_CM_ENTER {
		CAF_CM_VALIDATE_SMARTPTR(thisXml);

		const CXmlElement::SmartPtrCElementCollection invokeOperationChildrenXml =
			thisXml->findRequiredChildren("invokeOperation");

		std::deque<SmartPtrCProviderInvokeOperationDoc> invokeOperationVal;
		if (! invokeOperationChildrenXml.IsNull() && ! invokeOperationChildrenXml->empty()) {
			for (TConstIterator<CXmlElement::CElementCollection> invokeOperationXmlIter(*invokeOperationChildrenXml);
				invokeOperationXmlIter; invokeOperationXmlIter++) {
				const SmartPtrCXmlElement invokeOperationXml = invokeOperationXmlIter->second;
				const SmartPtrCProviderInvokeOperationDoc invokeOperationDoc =
					ProviderInvokeOperationXml::parse(invokeOperationXml);
				invokeOperationVal.push_back(invokeOperationDoc);
			}
		}

		providerInvokeOperationCollectionDoc.CreateInstance();
		providerInvokeOperationCollectionDoc->initialize(
			invokeOperationVal);
	}
	CAF_CM_EXIT;

	return providerInvokeOperationCollectionDoc;
}

