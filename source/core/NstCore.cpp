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

#include "NstCore.hpp"

#ifndef NST_NATIVE_QWORD

#if NST_GCC && !defined(__STRICT_ANSI__)
#warning "performance warning, no native 64bit integer support!"
#endif

#include "NstAssert.hpp"

namespace Nes
{
	void qaword::Multiply(qaword multiplier)
	{
		qaword multiplicand(*this);
		lo = 0;
		hi = 0;

		while (multiplicand)
		{
			if (multiplicand.lo & 0x1)
				(*this) += multiplier;

			multiplicand.lo = (multiplicand.lo >> 1) | (multiplicand.hi << 31 & LO_MASK);
			multiplicand.hi >>= 1;

			multiplier.hi = (multiplier.hi << 1 & LO_MASK) | (multiplier.lo >> 31);
			multiplier.lo = (multiplier.lo << 1 & LO_MASK);
		}
	}

	void qaword::Divide(qaword& dividend,const qaword divisor,const bool mod)
	{
		NST_ASSERT( bool(divisor) );

		qaword remainder(0);
		qaword quotient(0);

		if (divisor < dividend)
		{
			uint bits = 64;

			do
			{
				remainder.hi = (remainder.hi << 1 & LO_MASK) | (remainder.lo >> 31);
				remainder.lo = (remainder.lo << 1 & LO_MASK) | (dividend.hi >> 31);
				dividend.hi = (dividend.hi << 1 & LO_MASK) | (dividend.lo >> 31);
				dividend.lo = (dividend.lo << 1 & LO_MASK);
				--bits;
			}
			while (remainder < divisor);

			for (;;)
			{
				qaword tmp(remainder);
				tmp -= divisor;

				quotient.hi = (quotient.hi << 1 & LO_MASK) | (quotient.lo >> 31);
				quotient.lo = (quotient.lo << 1 & LO_MASK);

				if (!(tmp.hi & LO_MSB))
				{
					quotient.lo |= 0x1;
					remainder = tmp;
				}

				if (!bits)
					break;

				--bits;

				remainder.hi = (remainder.hi << 1 & LO_MASK) | (remainder.lo >> 31);
				remainder.lo = (remainder.lo << 1 & LO_MASK) | (dividend.hi >> 31);
				dividend.hi = (dividend.hi << 1 & LO_MASK) | (dividend.lo >> 31);
				dividend.lo = (dividend.lo << 1 & LO_MASK);
			}
		}
		else if (divisor == dividend)
		{
			quotient = 1;
		}
		else
		{
			remainder = dividend;
		}

		if (!mod)
			dividend = quotient;
		else
			dividend = remainder;
	}

	void qaword::Shl(const uint v)
	{
		NST_ASSERT( v < 64 );

		if (v)
		{
			if (v < 32)
			{
				dword t = lo >> (32-v);
				lo = (lo << v) & LO_MASK;
				hi = (hi << v | t) & LO_MASK;
			}
			else
			{
				hi = (lo << (v-32)) & LO_MASK;
				lo = 0;
			}
		}
	}

	void qaword::Shr(const uint v)
	{
		NST_ASSERT( v < 64 );

		if (v)
		{
			if (v < 32)
			{
				dword t = (hi << (32-v)) & LO_MASK;
				hi = hi >> v;
				lo = lo >> v | t;
			}
			else
			{
				lo = hi >> (v-32);
				hi = 0;
			}
		}
	}
}

#endif
