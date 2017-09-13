/*
 *  Author: bwilliams
 *  Created: Nov 16, 2015
 *
 *  Copyright (c) 2015 Vmware, Inc.  All rights reserved.
 *  -- VMware Confidential
 *
 */

#ifndef CertPathCollectionXml_h_
#define CertPathCollectionXml_h_

namespace Caf {

	/// Streams the CertPathCollection class to/from XML
	namespace CertPathCollectionXml {

		/// Adds the CertPathCollectionDoc into the XML.
		void CAFCORETYPESXML_LINKAGE add(
			const SmartPtrCCertPathCollectionDoc certPathCollectionDoc,
			const SmartPtrCXmlElement thisXml);

		/// Parses the CertPathCollectionDoc from the XML.
		SmartPtrCCertPathCollectionDoc CAFCORETYPESXML_LINKAGE parse(
			const SmartPtrCXmlElement thisXml);
	}
}

#endif
