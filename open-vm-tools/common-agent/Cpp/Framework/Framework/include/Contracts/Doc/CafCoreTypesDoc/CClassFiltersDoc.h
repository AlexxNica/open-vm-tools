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

#ifndef CClassFiltersDoc_h_
#define CClassFiltersDoc_h_

namespace Caf {

/// A simple container for objects of type ClassFilters
class CClassFiltersDoc {
public:
	CClassFiltersDoc() :
		_isInitialized(false) {}
	virtual ~CClassFiltersDoc() {}

public:
	/// Initializes the object with everything required by this
	/// container. Once initialized, this object cannot
	/// be changed (i.e. it is immutable).
	void initialize(
		const std::string dialect = std::string(),
		const std::deque<std::string> classFilter = std::deque<std::string>()) {
		if (! _isInitialized) {
			_dialect = dialect;
			_classFilter = classFilter;

			_isInitialized = true;
		}
	}

public:
	/// Accessor for the Dialect
	std::string getDialect() const {
		return _dialect;
	}

	/// Accessor for the ClassFilter
	std::deque<std::string> getClassFilter() const {
		return _classFilter;
	}

private:
	bool _isInitialized;

	std::string _dialect;
	std::deque<std::string> _classFilter;

private:
	CAF_CM_DECLARE_NOCOPY(CClassFiltersDoc);
};

CAF_DECLARE_SMART_POINTER(CClassFiltersDoc);

}

#endif
