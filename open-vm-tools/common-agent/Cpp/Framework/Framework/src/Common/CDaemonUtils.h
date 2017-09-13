/*
 *	 Author: mdonahue
 *  Created: Jun 9, 2011
 *
 *	Copyright (c) 2010 Vmware, Inc.  All rights reserved.
 *	-- VMware Confidential
 */

#ifndef CDAEMONUTILS_H_
#define CDAEMONUTILS_H_

class CDaemonUtils {
public:
	static void MakeDaemon(
			int32 argc,
			char** argv,
			const char* processName,
			void(*pfnShutdownHandler)(int32 signalNum),
			bool& isDaemonized,
			bool& logInfos);

	CAF_CM_DECLARE_NOCREATE(CDaemonUtils);
};

#endif /* CDAEMONUTILS_H_ */
