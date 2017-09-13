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

#ifndef ManifestXml_h_
#define ManifestXml_h_

namespace Caf {

	/// Streams the Manifest class to/from XML
	namespace ManifestXml {

		/// Adds the ManifestDoc into the XML.
		void RESPONSEXML_LINKAGE add(
			const SmartPtrCManifestDoc manifestDoc,
			const SmartPtrCXmlElement thisXml);

		/// Parses the ManifestDoc from the XML.
		SmartPtrCManifestDoc RESPONSEXML_LINKAGE parse(
			const SmartPtrCXmlElement thisXml);
	}
}

#endif
