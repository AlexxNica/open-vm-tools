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

using namespace Caf;

std::string XmlRoots::saveErrorResponseToString(
	const SmartPtrCErrorResponseDoc errorResponseDoc) {
	CAF_CM_STATIC_FUNC_VALIDATE("XmlRoots", "saveErrorResponseToString");

	std::string rc;

	CAF_CM_ENTER {
		CAF_CM_VALIDATE_SMARTPTR(errorResponseDoc);

		const std::string schemaNamespace = DocXmlUtils::getSchemaNamespace("fx");
		const std::string schemaLocation = DocXmlUtils::getSchemaLocation("fx/Response.xsd");

		const SmartPtrCXmlElement rootXml = CXmlUtils::createRootElement(
			"errorResponse", schemaNamespace, schemaLocation);
		ErrorResponseXml::add(errorResponseDoc, rootXml);

		rc = rootXml->saveToString();
	}
	CAF_CM_EXIT;

	return rc;
}

SmartPtrCErrorResponseDoc XmlRoots::parseErrorResponseFromString(
	const std::string xml) {
	CAF_CM_STATIC_FUNC_VALIDATE("XmlRoots", "parseErrorResponseFromString");

	SmartPtrCErrorResponseDoc errorResponseDoc;

	CAF_CM_ENTER {
		CAF_CM_VALIDATE_STRING(xml);

		const SmartPtrCXmlElement rootXml = CXmlUtils::parseString(xml, "caf:errorResponse");
		errorResponseDoc = ErrorResponseXml::parse(rootXml);
	}
	CAF_CM_EXIT;

	return errorResponseDoc;
}

void XmlRoots::saveErrorResponseToFile(
	const SmartPtrCErrorResponseDoc errorResponseDoc,
	const std::string filePath) {
	CAF_CM_STATIC_FUNC_LOG_VALIDATE("XmlRoots", "saveErrorResponseToFile");

	CAF_CM_ENTER {
		CAF_CM_VALIDATE_SMARTPTR(errorResponseDoc);
		CAF_CM_VALIDATE_STRING(filePath);

		CAF_CM_LOG_DEBUG_VA1("Saving to file - %s", filePath.c_str());

		const std::string errorResponseStr =
			saveErrorResponseToString(errorResponseDoc);
		FileSystemUtils::saveTextFile(filePath, errorResponseStr);
	}
	CAF_CM_EXIT;
}

SmartPtrCErrorResponseDoc XmlRoots::parseErrorResponseFromFile(
	const std::string filePath) {
	CAF_CM_STATIC_FUNC_VALIDATE("XmlRoots", "parseErrorResponseFromFile");

	SmartPtrCErrorResponseDoc errorResponseDoc;

	CAF_CM_ENTER {
		CAF_CM_VALIDATE_STRING(filePath);

		const SmartPtrCXmlElement rootXml = CXmlUtils::parseFile(filePath, "caf:errorResponse");
		errorResponseDoc = ErrorResponseXml::parse(rootXml);
	}
	CAF_CM_EXIT;

	return errorResponseDoc;
}

std::string XmlRoots::saveProviderResponseToString(
	const SmartPtrCProviderResponseDoc providerResponseDoc) {
	CAF_CM_STATIC_FUNC_VALIDATE("XmlRoots", "saveProviderResponseToString");

	std::string rc;

	CAF_CM_ENTER {
		CAF_CM_VALIDATE_SMARTPTR(providerResponseDoc);

		const std::string schemaNamespace = DocXmlUtils::getSchemaNamespace("fx");
		const std::string schemaLocation = DocXmlUtils::getSchemaLocation("fx/Response.xsd");

		const SmartPtrCXmlElement rootXml = CXmlUtils::createRootElement(
			"providerResponse", schemaNamespace, schemaLocation);
		ProviderResponseXml::add(providerResponseDoc, rootXml);

		rc = rootXml->saveToString();
	}
	CAF_CM_EXIT;

	return rc;
}

SmartPtrCProviderResponseDoc XmlRoots::parseProviderResponseFromString(
	const std::string xml) {
	CAF_CM_STATIC_FUNC_VALIDATE("XmlRoots", "parseProviderResponseFromString");

	SmartPtrCProviderResponseDoc providerResponseDoc;

	CAF_CM_ENTER {
		CAF_CM_VALIDATE_STRING(xml);

		const SmartPtrCXmlElement rootXml = CXmlUtils::parseString(xml, "caf:providerResponse");
		providerResponseDoc = ProviderResponseXml::parse(rootXml);
	}
	CAF_CM_EXIT;

	return providerResponseDoc;
}

void XmlRoots::saveProviderResponseToFile(
	const SmartPtrCProviderResponseDoc providerResponseDoc,
	const std::string filePath) {
	CAF_CM_STATIC_FUNC_LOG_VALIDATE("XmlRoots", "saveProviderResponseToFile");

	CAF_CM_ENTER {
		CAF_CM_VALIDATE_SMARTPTR(providerResponseDoc);
		CAF_CM_VALIDATE_STRING(filePath);

		CAF_CM_LOG_DEBUG_VA1("Saving to file - %s", filePath.c_str());

		const std::string providerResponseStr =
			saveProviderResponseToString(providerResponseDoc);
		FileSystemUtils::saveTextFile(filePath, providerResponseStr);
	}
	CAF_CM_EXIT;
}

SmartPtrCProviderResponseDoc XmlRoots::parseProviderResponseFromFile(
	const std::string filePath) {
	CAF_CM_STATIC_FUNC_VALIDATE("XmlRoots", "parseProviderResponseFromFile");

	SmartPtrCProviderResponseDoc providerResponseDoc;

	CAF_CM_ENTER {
		CAF_CM_VALIDATE_STRING(filePath);

		const SmartPtrCXmlElement rootXml = CXmlUtils::parseFile(filePath, "caf:providerResponse");
		providerResponseDoc = ProviderResponseXml::parse(rootXml);
	}
	CAF_CM_EXIT;

	return providerResponseDoc;
}

std::string XmlRoots::saveResponseToString(
	const SmartPtrCResponseDoc responseDoc) {
	CAF_CM_STATIC_FUNC_VALIDATE("XmlRoots", "saveResponseToString");

	std::string rc;

	CAF_CM_ENTER {
		CAF_CM_VALIDATE_SMARTPTR(responseDoc);

		const std::string schemaNamespace = DocXmlUtils::getSchemaNamespace("fx");
		const std::string schemaLocation = DocXmlUtils::getSchemaLocation("fx/Response.xsd");

		const SmartPtrCXmlElement rootXml = CXmlUtils::createRootElement(
			"response", schemaNamespace, schemaLocation);
		ResponseXml::add(responseDoc, rootXml);

		rc = rootXml->saveToString();
	}
	CAF_CM_EXIT;

	return rc;
}

SmartPtrCResponseDoc XmlRoots::parseResponseFromString(
	const std::string xml) {
	CAF_CM_STATIC_FUNC_VALIDATE("XmlRoots", "parseResponseFromString");

	SmartPtrCResponseDoc responseDoc;

	CAF_CM_ENTER {
		CAF_CM_VALIDATE_STRING(xml);

		const SmartPtrCXmlElement rootXml = CXmlUtils::parseString(xml, "caf:response");
		responseDoc = ResponseXml::parse(rootXml);
	}
	CAF_CM_EXIT;

	return responseDoc;
}

void XmlRoots::saveResponseToFile(
	const SmartPtrCResponseDoc responseDoc,
	const std::string filePath) {
	CAF_CM_STATIC_FUNC_LOG_VALIDATE("XmlRoots", "saveResponseToFile");

	CAF_CM_ENTER {
		CAF_CM_VALIDATE_SMARTPTR(responseDoc);
		CAF_CM_VALIDATE_STRING(filePath);

		CAF_CM_LOG_DEBUG_VA1("Saving to file - %s", filePath.c_str());

		const std::string responseStr =
			saveResponseToString(responseDoc);
		FileSystemUtils::saveTextFile(filePath, responseStr);
	}
	CAF_CM_EXIT;
}

SmartPtrCResponseDoc XmlRoots::parseResponseFromFile(
	const std::string filePath) {
	CAF_CM_STATIC_FUNC_VALIDATE("XmlRoots", "parseResponseFromFile");

	SmartPtrCResponseDoc responseDoc;

	CAF_CM_ENTER {
		CAF_CM_VALIDATE_STRING(filePath);

		const SmartPtrCXmlElement rootXml = CXmlUtils::parseFile(filePath, "caf:response");
		responseDoc = ResponseXml::parse(rootXml);
	}
	CAF_CM_EXIT;

	return responseDoc;
}

std::string XmlRoots::saveProviderEventResponseToString(
	const SmartPtrCProviderEventResponseDoc providerEventResponseDoc) {
	CAF_CM_STATIC_FUNC_VALIDATE("XmlRoots", "saveProviderEventResponseToString");

	std::string rc;

	CAF_CM_ENTER {
		CAF_CM_VALIDATE_SMARTPTR(providerEventResponseDoc);

		const std::string schemaNamespace = DocXmlUtils::getSchemaNamespace("fx");
		const std::string schemaLocation = DocXmlUtils::getSchemaLocation("fx/Response.xsd");

		const SmartPtrCXmlElement rootXml = CXmlUtils::createRootElement(
			"providerEventResponse", schemaNamespace, schemaLocation);
		ProviderEventResponseXml::add(providerEventResponseDoc, rootXml);

		rc = rootXml->saveToString();
	}
	CAF_CM_EXIT;

	return rc;
}

SmartPtrCProviderEventResponseDoc XmlRoots::parseProviderEventResponseFromString(
	const std::string xml) {
	CAF_CM_STATIC_FUNC_VALIDATE("XmlRoots", "parseProviderEventResponseFromString");

	SmartPtrCProviderEventResponseDoc providerEventResponseDoc;

	CAF_CM_ENTER {
		CAF_CM_VALIDATE_STRING(xml);

		const SmartPtrCXmlElement rootXml = CXmlUtils::parseString(xml, "caf:providerEventResponse");
		providerEventResponseDoc = ProviderEventResponseXml::parse(rootXml);
	}
	CAF_CM_EXIT;

	return providerEventResponseDoc;
}

void XmlRoots::saveProviderEventResponseToFile(
	const SmartPtrCProviderEventResponseDoc providerEventResponseDoc,
	const std::string filePath) {
	CAF_CM_STATIC_FUNC_LOG_VALIDATE("XmlRoots", "saveProviderEventResponseToFile");

	CAF_CM_ENTER {
		CAF_CM_VALIDATE_SMARTPTR(providerEventResponseDoc);
		CAF_CM_VALIDATE_STRING(filePath);

		CAF_CM_LOG_DEBUG_VA1("Saving to file - %s", filePath.c_str());

		const std::string providerEventResponseStr =
			saveProviderEventResponseToString(providerEventResponseDoc);
		FileSystemUtils::saveTextFile(filePath, providerEventResponseStr);
	}
	CAF_CM_EXIT;
}

SmartPtrCProviderEventResponseDoc XmlRoots::parseProviderEventResponseFromFile(
	const std::string filePath) {
	CAF_CM_STATIC_FUNC_VALIDATE("XmlRoots", "parseProviderEventResponseFromFile");

	SmartPtrCProviderEventResponseDoc providerEventResponseDoc;

	CAF_CM_ENTER {
		CAF_CM_VALIDATE_STRING(filePath);

		const SmartPtrCXmlElement rootXml = CXmlUtils::parseFile(filePath, "caf:providerEventResponse");
		providerEventResponseDoc = ProviderEventResponseXml::parse(rootXml);
	}
	CAF_CM_EXIT;

	return providerEventResponseDoc;
}
