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

#ifndef ParameterCollectionXml_h_
#define ParameterCollectionXml_h_

namespace Caf {

	/// Streams the ParameterCollection class to/from XML
	namespace ParameterCollectionXml {

		/// Adds the ParameterCollectionDoc into the XML.
		void CAFCORETYPESXML_LINKAGE add(
			const SmartPtrCParameterCollectionDoc parameterCollectionDoc,
			const SmartPtrCXmlElement thisXml);

		/// Parses the ParameterCollectionDoc from the XML.
		SmartPtrCParameterCollectionDoc CAFCORETYPESXML_LINKAGE parse(
			const SmartPtrCXmlElement thisXml);
	}
}

#endif
