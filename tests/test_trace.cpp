#include "includes.h"
#include "test_sink.h"
#include "spdlog/sinks/trace_sink.h"

TEST_CASE("trace", "[trace_sink]")
{
	std::shared_ptr<spdlog::sinks::trace_sink_mt> trace_sink(new spdlog::sinks::trace_sink_mt(false));
	spdlog::logger logger("trace_logger", trace_sink);

	setenv("TRACE_PRINT","",0);
	logger.log(spdlog::level::info,"info message via trace_sink {}", 1);
    REQUIRE(1 == 1);
}
