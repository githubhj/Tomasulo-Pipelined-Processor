/*
 * procsim_classes.cpp
 *
 *  Created on: Nov 2, 2014
 *      Author: Harshit
 */

#include <cstdint>
#include <cstdio>
#include "inttypes.h"
#include <vector>
#include <list>
#include "procsim.hpp"
#include "procsim_classes.h"



/********************/
/*	Dispatch Class	*/
/********************/

_dispatcher::_dispatcher():fetch_rate(0){}

_dispatcher::_dispatcher(uint64_t _fetch_rate):fetch_rate(_fetch_rate){}

_dispatcher::~_dispatcher(){}

bool _dispatcher::fetch_instructions(uint64_t clock){

	proc_inst_t* fetch_queue = new proc_inst_t[fetch_rate];
	bool eod_file=false;
	for(uint32_t i=0; i< fetch_rate; i++){
		//read instruction from file

		if(read_instruction((fetch_queue+i))){
			fetch_queue[i].fetched_cycle = clock;
			fetch_queue[i].inst_no = ((clock-1)*fetch_rate)+i+1;
			dispatch_queue.push_back(fetch_queue[i]);
			eod_file=false;
		}
		else{
			eod_file=true;
			break;
		}
	}

	return eod_file;
}

_dispatcher* dispatcher = NULL;



/********************/
/*	Logger  Class	*/
/********************/

