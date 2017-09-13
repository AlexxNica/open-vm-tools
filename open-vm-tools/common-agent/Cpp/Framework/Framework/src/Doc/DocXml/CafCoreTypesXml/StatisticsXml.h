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

#ifndef StatisticsXml_h_
#define StatisticsXml_h_

namespace Caf {

	/// Streams the Statistics class to/from XML
	namespace StatisticsXml {

		/// Adds the StatisticsDoc into the XML.
		void CAFCORETYPESXML_LINKAGE add(
			const SmartPtrCStatisticsDoc statisticsDoc,
			const SmartPtrCXmlElement thisXml);

		/// Parses the StatisticsDoc from the XML.
		SmartPtrCStatisticsDoc CAFCORETYPESXML_LINKAGE parse(
			const SmartPtrCXmlElement thisXml);
	}
}

#endif
