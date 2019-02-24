#include <stdexcept>
#include <type_traits>

#include "fsm.h"

class state_machine: public fsmlite::fsm<state_machine> {
    friend class fsm;  // base class needs access to transition_table
public:
    enum states { Init, Exit };

    struct event {};

private:
    void process(const event& e) {
        process_event(e);
    }

private:
    typedef state_machine m;

    using transition_table = table<
//              Start Event  Target Action
//  -----------+-----+------+------+-----------+-
    mem_fn_row< Init, event, Exit,  &m::process >
//  -----------+-----+------+------+-----------+-
    >;
};

int main()
{
    state_machine m;
    try {
        m.process_event(state_machine::event{});
    } catch (std::logic_error& e) {
        return 0;
    }
    return 1;  /* LCOV_EXCL_LINE */
}
