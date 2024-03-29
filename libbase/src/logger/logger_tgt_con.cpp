﻿#include <libbase/logger.hpp>

#include <libbase/lock.hpp>
#include <libbase/memory.hpp>
#include <libbase/console.hpp>

namespace Base {
	namespace Logger {

		WORD LogLevelColors[(int)Level::Fatal + 1] = {
			0x8,
			0x6,
			0,
			0x3,
			0xA,
			0xE,
			0xC,
			0x4,
		};

		struct LogToConsole: public Target_i, private Uncopyable {
			virtual ~LogToConsole();

			virtual void out(const Module_i * lgr, Level lvl, PCWSTR str, size_t size) const;

			virtual void out(PCWSTR str, size_t size) const;

			LogToConsole();

		private:
			Base::auto_destroy<Lock::SyncUnit_i*> m_sync;
		};


		LogToConsole::~LogToConsole() {
		}

		LogToConsole::LogToConsole() :
			m_sync(Lock::get_CritSection()) {
		}

		void LogToConsole::out(const Module_i * lgr, Level lvl, PCWSTR str, size_t size) const {
			auto lk(m_sync->lock_scope());
			if (lgr->is_color_mode()) {
				ConsoleColor color(LogLevelColors[(int)lvl]);
				consoleout(str, size);
			} else {
				consoleout(str, size);
			}
		}

		void LogToConsole::out(PCWSTR str, size_t size) const {
			auto lk(m_sync->lock_scope());
			consoleout(str, size);
		}


		///=========================================================================================
		Target_i * get_TargetToConsole() {
			return new LogToConsole();
		}

	}
}
