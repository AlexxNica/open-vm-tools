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

#ifndef ProviderResponseXml_h_
#define ProviderResponseXml_h_

namespace Caf {

	/// Streams the ProviderResponse class to/from XML
	namespace ProviderResponseXml {

		/// Adds the ProviderResponseDoc into the XML.
		void RESPONSEXML_LINKAGE add(
			const SmartPtrCProviderResponseDoc providerResponseDoc,
			const SmartPtrCXmlElement thisXml);

		/// Parses the ProviderResponseDoc from the XML.
		SmartPtrCProviderResponseDoc RESPONSEXML_LINKAGE parse(
			const SmartPtrCXmlElement thisXml);
	}
}

#endif
