#include "procsim.hpp"
#include <map>
#include <vector>
#include <iostream>
#include <stdio.h>

using namespace std;

/**
 * Global Variables
 *
 * Dispatch Queue & Fetch Rate
 * Schedule Queue & Schedule Queue Size
 * Register File
 * Functional Unit Score-board
 * Result Bus & result bus size
 */


//Dispatch Queue
_dispatch_queue dispatch_queue;
uint64_t fetch_rate;


//Schedule Queue
_schedule_queue schedule_queue;
uint64_t schedule_queue_size;

//Register File
proc_regfile_reg_t* register_file = NULL;

//Functional Unit Scoreboard
proc_scoreboard_t scoreboard;

//Result Bus & Result Bus Size
uint64_t result_bus_size;
_result_bus result_bus;

/**
 * Subroutine for initializing the processor. You many add and initialize any global or heap
 * variables as needed.
 * XXX: You're responsible for completing this routine
 *
 * @r ROB size
 * @k0 Number of k0 FUs
 * @k1 Number of k1 FUs
 * @k2 Number of k2 FUs
 * @f Number of instructions to fetch
 */

void setup_proc(uint64_t r, uint64_t k0, uint64_t k1, uint64_t k2, uint64_t f) 
{
	//Setup fetch rate
	if(f == 0){
		fprintf(stderr, "\n\n**** Please Enter f>0 ****");
		exit(-1);
	}
	else{
		fetch_rate = f;
	}

	if(k0 || k1 || k2){

		//Setup Schedule Queue
		schedule_queue_size = 2*(k0+k1+k2);

		//Setup ScoreBoard
		scoreboard.k0 = new proc_func_unit_t[k0];
		for(uint32_t i =0; i<k0; i++){
			scoreboard.k0[i].busy =0;
		}
		scoreboard.k0size = k0;

		scoreboard.k1 = new proc_func_unit_t[k1];
		for(uint32_t i =0; i<k1; i++){
			scoreboard.k1[i].busy =0;
		}
		scoreboard.k1size = k1;

		scoreboard.k2 = new proc_func_unit_t[k2];
		for(uint32_t i =0; i<k2; i++){
			scoreboard.k2[i].busy =0;
		}
		scoreboard.k2size = k2;
	}
	else{
		fprintf(stderr, "\n\n**** Please Enter any k>0 ****");
		exit(-1);
	}


	//Setup Register File
	register_file = new proc_regfile_reg_t[REGFILE_SIZE];

	for(uint32_t i =0; i<REGFILE_SIZE; i++){
		register_file[i].ready = 1;
		register_file[i].tag = -1;
	}

	//Setup Result Bus Size
	if(r==0){
		fprintf(stderr, "\n\n**** Please Enter r>0 ****");
		exit(-1);
	}
	else{
		result_bus_size = r;
	}
}

/**
 * Subroutine that simulates the processor.
 *   The processor should fetch instructions as appropriate, until all instructions have executed
 * XXX: You're responsible for completing this routine
 *
 * @p_stats Pointer to the statistics structure
 */
void run_proc(proc_stats_t* p_stats)
{
	//System Clock
	uint64_t clock=0;
	//An array to buffer Average Dispatch Size(Comments in README)
	double avg_dispatch_size[] ={0.0,0.0,0.0};
	//Number of Instructions Fetched
	uint64_t fetched_instructions = 0;

	//Switches to control all states
	bool full_scheduling_done = false;
	bool full_dispatching_done =false;
	bool full_fetching_done = false;

	//Output map, A Buffer of Print Statements
	std::map<uint64_t, proc_output_t> output_map;

	//Temporary Fetch Queue
	vector<proc_inst_t>fetch_queue;

	//Print Output Header
	printf("INST\tFETCH\tDISP\tSCHED\tEXEC\tSTATE\n");

	//Till Schedule Queue Has data execute this loop
	while(!full_scheduling_done){

		//CLOCK INCREMENT:
		clock++;

		/****************************************************************************************************************************************/
		/*****												PRINT COMPLETED INST ON STDOUT													*****/
		/****************************************************************************************************************************************/
		//PRINT COMPLETE INSTRUCTIONS:
		for(std::map<uint64_t, proc_output_t>::iterator it = output_map.begin(); it != output_map.end(); it++){
			if(it->second.COMPLETE == true){
				printf("%lu\t%lu\t%lu\t%lu\t%lu\t%lu\n", it->first, it->second.FETCH,it->second.DISP,it->second.SCHED,it->second.EXEC,it->second.STATE);
				output_map.erase(it);
			}
			else{
				break;
			}
		}
		//END OF PRINTING
		/************************************************************END OF PRINTING*************************************************************/


		/****************************************************************************************************************************************/
		/*************************************************DELETE INSTRUCTIONS FROM SCHEDULING QUEUE**********************************************/
		/****************************************************************************************************************************************/
		//DELETE INSTRUCTIONS FROM SCHEDULING QUEUE
		if(!full_scheduling_done){

			uint64_t delete_count=0;

			//COUNT ONLY THOSE MARKED FOR DELETE
			for(_schedule_queue::iterator it = schedule_queue.begin(); it != schedule_queue.end(); it++){
				if((it->inst_readytofire) && (it->inst_scheduled) && (it->inst_fired)&&(it->inst_completed)&&(it->inst_markdelete)){
					delete_count++;
				}
			}
			//END OF COUNTING

			//DELETE THE COUNTED INSTRUCTIONS
			for(uint64_t i=0; i<delete_count;i++){
				for(_schedule_queue::iterator it = schedule_queue.begin(); it != schedule_queue.end(); it++){
					if((it->inst_readytofire) && (it->inst_scheduled) && (it->inst_fired)&&(it->inst_completed)&&(it->inst_markdelete)){
						schedule_queue.erase(it);
						break;
					}
				}
			}
			//END OF DELETING

			//EMPTY RESULT BUS
			result_bus.clear();

			//IF DISPATCHING WAS COMPLETE AND SCHEDULING QUEUE GOT EMPTY, THEN TERMINATE FROMT THIS LOOP
			if(schedule_queue.size()==0 && full_dispatching_done){
				full_scheduling_done = true;
			}
		}
		//END OF DELETE
		/***********************************************************END OF DELETE********************************************************************/


		/****************************************************************************************************************************************/
		/****************************************MARK FOR DELETE INSTRUCTIONS FROM SCHEDULING QUEUE**********************************************/
		/****************************************************************************************************************************************/
		//MARK FOR DELETION
		for(_schedule_queue::iterator it = schedule_queue.begin(); it != schedule_queue.end(); it++){
			if((it->inst_readytofire) && (it->inst_scheduled) && (it->inst_fired)&&(it->inst_completed)&&(!it->inst_markdelete)){
				it->inst_markdelete = true;
			}
		}
		//END OF MARK FOR DELETION
		/*******************************************************END OF MARK FOR DELETE****************************************************************/

		/****************************************************************************************************************************************/
		/**************************************DO STATUS UPDATE OF INSTRUCTIONS WHICH CAN GET RESULT BUS*****************************************/
		/****************************************************************************************************************************************/
		//STATUS UPDATE
		if(!full_scheduling_done){

			//Check if scheduling queue if Empty of not
			if(!schedule_queue.empty()){

				//If not Empty
				uint32_t result_bus_inst=0, temp_result_bus_size=0;

				//Check how many instructions want result bus
				for(_schedule_queue::iterator it = schedule_queue.begin(); it != schedule_queue.end(); it++){
					//If instruction was fired
					if((it->inst_readytofire) && (it->inst_scheduled) && (it->inst_fired)&&(!it->inst_completed)){
						result_bus_inst++;
					}
				}

				//If the instructions who want result bus is less than result bus size
				if(result_bus_inst >= result_bus_size){
					temp_result_bus_size = result_bus_size;
				}

				//Else make the result bus size equal to result bus
				else{
					temp_result_bus_size = result_bus_inst;
				}

				//Till I have got all instructions i needed
				while(result_bus.size() != temp_result_bus_size){

					//Iterate over Scheduling queue to find instruction to be retired
					for(_schedule_queue::iterator it = schedule_queue.begin(); it != schedule_queue.end(); it++){

						//Make inst found hook true
						bool inst_found = true;

						//If instruction was fired
						if((it->inst_readytofire) && (it->inst_scheduled) && (it->inst_fired)&&(!it->inst_completed)){

							//Check if any previous instruction was pending
							for(_schedule_queue::iterator it2 = it; it2 != schedule_queue.end(); it2++){

								//If instruction was fired
								if((it2->inst_readytofire) && (it2->inst_scheduled) && (it2->inst_fired)&&(!it2->inst_completed)&&(it2!=it)){

									//If any pending instruction had more priority, then this is not the inst we needed
									if(it->fired_cycle > it2->fired_cycle){
										//make inst found hook false and go for next inst
										inst_found = false;
										break;
									}
								}
							}

							//If not then we found our retiring instruction
							if(inst_found){

								//Mark print buffer
								output_map[it->inst_no].STATE = clock;
								output_map[it->inst_no].COMPLETE = true;

								//Mark schedule queue as complete
								it->inst_completed = true;
								it->completed_cycle = clock;

								//get result bus
								proc_result_bus_t temp_result_bus_proc;
								temp_result_bus_proc.busy = true;
								temp_result_bus_proc.dst_reg.tag = it->dst_reg.tag;
								temp_result_bus_proc.dst_reg.reg_no = it->dst_reg.reg_no;
								result_bus.push_back(temp_result_bus_proc);
								//End of result bus update

								//Schedule queue update
								for(_schedule_queue::iterator it3 = schedule_queue.begin(); it3 != schedule_queue.end(); it3++){

									//check src Regs
									if(it3->inst_scheduled && (!it3->inst_readytofire) && (!it3->inst_fired)&&(!it3->inst_completed)){

										//src0
										if(it3->src_reg[0].ready == false && (it3->src_reg[0].tag == it->dst_reg.tag)){
											it3->src_reg[0].ready = true;
											it3->src_reg[0].tag = -1;
										}

										//src1
										if(it3->src_reg[1].ready == false && (it3->src_reg[1].tag == it->dst_reg.tag)){
											it3->src_reg[1].ready = true;
											it3->src_reg[1].tag = -1;
										}
									}
								}
								//End of Schedule queue Update

								//Free Func Unit
								switch(it->func_unit){
								case(0):
									for(int32_t i =0; i<scoreboard.k0size;i++){
										if(scoreboard.k0[i].busy==true){
											scoreboard.k0[i].busy=false;
											break;
										}
									}
									break;

								case(-1):
								case(1):
									for(int32_t i =0; i<scoreboard.k1size;i++){
										if(scoreboard.k1[i].busy==true){
											scoreboard.k1[i].busy=false;
											break;
										}
									}
									break;

								case(2):
									for(int32_t i =0; i<scoreboard.k2size;i++){
										if(scoreboard.k2[i].busy==true){
											scoreboard.k2[i].busy=false;
											break;
										}
									}
									break;

								default:
									break;

								}
								//End of Free Func Unit

								//RegFile update
								if(it->dst_reg.reg_no!=-1){
									if(register_file[it->dst_reg.reg_no].tag == it->dst_reg.tag){
										register_file[it->dst_reg.reg_no].ready=true;
										register_file[it->dst_reg.reg_no].tag = -1;
									}
								}
								//End of register file update

								//If we filled the result bus as needed then break
								if(result_bus.size()!=temp_result_bus_size){
									break;
								}
								//break

							}
						}

						//If we filled the result break go party
						if(result_bus.size() == temp_result_bus_size){
							break;
						}
					}

					//If we filled the result break go party
					if(result_bus.size() == temp_result_bus_size){
						break;
					}
				}
			}
		}
		/*******************************************************END OF STAUS UPDATE**************************************************************/

		/****************************************************************************************************************************************/
		/*****************************************************FIRE EXEC INSTRUCTIONS*************************************************************/
		/****************************************************************************************************************************************/
		if(!full_scheduling_done){
			for(_schedule_queue::iterator it = schedule_queue.begin(); it != schedule_queue.end(); it++){
				if((it->inst_readytofire) && (it->inst_scheduled) && (!it->inst_completed) && (!it->inst_fired)){

					//If instruction's functional unit is available
					switch(it->func_unit){

						//Case for 0th functional unit
						case(0):
							for(int32_t i=0; i<scoreboard.k0size; i++){
								if(scoreboard.k0[i].busy==0){
									scoreboard.k0[i].busy=1;
									it->inst_fired = true;
									it->fired_cycle =clock;
									output_map[it->inst_no].EXEC =clock;
									break;
								}
							}
							break;

						//Case for 1th functional unit
						case(-1):
						case(1):
							for(int32_t i=0; i<scoreboard.k1size; i++){
								if(scoreboard.k1[i].busy==0){
									scoreboard.k1[i].busy=1;
									it->inst_fired = true;
									it->fired_cycle =clock;
									output_map[it->inst_no].EXEC =clock;
									break;
								}
							}
							break;

						//Case for 2nd Func Unit
						case(2):
							for(int32_t i=0; i<scoreboard.k2size; i++){
								if(scoreboard.k2[i].busy==0){
									scoreboard.k2[i].busy=1;
									it->inst_fired = true;
									it->fired_cycle =clock;
									output_map[it->inst_no].EXEC =clock;
									break;
								}
							}
							break;

						default:
							break;
					}
				}
			}
		}
		/*******************************************************END OF EXEC**************************************************************/

		/****************************************************************************************************************************************/
		/*****************************************************SCHEDULE INSTRUCTIONS**************************************************************/
		/****************************************************************************************************************************************/
		//Schedule Instructions
		if(!full_scheduling_done){

			if(schedule_queue.size()!=schedule_queue_size){

				uint32_t current_schedule_size = schedule_queue.size();

				if(!dispatch_queue.empty()){

					for(uint32_t i =0 ; i< (schedule_queue_size - current_schedule_size); i++){
						_dispatch_queue::iterator it = dispatch_queue.begin();
						if(!dispatch_queue.empty()){
							proc_schedule_inst_t temp_schedule;

							temp_schedule.inst_no = it->inst_no;
							temp_schedule.func_unit = it->op_code;

							//Update src 1 register
							if(it->src_reg[0]==(-1)){
								temp_schedule.src_reg[0].ready = true;
								temp_schedule.src_reg[0].tag = -1;
							}
							else if(register_file[it->src_reg[0]].ready){
								temp_schedule.src_reg[0].ready = true;
								temp_schedule.src_reg[0].tag = -1;
							}
							else{
								temp_schedule.src_reg[0].ready = false;
								temp_schedule.src_reg[0].tag = register_file[it->src_reg[0]].tag;
							}

							//Update src 2 register
							if(it->src_reg[1]==(-1)){
								temp_schedule.src_reg[1].ready = true;
								temp_schedule.src_reg[1].tag = -1;
							}
							else if(register_file[it->src_reg[1]].ready){
								temp_schedule.src_reg[1].ready = true;
								temp_schedule.src_reg[1].tag = -1;
							}
							else{
								temp_schedule.src_reg[1].ready = false;
								temp_schedule.src_reg[1].tag = register_file[it->src_reg[1]].tag;
							}

							//Get Dst reg no.
							temp_schedule.dst_reg.reg_no = it->dest_reg;
							//Get reg no. tag
							temp_schedule.dst_reg.tag = it->inst_no;

							//update regfile
							if(it->dest_reg != -1){
								register_file[it->dest_reg].tag = it->inst_no;
								register_file[it->dest_reg].ready = false;
							}


							//Update Flags
							temp_schedule.mark_delete_cycle = 0;
							temp_schedule.completed_cycle = 0;
							temp_schedule.fired_cycle = 0;
							temp_schedule.readytofire_cycle = 0;
							temp_schedule.scheduled_cycle = clock;
							temp_schedule.inst_markdelete = false;
							temp_schedule.inst_completed = false;
							temp_schedule.inst_fired = false;
							temp_schedule.inst_readytofire = false;
							temp_schedule.inst_scheduled = true;

							//Update Output Buffer
							output_map[it->inst_no].SCHED = clock;
							//Push on Schedule Queue
							schedule_queue.push_back(temp_schedule);
							//Remove From Dispatch Queue
							dispatch_queue.pop_front();
						}
					}
				}
			}

			//If Instruction is ready, then make it ready to fire
			for(_schedule_queue::iterator it = schedule_queue.begin(); it != schedule_queue.end(); it++){

				//Check if instruction is ready to fire
				if((it->src_reg[0].ready)&&(it->src_reg[1].ready)&&(it->inst_scheduled)&&(!it->inst_readytofire)&&(!it->inst_fired)&&(!it->inst_completed)){
					it->inst_readytofire = true;
					it->readytofire_cycle = clock;
				}
			}

			if(full_dispatching_done == true && dispatch_queue.size()==0 && schedule_queue.size() ==0){
				full_scheduling_done = true;
			}
		}
		/*******************************************************END OF SCHEDULING**************************************************************/

		/****************************************************************************************************************************************/
		/*****************************************************DISPATCH INSTRUCTIONS**************************************************************/
		/****************************************************************************************************************************************/
		if(!full_dispatching_done){

			//For Dispatching Instructions
			for(uint32_t i=0; i< fetch_queue.size(); i++){

				if((!fetch_queue.empty())){
					dispatch_queue.push_back(fetch_queue[i]);
					output_map[fetch_queue[i].inst_no].DISP = clock;
				}
			}
			fetch_queue.clear();

			if(full_fetching_done==true)
			{
				full_dispatching_done=true;
			}

		}
		/*********************************************************END OF SCHEDULING***************************************************************/

		//Update P_stats
		avg_dispatch_size[0] = avg_dispatch_size[1];
		avg_dispatch_size[1] = avg_dispatch_size[2];
		avg_dispatch_size[2] = avg_dispatch_size[2]*(clock-1);
		avg_dispatch_size[2] = ((avg_dispatch_size[2]+dispatch_queue.size())/clock);

		//Update P_stats
		if(p_stats->max_disp_size < dispatch_queue.size()){
			p_stats->max_disp_size = dispatch_queue.size();
		}

		/****************************************************************************************************************************************/
		/********************************************************FETCH INSTRUCTIONS**************************************************************/
		/****************************************************************************************************************************************/
		//Fetch if the file is not completely fetched
		if(!full_fetching_done){

			//For Fetching Instructions
			for(uint32_t i=0; i< fetch_rate; i++){
				//read instruction from file
				proc_output_t temp_output;
				proc_inst_t temp_fetched_inst;

				//Read Instruction from file
				if(read_instruction(&temp_fetched_inst)){
					temp_fetched_inst.fetch_cycle = clock;
					temp_fetched_inst.disp_cycle = 0;
					temp_fetched_inst.disp = false;
					temp_fetched_inst.inst_no = ((clock-1)*fetch_rate)+i+1;
					fetch_queue.push_back(temp_fetched_inst);
					full_fetching_done=false;

					//Create temp output struct
					temp_output.FETCH=clock;
					temp_output.DISP=0;
					temp_output.SCHED=0;
					temp_output.EXEC=0;
					temp_output.STATE=0;
					temp_output.COMPLETE = false;

					//Log the fetched instruction
					output_map.insert(std::pair<uint64_t,proc_output_t>(fetch_queue[i].inst_no, temp_output));

					fetched_instructions++;
				}
				//End of read instruction from file

				//Check if fetching is complete or not
				else{
					full_fetching_done=true;
					break;
				}
				//Fetching Complete

			}
			//End of fetching instructions
		}
		//Fetching for this cycle done!
	}
	/****************************************************************END OF FETCH********************************************************************/

	p_stats->avg_disp_size = avg_dispatch_size[0];
	p_stats->cycle_count = clock-2;
	p_stats->retired_instruction = fetched_instructions;
	p_stats->avg_inst_fired = (double)fetched_instructions/(clock-2);
	p_stats->avg_inst_retired = (double)fetched_instructions/(clock-2);

}

/**
 * Subroutine for cleaning up any outstanding instructions and calculating overall statistics
 * such as average IPC, average fire rate etc.
 * XXX: You're responsible for completing this routine
 *
 * @p_stats Pointer to the statistics structure
 */



void complete_proc(proc_stats_t *p_stats) {}

