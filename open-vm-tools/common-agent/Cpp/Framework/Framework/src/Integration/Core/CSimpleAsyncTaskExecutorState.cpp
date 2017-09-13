/*
 *	Author: bwilliams
 *  Created: Oct 20, 2011
 *
 *	Copyright (c) 2011 Vmware, Inc.  All rights reserved.
 *	-- VMware Confidential
 */

#include "stdafx.h"
#include "CSimpleAsyncTaskExecutorState.h"

using namespace Caf;

CSimpleAsyncTaskExecutorState::CSimpleAsyncTaskExecutorState() :
	_isInitialized(false),
	_runnableState(ITaskExecutor::ETaskStateNotStarted),
	CAF_CM_INIT_LOG("CSimpleAsyncTaskExecutorState") {
	CAF_CM_INIT_THREADSAFE;
}

CSimpleAsyncTaskExecutorState::~CSimpleAsyncTaskExecutorState() {
}

void CSimpleAsyncTaskExecutorState::initialize(
	const SmartPtrIRunnable& runnable,
	const SmartPtrIErrorHandler& errorHandler) {
	CAF_CM_FUNCNAME_VALIDATE("initialize");
	CAF_CM_PRECOND_ISNOTINITIALIZED(_isInitialized);
	CAF_CM_VALIDATE_INTERFACE(runnable);
	CAF_CM_VALIDATE_INTERFACE(errorHandler);

	_runnable = runnable;
	_errorHandler = errorHandler;

	_threadSignalStart.initialize("Start");
	_threadSignalStop.initialize("Stop");

	_isInitialized = true;
}

ITaskExecutor::ETaskState CSimpleAsyncTaskExecutorState::getState() const {
	CAF_CM_FUNCNAME_VALIDATE("getState");
	CAF_CM_LOCK_UNLOCK;
	CAF_CM_PRECOND_ISINITIALIZED(_isInitialized);

	return _runnableState;
}

std::string CSimpleAsyncTaskExecutorState::getStateStr() const {
	CAF_CM_FUNCNAME_VALIDATE("getStateStr");
	CAF_CM_PRECOND_ISINITIALIZED(_isInitialized);

	std::string rc;
	switch(getState()) {
		case ITaskExecutor::ETaskStateNotStarted:
			rc = "NotStarted";
		break;
		case ITaskExecutor::ETaskStateStarted:
			rc = "Started";
		break;
		case ITaskExecutor::ETaskStateStopping:
			rc = "Stopping";
		break;
		case ITaskExecutor::ETaskStateFinished:
			rc = "Finished";
		break;
		case ITaskExecutor::ETaskStateFailed:
			rc = "Failed";
		break;
		default:
			rc = "Unknown";
		break;
	}

	return rc;
}

void CSimpleAsyncTaskExecutorState::setState(
	const ITaskExecutor::ETaskState runnableState) {
	CAF_CM_FUNCNAME_VALIDATE("setState");
	CAF_CM_LOCK_UNLOCK;
	CAF_CM_PRECOND_ISINITIALIZED(_isInitialized);

	_runnableState = runnableState;
}

SmartPtrIRunnable CSimpleAsyncTaskExecutorState::getRunnable() const {
	CAF_CM_FUNCNAME_VALIDATE("getRunnable");
	CAF_CM_LOCK_UNLOCK;
	CAF_CM_PRECOND_ISINITIALIZED(_isInitialized);

	return _runnable;
}

SmartPtrIErrorHandler CSimpleAsyncTaskExecutorState::getErrorHandler() const {
	CAF_CM_FUNCNAME_VALIDATE("getErrorHandler");
	CAF_CM_LOCK_UNLOCK;
	CAF_CM_PRECOND_ISINITIALIZED(_isInitialized);

	return _errorHandler;
}

void CSimpleAsyncTaskExecutorState::signalStart() {
	CAF_CM_FUNCNAME_VALIDATE("signalStart");
	CAF_CM_LOCK_UNLOCK;
	CAF_CM_PRECOND_ISINITIALIZED(_isInitialized);

	CAF_CM_LOG_DEBUG_VA1("Signal (%s)", _threadSignalStart.getName().c_str());
	_threadSignalStart.signal();
}

void CSimpleAsyncTaskExecutorState::waitForStart(
	SmartPtrCAutoMutex& mutex,
	const uint32 timeoutMs) {
	CAF_CM_FUNCNAME_VALIDATE("waitForStart");
	CAF_CM_PRECOND_ISINITIALIZED(_isInitialized);

	CAF_CM_LOG_DEBUG_VA2("Wait (%s) - waitMs: %d", _threadSignalStart.getName().c_str(), timeoutMs);
	_threadSignalStart.wait(mutex, timeoutMs);
}

void CSimpleAsyncTaskExecutorState::signalStop() {
	CAF_CM_FUNCNAME_VALIDATE("signalStop");
	CAF_CM_LOCK_UNLOCK;
	CAF_CM_PRECOND_ISINITIALIZED(_isInitialized);

	CAF_CM_LOG_DEBUG_VA1("Signal (%s)", _threadSignalStop.getName().c_str());
	_threadSignalStop.signal();
}

void CSimpleAsyncTaskExecutorState::detach() {
	CAF_CM_FUNCNAME_VALIDATE("detach");
	CAF_CM_LOCK_UNLOCK;
	CAF_CM_PRECOND_ISINITIALIZED(_isInitialized);

	_runnable = NULL;
	_errorHandler = NULL;
}

void CSimpleAsyncTaskExecutorState::waitForStop(
	SmartPtrCAutoMutex& mutex,
	const uint32 timeoutMs) {
	CAF_CM_FUNCNAME_VALIDATE("waitForStop");
	CAF_CM_PRECOND_ISINITIALIZED(_isInitialized);

	CAF_CM_LOG_DEBUG_VA2("Wait (%s) - waitMs: %d", _threadSignalStop.getName().c_str(), timeoutMs);
	_threadSignalStop.wait(mutex, timeoutMs);
}
