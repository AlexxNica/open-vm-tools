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

#ifndef EventKeyXml_h_
#define EventKeyXml_h_

namespace Caf {

	/// Streams the EventKey class to/from XML
	namespace EventKeyXml {

		/// Adds the EventKeyDoc into the XML.
		void RESPONSEXML_LINKAGE add(
			const SmartPtrCEventKeyDoc eventKeyDoc,
			const SmartPtrCXmlElement thisXml);

		/// Parses the EventKeyDoc from the XML.
		SmartPtrCEventKeyDoc RESPONSEXML_LINKAGE parse(
			const SmartPtrCXmlElement thisXml);
	}
}

#endif
