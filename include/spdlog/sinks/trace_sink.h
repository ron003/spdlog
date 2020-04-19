// Copyright(c) 2015-present, Gabi Melman & spdlog contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include <spdlog/details/null_mutex.h>
#include <spdlog/sinks/base_sink.h>

#include <TRACE/trace.h>
#include <chrono>
#include <mutex>
#include <string>

namespace spdlog {
namespace sinks {
/*
 * TRACE sink
 */
template<typename Mutex>
class trace_sink : public base_sink<Mutex>
{
public:
    explicit trace_sink(bool use_raw_msg = true)
		: trace_lvls_{{/* spdlog::level::trace      */ TLVL_TRACE,
              /* spdlog::level::debug      */ TLVL_DEBUG,
              /* spdlog::level::info       */ TLVL_INFO,
              /* spdlog::level::warn       */ TLVL_WARNING,
              /* spdlog::level::err        */ TLVL_ERROR,
              /* spdlog::level::critical   */ TLVL_ERROR,
              /* spdlog::level::off        */ TLVL_INFO}}
		, use_raw_msg_(use_raw_msg)
	{}

protected:
    void sink_it_(const details::log_msg &msg) override
    {
		if TRACE_INIT_CHECK(TRACE_NAME)
		{
			auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(msg.time.time_since_epoch());
			struct timeval lclTime={microseconds.count()/1000000, microseconds.count()%1000000};
			uint8_t      lvl_ = trace_lvl_from_level(msg);
			char         _ins[32];
			const char * filename = msg.source.filename;
			int          line = msg.source.line;
			const char * funcname = msg.source.funcname;
			string_view_t payload;
			memory_buf_t formatted;
			if (use_raw_msg_)
				payload = msg.payload;
			else
			{
				base_sink<Mutex>::formatter_->format(msg, formatted);
				payload = string_view_t(formatted.data(), formatted.size());
			}
			const char * message  = payload.data();
			uint16_t     nargs    = 0;
			if (traceControl_rwp->mode.bits.M && (traceNamLvls_p[traceTID].M & TLVLMSK(lvl_)))
			{
				trace(&lclTime, traceTID, lvl_, line, 0 TRACE_XTRA_PASSED, message);
			}
			if (traceControl_rwp->mode.bits.S && (traceNamLvls_p[traceTID].S & TLVLMSK(lvl_)))
			{
				bool do_s = false;
				switch (msg.level)      // A static limit_info_t PER spdlog::level PER thread PER compilation unit
				{
				case spdlog::level::critical:
					{
						static TRACE_THREAD_LOCAL limit_info_t _info = {/*TRACE_ATOMIC_INIT,*/ 0, lsFREE, 0};
						if (limit_do_print(&lclTime, &_info, _ins, sizeof(_ins))) do_s = true;
					}
					break;
				case spdlog::level::err:
					{
						static TRACE_THREAD_LOCAL limit_info_t _info = {/*TRACE_ATOMIC_INIT,*/ 0, lsFREE, 0};
						if (limit_do_print(&lclTime, &_info, _ins, sizeof(_ins))) do_s = true;
					}
					break;
				case spdlog::level::warn:
					{
						static TRACE_THREAD_LOCAL limit_info_t _info = {/*TRACE_ATOMIC_INIT,*/ 0, lsFREE, 0};
						if (limit_do_print(&lclTime, &_info, _ins, sizeof(_ins))) do_s = true;
					}
					break;
				case spdlog::level::info:
					{
						static TRACE_THREAD_LOCAL limit_info_t _info = {/*TRACE_ATOMIC_INIT,*/ 0, lsFREE, 0};
						if (limit_do_print(&lclTime, &_info, _ins, sizeof(_ins))) do_s = true;
					}
					break;
				case spdlog::level::debug:
					{
						static TRACE_THREAD_LOCAL limit_info_t _info = {/*TRACE_ATOMIC_INIT,*/ 0, lsFREE, 0};
						if (limit_do_print(&lclTime, &_info, _ins, sizeof(_ins))) do_s = true;
					}
					break;
				case spdlog::level::trace:
					{
						static TRACE_THREAD_LOCAL limit_info_t _info = {/*TRACE_ATOMIC_INIT,*/ 0, lsFREE, 0};
						if (limit_do_print(&lclTime, &_info, _ins, sizeof(_ins))) do_s = true;
					}
					break;
				case spdlog::level::off:
				case spdlog::level::n_levels:
					{
						static TRACE_THREAD_LOCAL limit_info_t _info = {/*TRACE_ATOMIC_INIT,*/ 0, lsFREE, 0};
						if (limit_do_print(&lclTime, &_info, _ins, sizeof(_ins))) do_s = true;
					}
					break;
				}
				if (do_s) TRACE_LOG_FUNCTION(&lclTime, traceTID, lvl_, _ins, filename, line, funcname, nargs, message);
			}
		}
    }

    void flush_() override {}

private:
	using levels_array = std::array<uint8_t, 7>;
	levels_array trace_lvls_;
	uint8_t trace_lvl_from_level(const details::log_msg &msg) const
    {
        return trace_lvls_.at(static_cast<levels_array::size_type>(msg.level));
    }
    bool use_raw_msg_;
};

using trace_sink_mt = trace_sink<std::mutex>;   // just formatter_
using trace_sink_st = trace_sink<details::null_mutex>;

} // namespace sinks
} // namespace spdlog
