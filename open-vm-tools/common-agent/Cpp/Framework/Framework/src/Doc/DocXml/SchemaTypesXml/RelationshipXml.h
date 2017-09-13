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

#ifndef RelationshipXml_h_
#define RelationshipXml_h_

namespace Caf {

	/// Streams the Relationship class to/from XML
	namespace RelationshipXml {

		/// Adds the RelationshipDoc into the XML.
		void SCHEMATYPESXML_LINKAGE add(
			const SmartPtrCRelationshipDoc relationshipDoc,
			const SmartPtrCXmlElement thisXml);

		/// Parses the RelationshipDoc from the XML.
		SmartPtrCRelationshipDoc SCHEMATYPESXML_LINKAGE parse(
			const SmartPtrCXmlElement thisXml);
	}
}

#endif
