#pragma once

#include <thread>
#include <functional>
#include <vector>
#include <mutex>
#include <queue>

class Workers
{
    typedef std::function<void()> Func;
public:
    Workers(unsigned int threads_count): _threads(threads_count) { }
    void push(Func f)
    {
        _tasks.push(f);
    }
    void run()
    {
        int idx = 0;
        for(auto &t: _threads)
        {
            idx++;
            t = std::thread([idx, this]()
            {
                while(auto f = pop()) 
                { 
                    f(); 
                };
            });
        }        
        for(auto &t: _threads) t.join();
    }
protected:
    Func pop()
    {
        std::lock_guard<std::mutex> lock(_mutex);
        if(!_tasks.empty()) {
            auto f = _tasks.front();
            _tasks.pop();
            return f;
        }
        return {};
    }
protected:
    std::mutex _mutex;
    std::queue<Func> _tasks;  
    std::vector<std::thread> _threads;
};
