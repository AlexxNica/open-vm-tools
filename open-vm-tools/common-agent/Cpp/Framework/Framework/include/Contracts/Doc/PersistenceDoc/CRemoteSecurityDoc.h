/*
 *  Author: bwilliams
 *  Created: Nov 16, 2015
 *
 *  Copyright (c) 2015 Vmware, Inc.  All rights reserved.
 *  -- VMware Confidential
 *
 */

#ifndef CRemoteSecurityDoc_h_
#define CRemoteSecurityDoc_h_

namespace Caf {

/// A simple container for objects of type CRemoteSecurityDoc
class CRemoteSecurityDoc {
public:
	CRemoteSecurityDoc() :
		_isInitialized(false) {}
	virtual ~CRemoteSecurityDoc() {}

public:
	/// Initializes the object with everything required by this
	/// container. Once initialized, this object cannot
	/// be changed (i.e. it is immutable).
	void initialize(
		const std::string& remoteId = std::string(),
		const std::string& protocolName = std::string(),
		const std::string& cmsCert = std::string(),
		const std::string& cmsCipherName = std::string(),
		const SmartPtrCCertCollectionDoc& cmsCertCollection = SmartPtrCCertCollectionDoc(),
		const std::string& cmsCertPath = std::string(),
		const SmartPtrCCertPathCollectionDoc& cmsCertPathCollection = SmartPtrCCertPathCollectionDoc()) {
		if (! _isInitialized) {
			_remoteId = remoteId;
			_protocolName = protocolName;
			_cmsCert = cmsCert;
			_cmsCipherName = cmsCipherName;
			_cmsCertCollection = cmsCertCollection;

			_cmsCertPath = cmsCertPath;
			_cmsCertPathCollection = cmsCertPathCollection;

			_isInitialized = true;
		}
	}

public:
	/// Accessor for the RemoteId
	std::string getRemoteId() const {
		return _remoteId;
	}

	/// Accessor for the ProtocolName
	std::string getProtocolName() const {
		return _protocolName;
	}

	/// Accessor for the cmsCert
	std::string getCmsCert() const {
		return _cmsCert;
	}

	/// Accessor for the CmsCipher
	std::string getCmsCipherName() const {
		return _cmsCipherName;
	}

	/// Accessor for the CertCollection
	SmartPtrCCertCollectionDoc getCmsCertCollection() const {
		return _cmsCertCollection;
	}

	/// Accessor for the cmsCertPath
	std::string getCmsCertPath() const {
		return _cmsCertPath;
	}

	/// Accessor for the CertPathCollection
	SmartPtrCCertPathCollectionDoc getCmsCertPathCollection() const {
		return _cmsCertPathCollection;
	}

private:
	std::string _remoteId;
	std::string _protocolName;
	std::string _cmsCert;
	std::string _cmsCipherName;
	SmartPtrCCertCollectionDoc _cmsCertCollection;

	std::string _cmsCertPath;
	SmartPtrCCertPathCollectionDoc _cmsCertPathCollection;

	bool _isInitialized;

private:
	CAF_CM_DECLARE_NOCOPY(CRemoteSecurityDoc);
};

CAF_DECLARE_SMART_POINTER(CRemoteSecurityDoc);

}

#endif
