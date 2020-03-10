#ifndef PROFILER_H
#define PROFILER_H

#include <map>
#include <chrono>

class Profiler
{
public:
	static Profiler& instance();
	void resumeBlock(const std::string & blockName);
	void suspendBlock(const std::string& blockName);

	void printResult();

private:
	Profiler() {}

	struct ProfileInfo
	{
		std::chrono::system_clock::time_point startTime;
		std::chrono::nanoseconds elapsedTime;
		bool started = false;
	};
	std::map<std::string, ProfileInfo> profiles;

};

#endif //PROFILER_H