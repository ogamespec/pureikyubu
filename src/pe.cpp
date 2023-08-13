// PE - pixel engine
#include "pch.h"

using namespace Debug;

// Handling access to the PE registers available to the CPU and EFB

namespace GX
{

	uint16_t GXCore::PeReadReg(PEMappedRegister id)
	{
		switch (id)
		{
			case PEMappedRegister::PE_PI_INTRCTRL_ID:
				return state.peregs.sr;

			case PEMappedRegister::PE_PI_TOKEN_ID:
				return state.peregs.token;

			default:
				return 0;
		}
	}

	void GXCore::PeWriteReg(PEMappedRegister id, uint16_t value)
	{
		switch (id)
		{
			case PEMappedRegister::PE_PI_INTRCTRL_ID:

				// clear interrupts
				if (state.peregs.sr & PE_SR_DONE)
				{
					state.peregs.sr &= ~PE_SR_DONE;
					PIClearInt(PI_INTERRUPT_PE_FINISH);
				}
				if (state.peregs.sr & PE_SR_TOKEN)
				{
					state.peregs.sr &= ~PE_SR_TOKEN;
					PIClearInt(PI_INTERRUPT_PE_TOKEN);
				}

				// set mask bits
				if (value & PE_SR_DONEMSK) state.peregs.sr |= PE_SR_DONEMSK;
				else state.peregs.sr &= ~PE_SR_DONEMSK;
				if (value & PE_SR_TOKENMSK) state.peregs.sr |= PE_SR_TOKENMSK;
				else state.peregs.sr &= ~PE_SR_TOKENMSK;

				break;

			default:
				break;
		}
	}

	// Currently, Cpu2Efb emulation is not supported. It is planned to be forwarded to the graphic Backend.

	uint32_t GXCore::EfbPeek(uint32_t addr)
	{
		Report(Channel::GP, "EfbPeek, address: 0x%08X\n", addr);
		return 0;
	}

	void GXCore::EfbPoke(uint32_t addr, uint32_t value)
	{
		Report(Channel::GP, "EfbPoke, address: 0x%08X, value: 0x%08X\n", addr, value);
	}

	// set clear rules
	void GXCore::GL_SetClear(Color clr, uint32_t z)
	{
		cr = clr.R;
		cg = clr.G;
		cb = clr.B;
		ca = clr.A;
		clear_z = z;
		set_clear = true;

		/*/
			if(set_clear)
			{
				glClearColor(
					(float)(cr / 255.0f),
					(float)(cg / 255.0f),
					(float)(cb / 255.0f),
					(float)(ca / 255.0f)
				);

				glClearDepth((double)(clear_z / 16777215.0));

				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				set_clear = FALSE;
			}
		/*/
	}

	// sel:0 - file, sel:1 - memory
	void GXCore::GL_DoSnapshot(bool sel, FILE* f, uint8_t* dst, int width, int height)
	{
		uint8_t      hdr[14 + 40];   // bmp header
		uint16_t* phdr;
		uint16_t     s, t;
		uint8_t* buf, * ptr;
		float   ds, dt, d0, d1;
		bool    linear = false;

		// allocate temporary buffer
		buf = (uint8_t*)malloc(scr_w * scr_h * 3);

		// calculate aspects
		ds = (float)scr_w / (float)width;
		dt = (float)scr_h / (float)height;
		if (ds != 1.0f) linear = true;

		// write hardcoded header
		memset(hdr, 0, sizeof(hdr));
		hdr[0] = 'B'; hdr[1] = 'M'; hdr[2] = 0x36;
		hdr[4] = 0x20; hdr[10] = 0x36;
		hdr[14] = 40;
		phdr = (uint16_t*)(&hdr[0x12]); *phdr = (uint16_t)width;
		phdr = (uint16_t*)(&hdr[0x16]); *phdr = (uint16_t)height;
		hdr[26] = 1; hdr[28] = 24; hdr[36] = 0x20;
		if (sel)
		{
			memcpy(dst, hdr, sizeof(hdr));
			dst += sizeof(hdr);
		}
		else fwrite(hdr, 1, sizeof(hdr), f);

		// read opengl buffer
		glReadPixels(0, 0, scr_w, scr_h, GL_RGB, GL_UNSIGNED_BYTE, buf);

		// write texture image
		for (t = 0, d0 = 0; t < height; t++, d0 += dt)
		{
			for (s = 0, d1 = 0; s < width; s++, d1 += ds)
			{
				uint8_t  prev[3] = { 0 };
				uint8_t  rgb[3];     // RGB triplet
				ptr = &buf[3 * (scr_w * (int)d0 + (int)d1)];
				{
					// linear filter
					if (s && linear)
					{
						rgb[2] = (*ptr++ + prev[2]) >> 1;
						rgb[1] = (*ptr++ + prev[1]) >> 1;
						rgb[0] = (*ptr++ + prev[0]) >> 1;
					}
					else
					{
						rgb[2] = *ptr++;
						rgb[1] = *ptr++;
						rgb[0] = *ptr++;
					}

					if (linear)
					{
						prev[2] = rgb[2];
						prev[1] = rgb[1];
						prev[0] = rgb[0];
					}

					if (sel) { memcpy(dst, rgb, 3); dst += 3; }
					else fwrite(rgb, 1, 3, f);
				}
			}
		}

		free(buf);
	}

	void GXCore::GL_MakeSnapshot(char* path)
	{
		if (make_shot) return;
		snap_w = scr_w, snap_h = scr_h;
		// create new file    
		if (snap_file)
		{
			fclose(snap_file);
			snap_file = NULL;
		}
		snap_file = fopen(path, "wb");
		if (snap_file) make_shot = true;
	}

	// make small snapshot for savestate
	// new size 160x120
	void GXCore::GL_SaveBitmap(uint8_t* buf)
	{
		GL_DoSnapshot(true, NULL, buf, 160, 120);
	}

	std::string GXCore::PE_GenShader()
	{
		std::string text;

		text += "#version 330 core\n";

		// From the vertex shader
		text += "uniform vec4 color0;\n";
		text += "uniform vec4 color1;\n";

		text += "void main()\n";
		text += "{\n";
		text += "    gl_FragColor = color0;\n";
		text += "}";

		return text;
	}

	void GXCore::PE_UploadShader(const char * source)
	{
		char infoLog[0x1000]{};
		int success;

		// Perform magic spells to compile the shader program

		frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(frag_shader, 1, &source, nullptr);
		glCompileShader(frag_shader);

		glGetShaderiv(frag_shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(frag_shader, sizeof(infoLog), nullptr, infoLog);
			Report(Channel::PE, "SHADER COMPILE ERROR: %s\n", infoLog);
			Halt("Halted.\n");
		};

		// Perform magic spells to "link" shader programs and further use them instead of a fixed pipeline

		shader_prog = glCreateProgram();
		glAttachShader(shader_prog, frag_shader);
		glLinkProgram(shader_prog);

		glGetProgramiv(shader_prog, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shader_prog, sizeof(infoLog), nullptr, infoLog);
			Report(Channel::PE, "SHADER LINK ERROR: %s\n", infoLog);
			Halt("Halted.\n");
		}

		glDeleteShader(frag_shader);

		glUseProgram(shader_prog);

		Report(Channel::PE, "Shader program is uploaded to GPU\n");
	}

	void GXCore::PE_DisposeShader()
	{
		// TODO: Is that enough?

		//glDeleteProgram(shader_prog);
	}

}

//
// Stubs
//

static void PERegRead(uint32_t addr, uint32_t* reg)
{
	*reg = Flipper::Gx->PeReadReg((GX::PEMappedRegister)((addr & 0xFF) >> 1));
}

static void PERegWrite(uint32_t addr, uint32_t data)
{
	Flipper::Gx->PeWriteReg((GX::PEMappedRegister)((addr & 0xFF) >> 1), data);
}

void PEOpen()
{
	Report(Channel::CP, "Pixel Engine (for GX)\n");

	// Pixel Engine
	PISetTrap(16, PI_REG16_TO_SPACE(PI_REGSPACE_PE, GX::PEMappedRegister::PE_PI_ZMODE_ID), PERegRead, PERegWrite);
	PISetTrap(16, PI_REG16_TO_SPACE(PI_REGSPACE_PE, GX::PEMappedRegister::PE_PI_CMODE0_ID), PERegRead, PERegWrite);
	PISetTrap(16, PI_REG16_TO_SPACE(PI_REGSPACE_PE, GX::PEMappedRegister::PE_PI_CMODE1_ID), PERegRead, PERegWrite);
	PISetTrap(16, PI_REG16_TO_SPACE(PI_REGSPACE_PE, GX::PEMappedRegister::PE_PI_ALPHA_THRES_ID), PERegRead, PERegWrite);
	PISetTrap(16, PI_REG16_TO_SPACE(PI_REGSPACE_PE, GX::PEMappedRegister::PE_PI_CONTROL_ID), PERegRead, PERegWrite);
	PISetTrap(16, PI_REG16_TO_SPACE(PI_REGSPACE_PE, GX::PEMappedRegister::PE_PI_INTRCTRL_ID), PERegRead, PERegWrite);
	PISetTrap(16, PI_REG16_TO_SPACE(PI_REGSPACE_PE, GX::PEMappedRegister::PE_PI_TOKEN_ID), PERegRead, PERegWrite);

	PISetTrap(16, PI_REG16_TO_SPACE(PI_REGSPACE_PE, GX::PEMappedRegister::PE_PI_PERF_COUNTER_0L_ID), PERegRead, PERegWrite);
	PISetTrap(16, PI_REG16_TO_SPACE(PI_REGSPACE_PE, GX::PEMappedRegister::PE_PI_PERF_COUNTER_0H_ID), PERegRead, PERegWrite);
	PISetTrap(16, PI_REG16_TO_SPACE(PI_REGSPACE_PE, GX::PEMappedRegister::PE_PI_PERF_COUNTER_1L_ID), PERegRead, PERegWrite);
	PISetTrap(16, PI_REG16_TO_SPACE(PI_REGSPACE_PE, GX::PEMappedRegister::PE_PI_PERF_COUNTER_1H_ID), PERegRead, PERegWrite);
	PISetTrap(16, PI_REG16_TO_SPACE(PI_REGSPACE_PE, GX::PEMappedRegister::PE_PI_PERF_COUNTER_2L_ID), PERegRead, PERegWrite);
	PISetTrap(16, PI_REG16_TO_SPACE(PI_REGSPACE_PE, GX::PEMappedRegister::PE_PI_PERF_COUNTER_2H_ID), PERegRead, PERegWrite);
	PISetTrap(16, PI_REG16_TO_SPACE(PI_REGSPACE_PE, GX::PEMappedRegister::PE_PI_PERF_COUNTER_3L_ID), PERegRead, PERegWrite);
	PISetTrap(16, PI_REG16_TO_SPACE(PI_REGSPACE_PE, GX::PEMappedRegister::PE_PI_PERF_COUNTER_3H_ID), PERegRead, PERegWrite);
	PISetTrap(16, PI_REG16_TO_SPACE(PI_REGSPACE_PE, GX::PEMappedRegister::PE_PI_PERF_COUNTER_4L_ID), PERegRead, PERegWrite);
	PISetTrap(16, PI_REG16_TO_SPACE(PI_REGSPACE_PE, GX::PEMappedRegister::PE_PI_PERF_COUNTER_4H_ID), PERegRead, PERegWrite);
	PISetTrap(16, PI_REG16_TO_SPACE(PI_REGSPACE_PE, GX::PEMappedRegister::PE_PI_PERF_COUNTER_5L_ID), PERegRead, PERegWrite);
	PISetTrap(16, PI_REG16_TO_SPACE(PI_REGSPACE_PE, GX::PEMappedRegister::PE_PI_PERF_COUNTER_5H_ID), PERegRead, PERegWrite);
}

void PEClose()
{
}
