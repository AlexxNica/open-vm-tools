/*
 *  Author: bwilliams
 *  Created: Nov 16, 2015
 *
 *  Copyright (c) 2015 Vmware, Inc.  All rights reserved.
 *  -- VMware Confidential
 *
 */

#include "stdafx.h"
#include "RemoteSecurityCollectionXml.h"

using namespace Caf;

void RemoteSecurityCollectionXml::add(
	const SmartPtrCRemoteSecurityCollectionDoc remoteSecurityCollectionDoc,
	const SmartPtrCXmlElement thisXml) {
	CAF_CM_STATIC_FUNC_VALIDATE("RemoteSecurityCollectionXml", "add");
	CAF_CM_VALIDATE_SMARTPTR(remoteSecurityCollectionDoc);
	CAF_CM_VALIDATE_SMARTPTR(thisXml);

	const std::deque<SmartPtrCRemoteSecurityDoc> remoteSecurityVal =
		remoteSecurityCollectionDoc->getRemoteSecurity();
	if (! remoteSecurityVal.empty()) {
		for (TConstIterator<std::deque<SmartPtrCRemoteSecurityDoc> > remoteSecurityIter(remoteSecurityVal);
			remoteSecurityIter; remoteSecurityIter++) {
			const SmartPtrCXmlElement remoteSecurityXml =
				thisXml->createAndAddElement("remoteSecurity");
			RemoteSecurityXml::add(*remoteSecurityIter, remoteSecurityXml);
		}
	}
}

SmartPtrCRemoteSecurityCollectionDoc RemoteSecurityCollectionXml::parse(
	const SmartPtrCXmlElement thisXml) {
	CAF_CM_STATIC_FUNC_VALIDATE("RemoteSecurityCollectionXml", "parse");
	CAF_CM_VALIDATE_SMARTPTR(thisXml);

	const CXmlElement::SmartPtrCElementCollection remoteSecurityChildrenXml =
		thisXml->findOptionalChildren("remoteSecurity");

	std::deque<SmartPtrCRemoteSecurityDoc> remoteSecurityVal;
	if (! remoteSecurityChildrenXml.IsNull() && ! remoteSecurityChildrenXml->empty()) {
		for (TConstIterator<CXmlElement::CElementCollection> remoteSecurityXmlIter(*remoteSecurityChildrenXml);
			remoteSecurityXmlIter; remoteSecurityXmlIter++) {
			const SmartPtrCXmlElement remoteSecurityXml = remoteSecurityXmlIter->second;
			const SmartPtrCRemoteSecurityDoc remoteSecurityDoc =
				RemoteSecurityXml::parse(remoteSecurityXml);
			remoteSecurityVal.push_back(remoteSecurityDoc);
		}
	}

	SmartPtrCRemoteSecurityCollectionDoc remoteSecurityCollectionDoc;
	remoteSecurityCollectionDoc.CreateInstance();
	remoteSecurityCollectionDoc->initialize(
		remoteSecurityVal);

	return remoteSecurityCollectionDoc;
}

