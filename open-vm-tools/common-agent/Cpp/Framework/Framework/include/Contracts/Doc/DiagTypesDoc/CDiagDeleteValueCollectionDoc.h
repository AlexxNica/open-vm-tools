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

#ifndef CDiagDeleteValueCollectionDoc_h_
#define CDiagDeleteValueCollectionDoc_h_

namespace Caf {

/// A simple container for objects of type DiagDeleteValueCollection
class CDiagDeleteValueCollectionDoc {
public:
	CDiagDeleteValueCollectionDoc() :
		_isInitialized(false) {}
	virtual ~CDiagDeleteValueCollectionDoc() {}

public:
	/// Initializes the object with everything required by this
	/// container. Once initialized, this object cannot
	/// be changed (i.e. it is immutable).
	void initialize(
		const std::deque<SmartPtrCDiagDeleteValueDoc> deleteValueCollection) {
		if (! _isInitialized) {
			_deleteValueCollection = deleteValueCollection;

			_isInitialized = true;
		}
	}

public:
	/// Accessor for the DeleteValue
	std::deque<SmartPtrCDiagDeleteValueDoc> getDeleteValueCollection() const {
		return _deleteValueCollection;
	}

private:
	bool _isInitialized;

	std::deque<SmartPtrCDiagDeleteValueDoc> _deleteValueCollection;

private:
	CAF_CM_DECLARE_NOCOPY(CDiagDeleteValueCollectionDoc);
};

CAF_DECLARE_SMART_POINTER(CDiagDeleteValueCollectionDoc);

}

#endif
