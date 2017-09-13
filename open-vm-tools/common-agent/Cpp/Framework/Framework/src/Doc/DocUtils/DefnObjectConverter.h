/*
 *  Author: bwilliams
 *  Created: April 6, 2012
 *
 *  Copyright (c) 2012 Vmware, Inc.  All rights reserved.
 *  -- VMware Confidential
 *
 */

#ifndef DefnObjectConverter_h_
#define DefnObjectConverter_h_

namespace Caf {
class DOCUTILS_LINKAGE DefnObjectConverter {
public:
	static std::string toString(
		const SmartPtrCDataClassInstanceDoc dataClassInstance);

	static SmartPtrCDataClassInstanceDoc fromString(
		const std::string defnObjectXmlStr);

private:
	static void addDataClassSubInstance(
		const SmartPtrCDataClassSubInstanceDoc dataClassSubInstance,
		const SmartPtrCXmlElement thisXml);

	static SmartPtrCDataClassSubInstanceDoc parseDataClassSubInstance(
		const SmartPtrCXmlElement dataClassInstanceXml,
		const bool isDataClassInstance);

	static std::deque<SmartPtrCCmdlMetadataDoc> parseCmdlMetadata(
		const SmartPtrCXmlElement thisXml);

	static void addCmdlMetadata(
		const std::deque<SmartPtrCCmdlMetadataDoc> cmdlMetadataCollection,
		const SmartPtrCXmlElement thisXml);

private:
	CAF_CM_DECLARE_NOCREATE(DefnObjectConverter);
};

}

#endif
