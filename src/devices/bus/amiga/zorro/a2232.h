// license:GPL-2.0+
// copyright-holders:Dirk Best
/***************************************************************************

    Commodore A2232

    Zorro-II Serial Card

    Provides the Amiga with 7 additional RS232 ports.

***************************************************************************/

#ifndef MAME_BUS_AMIGA_ZORRO_A2232_H
#define MAME_BUS_AMIGA_ZORRO_A2232_H

#pragma once

#include "zorro.h"
#include "machine/autoconfig.h"
#include "cpu/m6502/m65ce02.h"
#include "machine/input_merger.h"
#include "machine/mos6551.h"
#include "machine/mos6526.h"
#include "bus/rs232/rs232.h"


namespace bus { namespace amiga { namespace zorro {

//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> a2232_device

class a2232_device : public device_t, public device_zorro2_card_interface, public amiga_autoconfig
{
public:
	// construction/destruction
	a2232_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	// zorro slot
	DECLARE_READ16_MEMBER( shared_ram_r );
	DECLARE_WRITE16_MEMBER( shared_ram_w );
	DECLARE_READ16_MEMBER( irq_ack_r );
	DECLARE_WRITE16_MEMBER( irq_ack_w );
	DECLARE_READ16_MEMBER( reset_low_r );
	DECLARE_WRITE16_MEMBER( reset_low_w );
	DECLARE_READ16_MEMBER( irq_r );
	DECLARE_WRITE16_MEMBER( irq_w );
	DECLARE_READ16_MEMBER( reset_high_r );
	DECLARE_WRITE16_MEMBER( reset_high_w );

	void iocpu_map(address_map &map);

protected:
	virtual void device_add_mconfig(machine_config &config) override ATTR_COLD;
	virtual void device_start() override ATTR_COLD;
	virtual void device_reset_after_children() override;

	// device_zorro2_card_interface overrides
	virtual DECLARE_WRITE_LINE_MEMBER( cfgin_w ) override;

	// amiga_autoconfig overrides
	virtual void autoconfig_base_address(offs_t address) override;

private:
	// cpu
	WRITE8_MEMBER( int2_w );
	WRITE8_MEMBER( irq_ack8_w );

	// acia
	template<int N> DECLARE_READ8_MEMBER( acia_r );
	template<int N> DECLARE_WRITE8_MEMBER( acia_w );

	// cia
	DECLARE_READ8_MEMBER( cia_port_a_r );
	DECLARE_READ8_MEMBER( cia_port_b_r );
	DECLARE_WRITE8_MEMBER( cia_port_b_w );
	DECLARE_READ8_MEMBER( cia_r );
	DECLARE_WRITE8_MEMBER( cia_w );

	// rs232
	DECLARE_WRITE_LINE_MEMBER( rs232_1_rxd_w );
	DECLARE_WRITE_LINE_MEMBER( rs232_1_dcd_w );
	DECLARE_WRITE_LINE_MEMBER( rs232_1_cts_w );
	DECLARE_WRITE_LINE_MEMBER( rs232_2_dcd_w );
	DECLARE_WRITE_LINE_MEMBER( rs232_2_cts_w );
	DECLARE_WRITE_LINE_MEMBER( rs232_3_dcd_w );
	DECLARE_WRITE_LINE_MEMBER( rs232_3_cts_w );
	DECLARE_WRITE_LINE_MEMBER( rs232_4_dcd_w );
	DECLARE_WRITE_LINE_MEMBER( rs232_4_cts_w );
	DECLARE_WRITE_LINE_MEMBER( rs232_5_dcd_w );
	DECLARE_WRITE_LINE_MEMBER( rs232_5_cts_w );
	DECLARE_WRITE_LINE_MEMBER( rs232_6_dcd_w );
	DECLARE_WRITE_LINE_MEMBER( rs232_6_cts_w );
	DECLARE_WRITE_LINE_MEMBER( rs232_7_dcd_w );
	DECLARE_WRITE_LINE_MEMBER( rs232_7_cts_w );

	required_device<m65ce02_device> m_iocpu;
	required_device<input_merger_device> m_ioirq;
	required_device_array<mos6551_device, 7> m_acia;
	required_device<mos8520_device> m_cia;
	required_shared_ptr<uint8_t> m_shared_ram;

	uint8_t m_cia_port_a;
	uint8_t m_cia_port_b;
};

} } } // namespace bus::amiga::zorro

// device type definition
DECLARE_DEVICE_TYPE_NS(ZORRO_A2232, bus::amiga::zorro, a2232_device)

#endif // MAME_BUS_AMIGA_ZORRO_A2232_H
