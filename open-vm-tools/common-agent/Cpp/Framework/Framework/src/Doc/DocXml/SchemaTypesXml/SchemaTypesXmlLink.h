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

#ifndef SchemaTypesXml_Link_h_
#define SchemaTypesXml_Link_h_

#ifndef SCHEMATYPESXML_LINKAGE
    #ifdef WIN32
        #ifdef FRAMEWORK_BUILD
            #define SCHEMATYPESXML_LINKAGE __declspec(dllexport)
        #else
            #define SCHEMATYPESXML_LINKAGE __declspec(dllimport)
        #endif
    #else
        #define SCHEMATYPESXML_LINKAGE
    #endif
#endif

#include "SchemaTypesXmlInc.h"

#endif /* SchemaTypesXml_Link_h_ */
