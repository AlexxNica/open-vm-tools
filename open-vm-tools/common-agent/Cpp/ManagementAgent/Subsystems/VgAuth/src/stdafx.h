/*
 *	 Author: bwilliams
 *  Created: Oct 22, 2010
 *
 *	Copyright (c) 2010 Vmware, Inc.  All rights reserved.
 *	-- VMware Confidential
 */

#ifndef stdafx_h_
#define stdafx_h_

namespace Caf {
	extern const char* _sObjIdGuestAuthenticatorInstance;
	extern const char* _sObjIdGuestAuthenticator;
}

#include <CommonDefines.h>
#include <IntegrationContracts.h>
#include <CafContracts.h>
#include <Integration.h>
#include <DocXml.h>
#include <DocUtils.h>

#ifndef __APPLE__
#include <VGAuthAuthentication.h>

#include "IVgAuthImpersonation.h"
#include "CVgAuthError.h"
#include "CVgAuthContext.h"
#include "CVgAuthUserHandle.h"
#include "CVgAuthImpersonation.h"
#include "CVgAuthInitializer.h"
#include "CGuestAuthenticatorInstance.h"
#include "CGuestAuthenticator.h"
#endif

#endif // #ifndef stdafx_h_
