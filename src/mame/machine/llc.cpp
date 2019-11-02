// license:BSD-3-Clause
// copyright-holders:Miodrag Milanovic, Robbbert
/***************************************************************************

        LLC driver by Miodrag Milanovic

        17/04/2009 Preliminary driver.

****************************************************************************/


#include "emu.h"
#include "includes/llc.h"


// LLC1 BASIC keyboard
READ8_MEMBER(llc_state::llc1_port2_b_r)
{
	uint8_t retVal = 0;

	if (m_term_status)
	{
		retVal = m_term_status;
		m_term_status = 0;
	}
	else
		retVal = m_term_data;

	return retVal;
}

READ8_MEMBER(llc_state::llc1_port2_a_r)
{
	return 0;
}

// LLC1 Monitor keyboard
READ8_MEMBER(llc_state::llc1_port1_a_r)
{
	uint8_t data = 0;
	if (!BIT(m_porta, 4))
		data = ioport("X4")->read();
	if (!BIT(m_porta, 5))
		data = ioport("X5")->read();
	if (!BIT(m_porta, 6))
		data = ioport("X6")->read();
	if (data & 0xf0)
		data = (data >> 4) | 0x80;

	data |= (m_porta & 0x70);

	// do not repeat key
	if (data & 15)
	{
		if (data == m_llc1_key)
			data &= 0x70;
		else
			m_llc1_key = data;
	}
	else
	if ((data & 0x70) == (m_llc1_key & 0x70))
		m_llc1_key = 0;

	return data;
}

WRITE8_MEMBER(llc_state::llc1_port1_a_w)
{
	m_porta = data;
}

WRITE8_MEMBER(llc_state::llc1_port1_b_w)
{
	static uint8_t count = 0, digit = 0;

	if (data == 0)
	{
		digit = 0;
		count = 0;
	}
	else
		count++;

	if (count == 1)
	{
		if (digit < 8)
			m_digits[digit] = data & 0x7f;
	}
	else
	if (count == 3)
	{
		count = 0;
		digit++;
	}
}

void llc_state::init_llc1()
{
}

MACHINE_RESET_MEMBER(llc_state,llc1)
{
	m_term_status = 0;
	m_llc1_key = 0;
}

MACHINE_START_MEMBER(llc_state,llc1)
{
	m_digits.resolve();
}

/* Driver initialization */
void llc_state::init_llc2()
{
	m_p_videoram = m_ram->pointer() + 0xc000;
}

MACHINE_RESET_MEMBER(llc_state,llc2)
{
	address_space &space = m_maincpu->space(AS_PROGRAM);

	space.unmap_write(0x0000, 0xbfff);
	space.install_rom(0x0000, 0xbfff, memregion("maincpu")->base());
	space.install_ram(0xc000, 0xffff, m_ram->pointer() + 0xc000);

}

WRITE8_MEMBER(llc_state::llc2_rom_disable_w)
{
	address_space &mem_space = m_maincpu->space(AS_PROGRAM);
	uint8_t *ram = m_ram->pointer();

	mem_space.install_ram(0x0000, 0xffff, ram);

}

WRITE8_MEMBER(llc_state::llc2_basic_enable_w)
{
	address_space &mem_space = m_maincpu->space(AS_PROGRAM);
	if (data & 0x02)
	{
		mem_space.unmap_write(0x4000, 0x5fff);
		mem_space.install_rom(0x4000, 0x5fff, memregion("maincpu")->base() + 0x10000);
	}
	else
	{
		mem_space.install_ram(0x4000, 0x5fff, m_ram->pointer() + 0x4000);
	}

}

READ8_MEMBER(llc_state::llc2_port1_b_r)
{
	return 0;
}

WRITE8_MEMBER(llc_state::llc2_port1_b_w)
{
	m_speaker->level_w(BIT(data, 6));
	m_rv = BIT(data, 5);
}

READ8_MEMBER(llc_state::llc2_port2_a_r)
{
	return 0; // bit 2 low or hangs on ^Z^X^C sequence
}
