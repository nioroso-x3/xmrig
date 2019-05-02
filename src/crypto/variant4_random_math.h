#ifndef VARIANT4_RANDOM_MATH_H
#define VARIANT4_RANDOM_MATH_H
#include <sys/mman.h>
extern "C"
{
    #include "c_blake256.h"
}
typedef uint64_t(*fn2)(uint32_t*,uint32_t*);
typedef uint64_t(*fn1)(uint32_t*);
#define INST_LEN  2048

enum V4_Settings
{
	// Generate code with minimal theoretical latency = 45 cycles, which is equivalent to 15 multiplications
	TOTAL_LATENCY = 15 * 3,
	
	// Always generate at least 60 instructions
	NUM_INSTRUCTIONS_MIN = 60,

	// Never generate more than 70 instructions (final RET instruction doesn't count here)
	NUM_INSTRUCTIONS_MAX = 70,

	// Available ALUs for mul_
	// Modern CPUs typically have only 1 ALU which can do multiplications
	ALU_COUNT_mul_ = 1,

	// Total available ALUs
	// Modern CPUs have 4 ALUs, but we use only 3 because random math executes together with other main loop code
	ALU_COUNT = 3,
};

enum V4_InstructionList
{
	mul_,	// a*b
	add_,	// a+b + C, C is an unsigned 32-bit constant
	sub_,	// a-b
	ror_,	// rotate right "a" by "b & 31" bits
	rol_,	// rotate left "a" by "b & 31" bits
	xor_,	// a^b
	RET,	// finish execution
	V4_INSTRUCTION_COUNT = RET,
};

// V4_InstructionDefinition is used to generate code from random data
// Every random sequence of bytes is a valid code
//
// There are 9 registers in total:
// - 4 variable registers
// - 5 constant registers initialized from loop variables
// This is why dst_index is 2 bits
enum V4_InstructionDefinition
{
	V4_OPCODE_BITS = 3,
	V4_DST_INDEX_BITS = 2,
	V4_SRC_INDEX_BITS = 3,
};

struct V4_Instruction
{
	uint16_t opcode;   //pad to 64bits
	uint8_t dst_index;
	uint8_t src_index;
	uint32_t C;
};
typedef struct V4_Instruction v4_ins; 
#ifndef FORCEINLINE
#ifdef __GNUC__
#define FORCEINLINE __attribute__((always_inline)) inline
#elif _MSC_VER
#define FORCEINLINE __forceinline
#else
#define FORCEINLINE inline
#endif
#endif


#ifndef UNREACHABLE_CODE
#ifdef __GNUC__
#define UNREACHABLE_CODE __builtin_unreachable()
#elif _MSC_VER
#define UNREACHABLE_CODE __assume(false)
#else
#define UNREACHABLE_CODE
#endif
#endif

#define D(d)    (d << 21)
#define S(s)		(s << 21)
#define A(a)		(a << 16)
#define B(b)		(b << 11)
#define MC(c)		(c << 6)
#define ME(e)		(e << 1)
#define IMM(imm)	((imm) & 0xffff)
#define CRD(d)		((d) << 21)

#define HI(opcode)	((opcode) << 26)
#define LO(opcode)	((opcode) << 1)

#define ADD		(uint32_t)(HI(31) | LO(266))
#define ADDI  (uint32_t)(HI(14))
#define ADDIS (uint32_t)(HI(15))
#define LD		(uint32_t)(HI(58) | 0)
#define LWZ		(uint32_t)(HI(32))
#define MULLW		(uint32_t)(HI(31) | LO(235))
#define NEG		(uint32_t)(HI(31) | LO(104))
#define OR	 	(uint32_t)(HI(31) | LO(444))
#define ORI    (uint32_t)(HI(24))
#define RLWINM (uint32_t)(HI(21))
#define SLW		(uint32_t)(HI(31) | LO(24))
#define SRW		(uint32_t)(HI(31) | LO(536))
#define STW		(uint32_t)(HI(36))
#define SUBF		(uint32_t)(HI(31) | LO(40))
#define XOR		(uint32_t)(HI(31) | LO(316))
#define SUBFIC (uint32_t)(HI(8))
#define CLRLWI  (uint32_t)9999
#define ROTLW (uint32_t)HI(23)
#define RLWINM_27_31 (uint32_t)9997
uint32_t gen_op(uint32_t op,uint32_t a0, uint32_t a1, uint32_t a2 ){
  switch (op){
    case(ADD):
      op = ADD | D((uint8_t)a0) | A((uint8_t)a1) | B((uint8_t)a2);
      break;
    case(SUBF):
      op = SUBF | D((uint8_t)a0) | A((uint8_t)a1) | B((uint8_t)a2);
      break;
    case(MULLW):
      op = MULLW | D((uint8_t)a0) | A((uint8_t)a1) | B((uint8_t)a2);
      break;
    case(XOR):
      op = XOR | D((uint8_t)a0) | A((uint8_t)a1) | B((uint8_t)a2);
      break;
    case(LWZ):
      op = LWZ | D((uint8_t)a0) | A((uint8_t)a1) | IMM((uint16_t)a2);
      break;
    case(SUBFIC):
      op = SUBFIC | D((uint8_t)a0) | A((uint8_t)a1) | IMM((uint16_t)a2);
      break;
    case(CLRLWI):
      op = RLWINM | S((uint8_t)a1) | A((uint8_t)a0) | B((uint8_t)0) | MC((uint8_t)a2) | ME((uint8_t)31);
      break;
    case(NEG):
      op = NEG | D((uint8_t)a0) | A((uint8_t)a1);
      break;
    case(SRW):
      op = SRW | S((uint8_t)a1) | A((uint8_t)a0) | B((uint8_t)a2);
      break;
    case(SLW):
      op = SLW | S((uint8_t)a1) | A((uint8_t)a0) | B((uint8_t)a2);
      break;
    case(OR):
      op = OR | S((uint8_t)a1) | A((uint8_t)a0) | B((uint8_t)a2);
      break;
    case(ORI):
      op = ORI | S((uint8_t)a1) | A((uint8_t)a0) | IMM((uint16_t)a2);
      break;
    case(ADDIS):
      op = ADDIS | D((uint8_t)a0) | A((uint8_t)a1) | IMM((uint16_t)a2);
      break;
    case(ADDI):
      op = ADDI | D((uint8_t)a0) | A((uint8_t)a1) | IMM((uint16_t)a2);
      break;
    case(ROTLW):
      op = ROTLW | S((uint8_t)a1) | A((uint8_t)a0) | B((uint8_t)a2) | MC((uint8_t)0) | ME((uint8_t)31);
      break;
    case(RLWINM_27_31):
      op = RLWINM | S((uint8_t)a1) | A((uint8_t)a0) | B((uint8_t)a2) | MC((uint8_t)27) | ME((uint8_t)31);
      break;
  }
  return op;
}
//powerpc code hex
uint32_t prolog[] = {
0x7c0802a6,   //mflr r0 save lr to r0
0xf8010010,  //std r0 16(r1) save r0 to stack
0x0000000 //end stream
};

uint32_t save_reg [] = {
0xf9c1ff70, //    std     r14,-144(r1)
0xf9e1ff78, //    std     r15,-136(r1)
0xfa01ff80, //    std     r16,-128(r1)
0xfa21ff88, //    std     r17,-120(r1)
0xfa41ff90, //    std     r18,-112(r1)
0x00000000
};
uint32_t restore_reg [] = {
0xe9c1ff70,  //   ld      r14,-144(r1)
0xe9e1ff78,  //   ld      r15,-136(r1)
0xea01ff80,  //   ld      r16,-128(r1)
0xea21ff88,  //   ld      r17,-120(r1)
0xea41ff90,  //   ld      r18,-112(r1)
0x00000000
};
uint32_t r3_to_reg[] = {
0x80e30000,//    lwz     r7,0(r3)
0x81030004, //    lwz     r8,4(r3)
0x81230008, //    lwz     r9,8(r3)
0x8143000c, //    lwz     r10,12(r3)
0x81c30010, //    lwz     r14,16(r3)
0x81e30014, //    lwz     r15,20(r3)
0x82030018, //    lwz     r16,24(r3)
0x8223001c, //    lwz     r17,28(r3)
0x82430020, //    lwz     r18,32(r3)
0x00000000
};
uint32_t reg_to_r3[] = {
0x90e30000, //    stw     r7,0(r3)
0x91030004, //    stw     r8,4(r3)
0x91230008, //    stw     r9,8(r3)
0x9143000c, //    stw     r10,12(r3)
0x91c30010, //    stw     r14,16(r3)
0x91e30014, //    stw     r15,20(r3)
0x92030018, //    stw     r16,24(r3)
0x9223001c, //    stw     r17,28(r3)
0x92430020, //    stw     r18,32(r3)
0x00000000
};

uint32_t epilog[] = {
0xe8010010 ,  //ld r0,16(r1) load lr from stack to r0
0x7c0803a6 ,  //restore link register
0x4e800020, //jump to lr
0x00000000 //end stream
};

void* JIT_init() {
  //allocate exec memory
  //printf("Started JIT");
  uint32_t* memory = (uint32_t*)mmap(NULL,             // address
                      INST_LEN*sizeof(uint32_t),             // size
                      PROT_READ | PROT_WRITE | PROT_EXEC,
                      MAP_PRIVATE | MAP_ANONYMOUS,
                      -1,               // fd (not used here)
                      0);               // offset (not used here)
  //printf("Started JIT at %p\n",memory);
  return (void*)memory;
}

void JIT_load(void* execmem, uint32_t* code){
  uint32_t idx = 0;
  uint32_t* inst = (uint32_t*)execmem;
  for (;idx < INST_LEN; ++idx){
    if (inst[idx] == 0x00000000){
      inst = inst+idx;
      //printf("Currently %d instructions in buffer\n",idx);
      break;
    }
  }
  for (idx = 0; code[idx] != 0x00000000; ++idx){
    inst[idx] = code[idx];
  }

}
void JIT_end(void* execmem){
  //printf("Ended JIT at %p\n",execmem);
  munmap(execmem, INST_LEN*sizeof(uint32_t));
}

void* JIT_compile_v3(v4_ins* op)
{
  //this function takes only one argument, pointer to data. C values are encoded directly in immediate add instructions.
  //data is loaded in registers 7,8,9,10,14,15,16,17,18
  //as of now, this generator is as fast as gcc-8 and clang-9 generated code. 
  void* f = JIT_init();
  uint8_t regN[] = {7,8,9,10,14,15,16,17,18};
  uint8_t useRegs0[] = {4,5,6,11, 12,};
  JIT_load(f,prolog);
  JIT_load(f,save_reg);
  JIT_load(f,r3_to_reg);
  uint32_t s = 0;
  for (uint32_t i = 0; i < 70; ++i)
	{ 
    uint8_t dst = op[i].dst_index;
    uint8_t src = op[i].src_index;
    uint32_t tmp[] = {0,0,0,0,0,0,0,0};
    int16_t* C;
    uint8_t r0 = useRegs0[s%sizeof(useRegs0)];
    switch (op[i].opcode) 
		{ 
		case mul_: 
      tmp[0] = gen_op(MULLW,regN[dst],regN[dst],regN[src]);
      JIT_load(f,tmp);
			break; 
		case add_:
      C = (int16_t*)&op[i].C;
      tmp[0] = gen_op(ADD,regN[dst],regN[dst],regN[src]);
      if(C[0] < 0){
        tmp[1] = gen_op(ADDIS,regN[dst],regN[dst],C[1]+1);
      }else{
        tmp[1] = gen_op(ADDIS,regN[dst],regN[dst],C[1]);
      }
      tmp[2] = gen_op(ADDI,regN[dst],regN[dst],C[0]); 
      JIT_load(f,tmp);
			break; 
		case sub_: 
      tmp[0] = gen_op(SUBF,regN[dst],regN[src],regN[dst]);
      JIT_load(f,tmp);
			break; 
		case ror_:
      tmp[0] = gen_op(NEG,r0,regN[src],0);
      tmp[1] = gen_op(ROTLW,regN[dst],regN[dst],r0);
      s++;
      JIT_load(f,tmp);
			break; 
		case rol_: 
      tmp[0] = gen_op(ROTLW,regN[dst],regN[dst],regN[src]);
      JIT_load(f,tmp);
			break; 
		case xor_: 
      tmp[0] = gen_op(XOR,regN[dst],regN[dst],regN[src]);
      JIT_load(f,tmp);
			break; 
		case RET: 
      JIT_load(f,reg_to_r3);
      JIT_load(f,restore_reg);
      JIT_load(f,epilog);
      return f;
			break; 
		default: 
			UNREACHABLE_CODE; 
			break; 
		}
	}
  return f;
}


// Random math interpreter's loop is fully unrolled and inlined to achieve 100% branch prediction on CPU:
// every switch-case will point to the same destination on every iteration of Cryptonight main loop
//
// This is about as fast as it can get without using low-level machine code generation


template<typename v4_reg>
static void v4_random_math(const struct V4_Instruction* code, v4_reg* r)
{
	enum
	{
		REG_BITS = sizeof(v4_reg) * 8,
	};

#define V4_EXEC(i) \
	{ \
		const struct V4_Instruction* op = code + i; \
		const v4_reg src = r[op->src_index]; \
		v4_reg* dst = r + op->dst_index; \
		switch (op->opcode) \
		{ \
		case mul_: \
			*dst *= src; \
			break; \
		case add_: \
			*dst += src + op->C; \
			break; \
		case sub_: \
			*dst -= src; \
			break; \
		case ror_: \
			{ \
				const uint32_t shift = src % REG_BITS; \
				*dst = (*dst >> shift) | (*dst << ((REG_BITS - shift) % REG_BITS)); \
			} \
			break; \
		case rol_: \
			{ \
				const uint32_t shift = src % REG_BITS; \
				*dst = (*dst << shift) | (*dst >> ((REG_BITS - shift) % REG_BITS)); \
			} \
			break; \
		case xor_: \
			*dst ^= src; \
			break; \
		case RET: \
			return; \
		default: \
			UNREACHABLE_CODE; \
			break; \
		} \
	}

#define V4_EXEC_10(j) \
	V4_EXEC(j + 0) \
	V4_EXEC(j + 1) \
	V4_EXEC(j + 2) \
	V4_EXEC(j + 3) \
	V4_EXEC(j + 4) \
	V4_EXEC(j + 5) \
	V4_EXEC(j + 6) \
	V4_EXEC(j + 7) \
	V4_EXEC(j + 8) \
	V4_EXEC(j + 9)

	// Generated program can have 60 + a few more (usually 2-3) instructions to achieve required latency
	// I've checked all block heights < 10,000,000 and here is the distribution of program sizes:
	//
	// 60      27960
	// 61      105054
	// 62      2452759
	// 63      5115997
	// 64      1022269
	// 65      1109635
	// 66      153145
	// 67      8550
	// 68      4529
	// 69      102

	// Unroll 70 instructions here
	V4_EXEC_10(0);		// instructions 0-9
	V4_EXEC_10(10);		// instructions 10-19
	V4_EXEC_10(20);		// instructions 20-29
	V4_EXEC_10(30);		// instructions 30-39
	V4_EXEC_10(40);		// instructions 40-49
	V4_EXEC_10(50);		// instructions 50-59
	V4_EXEC_10(60);		// instructions 60-69

#undef V4_EXEC_10
#undef V4_EXEC
}

// If we don't have enough data available, generate more
static FORCEINLINE void check_data(size_t* data_index, const size_t bytes_needed, int8_t* data, const size_t data_size)
{
	if (*data_index + bytes_needed > data_size)
	{
		hash_extra_blake(data, data_size, (char*) data);
		*data_index = 0;
	}
}

// Generates as many random math operations as possible with given latency and ALU restrictions
// "code" array must have space for NUM_INSTRUCTIONS_MAX+1 instructions



template<xmrig::Variant VARIANT>
static int v4_random_math_init(struct V4_Instruction* code, const uint64_t height)
{
	// mul_ is 3 cycles, 3-way addition and rotations are 2 cycles, sub_/xor_ are 1 cycle
	// These latencies match real-life instruction latencies for Intel CPUs starting from Sandy Bridge and up to Skylake/Coffee lake
	//
	// AMD Ryzen has the same latencies except 1-cycle ror_/rol_, so it'll be a bit faster than Intel Sandy Bridge and newer processors
	// Surprisingly, Intel Nehalem also has 1-cycle ror_/rol_, so it'll also be faster than Intel Sandy Bridge and newer processors
	// AMD Bulldozer has 4 cycles latency for mul_ (slower than Intel) and 1 cycle for ror_/rol_ (faster than Intel), so average performance will be the same
	// Source: https://www.agner.org/optimize/instruction_tables.pdf
	const int op_latency[V4_INSTRUCTION_COUNT] = { 3, 2, 1, 2, 2, 1 };

	// Instruction latencies for theoretical ASIC implementation
	const int asic_op_latency[V4_INSTRUCTION_COUNT] = { 3, 1, 1, 1, 1, 1 };

	// Available ALUs for each instruction
	const int op_ALUs[V4_INSTRUCTION_COUNT] = { ALU_COUNT_mul_, ALU_COUNT, ALU_COUNT, ALU_COUNT, ALU_COUNT, ALU_COUNT };

	int8_t data[32];
	memset(data, 0, sizeof(data));
	uint64_t tmp = SWAP64LE(height);
	memcpy(data, &tmp, sizeof(uint64_t));
	if (VARIANT == xmrig::VARIANT_4)
	{
		data[20] = -38;
	}

	// Set data_index past the last byte in data
	// to trigger full data update with blake hash
	// before we start using it
	size_t data_index = sizeof(data);

	int code_size;

	// There is a small chance (1.8%) that register R8 won't be used in the generated program
	// So we keep track of it and try again if it's not used
	bool r8_used;
	do {
		int latency[9];
		int asic_latency[9];

		// Tracks previous instruction and value of the source operand for registers R0-R3 throughout code execution
		// byte 0: current value of the destination register
		// byte 1: instruction opcode
		// byte 2: current value of the source register
		//
		// Registers R4-R8 are constant and are treated as having the same value because when we do
		// the same operation twice with two constant source registers, it can be optimized into a single operation
		uint32_t inst_data[9] = { 0, 1, 2, 3, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF };

		bool alu_busy[TOTAL_LATENCY + 1][ALU_COUNT];
		bool is_rotation[V4_INSTRUCTION_COUNT];
		bool rotated[4];
		int rotate_count = 0;

		memset(latency, 0, sizeof(latency));
		memset(asic_latency, 0, sizeof(asic_latency));
		memset(alu_busy, 0, sizeof(alu_busy));
		memset(is_rotation, 0, sizeof(is_rotation));
		memset(rotated, 0, sizeof(rotated));
		is_rotation[ror_] = true;
		is_rotation[rol_] = true;

		int num_retries = 0;
		code_size = 0;

		int total_iterations = 0;
		r8_used = (VARIANT == xmrig::VARIANT_WOW);

		// Generate random code to achieve minimal required latency for our abstract CPU
		// Try to get this latency for all 4 registers
		while (((latency[0] < TOTAL_LATENCY) || (latency[1] < TOTAL_LATENCY) || (latency[2] < TOTAL_LATENCY) || (latency[3] < TOTAL_LATENCY)) && (num_retries < 64))
		{
			// Fail-safe to guarantee loop termination
			++total_iterations;
			if (total_iterations > 256)
				break;

			check_data(&data_index, 1, data, sizeof(data));

			const uint8_t c = ((uint8_t*)data)[data_index++];

			// mul_ = opcodes 0-2
			// add_ = opcode 3
			// sub_ = opcode 4
			// ror_/rol_ = opcode 5, shift direction is selected randomly
			// xor_ = opcodes 6-7
			uint8_t opcode = c & ((1 << V4_OPCODE_BITS) - 1);
			if (opcode == 5)
			{
				check_data(&data_index, 1, data, sizeof(data));
				opcode = (data[data_index++] >= 0) ? ror_ : rol_;
			}
			else if (opcode >= 6)
			{
				opcode = xor_;
			}
			else
			{
				opcode = (opcode <= 2) ? mul_ : (opcode - 2);
			}

			uint8_t dst_index = (c >> V4_OPCODE_BITS) & ((1 << V4_DST_INDEX_BITS) - 1);
			uint8_t src_index = (c >> (V4_OPCODE_BITS + V4_DST_INDEX_BITS)) & ((1 << V4_SRC_INDEX_BITS) - 1);

			const int a = dst_index;
			int b = src_index;

			// Don't do add_/sub_/xor_ with the same register
			if (((opcode == add_) || (opcode == sub_) || (opcode == xor_)) && (a == b))
			{
				// a is always < 4, so we don't need to check bounds here
				b = (VARIANT == xmrig::VARIANT_WOW) ? (a + 4) : 8;
				src_index = b;
			}

			// Don't do rotation with the same destination twice because it's equal to a single rotation
			if (is_rotation[opcode] && rotated[a])
			{
				continue;
			}

			// Don't do the same instruction (except mul_) with the same source value twice because all other cases can be optimized:
			// 2xadd_(a, b, C) = add_(a, b*2, C1+C2), same for sub_ and rotations
			// 2xxor_(a, b) = NOP
			if ((opcode != mul_) && ((inst_data[a] & 0xFFFF00) == (opcode << 8) + ((inst_data[b] & 255) << 16)))
			{
				continue;
			}

			// Find which ALU is available (and when) for this instruction
			int next_latency = (latency[a] > latency[b]) ? latency[a] : latency[b];
			int alu_index = -1;
			while (next_latency < TOTAL_LATENCY)
			{
				for (int i = op_ALUs[opcode] - 1; i >= 0; --i)
				{
					if (!alu_busy[next_latency][i])
					{
						// add_ is implemented as two 1-cycle instructions on a real CPU, so do an additional availability check
						if ((opcode == add_) && alu_busy[next_latency + 1][i])
						{
							continue;
						}

						// Rotation can only start when previous rotation is finished, so do an additional availability check
						if (is_rotation[opcode] && (next_latency < rotate_count * op_latency[opcode]))
						{
							continue;
						}

						alu_index = i;
						break;
					}
				}
				if (alu_index >= 0)
				{
					break;
				}
				++next_latency;
			}

			// Don't generate instructions that leave some register unchanged for more than 7 cycles
			if (next_latency > latency[a] + 7)
			{
				continue;
			}

			next_latency += op_latency[opcode];

			if (next_latency <= TOTAL_LATENCY)
			{
				if (is_rotation[opcode])
				{
					++rotate_count;
				}

				// Mark ALU as busy only for the first cycle when it starts executing the instruction because ALUs are fully pipelined
				alu_busy[next_latency - op_latency[opcode]][alu_index] = true;
				latency[a] = next_latency;

				// ASIC is supposed to have enough ALUs to run as many independent instructions per cycle as possible, so latency calculation for ASIC is simple
				asic_latency[a] = ((asic_latency[a] > asic_latency[b]) ? asic_latency[a] : asic_latency[b]) + asic_op_latency[opcode];

				rotated[a] = is_rotation[opcode];

				inst_data[a] = code_size + (opcode << 8) + ((inst_data[b] & 255) << 16);

				code[code_size].opcode = opcode;
				code[code_size].dst_index = dst_index;
				code[code_size].src_index = src_index;
				code[code_size].C = 0;

				if (src_index == 8)
				{
					r8_used = true;
				}

				if (opcode == add_)
				{
					// add_ instruction is implemented as two 1-cycle instructions on a real CPU, so mark ALU as busy for the next cycle too
					alu_busy[next_latency - op_latency[opcode] + 1][alu_index] = true;

					// add_ instruction requires 4 more random bytes for 32-bit constant "C" in "a = a + b + C"
					check_data(&data_index, sizeof(uint32_t), data, sizeof(data));
					uint32_t t;
					memcpy(&t, data + data_index, sizeof(uint32_t));
					code[code_size].C = SWAP32LE(t);
					data_index += sizeof(uint32_t);
				}

				++code_size;
				if (code_size >= NUM_INSTRUCTIONS_MIN)
				{
					break;
				}
			}
			else
			{
				++num_retries;
			}
		}

		// ASIC has more execution resources and can extract as much parallelism from the code as possible
		// We need to add a few more mul_ and ror_ instructions to achieve minimal required latency for ASIC
		// Get this latency for at least 1 of the 4 registers
		const int prev_code_size = code_size;
		while ((code_size < NUM_INSTRUCTIONS_MAX) && (asic_latency[0] < TOTAL_LATENCY) && (asic_latency[1] < TOTAL_LATENCY) && (asic_latency[2] < TOTAL_LATENCY) && (asic_latency[3] < TOTAL_LATENCY))
		{
			int min_idx = 0;
			int max_idx = 0;
			for (int i = 1; i < 4; ++i)
			{
				if (asic_latency[i] < asic_latency[min_idx]) min_idx = i;
				if (asic_latency[i] > asic_latency[max_idx]) max_idx = i;
			}

			const uint8_t pattern[3] = { ror_, mul_, mul_ };
			const uint8_t opcode = pattern[(code_size - prev_code_size) % 3];
			latency[min_idx] = latency[max_idx] + op_latency[opcode];
			asic_latency[min_idx] = asic_latency[max_idx] + asic_op_latency[opcode];

			code[code_size].opcode = opcode;
			code[code_size].dst_index = min_idx;
			code[code_size].src_index = max_idx;
			code[code_size].C = 0;
			++code_size;
		}

	// There is ~98.15% chance that loop condition is false, so this loop will execute only 1 iteration most of the time
	// It never does more than 4 iterations for all block heights < 10,000,000
	}  while (!r8_used || (code_size < NUM_INSTRUCTIONS_MIN) || (code_size > NUM_INSTRUCTIONS_MAX));

	// It's guaranteed that NUM_INSTRUCTIONS_MIN <= code_size <= NUM_INSTRUCTIONS_MAX here
	// Add final instruction to stop the interpreter
	code[code_size].opcode = RET;
	code[code_size].dst_index = 0;
	code[code_size].src_index = 0;
	code[code_size].C = 0;

	return code_size;
}


#endif
