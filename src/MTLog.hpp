#ifndef __mtlog__
#define __mtlog__

#include <queue>
#include <string>
#include <mutex>
#include <condition_variable>
#include <thread>

// based on Logger class from Professional C++, page 772
class MTLog {
public:
	MTLog();
	MTLog( const MTLog& rhs)=delete;
	MTLog& operator=( const MTLog& rhs )=delete;

	/**
	 * Gets a singleton instance of the logger.
	 * Note that this is threadsafe (how?) in C++11 and makes
	 * the assumption that C++11 is being used.  Unpredicted
	 * behavior otherwise.
	 */
	static MTLog* instance();

	/**
	 * Logs a message.
	 * @param log entry Message to log.
	 */
	void log( const std::string& entry );

private:
	/**
	 * Processes all queued entries.
	 */
	void processEntries();

private:
	std::mutex _mutex; /**< Lock mutex for accessing the queue. */
	std::condition_variable _condVar; /**< Condition variable to wait for non-empty queue. */
	std::queue<std::string> _queue; /**< Queue of messages to be logged. */
	std::thread _thread; /**< Thread for processing entries. */
};

#endif /* __mtlog__ */