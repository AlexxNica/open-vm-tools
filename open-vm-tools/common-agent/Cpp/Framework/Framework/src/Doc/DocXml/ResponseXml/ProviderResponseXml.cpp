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
#include "ProviderResponseXml.h"

using namespace Caf;

void ProviderResponseXml::add(
	const SmartPtrCProviderResponseDoc providerResponseDoc,
	const SmartPtrCXmlElement thisXml) {
	CAF_CM_STATIC_FUNC_VALIDATE("ProviderResponseXml", "add");

	CAF_CM_ENTER {
		CAF_CM_VALIDATE_SMARTPTR(providerResponseDoc);
		CAF_CM_VALIDATE_SMARTPTR(thisXml);

		const std::string clientIdVal =
			BasePlatform::UuidToString(providerResponseDoc->getClientId());
		if (! clientIdVal.empty()) {
			thisXml->addAttribute("clientId", clientIdVal);
		}

		const std::string requestIdVal =
			BasePlatform::UuidToString(providerResponseDoc->getRequestId());
		if (! requestIdVal.empty()) {
			thisXml->addAttribute("requestId", requestIdVal);
		}

		const std::string pmeIdVal = providerResponseDoc->getPmeId();
		if (! pmeIdVal.empty()) {
			thisXml->addAttribute("pmeId", pmeIdVal);
		}

		const SmartPtrCResponseHeaderDoc responseHeaderVal =
			providerResponseDoc->getResponseHeader();
		if (! responseHeaderVal.IsNull()) {
			const SmartPtrCXmlElement responseHeaderXml =
				thisXml->createAndAddElement("responseHeader");
			ResponseHeaderXml::add(responseHeaderVal, responseHeaderXml);
		}

		const SmartPtrCManifestDoc manifestVal =
			providerResponseDoc->getManifest();
		if (! manifestVal.IsNull()) {
			const SmartPtrCXmlElement manifestXml =
				thisXml->createAndAddElement("manifest");
			ManifestXml::add(manifestVal, manifestXml);
		}

		const SmartPtrCAttachmentCollectionDoc attachmentCollectionVal =
			providerResponseDoc->getAttachmentCollection();
		if (! attachmentCollectionVal.IsNull()) {
			const SmartPtrCXmlElement attachmentCollectionXml =
				thisXml->createAndAddElement("attachmentCollection");
			AttachmentCollectionXml::add(attachmentCollectionVal, attachmentCollectionXml);
		}

		const SmartPtrCStatisticsDoc statisticsVal =
			providerResponseDoc->getStatistics();
		if (! statisticsVal.IsNull()) {
			const SmartPtrCXmlElement statisticsXml =
				thisXml->createAndAddElement("statistics");
			StatisticsXml::add(statisticsVal, statisticsXml);
		}
	}
	CAF_CM_EXIT;
}

SmartPtrCProviderResponseDoc ProviderResponseXml::parse(
	const SmartPtrCXmlElement thisXml) {
	CAF_CM_STATIC_FUNC_VALIDATE("ProviderResponseXml", "parse");

	SmartPtrCProviderResponseDoc providerResponseDoc;

	CAF_CM_ENTER {
		CAF_CM_VALIDATE_SMARTPTR(thisXml);

		const std::string clientIdStrVal =
			thisXml->findOptionalAttribute("clientId");
		UUID clientIdVal = CAFCOMMON_GUID_NULL;
		if (! clientIdStrVal.empty()) {
			BasePlatform::UuidFromString(clientIdStrVal.c_str(), clientIdVal);
		}

		const std::string requestIdStrVal =
			thisXml->findOptionalAttribute("requestId");
		UUID requestIdVal = CAFCOMMON_GUID_NULL;
		if (! requestIdStrVal.empty()) {
			BasePlatform::UuidFromString(requestIdStrVal.c_str(), requestIdVal);
		}

		const std::string pmeIdVal =
			thisXml->findOptionalAttribute("pmeId");

		const SmartPtrCXmlElement responseHeaderXml =
			thisXml->findOptionalChild("responseHeader");
		SmartPtrCResponseHeaderDoc responseHeaderVal;
		if (! responseHeaderXml.IsNull()) {
			responseHeaderVal = ResponseHeaderXml::parse(responseHeaderXml);
		}

		const SmartPtrCXmlElement manifestXml =
			thisXml->findOptionalChild("manifest");
		SmartPtrCManifestDoc manifestVal;
		if (! manifestXml.IsNull()) {
			manifestVal = ManifestXml::parse(manifestXml);
		}

		const SmartPtrCXmlElement attachmentCollectionXml =
			thisXml->findOptionalChild("attachmentCollection");
		SmartPtrCAttachmentCollectionDoc attachmentCollectionVal;
		if (! attachmentCollectionXml.IsNull()) {
			attachmentCollectionVal = AttachmentCollectionXml::parse(attachmentCollectionXml);
		}

		const SmartPtrCXmlElement statisticsXml =
			thisXml->findOptionalChild("statistics");
		SmartPtrCStatisticsDoc statisticsVal;
		if (! statisticsXml.IsNull()) {
			statisticsVal = StatisticsXml::parse(statisticsXml);
		}

		providerResponseDoc.CreateInstance();
		providerResponseDoc->initialize(
			clientIdVal,
			requestIdVal,
			pmeIdVal,
			responseHeaderVal,
			manifestVal,
			attachmentCollectionVal,
			statisticsVal);
	}
	CAF_CM_EXIT;

	return providerResponseDoc;
}

