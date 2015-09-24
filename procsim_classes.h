/*
 * procsim_classes.h
 *
 *  Created on: Nov 2, 2014
 *      Author: Harshit
 */

#ifndef PROCSIM_CLASSES_H_
#define PROCSIM_CLASSES_H_

#include <cstdint>
#include <cstdio>
#include "inttypes.h"
#include <vector>
#include <list>
#include "procsim.hpp"


class _scheduler{
public:
	_scheduler();
	_scheduler(uint64_t size);
	~_scheduler();

	//Getsize
	uint64_t getsize(){return size;}

	//Checks for availabilty and returns available size
	int32_t check_for_availablilty();

	//push data
	int32_t push_data(proc_schedule_inst_t);

	//Tag update
	void tag_update(int32_t tag);

	//Fire instrution
	void fire_instructions(proc_scoreboard_t);

	//Delete instruction
	void delete_completed();

	//Update buffers
	void update_logbuffer(_logbuffer);
	void update_outputbuffer(_outputbuffer);

private:
	uint64_t size;
	_schedule_queue schedule_queue;
};

class _dispatcher{
public:
	_dispatcher();
	~_dispatcher();

	_dispatcher(uint64_t);

	uint64_t getfetchrate(){return fetch_rate;}
	uint64_t getdispatchqueuesize();
	void removetop(){dispatch_queue.pop_front();}
	void markasdispatch(uint64_t,bool);
	bool fetch_instructions(uint64_t,bool);


public:
	uint64_t dispatched;
	uint64_t fetch_rate;
	_dispatch_queue dispatch_queue;
};

extern _dispatcher* dispatcher;

/********************/
/*	Logger Class	*/
/********************/
class _logger{
public:
	_logger();
	~_logger();
	_logger(char*);
	void printTool();

public:

	char[100] log_file;

}
#endif /* PROCSIM_CLASSES_H_ */
