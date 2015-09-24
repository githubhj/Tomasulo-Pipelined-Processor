#ifndef PROCSIM_HPP
#define PROCSIM_HPP

#include <cstdint>
#include <cstdio>
#include "inttypes.h"
#include <deque>
#include <list>
#include <map>

#define DEFAULT_K0 1
#define DEFAULT_K1 2
#define DEFAULT_K2 3
#define DEFAULT_R 8
#define DEFAULT_F 4
#define REGFILE_SIZE 128

typedef struct _proc_inst_t
{
    uint32_t instruction_address;
    int32_t op_code;
    int32_t src_reg[2];
    int32_t dest_reg;
    uint64_t fetch_cycle;
    uint64_t disp_cycle;
    bool disp;
    uint64_t inst_no;
    // You may introduce other fields as needed
    
} proc_inst_t;

typedef struct _proc_output_t{
	uint64_t FETCH;
	uint64_t DISP;
	uint64_t SCHED;
	uint64_t EXEC;
	uint64_t STATE;
	bool COMPLETE;
} proc_output_t;

/********************/
/*	Dispatch queue	*/
/********************/

////Single entry in Dispatch Queue
//typedef struct _proc_dispatch_inst_t{
//	uint64_t inst_no;
//	int32_t op_code;
//	int32_t src_reg[2];
//	int32_t dest_reg;
//	uint64_t fetched_cycle;
//
//
//} proc_dispatch_inst_t;

//Dispatch Queue: Vector od Dispatch elelments
typedef std::deque<proc_inst_t> _dispatch_queue;

extern _dispatch_queue dispatch_queue;

/************************/
/*	 Scheduling queue	*/
/************************/

//src reg proc with ready and tag
typedef struct _proc_src_reg_t{
	int32_t tag;
	bool ready;
} proc_src_reg_t;

//dst reg proc with reg no and tag
typedef struct _proc_dst_reg_t{
	int32_t tag;
	int32_t reg_no;
} proc_dst_reg_t;

//scheduling queue proc
typedef struct _proc_schedule_inst_t{
	uint64_t inst_no;
	int32_t func_unit; // maps to opcode
	proc_src_reg_t src_reg[2];
	proc_dst_reg_t dst_reg;
	bool inst_markdelete;
	uint64_t mark_delete_cycle;
	bool inst_completed;
	uint64_t completed_cycle;
	bool inst_readytofire;
	uint64_t  readytofire_cycle;
	bool inst_fired;
	uint64_t fired_cycle;
	bool inst_scheduled;
	uint64_t scheduled_cycle;
} proc_schedule_inst_t;

//Scheduling Queue: List of Scheduling Queue Proc
typedef std::list<proc_schedule_inst_t> _schedule_queue;
extern _schedule_queue schedule_queue;

/********************/
/*	Register File	*/
/********************/
typedef struct _proc_regfile_reg_t{
	int32_t tag;
	bool ready;
} proc_regfile_reg_t;

extern proc_regfile_reg_t* register_file;

/********************/
/*	FU ScoreBoard	*/
/********************/
typedef struct _proc_func_unit_t{
	//int32_t fu_no;
	bool busy;
	proc_dst_reg_t dst_reg;
} proc_func_unit_t;

typedef struct _proc_exec_t{
	proc_dst_reg_t dst_reg;
	int32_t fu_type;
	int32_t fu_no;
} proc_exec_t;

typedef std::deque<proc_exec_t> _exec_queue;

typedef struct _proc_scoreboad_t{
	proc_func_unit_t* k0;
	proc_func_unit_t* k1;
	proc_func_unit_t* k2;
	int32_t k0size;
	int32_t k1size;
	int32_t k2size;
} proc_scoreboard_t;

extern proc_scoreboard_t scoreboard;
/********************/
/*	Result Bus		*/
/********************/

typedef struct _proc_result_bus_t{
	bool busy;
	proc_dst_reg_t dst_reg;
} proc_result_bus_t;

typedef std::deque<proc_result_bus_t> _result_bus;

/********************/
/*	 Sort Buff		*/
/********************/
typedef struct _proc_sort_buff_t{
	int32_t fu_type;
	uint64_t fu_no;
	uint64_t inst_no;
	uint64_t clock;
} proc_sort_buff_t;

typedef std::deque<proc_sort_buff_t> _sort_buff;


/********************/
/*	Output Buffer	*/
/********************/

typedef struct _proc_outputbuffer_t{
	uint64_t fetch;
	uint64_t disp;
	uint64_t sched;
	uint64_t exec;
	uint64_t state;
	bool printed;
} proc_outputbuffer_t;

typedef std::map<uint64_t, proc_outputbuffer_t> _outputbuffer;
extern _outputbuffer outputbuffer;

/********************/
/*	Log Buffer		*/
/********************/
typedef struct _proc_logbuffer_t{
	uint64_t fetched;
	uint64_t dispateched;
	uint64_t scheduled;
	uint64_t executed;
	uint64_t state_update;
	bool printed;
} proc_logbuffer_t;

typedef std::map<uint64_t, proc_outputbuffer_t> _logbuffer;

extern _logbuffer logbuffer;

typedef struct _proc_stats_t
{
    float avg_inst_retired;
    float avg_inst_fired;
    float avg_disp_size;
    unsigned long max_disp_size;
    unsigned long retired_instruction;
    unsigned long cycle_count;
} proc_stats_t;

bool read_instruction(proc_inst_t* p_inst);

void setup_proc(uint64_t r, uint64_t k0, uint64_t k1, uint64_t k2, uint64_t f);
void run_proc(proc_stats_t* p_stats);
void complete_proc(proc_stats_t* p_stats);

#endif /* PROCSIM_HPP */
