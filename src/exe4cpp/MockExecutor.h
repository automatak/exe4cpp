/*
 * Copyright (c) 2018, Automatak LLC
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
 * following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following
 * disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following
 * disclaimer in the documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote
 * products derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef EXE4CPP_MOCKEXECUTOR_H
#define EXE4CPP_MOCKEXECUTOR_H

#include "exe4cpp/IExecutor.h"

#include <algorithm>
#include <cstddef>
#include <limits>
#include <memory>
#include <queue>

namespace exe4cpp
{

class MockTimer;

/**
* Mock implementation of IExecutor for testing
*/
class MockExecutor final : public IExecutor
{
    friend class MockTimer;

public:
    MockExecutor() = default;

    // ------ Implement IExecutor ------
    virtual Timer start(const duration_t&, const action_t& action) override;
    virtual Timer start(const steady_time_t&, const action_t& action) override;
    virtual void post(const action_t& action) override;
    virtual steady_time_t get_time() override;

    /**	@return true if an action was run. */
    bool run_one();

    /** Calls RunOne() up to some maximum number of times continuing while
        there are still events to dispatch

        @return the number of events dispatched
    */
    size_t run_many(size_t maximum = std::numeric_limits<size_t>::max());

    /** @return The number of active, pending timers and post operations */
    size_t num_active() const
    {
        return this->post_queue.size();
    }

    size_t num_pending_timers() const
    {
        return this->timers.size();
    }

    steady_time_t next_timer_expiration_abs() const;

    steady_time_t next_timer_expiration_rel() const;

    size_t advance_time(duration_t duration);

    // doesn't check timers_
    void add_time(duration_t duration);

    bool advance_to_next_timer();

    size_t num_timer_cancel() const
    {
        return this->num_timer_cancel_;
    }

private:
    size_t check_for_expired_timers();

    size_t num_timer_cancel_ = 0;

    bool find_expired_timer();

    void cancel(ITimer* timer);

    typedef std::deque<action_t> post_queue_t;
    typedef std::vector<std::shared_ptr<MockTimer>> timer_vector_t;

    steady_time_t current_time;

    post_queue_t post_queue;
    timer_vector_t timers;
};

/** @section desc Test timer class used in conjunction with MockExecutor */
class MockTimer final : public ITimer
{
    friend class MockExecutor;

public:
    MockTimer(MockExecutor*, const steady_time_t&, const action_t& action);

    // implement ITimer
    void cancel() override;
    steady_time_t expires_at() override;

private:
    steady_time_t time;
    MockExecutor* source;
    action_t action;
};

steady_time_t MockExecutor::next_timer_expiration_abs() const
{
    auto lt = [](const std::shared_ptr<MockTimer>& lhs, const std::shared_ptr<MockTimer>& rhs)
    {
        return lhs->expires_at() < rhs->expires_at();
    };
    auto min = std::min_element(this->timers.begin(), this->timers.end(), lt);
    if (min == this->timers.end())
    {
        return steady_time_t();
    }
    else
    {
        return (*min)->expires_at();
    }
}

steady_time_t MockExecutor::next_timer_expiration_rel() const
{
    auto lt = [](const std::shared_ptr<MockTimer>& lhs, const std::shared_ptr<MockTimer>& rhs)
    {
        return lhs->expires_at() < rhs->expires_at();
    };
    auto min = std::min_element(this->timers.begin(), this->timers.end(), lt);
    if (min == this->timers.end())
    {
        return steady_time_t::max();
    }
    else
    {
        return steady_time_t((*min)->expires_at() - this->current_time);
    }
}

size_t MockExecutor::check_for_expired_timers()
{
    size_t count = 0;
    while (find_expired_timer())
    {
        ++count;
    }
    return count;
}

bool MockExecutor::find_expired_timer()
{
    auto expired = [this](const std::shared_ptr<MockTimer>& timer)
    {
        return timer->expires_at() <= this->current_time;
    };

    auto iter = std::find_if(this->timers.begin(), this->timers.end(), expired);

    if (iter == this->timers.end())
    {
        return false;
    }
    else
    {
        // keep the timer alive until it's callback is completed.
        auto action = [timer = (*iter), action = (*iter)->action]() -> void { action(); };
        this->post_queue.push_back(action);
        this->timers.erase(iter);
        return true;
    }
}

size_t MockExecutor::advance_time(duration_t duration)
{
    this->add_time(duration);
    return this->check_for_expired_timers();
}

// doesn't check timers_
void MockExecutor::add_time(duration_t duration)
{
    this->current_time += duration;
}

bool MockExecutor::advance_to_next_timer()
{
    if (this->timers.empty())
    {
        return false;
    }
    else
    {
        const  auto timestamp = next_timer_expiration_abs();

        if (timestamp > this->current_time)
        {
            this->current_time = timestamp;
            return true;
        }
        else
        {
            return false;
        }
    }
}

bool MockExecutor::run_one()
{
    this->check_for_expired_timers();

    if (this->post_queue.size() > 0)
    {
        auto runnable = post_queue.front();
        this->post_queue.pop_front();
        runnable();
        return true;
    }
    else
    {
        return false;
    }
}

size_t MockExecutor::run_many(size_t aMaximum)
{
    size_t num = 0;
    while (num < aMaximum && this->run_one()) ++num;
    return num;
}

void MockExecutor::post(const action_t& runnable)
{
    this->post_queue.push_back(runnable);
}

steady_time_t MockExecutor::get_time()
{
    return current_time;
}

Timer MockExecutor::start(const duration_t& delay, const action_t& runnable)
{
    return start(current_time + delay, runnable);
}

Timer MockExecutor::start(const steady_time_t& time, const action_t& runnable)
{
    const auto timer = std::make_shared<MockTimer>(this, time, runnable);
    this->timers.push_back(timer);
    return Timer{timer};
}

void MockExecutor::cancel(ITimer* timer)
{
    const auto result = std::find_if(this->timers.begin(), this->timers.end(), [timer](const std::shared_ptr<MockTimer>& item)
    {
        return item.get() == timer;
    });

    if (result != this->timers.end())
    {
        ++num_timer_cancel_;
        this->timers.erase(result);
    }
}

MockTimer::MockTimer(MockExecutor* source, const steady_time_t& time, const action_t& runnable) :
    time{time},
    source{source},
    action{runnable}
{

}

void MockTimer::cancel()
{
    source->cancel(this);
}

steady_time_t MockTimer::expires_at()
{
    return this->time;
}

}

#endif

