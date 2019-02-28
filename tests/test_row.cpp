#include <cassert>

#include "fsm.h"

int value = 0;

#if __cplusplus >= 201703L

// global actions
void store(int i) { value = i; }
void clear() { value = 0; }

// global guards
bool is1(int i) { return i == 1; }

class state_machine: public fsmlite::fsm<state_machine> {
    friend class fsm;  // base class needs access to transition_table

public:
    enum states { Init, Running, Exit };

    using event = int;

public:
    // member guards
    static bool is2(int i) { return i == 2; }
    bool is3(int i) const { return i == 3; }

    // member actions
    static void store2() { value = 2; }
    void store3() { value = 3; }


private:
    using m = state_machine;

    using transition_table = table<
//       Start    Event  Target   Action      Guard
//  ----+--------+------+--------+-----------+-------+-
    row< Init,    event, Running, &store              >,
    row< Running, event, Running, &store,     &is1    >,
    row< Running, event, Running, &m::store2, &m::is2 >,
    row< Running, event, Running, &m::store3, &m::is3 >,
    row< Running, event, Exit,    &clear              >,
    row< Exit,    event, Exit                         >
//  ----+--------+------+--------+-----------+-------+-
    >;
};

#else

class state_machine: public fsmlite::fsm<state_machine> {
    friend class fsm;  // base class needs access to transition_table

public:
    enum states { Init, Running, Exit };

    using event = int;

public:
    // guards
    bool is1(const event& i) const { return i == 1; }
    bool is2(const event& i) const { return i == 2; }
    bool is3(const event& i) const { return i == 3; }

    // actions
    void store(const event& i) { value = i; }
    void store2(const event&) { value = 2; }
    void store3(const event&) { value = 3; }
    void clear(const event&) { value = 0; }

private:
    using m = state_machine;

    using transition_table = table<
//       Start    Event  Target   Action      Guard
//  ----+--------+------+--------+-----------+-------+-
    row< Init,    event, Running, &m::store           >,
    row< Running, event, Running, &m::store,  &m::is1 >,
    row< Running, event, Running, &m::store2, &m::is2 >,
    row< Running, event, Running, &m::store3, &m::is3 >,
    row< Running, event, Exit,    &m::clear           >,
    row< Exit,    event, Exit                         >
//  ----+--------+------+--------+-----------+-------+-
    >;
};

#endif

int main()
{
    state_machine m;
    assert(m.current_state() == state_machine::Init);
    assert(value == 0);

    m.process_event(42);
    assert(m.current_state() == state_machine::Running);
    assert(value == 42);

    m.process_event(1);
    assert(m.current_state() == state_machine::Running);
    assert(value == 1);

    m.process_event(2);
    assert(m.current_state() == state_machine::Running);
    assert(value == 2);

    m.process_event(3);
    assert(m.current_state() == state_machine::Running);
    assert(value == 3);

    m.process_event(42);
    assert(m.current_state() == state_machine::Exit);
    assert(value == 0);

    m.process_event(42);
    assert(m.current_state() == state_machine::Exit);
    assert(value == 0);

    return 0;
}
