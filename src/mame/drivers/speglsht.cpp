// license:BSD-3-Clause
// copyright-holders:Tomasz Slanina
/*
Super Eagle Shot
(c)1994 Seta (distributed by Visco)
-----------------------------------
driver by Tomasz Slanina


GOLF
E30-001A

CPU           : Integrated Device IDT79R3051-25J 9407C (R3000A)
Sound+Objects : ST-0016
OSC           : 50.0000MHz (X1) 42.9545MHz (X3)

ROMs:
SX004-01.PR0 - R3051 Main programs (MX27C4000)
SX004-02.PR1 |
SX004-03.PR2 |
SX004-04.PR3 /

SX004-05.RD0 - Data and Graphics (D23C8000SCZ)
SX004-06.RD1 /

SX004-07.ZPR - ST-0016 Program and Data (16M mask, read as 27c160)

GALs (not dumped):
SX004-08.27 (16V8B)
SX004-09.46 (16V8B)
SX004-10.59 (16V8B)
SX004-11.61 (22V10B)
SX004-12.62 (22V10B)
SX004-13.63 (22V10B)

Custom chips:
SETA ST-0015 60EN502F12 JAPAN 9415YAI (U18, 208pin PQFP, system controller)
SETA ST-0016 TC6187AF JAPAN 9348YAA (U68, 208pin PQFP, sound & object)

 R3051    ST-0015              SX004-01   49.9545MHz    ST-0016       5588-25
                               SX004-02      52B256-70  514256 514256
 50MHz                         SX004-03      52B256-70  SX004-07
 528257-70 514256-70 514256-70 SX004-04
 528257-70 514256-70 514256-70 SX004-05
 528257-70 514256-70 514256-70 SX004-06
           514256-70 514256-70
                                                NEC D6376

PCB Layout
----------

GOLF E30-001A
|-------------------------------------------------------------|
|     VOL                                                     |
|         MB3714                                              |
|                                                             |
|          D6376                          PAL6                |
|                              PAL5            MB3790         |
|                                                             |
|                                                             |
|                       PAL4                          TC528257|
|J                      PAL3               TC514256           |
|A                      PAL2               TC514256   TC528257|
|M                                         TC514256           |
|M                                         TC514256   TC528257|
|A                      PAL1 SX004-06.U35  TC514256           |
|                                          TC514256     50MHz |
|                            SX004-05.U34  TC514256           |
|           *       LH52B256               TC514256 |-------| |
|     SX004-07.U70  LH52B256 SX004-04.U33           | R3051 | |
|    TC514256 TC514256                              |       | |
|                 42.9545MHz SX004-03.U32           |-------| |
|          |-------|                      |-------|           |
|TC5588    |ST0016 |         SX004-02.U31 |ST0015 |           |
|          |       |                      |       |           |
|SW4 SW3   |-------|         SX004-01.U30 |-------|           |
|-------------------------------------------------------------|
Notes:
      VOL     : Master Volume Potentiometer
      MB3714  : Fujitsu MB3714 Power Amp
      D6376   : NEC uPD6376 2-channel 16-bit D/A convertor (SOIC16)
      MB3790  : Power Monitoring IC with Reset and Watchdog Timer Function (SOIC16)
      TC528257: Toshiba TC528257 32k x8 SRAM (SOJ28)
      TC5588  : Toshiba TC5588 8k x8 SRAM (SOJ28)
      TC514256: Toshiba TC514256 32k x8 SRAM (SOJ28)
      LH52B256: Sharp LH52B256 32k x8 SRAM (SOP28)
      R3051   : IDT 79R3051-25 CPU (PLCC84, Main CPU, R3000 core, running at 25MHz)
      ST0015  : Seta ST-0015 Custom (QFP208)
      ST0016  : Seta ST-0016 Custom (QFP208, Sub CPU with Z80 core)
                note - 42.9545MHz OSC is tied to ST-0016 on pin 191, there's probably
                internal divider. Actual chip might run at 7.159MHz (/6) or 14.31818MHz (/3)?
      PAL1    : Lattice GAL16V8B (DIP20, labelled 'SX004-10')
      PAL2    : Lattice GAL22V10B (DIP24, labelled 'SX004-11')
      PAL3    : Lattice GAL22V10B (DIP24, labelled 'SX004-12')
      PAL4    : Lattice GAL22V10B (DIP24, labelled 'SX004-13')
      PAL5    : Lattice GAL16V8B (DIP20, labelled 'SX004-09')
      PAL6    : Lattice GAL16V8B (DIP20, labelled 'SX004-08')
      SW3     : 8 position Dip Switch
      SW4     : 8 position Dip Switch
      U30,U31,
      U32,U33 : Macronix MX27C4000 512k x8 EPROM (DIP32, PCB labelled 'RPRO0', 'RPRO1', 'RPRO2', 'RPRO3')
      U34,U35 : 8M mask ROM (DIP42, PCB labelled 'RD0', 'RD1')
      U70     : 16M mask ROM (DIP42, PCB labelled 'ZPRO0')
      *       : Unpopulated position for 16M DIP42 mask ROM (PCB labelled 'ZPRO1')

*/

#include "emu.h"
#include "emupal.h"
#include "machine/st0016.h"
#include "cpu/mips/mips1.h"
#include <algorithm>

class speglsht_state : public driver_device
{
public:
	speglsht_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag),
			m_palette(*this, "palette"),
			m_maincpu(*this,"maincpu"),
			m_subcpu(*this, "sub"),
			m_shared(*this, "shared"),
			m_framebuffer(*this, "framebuffer"),
			m_cop_ram(*this, "cop_ram"),
			m_st0016_bank(*this, "st0016_bank")
			{ }

	void speglsht(machine_config &config);

	void init_speglsht();

private:
	required_device<palette_device> m_palette;
	required_device<st0016_cpu_device> m_maincpu;
	required_device<r3051_device> m_subcpu;

	required_shared_ptr<uint8_t> m_shared;
	required_shared_ptr<uint32_t> m_framebuffer;
	required_shared_ptr<uint32_t> m_cop_ram;

	required_memory_bank m_st0016_bank;

	std::unique_ptr<bitmap_ind16> m_bitmap;
	uint32_t m_videoreg;

	DECLARE_READ32_MEMBER(shared_r);
	DECLARE_WRITE32_MEMBER(shared_w);
	DECLARE_WRITE32_MEMBER(videoreg_w);
	DECLARE_WRITE32_MEMBER(cop_w);
	DECLARE_READ32_MEMBER(cop_r);
	DECLARE_READ32_MEMBER(irq_ack_clear);

	DECLARE_MACHINE_RESET(speglsht);
	virtual void machine_start() override;
	DECLARE_VIDEO_START(speglsht);
	uint32_t screen_update_speglsht(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect);

	DECLARE_WRITE8_MEMBER(st0016_rom_bank_w);
	void speglsht_mem(address_map &map);
	void st0016_io(address_map &map);
	void st0016_mem(address_map &map);
};


void speglsht_state::st0016_mem(address_map &map)
{
	map(0x0000, 0x7fff).rom();
	map(0x8000, 0xbfff).bankr("st0016_bank");
	//map(0xc000, 0xcfff).rw(FUNC(speglsht_state::st0016_sprite_ram_r), FUNC(speglsht_state::st0016_sprite_ram_w));
	//map(0xd000, 0xdfff).rw(FUNC(speglsht_state::st0016_sprite2_ram_r), FUNC(speglsht_state::st0016_sprite2_ram_w));
	map(0xe000, 0xe7ff).ram();
	map(0xe800, 0xe87f).ram();
	//map(0xe900, 0xe9ff) // sound - internal
	//map(0xea00, 0xebff).rw(FUNC(speglsht_state::st0016_palette_ram_r), FUNC(speglsht_state::st0016_palette_ram_w));
	//map(0xec00, 0xec1f).rw(FUNC(speglsht_state::st0016_character_ram_r), FUNC(speglsht_state::st0016_character_ram_w));
	map(0xf000, 0xffff).ram().share("shared");
}

void speglsht_state::machine_start()
{
	m_st0016_bank->configure_entries(0, 256, memregion("maincpu")->base(), 0x4000);
}

// common rombank? should go in machine/st0016 with larger address space exposed?
WRITE8_MEMBER(speglsht_state::st0016_rom_bank_w)
{
	m_st0016_bank->set_entry(data);
}


void speglsht_state::st0016_io(address_map &map)
{
	map.global_mask(0xff);
	//map(0x00, 0xbf).rw(FUNC(speglsht_state::st0016_vregs_r), FUNC(speglsht_state::st0016_vregs_w));
	map(0xe1, 0xe1).w(FUNC(speglsht_state::st0016_rom_bank_w));
	//map(0xe2, 0xe2).w(FUNC(speglsht_state::st0016_sprite_bank_w));
	//map(0xe3, 0xe4).w(FUNC(speglsht_state::st0016_character_bank_w));
	//map(0xe5, 0xe5).w(FUNC(speglsht_state::st0016_palette_bank_w));
	map(0xe6, 0xe6).nopw();
	map(0xe7, 0xe7).nopw();
	//map(0xf0, 0xf0).r(FUNC(speglsht_state::st0016_dma_r));
}

READ32_MEMBER(speglsht_state::shared_r)
{
	return m_shared[offset];
}

WRITE32_MEMBER(speglsht_state::shared_w)
{
	m_shared[offset]=data&0xff;
}

WRITE32_MEMBER(speglsht_state::videoreg_w)
{
	COMBINE_DATA(&m_videoreg);
}


WRITE32_MEMBER(speglsht_state::cop_w)
{
	COMBINE_DATA(&m_cop_ram[offset]);

	if(m_cop_ram[offset]&0x8000) //fix (sign)
	{
		m_cop_ram[offset]|=0xffff0000;
	}
}

//matrix * vector
READ32_MEMBER(speglsht_state::cop_r)
{
	int32_t *cop=(int32_t*)&m_cop_ram[0];

	union
	{
		int32_t  a;
		uint32_t b;
	}temp;

	switch (offset)
	{
		case 0x40/4:
		{
			temp.a=((cop[0x3]*cop[0x0]+cop[0x4]*cop[0x1]+cop[0x5]*cop[0x2])>>14)+cop[0xc];
			return temp.b;
		}

		case 0x44/4:
		{
			temp.a=((cop[0x6]*cop[0x0]+cop[0x7]*cop[0x1]+cop[0x8]*cop[0x2])>>14)+cop[0xd];
			return temp.b;
		}

		case 0x48/4:
		{
			temp.a=((cop[0x9]*cop[0x0]+cop[0xa]*cop[0x1]+cop[0xb]*cop[0x2])>>14)+cop[0xe];
			return temp.b;
		}
	}

	return 0;
}

READ32_MEMBER(speglsht_state::irq_ack_clear)
{
	m_subcpu->set_input_line(INPUT_LINE_IRQ4, CLEAR_LINE);
	return 0;
}

void speglsht_state::speglsht_mem(address_map &map)
{
	map(0x00000000, 0x000fffff).ram();
	map(0x01000000, 0x01007fff).ram(); //tested - STATIC RAM
	map(0x01600000, 0x0160004f).rw(FUNC(speglsht_state::cop_r), FUNC(speglsht_state::cop_w)).share("cop_ram");
	map(0x01800200, 0x01800203).w(FUNC(speglsht_state::videoreg_w));
	map(0x01800300, 0x01800303).portr("IN0");
	map(0x01800400, 0x01800403).portr("IN1");
	map(0x01a00000, 0x01afffff).ram().share("framebuffer");
	map(0x01b00000, 0x01b07fff).ram(); //cleared ...  video related ?
	map(0x01c00000, 0x01dfffff).rom().region("subdata", 0);
	map(0x0a000000, 0x0a003fff).rw(FUNC(speglsht_state::shared_r), FUNC(speglsht_state::shared_w));
	map(0x0fc00000, 0x0fdfffff).rom().mirror(0x10000000).region("subprog", 0);
	map(0x1eff0000, 0x1eff001f).ram();
	map(0x1eff003c, 0x1eff003f).r(FUNC(speglsht_state::irq_ack_clear));
}

static INPUT_PORTS_START( speglsht )
	PORT_START("IN0")
	PORT_BIT( 0x000000ff, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x00000100, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_PLAYER(2)
	PORT_BIT( 0x00000200, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_PLAYER(2)
	PORT_BIT( 0x00000400, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_PLAYER(2)
	PORT_BIT( 0x00000800, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_PLAYER(2)
	PORT_BIT( 0x00001000, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2)
	PORT_BIT( 0x00002000, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(2)
	PORT_BIT( 0x00004000, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(2)
	PORT_BIT( 0x00008000, IP_ACTIVE_LOW, IPT_START2 )
	PORT_BIT( 0xffff0000, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("IN1")
	PORT_DIPNAME( 0x00000007, 0x00000007, DEF_STR( Coin_A ) )   PORT_DIPLOCATION("SW1:1,2,3")
	PORT_DIPSETTING(          0x00000003, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(          0x00000004, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(          0x00000007, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(          0x00000001, "1C/1C or 2C/3C" ) /* 1 coin/1 credit or 2 coins/3 credits */
	PORT_DIPSETTING(          0x00000002, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(          0x00000006, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(          0x00000005, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(          0x00000000, "2C Start/1C Continue" )
	PORT_DIPNAME( 0x00000038, 0x00000038, DEF_STR( Coin_B ) )   PORT_DIPLOCATION("SW1:4,5,6")
	PORT_DIPSETTING(          0x00000018, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(          0x00000020, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(          0x00000038, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(          0x00000008, "1C/1C or 2C/3C" ) /* 1 coin/1 credit or 2 coins/3 credits */
	PORT_DIPSETTING(          0x00000010, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(          0x00000030, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(          0x00000028, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(          0x00000000, "2C Start/1C Continue" )
	PORT_DIPUNUSED_DIPLOC( 0x00000040, 0x00000040, "SW1:7" )
	PORT_DIPNAME( 0x00000080, 0x00000080, "Bonus for PAR Play" )    PORT_DIPLOCATION("SW1:8")
	PORT_DIPSETTING(          0x00000080, DEF_STR( None ) )
	PORT_DIPSETTING(          0x00000000, "Extra Hole" )
	PORT_DIPNAME( 0x00000300, 0x00000300, DEF_STR( Difficulty ) )   PORT_DIPLOCATION("SW2:1,2")
	PORT_DIPSETTING(          0x00000300, DEF_STR( Normal ) )
	PORT_DIPSETTING(          0x00000200, DEF_STR( Easy ) )
	PORT_DIPSETTING(          0x00000100, DEF_STR( Hard ) )
	PORT_DIPSETTING(          0x00000000, DEF_STR( Very_Hard ) )
	PORT_DIPNAME( 0x00000c00, 0x00000c00, "Number of Players" ) PORT_DIPLOCATION("SW2:3,4")
	PORT_DIPSETTING(          0x00000c00, "3" )
	PORT_DIPSETTING(          0x00000800, "4" )
	PORT_DIPSETTING(          0x00000400, "2" )
	PORT_DIPSETTING(          0x00000000, "1" )
	PORT_DIPNAME( 0x00001000, 0x00000000, DEF_STR( Demo_Sounds ) )  PORT_DIPLOCATION("SW2:5")
	PORT_DIPSETTING(          0x00001000, DEF_STR( Off ) )
	PORT_DIPSETTING(          0x00000000, DEF_STR( On ) )
	PORT_DIPNAME( 0x00002000, 0x00000000, "Control Panel" )     PORT_DIPLOCATION("SW2:6")
	PORT_DIPSETTING(          0x00002000, "Double" )
	PORT_DIPSETTING(          0x00000000, DEF_STR( Single ) )
	PORT_DIPNAME( 0x00004000, 0x00000000, "Country" )       PORT_DIPLOCATION("SW2:7")
	PORT_DIPSETTING(          0x00004000, DEF_STR( Japan ) )
	PORT_DIPSETTING(          0x00000000, DEF_STR( USA ) )
	PORT_DIPUNUSED_DIPLOC( 0x00008000, 0x00008000, "SW2:8" )
	PORT_SERVICE_NO_TOGGLE( 0x00010000, IP_ACTIVE_HIGH )
	PORT_BIT( 0x00020000, IP_ACTIVE_LOW, IPT_TILT )
	PORT_BIT( 0x00040000, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x00080000, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x00100000, IP_ACTIVE_LOW, IPT_SERVICE1 )
	PORT_BIT( 0x80a00000, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x00400000, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x01000000, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_PLAYER(1)
	PORT_BIT( 0x02000000, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_PLAYER(1)
	PORT_BIT( 0x04000000, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_PLAYER(1)
	PORT_BIT( 0x08000000, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_PLAYER(1)
	PORT_BIT( 0x10000000, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(1)
	PORT_BIT( 0x20000000, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(1)
	PORT_BIT( 0x40000000, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(1)
INPUT_PORTS_END

static GFXDECODE_START( gfx_speglsht )
GFXDECODE_END


MACHINE_RESET_MEMBER(speglsht_state,speglsht)
{
	std::fill(&m_shared[0],&m_shared[m_shared.bytes()],0);
}

VIDEO_START_MEMBER(speglsht_state,speglsht)
{
	m_bitmap = std::make_unique<bitmap_ind16>(512, 512);
//  VIDEO_START_CALL_MEMBER(st0016);
}

#define PLOT_PIXEL_RGB(x,y,r,g,b)   if(y>=0 && x>=0 && x<512 && y<512) \
{ \
		bitmap.pix32(y, x) = (b) | ((g)<<8) | ((r)<<16); \
}

uint32_t speglsht_state::screen_update_speglsht(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect)
{
	int x,y,dy;

	dy=(m_videoreg&0x20)?(256*512):0; //visible frame

	for(y=0;y<256;y++)
	{
		for(x=0;x<512;x++)
		{
			int tmp=dy+y*512+x;
			PLOT_PIXEL_RGB(x-67,y-5,(m_framebuffer[tmp]>>0)&0xff,(m_framebuffer[tmp]>>8)&0xff,(m_framebuffer[tmp]>>16)&0xff);
		}
	}

	//draw st0016 gfx to temporary bitmap (indexed 16)
	m_bitmap->fill(0);
	m_maincpu->draw_screen(screen, *m_bitmap, cliprect);

	//copy temporary bitmap to rgb 32 bit bitmap
	for(y=cliprect.min_y; y<cliprect.max_y;y++)
	{
		uint16_t *srcline = &m_bitmap->pix16(y);
		for(x=cliprect.min_x; x<cliprect.max_x;x++)
		{
			if(srcline[x])
			{
				rgb_t color = m_maincpu->palette().pen_color(srcline[x]);
				PLOT_PIXEL_RGB(x,y,color.r(),color.g(),color.b());
			}
		}
	}

	return 0;
}

void speglsht_state::speglsht(machine_config &config)
{
	/* basic machine hardware */
	ST0016_CPU(config, m_maincpu, 8000000); /* 8 MHz ? */
	m_maincpu->set_addrmap(AS_PROGRAM, &speglsht_state::st0016_mem);
	m_maincpu->set_addrmap(AS_IO, &speglsht_state::st0016_io);
	m_maincpu->set_vblank_int("screen", FUNC(speglsht_state::irq0_line_hold));

	R3051(config, m_subcpu, 25000000);
	m_subcpu->set_endianness(ENDIANNESS_LITTLE);
	m_subcpu->set_addrmap(AS_PROGRAM, &speglsht_state::speglsht_mem);
	m_subcpu->set_vblank_int("screen", FUNC(speglsht_state::irq4_line_assert));

	config.set_maximum_quantum(attotime::from_hz(6000));
	MCFG_MACHINE_RESET_OVERRIDE(speglsht_state,speglsht)

	/* video hardware */
	screen_device &screen(SCREEN(config, "screen", SCREEN_TYPE_RASTER));
	screen.set_refresh_hz(60);
	screen.set_vblank_time(ATTOSECONDS_IN_USEC(0));
	screen.set_size(512, 512);
	screen.set_visarea(0, 319, 8, 239-8);
	screen.set_screen_update(FUNC(speglsht_state::screen_update_speglsht));

	GFXDECODE(config, "gfxdecode", m_palette, gfx_speglsht);
	PALETTE(config, m_palette).set_entries(16*16*4+1);

	MCFG_VIDEO_START_OVERRIDE(speglsht_state,speglsht)
}

ROM_START( speglsht )
	ROM_REGION( 0x400000, "maincpu", 0 )
	ROM_LOAD( "sx004-07.u70", 0x000000, 0x200000, CRC(2d759cc4) SHA1(9fedd829190b2aab850b2f1088caaec91e8715dd) ) /* Noted as "ZPRO0" IE: Z80 (ST0016) Program 0 */
	/* U71 unpopulated, Noted as ZPRO1 */

	ROM_REGION32_LE( 0x200000, "subprog", 0 )
	ROM_LOAD32_BYTE( "sx004-04.u33", 0x00003, 0x80000, CRC(e46d2e57) SHA1(b1fb836ab2ce547dc2e8d1046d7ef835b87bb04e) ) /* Noted as "RPRO3" IE: R3000 Program 3 */
	ROM_LOAD32_BYTE( "sx004-03.u32", 0x00002, 0x80000, CRC(c6ffb00e) SHA1(f57ef45bb5c690c3e63101a36835d2687abfcdbd) ) /* Noted as "RPRO2" */
	ROM_LOAD32_BYTE( "sx004-02.u31", 0x00001, 0x80000, CRC(21eb46e4) SHA1(0ab21ed012c9a76e01c83b60c6f4670836dfa718) ) /* Noted as "RPRO1" */
	ROM_LOAD32_BYTE( "sx004-01.u30", 0x00000, 0x80000, CRC(65646949) SHA1(74931c230f4e4b1008fbc5fba169292e216aa23b) ) /* Noted as "RPRO0" */

	ROM_REGION32_LE( 0x200000, "subdata", 0)
	ROM_LOAD32_WORD( "sx004-05.u34", 0x000000, 0x100000, CRC(f3c69468) SHA1(81daef6d0596cb67bb6f87b39874aae1b1ffe6a6) ) /* Noted as "RD0" IE: R3000 Data 0 */
	ROM_LOAD32_WORD( "sx004-06.u35", 0x000002, 0x100000, CRC(5af78e44) SHA1(0131d50348fef80c2b100d74b7c967c6a710d548) ) /* Noted as "RD1" */
ROM_END


void speglsht_state::init_speglsht()
{
	m_maincpu->set_game_flag(3);
}


GAME( 1994, speglsht, 0, speglsht, speglsht, speglsht_state, init_speglsht, ROT0, "Seta",  "Super Eagle Shot", MACHINE_IMPERFECT_GRAPHICS )
