#include <boost/statechart/state_machine.hpp>
#include <boost/statechart/simple_state.hpp>
#include <boost/statechart/transition.hpp>
#include <boost/statechart/event.hpp>
#include <iostream>

namespace sc = boost::statechart;

// Define states as enum
enum class States {
    Idle,
    Working,
    Done
};

static std::size_t count = 0;

// Forward declarations
struct Idle;
struct Working;
struct Done;
struct StartWork;
struct FinishWork;
struct Reset;

// Events
struct StartWork : sc::event<StartWork> {};
struct FinishWork : sc::event<FinishWork> {};
struct Reset : sc::event<Reset> {};

// State machine
struct Process : sc::state_machine<Process, Idle> {};

// States implementation
struct Idle : sc::simple_state<Idle, Process> {
    typedef sc::transition<StartWork, Working> reactions;

    Idle() { std::cout << "Enter Idle state\n"; }
    ~Idle() { std::cout << "Exit Idle state\n"; }
};

struct Working : sc::simple_state<Working, Process> {
    typedef sc::transition<FinishWork, Done> reactions;

    Working() { std::cout << "Enter Working state\n"; }
    ~Working() { std::cout << "Exit Working state\n"; }
};

struct Done : sc::simple_state<Done, Process> {
    typedef sc::transition<Reset, Idle> reactions;

    Done() { std::cout << "Enter Done state\n"; }
    ~Done() { std::cout << "Exit Done state\n"; }
};

int test_state_machine() {
    Process p;
    p.initiate();  // Start in Idle state

    // Trigger state transitions
    p.process_event(StartWork());  // Idle -> Working
    p.process_event(FinishWork()); // Working -> Done
    p.process_event(Reset());      // Done -> Idle

    return 0;
}

