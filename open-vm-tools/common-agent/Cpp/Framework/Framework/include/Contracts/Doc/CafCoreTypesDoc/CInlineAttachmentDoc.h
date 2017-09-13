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

#ifndef CInlineAttachmentDoc_h_
#define CInlineAttachmentDoc_h_

namespace Caf {

/// A simple container for objects of type InlineAttachment
class CInlineAttachmentDoc {
public:
	CInlineAttachmentDoc() :
		_isInitialized(false) {}
	virtual ~CInlineAttachmentDoc() {}

public:
	/// Initializes the object with everything required by this
	/// container. Once initialized, this object cannot
	/// be changed (i.e. it is immutable).
	void initialize(
		const std::string name,
		const std::string type,
		const std::string value) {
		if (! _isInitialized) {
			_name = name;
			_type = type;
			_value = value;

			_isInitialized = true;
		}
	}

public:
	/// Accessor for the Name
	std::string getName() const {
		return _name;
	}

	/// Accessor for the Type
	std::string getType() const {
		return _type;
	}

	/// Accessor for the Value
	std::string getValue() const {
		return _value;
	}

private:
	bool _isInitialized;

	std::string _name;
	std::string _type;
	std::string _value;

private:
	CAF_CM_DECLARE_NOCOPY(CInlineAttachmentDoc);
};

CAF_DECLARE_SMART_POINTER(CInlineAttachmentDoc);

}

#endif
