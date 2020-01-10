#ifndef _CON_QUEUE_H_
#define _CON_QUEUE_H_
#include<condition_variable>
#include<mutex>
#include<queue>
    template<typename Data>
    class ConcurrentQueue
    {
        private:
            std::queue<Data> m_queue;
            std::mutex m_mutex;
            std::condition_variable m_condition_variable;
        public:
            ConcurrentQueue() = default;
            ConcurrentQueue(const ConcurrentQueue&) = delete;
            ConcurrentQueue& operator=(const ConcurrentQueue&) = delete;
            void push(Data const& data)
            {
                std::unique_lock<std::mutex> lock(m_mutex);
                m_queue.push(data);
                lock.unlock();
                m_condition_variable.notify_one();
            }

            bool empty() const
            {
                std::lock_guard<std::mutex> lock(m_mutex); 
                return m_queue.empty();
            }

            bool try_pop(Data& popped_value)
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                if(m_queue.empty())
                {
                    return false;
                }
        
                popped_value=m_queue.front();
                m_queue.pop();
                return true;
            }

            void pop(Data& popped_value)
            {
                std::unique_lock<std::mutex> lock(m_mutex);
                while(m_queue.empty())
                {
                    m_condition_variable.wait(lock);
                }
        
                popped_value=m_queue.front();
                m_queue.pop();
            }

            Data pop(){
                std::unique_lock<std::mutex> lock(m_mutex);
                while(m_queue.empty())
                {
                    m_condition_variable.wait(lock);
                }
        
                auto popped_value=m_queue.front();
                m_queue.pop();
                return popped_value;
            }

    };
#endif
