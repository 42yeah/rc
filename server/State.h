#pragma once

class Worker;

// It's just an FSM. It's just cartridge. I should've just named it state.
class State {
public:
	virtual void init(Worker& worker) = 0;

	virtual void update(Worker& worker) = 0;

	virtual void destroy(Worker& worker) = 0;
};

