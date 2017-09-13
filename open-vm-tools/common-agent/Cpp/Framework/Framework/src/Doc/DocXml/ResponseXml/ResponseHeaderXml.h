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

#ifndef ResponseHeaderXml_h_
#define ResponseHeaderXml_h_

namespace Caf {

	/// Streams the ResponseHeader class to/from XML
	namespace ResponseHeaderXml {

		/// Adds the ResponseHeaderDoc into the XML.
		void RESPONSEXML_LINKAGE add(
			const SmartPtrCResponseHeaderDoc responseHeaderDoc,
			const SmartPtrCXmlElement thisXml);

		/// Parses the ResponseHeaderDoc from the XML.
		SmartPtrCResponseHeaderDoc RESPONSEXML_LINKAGE parse(
			const SmartPtrCXmlElement thisXml);
	}
}

#endif
