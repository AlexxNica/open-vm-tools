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

#ifndef PayloadEnvelopeXml_Link_h_
#define PayloadEnvelopeXml_Link_h_

#ifndef PAYLOADENVELOPEXML_LINKAGE
    #ifdef WIN32
        #ifdef FRAMEWORK_BUILD
            #define PAYLOADENVELOPEXML_LINKAGE __declspec(dllexport)
        #else
            #define PAYLOADENVELOPEXML_LINKAGE __declspec(dllimport)
        #endif
    #else
        #define PAYLOADENVELOPEXML_LINKAGE
    #endif
#endif

#include "PayloadEnvelopeXmlInc.h"

#endif /* PayloadEnvelopeXml_Link_h_ */
