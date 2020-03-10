#include <iostream>
#include "Profiler.h"

using namespace std;
using namespace chrono;

Profiler & Profiler::instance()
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	static Profiler profiler;
	return profiler;
}

void Profiler::resumeBlock(const string & blockName)
{
	if (profiles[blockName].started != false) {
		suspendBlock(blockName);
	}
	profiles[blockName].startTime = system_clock::now();
	profiles[blockName].started = true;
}

void Profiler::suspendBlock(const string & blockName)
{	
	profiles[blockName].elapsedTime += system_clock::now() - profiles[blockName].startTime;;
	profiles[blockName].startTime = system_clock::time_point();
	profiles[blockName].started = false;
}

void Profiler::printResult()
{
	for (auto const &item : profiles) {
		cout << "[" << item.first.c_str() << "]: " << item.second.elapsedTime.count() << " nsec" << endl;
	}
}
