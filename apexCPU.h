#ifndef APEXCPU_H // Guard against recursive includes
#define APEXCPU_H
#include <stdarg.h> // To enable reportStage

enum stageStatus_enum {
	stage_squashed,
	stage_stalled,
	stage_noAction,
	stage_ready,
	stage_actionComplete
};

enum stage_enum {
	fetch,
	decode,
	dispatch,
	issue,
	fu_alu1,
	fu_mul1,fu_mul2,fu_mul3,
	fu_ld1,fu_ld2,fu_ld3,
	fu_st1,fu_st2,fu_st3,
	fu_br1,fu_br2,fu_br3,
	writeback
};

enum fu_enum {
	no_fu=0, // TODO: Can we get rid of this???
	alu_fu=fu_alu1,  // Is mapping to first stage useful?
	mult_fu=fu_mul1,
	load_fu=fu_ld1,
	store_fu=fu_st1,
	br_fu=fu_br1
};

struct apexStage_struct {
	int pc;
	int instruction;
	int opcode;
	enum fu_enum fu;
	int dr;
	int sr1;
	int sr2;
	int imm;
	int offset;
	int op1Valid;
	int op1;
	int op2Valid;
	int op2;
	int result;
	int effectiveAddr;
	char report[128];
	enum stageStatus_enum status;
	int branch_taken;
};

struct CC_struct {
		int z;
		int p;
	};

enum fwdBustype_enum{
	ONEFWD,
	TWOFWD,
	THREEFWD
};

struct fwdBus_struct {
	int tag;
	int valid;
	int value;
	enum fwdBustype_enum fwdtype;
};

struct storevalue_struct{
	int v;
	int tag;
	int value;
};

struct address_struct{
	int v;
	int value;
};

struct source_struct{
	int v;
	int tag;
	int value;
};

struct instructionQueue_struct{
	int a;
	int mnemonic;
	int fu;
	struct source_struct source1;
	struct source_struct source2;
	int lp;
	int dest;
	int btag;
};

struct loadstoreQueue_struct{
	int a;
	int ls;
	struct storevalue_struct storeval;
	struct address_struct address;
	struct CC_struct cc;
	int dest;
};

struct pregs_struct{
	int v;
	int value;
	int p;
	int z;
};

struct rob_struct{
	int a;
	int pc;
	int areg;
	int preg;
	int lsq;
	int btag;
	int ppr;
};

struct apexCPU_struct {
	int pc;
	int reg[16];
	int regValid[16];
	int rat[16];
	int rrat[16];
	int prf_freelist[32];
	int prcc;
	struct CC_struct cc;
	struct apexStage_struct stage[writeback+1];
	int codeMem[128]; // addresses 0x4000 - 0x4200
	int dataMem[128]; // addresses 0x0000 - 0x0200
	int lowMem;
	int highMem;
	int t;
	int numInstructions;
	int instr_retired;
	int halt_fetch;
	int stop;
	char abend[64];
	struct fwdBus_struct fwdBus[3];
	struct instructionQueue_struct iq[6];
	struct rob_struct rob[6];
	struct loadstoreQueue_struct lsq[6];
	struct pregs_struct preg[32];
	int headtail[2];
	int piq;
};
typedef struct apexCPU_struct * cpu;

#define isFU(x) (x>=fu_alu1 && x<=fu_br3)

extern char *stageName[writeback+1]; // defined/initialized in apexCPU.c
typedef void (*opStageFn)(cpu cpu); // Needed in apexOpcodes.h

#include "apexOpcodes.h"


void initCPU(cpu cpu);
void loadCPU(cpu cpu,char * objFileName);
void printState(cpu cpu);
void cycleCPU(cpu cpu);
void printStats(cpu cpu);
void reportStage(cpu cpu,enum stage_enum s,const char* fmt,...);

#endif