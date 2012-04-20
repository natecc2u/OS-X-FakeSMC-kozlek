/*
 *  GeForceX.h
 *  HWSensors
 *
 *  Created by kozlek on 19/04/12.
 *  Copyright 2010 kozlek. All rights reserved.
 *
 */

/*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the "Software"),
* to deal in the Software without restriction, including without limitation
* the rights to use, copy, modify, merge, publish, distribute, sublicense,
* and/or sell copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice (including the next
* paragraph) shall be included in all copies or substantial portions of the
* Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
* DEALINGS IN THE SOFTWARE.
*/

#include <IOKit/IOService.h>
#include <IOKit/pci/IOPCIDevice.h>

#include "FakeSMCPlugin.h"

enum NVBiosType {
    NVBIOS_BIT,
    NVBIOS_BMP
};

enum NVClockSource{
    NVClockCore     = 1,
    NVClockShader   = 2,
    NVClockRop      = 3,
    NVClockCopy     = 4,
    NVClockDaemon   = 5,
    NVClockVdec     = 6,
    NVCLockMemory   = 8
};

struct NVBios {
   	uint8_t*    data;
	uint32_t    length;
    NVBiosType  type;
    uint32_t    offset;
};

struct NVBitEntry {
	uint8_t     id;
	uint8_t     version;
	uint16_t    length;
	uint16_t    offset;
	uint8_t*    data;
};

struct NVSensorConstants {
	UInt16 offset_constant;
	SInt16 offset_mult;
	SInt16 offset_div;
	SInt16 slope_mult;
	SInt16 slope_div;
    SInt16 temp_critical;
    SInt16 temp_down_clock;
    SInt16 temp_fan_boost;
    SInt16 fan_min_duty;
    SInt16 fan_max_duty;
    SInt16 fan_pwm_freq;
};

enum NV50ClockSource {
	nv50_clk_src_crystal,
	nv50_clk_src_href,
	nv50_clk_src_hclk,
	nv50_clk_src_hclkm3,
	nv50_clk_src_hclkm3d2,
	nv50_clk_src_host,
	nv50_clk_src_nvclk,
	nv50_clk_src_sclk,
	nv50_clk_src_mclk,
	nv50_clk_src_vdec,
	nv50_clk_src_dom6
};

/* VRAM/fb configuration */
enum NVVRAMType {
    NV_MEM_TYPE_UNKNOWN = 0,
    NV_MEM_TYPE_STOLEN  = 1,
    NV_MEM_TYPE_SGRAM   = 2,
    NV_MEM_TYPE_SDRAM   = 3,
    NV_MEM_TYPE_DDR1    = 4,
    NV_MEM_TYPE_DDR2    = 5,
    NV_MEM_TYPE_DDR3    = 6,
    NV_MEM_TYPE_GDDR2   = 7,
    NV_MEM_TYPE_GDDR3   = 8,
    NV_MEM_TYPE_GDDR4   = 9,
    NV_MEM_TYPE_GDDR5   = 10
};

static const struct NVVRAMTypes {
	int value;
	const char *name;
} NVVRAMTypeMap[] = {
    { NV_MEM_TYPE_UNKNOWN, "unknown type" },
	{ NV_MEM_TYPE_STOLEN , "stolen system memory" },
	{ NV_MEM_TYPE_SGRAM  , "SGRAM" },
	{ NV_MEM_TYPE_SDRAM  , "SDRAM" },
	{ NV_MEM_TYPE_DDR1   , "DDR1" },
	{ NV_MEM_TYPE_DDR2   , "DDR2" },
	{ NV_MEM_TYPE_DDR3   , "DDR3" },
	{ NV_MEM_TYPE_GDDR2  , "GDDR2" },
	{ NV_MEM_TYPE_GDDR3  , "GDDR3" },
	{ NV_MEM_TYPE_GDDR4  , "GDDR4" },
	{ NV_MEM_TYPE_GDDR5  , "GDDR5" }
};

class GeForceX : public FakeSMCPlugin
{
    OSDeclareDefaultStructors(GeForceX)    
	
private:
    IOPCIDevice*    device;
    UInt32          chipset;
    UInt32          card_type;
    
    volatile UInt8* PMC;
    
    NVBios          bios;
    
    UInt32          crystal;
    NVSensorConstants sensor_constants;
    
    UInt64          vram_size;
    NVVRAMType      vram_type;
    
    void            nv20_get_vram();
    void            nvc0_get_vram();
    UInt32          get_vram_multiplier();
    
    UInt32          nv40_read_pll_1(UInt32 reg);
    UInt32          nv40_read_pll_2(UInt32 reg);
    UInt32          nv40_read_clk(UInt32 src);
    UInt32          nv40_get_clock(NVClockSource name);
    
    UInt32          nva3_read_vco(int clk);
    UInt32          nva3_read_clk(int clk, bool ignore_en);
    UInt32          nva3_read_pll(int clk, UInt32 pll);
    UInt32          nva3_get_clock(NVClockSource name);
    
    UInt32          nv50_read_div();
    UInt32          nv50_read_pll_src(UInt32 base);
    UInt32          nv50_read_pll_ref(UInt32 base);
    UInt32          nv50_read_pll(UInt32 base);
    UInt32          nv50_read_clk(NV50ClockSource source);
    UInt32          nv50_get_clock(NVClockSource name);
    
    UInt32          nvc0_read_vco(UInt32 dsrc);
    UInt32          nvc0_read_pll(UInt32 pll);
    UInt32          nvc0_read_div(UInt32 doff, UInt32 dsrc, UInt32 dctl);
    UInt32          nvc0_read_mem();
    UInt32          nvc0_read_clk(UInt32 clk);
    UInt32          nvc0_get_clock(NVClockSource name);
    
    int             nv40_sensor_setup();
    int             nv40_get_temperature();
    int             nv84_get_temperature();
    
    NVVRAMType      nouveau_mem_vbios_type();
    int             score_vbios(const bool writeable);
    void            bios_shadow_pramin();
    void            bios_shadow_prom();
    
protected:
    virtual float       getSensorValue(FakeSMCSensor *sensor);
    
public:
    virtual IOService*	probe(IOService *provider, SInt32 *score);
    virtual bool		start(IOService *provider);
    virtual void		free(void);
};