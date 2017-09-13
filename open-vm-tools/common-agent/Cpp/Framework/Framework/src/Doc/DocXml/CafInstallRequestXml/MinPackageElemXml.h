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

#ifndef MinPackageElemXml_h_
#define MinPackageElemXml_h_

namespace Caf {

	/// Streams the MinPackageElem class to/from XML
	namespace MinPackageElemXml {

		/// Adds the MinPackageElemDoc into the XML.
		void CAFINSTALLREQUESTXML_LINKAGE add(
			const SmartPtrCMinPackageElemDoc minPackageElemDoc,
			const SmartPtrCXmlElement thisXml);

		/// Parses the MinPackageElemDoc from the XML.
		SmartPtrCMinPackageElemDoc CAFINSTALLREQUESTXML_LINKAGE parse(
			const SmartPtrCXmlElement thisXml);
	}
}

#endif
