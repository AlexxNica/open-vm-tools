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

#ifndef ClassCardinalityXml_h_
#define ClassCardinalityXml_h_

namespace Caf {

	/// Streams the ClassCardinality class to/from XML
	namespace ClassCardinalityXml {

		/// Adds the ClassCardinalityDoc into the XML.
		void SCHEMATYPESXML_LINKAGE add(
			const SmartPtrCClassCardinalityDoc classCardinalityDoc,
			const SmartPtrCXmlElement thisXml);

		/// Parses the ClassCardinalityDoc from the XML.
		SmartPtrCClassCardinalityDoc SCHEMATYPESXML_LINKAGE parse(
			const SmartPtrCXmlElement thisXml);
	}
}

#endif
