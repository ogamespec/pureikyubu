// Main module with interface for Flipper (HW)

#include "pch.h"

namespace GX
{

	GXCore::GXCore()
	{
		JDI::Hub.AddNode(GX_JDI_JSON, gx_init_handlers);

		memset(&state, 0, sizeof(state));
	}

	GXCore::~GXCore()
	{
		JDI::Hub.RemoveNode(GX_JDI_JSON);
	}

	void GXCore::Open()
	{
		memset(&state, 0, sizeof(state));

		state.tickPerFifo = 100;
		state.updateTbrValue = Gekko::Gekko->GetTicks() + state.tickPerFifo;

		state.cp_thread = new Thread(CPThread, false, this, "CPThread");
	}

	void GXCore::Close()
	{
		if (state.cp_thread)
		{
			delete state.cp_thread;
			state.cp_thread = nullptr;
		}
	}

}