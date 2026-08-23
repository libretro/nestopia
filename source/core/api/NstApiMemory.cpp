////////////////////////////////////////////////////////////////////////////////////////
//
// Nestopia - NES/Famicom emulator written in C++
//
// Copyright (C) 2003-2008 Martin Freij
//
// This file is part of Nestopia.
//
// Nestopia is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// Nestopia is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Nestopia; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
////////////////////////////////////////////////////////////////////////////////////////

#include "../NstMachine.hpp"
#include "../NstImage.hpp"
#include "NstApiMemory.hpp"

namespace Nes
{
	namespace Api
	{
		ulong Memory::NumRegions() const throw()
		{
			if (!emulator.image)
				return 0;

			// Region 0 is CPU RAM; the image contributes the rest.
			return 1 + emulator.image->NumMemoryRegions();
		}

		Memory::Region Memory::GetRegion(ulong index) const throw()
		{
			Region region;

			region.kind    = KIND_SYSTEM_RAM;
			region.address = 0x0000;
			region.size    = 0;
			region.data    = NULL;
			region.battery = false;

			if (index >= NumRegions())
				return region;

			if (index == 0)
			{
				region.size = Core::Cpu::RAM_SIZE;
				region.data = emulator.cpu.GetRam();
				return region;
			}

			const Core::Image::MemoryRegion source
			(
				emulator.image->GetMemoryRegion( uint(index) - 1 )
			);

			switch (source.kind)
			{
				case Core::Image::MemoryRegion::KIND_EXPANSION_RAM:

					region.kind = KIND_EXPANSION_RAM;
					break;

				case Core::Image::MemoryRegion::KIND_DISK_RAM:

					region.kind = KIND_DISK_RAM;
					break;

				default:

					region.kind = KIND_WORK_RAM;
					break;
			}

			region.address = source.address;
			region.size    = source.size;
			region.data    = source.data;
			region.battery = source.battery;

			return region;
		}
	}
}
