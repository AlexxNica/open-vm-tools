/*
 *	 Author: bwilliams
 *  Created: Oct 22, 2010
 *
 *	Copyright (c) 2010 Vmware, Inc.  All rights reserved.
 *	-- VMware Confidential
 */

#ifndef CErrorToResponseTransformerInstance_h_
#define CErrorToResponseTransformerInstance_h_

namespace Caf {

class CErrorToResponseTransformerInstance :
	public TCafSubSystemObjectRoot<CErrorToResponseTransformerInstance>,
	public IIntegrationObject,
	public IIntegrationComponentInstance,
	public IErrorProcessor {
public:
	CErrorToResponseTransformerInstance();
	virtual ~CErrorToResponseTransformerInstance();

	CAF_DECLARE_OBJECT_IDENTIFIER(_sObjIdErrorToResponseTransformerInstance)

	CAF_BEGIN_INTERFACE_MAP(CErrorToResponseTransformerInstance)
		CAF_INTERFACE_ENTRY(IIntegrationObject)
		CAF_INTERFACE_ENTRY(IIntegrationComponentInstance)
		CAF_INTERFACE_ENTRY(IErrorProcessor)
	CAF_END_INTERFACE_MAP()

public: // IIntegrationObject
	void initialize(
		const IBean::Cargs& ctorArgs,
		const IBean::Cprops& properties,
		const SmartPtrIDocument& configSection);

	std::string getId() const;

public: // IIntegrationComponentInstance
	void wire(
		const SmartPtrIAppContext& appContext,
		const SmartPtrIChannelResolver& channelResolver);

public: // IErrorProcessor
	SmartPtrIIntMessage processErrorMessage(
		const SmartPtrIIntMessage& message);

private:
	bool _isInitialized;
	std::string _id;

private:
	CAF_CM_CREATE;
	CAF_CM_CREATE_LOG;
	CAF_CM_DECLARE_NOCOPY(CErrorToResponseTransformerInstance);
};

}

#endif // #ifndef CErrorToResponseTransformerInstance_h_
