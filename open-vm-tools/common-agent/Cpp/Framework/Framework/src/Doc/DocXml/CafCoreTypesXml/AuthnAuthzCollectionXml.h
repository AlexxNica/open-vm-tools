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

#ifndef AuthnAuthzCollectionXml_h_
#define AuthnAuthzCollectionXml_h_

namespace Caf {

	/// Streams the AuthnAuthzCollection class to/from XML
	namespace AuthnAuthzCollectionXml {

		/// Adds the AuthnAuthzCollectionDoc into the XML.
		void CAFCORETYPESXML_LINKAGE add(
			const SmartPtrCAuthnAuthzCollectionDoc authnAuthzCollectionDoc,
			const SmartPtrCXmlElement thisXml);

		/// Parses the AuthnAuthzCollectionDoc from the XML.
		SmartPtrCAuthnAuthzCollectionDoc CAFCORETYPESXML_LINKAGE parse(
			const SmartPtrCXmlElement thisXml);
	}
}

#endif
