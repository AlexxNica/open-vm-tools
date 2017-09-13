/*
 *  Author: bwilliams
 *  Created: April 6, 2012
 *
 *  Copyright (c) 2012 Vmware, Inc.  All rights reserved.
 *  -- VMware Confidential
 *
 *  This code was generated by the script "build/dev/codeGen/genCppDoc". Please
 *  speak to Brian W. before modifying it by hand.
 *
 */

#ifndef CProviderCollectSchemaRequestDoc_h_
#define CProviderCollectSchemaRequestDoc_h_

namespace Caf {

/// A simple container for objects of type ProviderCollectSchemaRequest
class CProviderCollectSchemaRequestDoc {
public:
	CProviderCollectSchemaRequestDoc() :
		_clientId(CAFCOMMON_GUID_NULL),
		_requestId(CAFCOMMON_GUID_NULL),
		_jobId(CAFCOMMON_GUID_NULL),
		_isInitialized(false) {}
	virtual ~CProviderCollectSchemaRequestDoc() {}

public:
	/// Initializes the object with everything required by this
	/// container. Once initialized, this object cannot
	/// be changed (i.e. it is immutable).
	void initialize(
		const UUID clientId,
		const UUID requestId,
		const std::string pmeId,
		const UUID jobId,
		const std::string outputDir,
		const SmartPtrCProviderRequestHeaderDoc requestHeader) {
		if (! _isInitialized) {
			_clientId = clientId;
			_requestId = requestId;
			_pmeId = pmeId;
			_jobId = jobId;
			_outputDir = outputDir;
			_requestHeader = requestHeader;

			_isInitialized = true;
		}
	}

public:
	/// Accessor for the ClientId
	UUID getClientId() const {
		return _clientId;
	}

	/// Accessor for the RequestId
	UUID getRequestId() const {
		return _requestId;
	}

	/// Accessor for the PmeId
	std::string getPmeId() const {
		return _pmeId;
	}

	/// Accessor for the JobId
	UUID getJobId() const {
		return _jobId;
	}

	/// Accessor for the OutputDir
	std::string getOutputDir() const {
		return _outputDir;
	}

	/// Accessor for the RequestHeader
	SmartPtrCProviderRequestHeaderDoc getRequestHeader() const {
		return _requestHeader;
	}

private:
	UUID _clientId;
	UUID _requestId;
	std::string _pmeId;
	UUID _jobId;
	std::string _outputDir;
	SmartPtrCProviderRequestHeaderDoc _requestHeader;
	bool _isInitialized;

private:
	CAF_CM_DECLARE_NOCOPY(CProviderCollectSchemaRequestDoc);
};

CAF_DECLARE_SMART_POINTER(CProviderCollectSchemaRequestDoc);

}

#endif
