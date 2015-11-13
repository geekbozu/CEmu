/* CPU processing implementations */

#ifndef CONTEXT_H
#define CONTEXT_H

#include <core/cpu.h>
#include <core/registers.h>

#ifdef __cplusplus
extern "C" {
#endif

// Standard CONTEXT state
struct execution_context {
	int cycles;	// Number of cycles per execuction block
	eZ80cpu_t *cpu; // Pointer to Global CPU state
	union {
		uint8_t opcode;  // Current OpCode
		struct {
		  uint8_t z : 3;
		  uint8_t y : 3;
		  uint8_t x : 2;
		};
		struct {
		  uint8_t   : 3;
		  uint8_t q : 1;
		  uint8_t p : 2;
	  };
  };
  uint8_t (*nu)();     // ADL get next Unsigned byte
  int8_t (*ns)();      // ADL get next Signed byte
  uint32_t (*nw)();    // ADL get next Word
};

// Type definitions
typedef struct execution_context execution_context_t;

// Global CONTEXT state
extern execution_context_t context;
extern eZ80cpu_t cpu;

// Available Functions
void context_init();

void set_is_context();
void set_il_context();

uint8_t is_read_next_byte();
int8_t is_read_next_signed_byte();
uint32_t is_read_next_word();

uint8_t il_read_next_byte();
int8_t il_read_next_signed_byte();
uint32_t il_read_next_word();

uint8_t HorIHr();
uint8_t HorIHw(const uint8_t value);
uint8_t LorILr();
uint8_t LorILw(const uint8_t value);

uint32_t HLorIr();
uint32_t HLorIw(const uint32_t value);

uint8_t indHLorIr();
uint8_t indHLorIw(const uint8_t value);

uint8_t read_reg(const int i);
uint8_t write_reg(const int i, const uint8_t value);
uint8_t read_write_reg(const int read, const int write);

uint32_t read_rp(const int i);
uint32_t read_rp2(const int i);
uint32_t write_rp(const int i, const uint32_t value);
uint32_t write_rp2(const int i, const uint32_t value);

uint8_t read_cc(const int i);

#ifdef __cplusplus
}
#endif

#endif