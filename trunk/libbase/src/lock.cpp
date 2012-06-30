﻿#include <libbase/lock.hpp>

namespace Base {

	SyncUnit_i::~SyncUnit_i() {
	}

	LockWatcher::~LockWatcher() {
		if (m_unit) {
			m_unit->release();
			m_unit = nullptr;
		}
	}

	LockWatcher::LockWatcher(SyncUnit_i * unit) :
		m_unit(unit) {
		m_unit->lock();
	}

	LockWatcher::LockWatcher(LockWatcher && rhs):
	m_unit(nullptr) {
		swap(rhs);
	}

	LockWatcher & LockWatcher::operator =(LockWatcher && rhs) {
		if (this != &rhs) {
			LockWatcher().swap(*this);
			swap(rhs);
		}
		return *this;
	}

	void LockWatcher::swap(LockWatcher & rhs) {
		using std::swap;
		swap(m_unit, rhs.m_unit);
	}

	LockWatcher::LockWatcher() :
		m_unit(nullptr) {
	}

}