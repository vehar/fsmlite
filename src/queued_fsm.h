/*
 * Copyright (c) 2015-2018 Thomas Kemmer
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef FSMLITE_QUEUED_FSM_H
#define FSMLITE_QUEUED_FSM_H

#include "fsm.h"

#include <memory>
#include <queue>

namespace fsmlite {
    /**
     * Finite state machine (FSM) base class template.
     *
     * @tparam Derived the derived state machine class
     *
     * @tparam State the FSM's state type, defaults to `int`
     */
    template<class Derived, class State = int, template<class...> class Queue = std::queue>
    // FIXME: deriving (at least publicly) may not be the best idea...
    class queued_fsm: public fsm<Derived, State> {
        typedef fsm<Derived, State> base_type;

    public:
        typedef typename base_type::state_type state_type;

    public:
        /**
         * Create a state machine with an optional initial state.
         *
         * @param init_state the FSM's initial state
         */
        queued_fsm(state_type init_state = state_type()) : base_type(init_state) {}

        /**
         * Post an event.
         *
         * @tparam Event the event tyoe
         *
         * @param event the event instance
         */
        template<class Event>
        void process_event(const Event& event) {
            if (processing) {
                queue.push(make_event_ptr(event));
            } else {
                processing_lock lock(*this);
                // FIXME: !queue.empty()
                base_type::process_event(event);
                while (!queue.empty()) {
                    queue.front()->process(*(base_type*)this);
                    queue.pop();
                }
            }
        }

    private:
        struct event_base {
            virtual ~event_base() {}
            virtual void process(base_type&) = 0;
        };

        template<class Event>
        class event_holder: public event_base {
        public:
            event_holder(const Event& e): event(e) {}
            void process(base_type& m) { m.process_event(event); }
        private:
            Event event;
        };

        typedef std::unique_ptr<event_base> event_ptr;

        template<class Event>
        event_ptr make_event_ptr(const Event& event) {
            return event_ptr(new event_holder<Event>(event));
        }

        class processing_lock {
        public:
            processing_lock(queued_fsm& fsm): m(fsm) {
                m.processing = true;
            }
            ~processing_lock() {
                // FIXME! m.queue.clear();
                m.processing = false;
            }

        private:
            queued_fsm& m;
        };

        bool processing = false;

        Queue<event_ptr> queue;
    };
}

#endif
