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

#ifndef InstallProviderSpecXml_h_
#define InstallProviderSpecXml_h_

namespace Caf {

	/// Streams the InstallProviderSpec class to/from XML
	namespace InstallProviderSpecXml {

		/// Adds the InstallProviderSpecDoc into the XML.
		void CAFINSTALLREQUESTXML_LINKAGE add(
			const SmartPtrCInstallProviderSpecDoc installProviderSpecDoc,
			const SmartPtrCXmlElement thisXml);

		/// Parses the InstallProviderSpecDoc from the XML.
		SmartPtrCInstallProviderSpecDoc CAFINSTALLREQUESTXML_LINKAGE parse(
			const SmartPtrCXmlElement thisXml);
	}
}

#endif
