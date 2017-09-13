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

#ifndef CLogicalRelationshipDoc_h_
#define CLogicalRelationshipDoc_h_

namespace Caf {

/// Definition of a relationship between classes that can be described by identifying the fields on the classes that uniquely identify the relationship
class CLogicalRelationshipDoc {
public:
	CLogicalRelationshipDoc() :
		_arity(ARITY_NONE),
		_isInitialized(false) {}
	virtual ~CLogicalRelationshipDoc() {}

public:
	/// Initializes the object with everything required by this
	/// container. Once initialized, this object cannot
	/// be changed (i.e. it is immutable).
	void initialize(
		const std::string namespaceVal,
		const std::string name,
		const std::string version,
		const ARITY_TYPE arity,
		const SmartPtrCClassCardinalityDoc dataClassLeft,
		const SmartPtrCClassCardinalityDoc dataClassRight,
		const std::deque<SmartPtrCJoinTypeDoc> joinCollection,
		const std::string description = std::string()) {
		if (! _isInitialized) {
			_namespaceVal = namespaceVal;
			_name = name;
			_version = version;
			_arity = arity;
			_dataClassLeft = dataClassLeft;
			_dataClassRight = dataClassRight;
			_joinCollection = joinCollection;
			_description = description;

			_isInitialized = true;
		}
	}

public:
	/// Accessor for the NamespaceVal
	std::string getNamespaceVal() const {
		return _namespaceVal;
	}

	/// Accessor for the Name
	std::string getName() const {
		return _name;
	}

	/// Accessor for the Version
	std::string getVersion() const {
		return _version;
	}

	/// Accessor for the Arity
	ARITY_TYPE getArity() const {
		return _arity;
	}

	/// Accessor for the DataClassLeft
	SmartPtrCClassCardinalityDoc getDataClassLeft() const {
		return _dataClassLeft;
	}

	/// Accessor for the DataClassRight
	SmartPtrCClassCardinalityDoc getDataClassRight() const {
		return _dataClassRight;
	}

	/// Defines a join condition of the relationship
	std::deque<SmartPtrCJoinTypeDoc> getJoinCollection() const {
		return _joinCollection;
	}

	/// Accessor for the Description
	std::string getDescription() const {
		return _description;
	}

private:
	std::string _namespaceVal;
	std::string _name;
	std::string _version;
	ARITY_TYPE _arity;
	SmartPtrCClassCardinalityDoc _dataClassLeft;
	SmartPtrCClassCardinalityDoc _dataClassRight;
	std::deque<SmartPtrCJoinTypeDoc> _joinCollection;
	std::string _description;
	bool _isInitialized;

private:
	CAF_CM_DECLARE_NOCOPY(CLogicalRelationshipDoc);
};

CAF_DECLARE_SMART_POINTER(CLogicalRelationshipDoc);

}

#endif
