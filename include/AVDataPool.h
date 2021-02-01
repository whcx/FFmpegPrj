#pragma once

#include <cstdlib>
#include <queue>
#include <list>
#include <memory>
#include <mutex>
#include <condition_variable>

template<typename AVType, size_t DATASIZE, size_t POOLDEFAULTSIZE = 4>
class AVDataPool
{
public:
	 AVDataPool::AVDataPool()
	 {
		 queue<AVType*> empty;
		 std::swap(m_queue, empty);
		 for (size_t i = 0; i < POOLDEFAULTSIZE; i++)
		 {
		 	m_queue.push(new AVType[DATASIZE]);
		 }
	 }

	std::shared_ptr<AVType> Get() {
		AVType* avTypePtr = nullptr;
		if (m_queue.empty()) {
			throw std::logic_error("No Data...");
		}
		std::cout << "Get :" << m_queue.size() << endl;
		std:unique_lock<std::mutex> uni_lock(m_mutex_condition);
		if (m_queue.size() == 0) {
			m_condition.wait(uni_lock);
		}

		avTypePtr = m_queue.front();
		m_queue.pop();

		return std::shared_ptr<AVType>(avTypePtr, [&](AVType* ptr) {
			m_queue.push(ptr);
			std::unique_lock<std::mutex> uni_lock(m_mutex_condition);
			m_condition.notify_all();
			std::cout << "return to pool :" << m_queue.size() << endl;
			});
	}

	~AVDataPool() {
		std::lock_guard<std::mutex> lock_g(m_mutex_condition);
		AVType* temp = m_queue.front();
		while (!m_queue.empty()) {
			AVType* temp = m_queue.front();
			m_queue.pop();
			delete[] temp;
		}
		std::cout << "reconstructor to pool :" << m_queue.size() << endl;
	}
	
private:
	std::queue<AVType*> m_queue;
	std::mutex m_mutex_condition;
	std::condition_variable m_condition;
};
