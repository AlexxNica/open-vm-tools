/*
 *	Author: bwilliams
 *  Created: Oct 20, 2011
 *
 *	Copyright (c) 2011 Vmware, Inc.  All rights reserved.
 *	-- VMware Confidential
 */

#ifndef CIntegrationAppContext_h_
#define CIntegrationAppContext_h_

#include "../../Common/CApplicationContext.h"

namespace Caf {

class INTEGRATIONCORE_LINKAGE CIntegrationAppContext :
	public IIntegrationAppContext,
	public IAppContext,
	public IChannelResolver {
public:
	CIntegrationAppContext();
	virtual ~CIntegrationAppContext();

public:
	void initialize(const uint32 timeoutMs);
	void initialize(const uint32 timeoutMs, const std::string& beanConfigPath);
	void terminate(const uint32 timeoutMs);

public: // IIntegrationAppContext
	SmartPtrIIntegrationObject getIntegrationObject(const std::string& id) const;
	void getIntegrationObject(const IID& iid, void **ppv) const;
	SmartPtrCObjectCollection getIntegrationObjects(const IID& iid) const;

public: // IAppContext
	SmartPtrIBean getBean(const std::string& name) const;

public: // IChannelResolver
	SmartPtrIMessageChannel resolveChannelName(
		const std::string& channelName) const;

	SmartPtrIIntegrationObject resolveChannelNameToObject(
		const std::string& channelName) const;

private:
	typedef std::multimap<int32, SmartPtrILifecycle> LifecycleBeans;

private:
	SmartPtrCIntegrationObjectCollection assign(
		const IAppContext::SmartPtrCBeans& contextBeans,
		const Cdeqstr& beanConfigPathCollection) const;

	void injectChannelInterceptors(
			const SmartPtrCIntegrationObjectCollection& integrationObjectCollection) const;

	void wire(
		const SmartPtrIAppContext& appContext,
		const SmartPtrCIntegrationObjectCollection& integrationObjectCollection,
		const SmartPtrCChannelResolver& channelResolver) const;

	void startStop(
		const LifecycleBeans& lifecycleBeans,
		const uint32 timeoutMs,
		const bool isStart) const;

	SmartPtrCXmlElement getBeanConfigRoot() const;

	void addBuiltIn(
		const std::string& beanId,
		const IAppContext::SmartPtrCBeans& beans,
		SmartPtrCIntegrationObjectCollection& integrationObjectCollection) const;

	std::string getDefaultBeanConfigPath() const;

	Cdeqstr getBeanConfigFiles(
		const std::string& beanConfigPath) const;

private:
	class CIntegrationAppContextWeakReference : public IIntegrationAppContext {
	public:
		CIntegrationAppContextWeakReference();

		void set(CIntegrationAppContext *context);

		SmartPtrIIntegrationObject getIntegrationObject(
				const std::string& id) const;

		void getIntegrationObject(
				const IID& iid,
				void **ppv) const;

		IIntegrationAppContext::SmartPtrCObjectCollection getIntegrationObjects(
				const IID& iid) const;

	private:
		CIntegrationAppContext *_context;
		CAF_CM_CREATE;
		CAF_CM_CREATE_THREADSAFE;
		CAF_CM_DECLARE_NOCOPY(CIntegrationAppContextWeakReference);
	};
	CAF_DECLARE_SMART_POINTER(CIntegrationAppContextWeakReference);

private:
	bool _isInitialized;
	bool _isIntegrationObjectCollectionReady;
	SmartPtrCApplicationContext _applicationContext;
	SmartPtrCChannelResolver _channelResolver;
	SmartPtrCIntegrationObjectCollection _integrationObjectCollection;
	SmartPtrCIntegrationAppContextWeakReference _weakSelfReference;
	LifecycleBeans _lifecycleBeans;

private:
	CAF_CM_CREATE;
	CAF_CM_CREATE_LOG;
	CAF_CM_DECLARE_NOCOPY(CIntegrationAppContext);
};

CAF_DECLARE_SMART_POINTER(CIntegrationAppContext);

}

#endif // #ifndef CIntegrationAppContext_h_
