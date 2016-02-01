#include "MTLog.hpp"
#include <iostream>

MTLog::MTLog() {
	_thread = std::thread{&MTLog::processEntries, this};
	_thread.detach(); // not sure; stops debug error upon exit
}

MTLog* MTLog::instance() {
	static MTLog inst;
	return &inst;
}

void MTLog::log( const std::string& entry ) {
	std::unique_lock<std::mutex> lock(_mutex);
	_queue.push(entry);
	_condVar.notify_all();
}

void MTLog::processEntries() {
	std::unique_lock<std::mutex> lock(_mutex);
	while( true ) {
		_condVar.wait(lock); // waits for non-empty queue due to break below
		lock.unlock();
		while( true ) {
			lock.lock();
			if( _queue.empty() ) {
				break; // finished with queue, break to wait for data in queue with cond var
			} else {
				// output
				std::cout << _queue.front().c_str();
				_queue.pop();
			}
			lock.unlock();
		}
	}
}