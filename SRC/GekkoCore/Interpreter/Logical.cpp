// Logical Instructions
#include "../pch.h"
#include "InterpreterPrivate.h"

namespace Gekko
{

	// We use macro programming to compress the source code.
	// Now I am not very willing to use such things.

	#define GPR(n) (core->regs.gpr[info.paramBits[(n)]])

	// ra = rs & rb
	void Interpreter::_and(DecoderInfo& info)
	{
		GPR(0) = GPR(1) & GPR(2);
		core->regs.pc += 4;
	}

	// ra = rs & rb, CR0
	void Interpreter::and_d(DecoderInfo& info)
	{
		_and(info);
		COMPUTE_CR0(GPR(0));
	}

	// ra = rs & ~rb
	void Interpreter::andc(DecoderInfo& info)
	{
		GPR(0) = GPR(1) & (~GPR(2));
		core->regs.pc += 4;
	}

	// ra = rs & ~rb, CR0
	void Interpreter::andc_d(DecoderInfo& info)
	{
		andc(info);
		COMPUTE_CR0(GPR(0));
	}

	// ra = rs & UIMM, CR0
	void Interpreter::andi_d(DecoderInfo& info)
	{
		uint32_t res = GPR(1) & (uint32_t)info.Imm.Unsigned;
		GPR(0) = res;
		COMPUTE_CR0(res);
		core->regs.pc += 4;
	}

	// ra = rs & (UIMM || 0x0000), CR0
	void Interpreter::andis_d(DecoderInfo& info)
	{
		uint32_t res = GPR(1) & ((uint32_t)info.Imm.Unsigned << 16);
		GPR(0) = res;
		COMPUTE_CR0(res);
		core->regs.pc += 4;
	}

	// n = 0
	// while n < 32
	//      if rs[n] = 1 then leave
	//      n = n + 1
	// ra = n
	void Interpreter::cntlzw(DecoderInfo& info)
	{
		uint32_t n, m, rs = GPR(1);
		for (n = 0, m = 1 << 31; n < 32; n++, m >>= 1)
		{
			if (rs & m) break;
		}

		GPR(0) = n;
		core->regs.pc += 4;
	}

	// n = 0
	// while n < 32
	//      if rs[n] = 1 then leave
	//      n = n + 1
	// ra = n
	// CR0
	void Interpreter::cntlzw_d(DecoderInfo& info)
	{
		cntlzw(info);
		COMPUTE_CR0(GPR(0));
	}

	// ra = rs EQV rb
	void Interpreter::eqv(DecoderInfo& info)
	{
		GPR(0) = ~(GPR(1) ^ GPR(2));
		core->regs.pc += 4;
	}

	// ra = rs EQV rb, CR0
	void Interpreter::eqv_d(DecoderInfo& info)
	{
		eqv(info);
		COMPUTE_CR0(GPR(0));
	}

	// sign = rs[24]
	// ra[24-31] = rs[24-31]
	// ra[0-23] = (24)sign
	void Interpreter::extsb(DecoderInfo& info)
	{
		GPR(0) = (uint32_t)(int32_t)(int8_t)(uint8_t)GPR(1);
		core->regs.pc += 4;
	}

	// sign = rs[24]
	// ra[24-31] = rs[24-31]
	// ra[0-23] = (24)sign
	// CR0
	void Interpreter::extsb_d(DecoderInfo& info)
	{
		extsb(info);
		COMPUTE_CR0(GPR(0));
	}

	// sign = rs[16]
	// ra[16-31] = rs[16-31]
	// ra[0-15] = (16)sign
	void Interpreter::extsh(DecoderInfo& info)
	{
		GPR(0) = (uint32_t)(int32_t)(int16_t)(uint16_t)GPR(1);
		core->regs.pc += 4;
	}

	// sign = rs[16]
	// ra[16-31] = rs[16-31]
	// ra[0-15] = (16)sign
	// CR0
	void Interpreter::extsh_d(DecoderInfo& info)
	{
		extsh(info);
		COMPUTE_CR0(GPR(0));
	}

	// ra = ~(rs & rb)
	void Interpreter::nand(DecoderInfo& info)
	{
		GPR(0) = ~(GPR(1) & GPR(2));
		core->regs.pc += 4;
	}

	// ra = ~(rs & rb), CR0
	void Interpreter::nand_d(DecoderInfo& info)
	{
		nand(info);
		COMPUTE_CR0(GPR(0));
	}

	// ra = ~(rs | rb)
	void Interpreter::nor(DecoderInfo& info)
	{
		GPR(0) = ~(GPR(1) | GPR(2));
		core->regs.pc += 4;
	}

	// ra = ~(rs | rb), CR0
	void Interpreter::nor_d(DecoderInfo& info)
	{
		nor(info);
		COMPUTE_CR0(GPR(0));
	}

	// ra = rs | rb
	void Interpreter::_or(DecoderInfo& info)
	{
		GPR(0) = GPR(1) | GPR(2);
		core->regs.pc += 4;
	}

	// ra = rs | rb, CR0
	void Interpreter::or_d(DecoderInfo& info)
	{
		_or(info);
		COMPUTE_CR0(GPR(0));
	}

	// ra = rs | ~rb
	void Interpreter::orc(DecoderInfo& info)
	{
		GPR(0) = GPR(1) | (~GPR(2));
		core->regs.pc += 4;
	}

	// ra = rs | ~rb, CR0
	void Interpreter::orc_d(DecoderInfo& info)
	{
		orc(info);
		COMPUTE_CR0(GPR(0));
	}

	// ra = rs | (0x0000 || UIMM)
	void Interpreter::ori(DecoderInfo& info)
	{
		GPR(0) = GPR(1) | (uint32_t)info.Imm.Unsigned;
		core->regs.pc += 4;
	}

	// ra = rs | (UIMM || 0x0000)
	void Interpreter::oris(DecoderInfo& info)
	{
		GPR(0) = GPR(1) | ((uint32_t)info.Imm.Unsigned << 16);
		core->regs.pc += 4;
	}

	// ra = rs ^ rb
	void Interpreter::_xor(DecoderInfo& info)
	{
		GPR(0) = GPR(1) ^ GPR(2);
		core->regs.pc += 4;
	}

	// ra = rs ^ rb, CR0
	void Interpreter::xor_d(DecoderInfo& info)
	{
		_xor(info);
		COMPUTE_CR0(GPR(0));
	}

	// ra = rs ^ (0x0000 || UIMM)
	void Interpreter::xori(DecoderInfo& info)
	{
		GPR(0) = GPR(1) ^ (uint32_t)info.Imm.Unsigned;
		core->regs.pc += 4;
	}

	// ra = rs ^ (UIMM || 0x0000)
	void Interpreter::xoris(DecoderInfo& info)
	{
		GPR(0) = GPR(1) ^ ((uint32_t)info.Imm.Unsigned << 16);
		core->regs.pc += 4;
	}

}
