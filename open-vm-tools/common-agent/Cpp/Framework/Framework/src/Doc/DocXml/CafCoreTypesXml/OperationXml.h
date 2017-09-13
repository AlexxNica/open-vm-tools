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

#ifndef OperationXml_h_
#define OperationXml_h_

namespace Caf {

	/// Streams the Operation class to/from XML
	namespace OperationXml {

		/// Adds the OperationDoc into the XML.
		void CAFCORETYPESXML_LINKAGE add(
			const SmartPtrCOperationDoc operationDoc,
			const SmartPtrCXmlElement thisXml);

		/// Parses the OperationDoc from the XML.
		SmartPtrCOperationDoc CAFCORETYPESXML_LINKAGE parse(
			const SmartPtrCXmlElement thisXml);
	}
}

#endif
