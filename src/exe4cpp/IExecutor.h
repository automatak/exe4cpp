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
#ifndef EXE4CPP_IEXECUTOR_H
#define EXE4CPP_IEXECUTOR_H

#include "exe4cpp/Timer.h"
#include "exe4cpp/ISteadyTimeSource.h"

/**
* @brief exe4cpp header-only library namespace
*/
namespace exe4cpp
{

/**
 * Interface that abstracts an event loop.
 *
 * Events can be posted for to execute immediately or some time in the future.  Events
 * are processed in the order they are posted.
 *
 */
class IExecutor : public ISteadyTimeSource
{
public:

    virtual ~IExecutor() {}

    /// @return start a new timer based on a relative time duration
    virtual Timer start(const duration_t& duration, const action_t& action) = 0;

    /// @return start a new timer based on an absolute timestamp of the steady clock
    virtual Timer start(const steady_time_t& expiration, const action_t& action) = 0;

    /// @return Thread-safe way to post an event to be handled asynchronously
    virtual void post(const action_t& action) = 0;
};

}

#endif
