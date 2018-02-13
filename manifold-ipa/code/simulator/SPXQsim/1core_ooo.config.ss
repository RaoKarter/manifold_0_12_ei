-package.name				16CORE
-package.thermal_library		3d-ice
-package.thermal_analysis_type	transient
-package.ambient_temperature		300.0
-package.temperature			300.0
-package.chip_length			2.1e-3	# 2.1mm
-package.chip_width			2.1e-3	# 2.1mm
-package.grid_rows			10
-package.grid_columns		10

-package.material					SILICON
-package.material.SILICON.thermal_conductivity		1.30e-4 
-package.material.SILICON.volumetric_heat_capacity	1.628e-12
-package.material					BEOL
-package.material.BEOL.thermal_conductivity		2.25e-6
-package.material.BEOL.volumetric_heat_capacity		2.175e-12

-package.conventional_heatsink					true
-package.conventional_heatsink.heat_transfer_coefficient	2.8e-8

-package.microchannel.type					2rm
-package.microchannel.height					100e-6	
-package.microchannel.channel_length				50e-6
-package.microchannel.wall_length				50e-6
-package.microchannel.wall_material				SILICON
-package.microchannel.coolant_flow_rate				42
-package.microchannel.coolant_heat_transfer_coefficient_top	5.7132e-8
-package.microchannel.coolant_heat_transfer_coefficient_bottom	4.7132e-8
-package.microchannel.coolant_volumetric_heat_capacity		4.172e-12
-package.microchannel.coolant_incoming_temperature		300.0

-package.die						DRAM3_IC
-package.die.DRAM3_IC.layer				SOURCE_LAYER
-package.die.DRAM3_IC.layer.SOURCE_LAYER.height		10e-6
-package.die.DRAM3_IC.layer.SOURCE_LAYER.material		SILICON
-package.die.DRAM3_IC.layer.SOURCE_LAYER.is_source_layer	true
-package.die.DRAM3_IC.layer				BOTTOM_LAYER
-package.die.DRAM3_IC.layer.BOTTOM_LAYER.height		25e-6
-package.die.DRAM3_IC.layer.BOTTOM_LAYER.material		SILICON

-package.die						DRAM2_IC
-package.die.DRAM2_IC.layer				TOP_LAYER
-package.die.DRAM2_IC.layer.TOP_LAYER.height		25e-6
-package.die.DRAM2_IC.layer.TOP_LAYER.material		BEOL
-package.die.DRAM2_IC.layer				SOURCE_LAYER
-package.die.DRAM2_IC.layer.SOURCE_LAYER.height		10e-6
-package.die.DRAM2_IC.layer.SOURCE_LAYER.material		SILICON
-package.die.DRAM2_IC.layer.SOURCE_LAYER.is_source_layer	true
-package.die.DRAM2_IC.layer				BOTTOM_LAYER
-package.die.DRAM2_IC.layer.BOTTOM_LAYER.height		25e-6
-package.die.DRAM2_IC.layer.BOTTOM_LAYER.material		SILICON

-package.die						DRAM1_IC
-package.die.DRAM1_IC.layer				TOP_LAYER
-package.die.DRAM1_IC.layer.TOP_LAYER.height		25e-6
-package.die.DRAM1_IC.layer.TOP_LAYER.material		BEOL
-package.die.DRAM1_IC.layer				SOURCE_LAYER
-package.die.DRAM1_IC.layer.SOURCE_LAYER.height		10e-6
-package.die.DRAM1_IC.layer.SOURCE_LAYER.material		SILICON
-package.die.DRAM1_IC.layer.SOURCE_LAYER.is_source_layer	true
-package.die.DRAM1_IC.layer				BOTTOM_LAYER
-package.die.DRAM1_IC.layer.BOTTOM_LAYER.height		25e-6
-package.die.DRAM1_IC.layer.BOTTOM_LAYER.material		SILICON

-package.die						DRAM0_IC
-package.die.DRAM0_IC.layer				TOP_LAYER
-package.die.DRAM0_IC.layer.TOP_LAYER.height		25e-6
-package.die.DRAM0_IC.layer.TOP_LAYER.material		BEOL
-package.die.DRAM0_IC.layer				SOURCE_LAYER
-package.die.DRAM0_IC.layer.SOURCE_LAYER.height		10e-6
-package.die.DRAM0_IC.layer.SOURCE_LAYER.material		SILICON
-package.die.DRAM0_IC.layer.SOURCE_LAYER.is_source_layer	true
-package.die.DRAM0_IC.layer				BOTTOM_LAYER
-package.die.DRAM0_IC.layer.BOTTOM_LAYER.height		25e-6
-package.die.DRAM0_IC.layer.BOTTOM_LAYER.material		SILICON


-package.die						L2CACHE_IC
-package.die.L2CACHE_IC.layer				TOP_LAYER
-package.die.L2CACHE_IC.layer.TOP_LAYER.height		25e-6
-package.die.L2CACHE_IC.layer.TOP_LAYER.material		BEOL
-package.die.L2CACHE_IC.layer				SOURCE_LAYER
-package.die.L2CACHE_IC.layer.SOURCE_LAYER.height		10e-6
-package.die.L2CACHE_IC.layer.SOURCE_LAYER.material		SILICON
-package.die.L2CACHE_IC.layer.SOURCE_LAYER.is_source_layer	true
-package.die.L2CACHE_IC.layer				BOTTOM_LAYER
-package.die.L2CACHE_IC.layer.BOTTOM_LAYER.height		25e-6
-package.die.L2CACHE_IC.layer.BOTTOM_LAYER.material		SILICON


-package.die						CORE_IC
-package.die.CORE_IC.layer				TOP_LAYER
-package.die.CORE_IC.layer.TOP_LAYER.height		25e-6
-package.die.CORE_IC.layer.TOP_LAYER.material		BEOL
-package.die.CORE_IC.layer				SOURCE_LAYER
-package.die.CORE_IC.layer.SOURCE_LAYER.height	10e-6
-package.die.CORE_IC.layer.SOURCE_LAYER.material	SILICON
-package.die.CORE_IC.layer.SOURCE_LAYER.is_source_layer true
-package.die.CORE_IC.layer				BOTTOM_LAYER
-package.die.CORE_IC.layer.BOTTOM_LAYER.height	25e-6
-package.die.CORE_IC.layer.BOTTOM_LAYER.material	SILICON



-package.stack						TOP_MOST
-package.stack.TOP_MOST.type				layer
-package.stack.TOP_MOST.height				50e-6
-package.stack.TOP_MOST.material			SILICON
-package.stack						DRAM3_DIE
-package.stack.DRAM3_DIE.type				die
-package.stack.DRAM3_DIE.die				DRAM3_IC
-package.stack						DRAM2_DIE
-package.stack.DRAM2_DIE.type				die
-package.stack.DRAM2_DIE.die				DRAM2_IC
-package.stack						DRAM1_DIE
-package.stack.DRAM1_DIE.type				die
-package.stack.DRAM1_DIE.die				DRAM1_IC
-package.stack						DRAM0_DIE
-package.stack.DRAM0_DIE.type				die
-package.stack.DRAM0_DIE.die				DRAM0_IC
-package.stack						DL2_DIE
-package.stack.DL2_DIE.type				die
-package.stack.DL2_DIE.die				L2CACHE_IC
-package.stack						CORE_DIE
-package.stack.CORE_DIE.type				die
-package.stack.CORE_DIE.die				CORE_IC
-package.stack						BOTTOM_MOST
-package.stack.BOTTOM_MOST.type				layer
-package.stack.BOTTOM_MOST.height			100e-6
-package.stack.BOTTOM_MOST.material			SILICON

# DRAM
-package.partition	DRAM3_DIE:VAULT0

-package.partition	DRAM2_DIE:VAULT0

-package.partition	DRAM1_DIE:VAULT0

-package.partition	DRAM0_DIE:VAULT0

# L2CACHE

-package.partition	DL2_DIE:SPOT0

# CORES

-package.partition	CORE_DIE:FRT0
-package.partition	CORE_DIE:FPU0
-package.partition	CORE_DIE:INT0
-package.partition	CORE_DIE:MEM0
-package.partition	CORE_DIE:SCH0
-package.end

-partition.name		DRAM3_DIE:VAULT0
-partition.x_left	0.0
-partition.y_bottom	0.0
-partition.layer	1
-partition.width	2.1e-3
-partition.length	2.1e-3
-partition.die		DRAM3_DIE
-partition.opt_for_clk			true # optimize the result on clock
-partition.embedded			false # embedded processor?
-partition.feature_size		16e-9 # technology node in meters
-partition.temperature			300 # temperature in Kelvin
-partition.component_type	        core # core, uncore, llc(non-private caches)
-partition.clock_frequency		1.000000e+09 # clock frequency in Hz
-partition.longer_channel_device		false
-partition.core_type			OOO
-partition.wire_type			global
-partition.device_type			hp
-partition.interconnect_projection	aggressive
-partition.wiring_type			global
-partition.end

-partition.name		DRAM2_DIE:VAULT0
-partition.x_left	0.0
-partition.y_bottom	0.0
-partition.layer	1
-partition.width	2.1e-3
-partition.length	2.1e-3
-partition.die		DRAM2_DIE
-partition.opt_for_clk			true # optimize the result on clock
-partition.embedded			false # embedded processor?
-partition.feature_size		16e-9 # technology node in meters
-partition.temperature			300 # temperature in Kelvin
-partition.component_type	        core # core, uncore, llc(non-private caches)
-partition.clock_frequency		1.000000e+09 # clock frequency in Hz
-partition.longer_channel_device		false
-partition.core_type			OOO
-partition.wire_type			global
-partition.device_type			hp
-partition.interconnect_projection	aggressive
-partition.wiring_type			global
-partition.end

-partition.name		DRAM1_DIE:VAULT0
-partition.x_left	0.0
-partition.y_bottom	0.0
-partition.layer	1
-partition.width	2.1e-3
-partition.length	2.1e-3
-partition.die		DRAM1_DIE
-partition.opt_for_clk			true # optimize the result on clock
-partition.embedded			false # embedded processor?
-partition.feature_size		16e-9 # technology node in meters
-partition.temperature			300 # temperature in Kelvin
-partition.component_type	        core # core, uncore, llc(non-private caches)
-partition.clock_frequency		1.000000e+09 # clock frequency in Hz
-partition.longer_channel_device		false
-partition.core_type			OOO
-partition.wire_type			global
-partition.device_type			hp
-partition.interconnect_projection	aggressive
-partition.wiring_type			global
-partition.end

-partition.name		DRAM0_DIE:VAULT0
-partition.x_left	0.0
-partition.y_bottom	0.0
-partition.layer	1
-partition.width	2.1e-3
-partition.length	2.1e-3
-partition.die		DRAM0_DIE
-partition.opt_for_clk			true # optimize the result on clock
-partition.embedded			false # embedded processor?
-partition.feature_size		16e-9 # technology node in meters
-partition.temperature			300 # temperature in Kelvin
-partition.component_type	        core # core, uncore, llc(non-private caches)
-partition.clock_frequency		1.000000e+09 # clock frequency in Hz
-partition.longer_channel_device		false
-partition.core_type			OOO
-partition.wire_type			global
-partition.device_type			hp
-partition.interconnect_projection	aggressive
-partition.wiring_type			global
-partition.end

# DRAM ENDS
# L2 CACHE BEGINS

-partition.name		DL2_DIE:SPOT0
-partition.x_left	0.0
-partition.y_bottom	0.0
-partition.layer	1
-partition.width	2.1e-3
-partition.length	2.1e-3
-partition.die		DL2_DIE
-partition.opt_for_clk			true # optimize the result on clock
-partition.embedded			false # embedded processor?
-partition.feature_size		16e-9 # technology node in meters
-partition.temperature			300 # temperature in Kelvin
-partition.component_type	        core # core, uncore, llc(non-private caches)
-partition.clock_frequency		1.000000e+09 # clock frequency in Hz
-partition.longer_channel_device		false
-partition.core_type			OOO
-partition.wire_type			global
-partition.device_type			hp
-partition.interconnect_projection	aggressive
-partition.wiring_type			global
-partition.end

# L2CACHE ENDS
# CORE0 BEGINS

-partition.name		CORE_DIE:FRT0
-partition.x_left	1.48e-3
-partition.y_bottom	1.20e-3
-partition.layer	0
-partition.width	0.90e-3
-partition.length	0.62e-3
-partition.die		CORE_DIE
-partition.opt_for_clk			true # optimize the result on clock
-partition.embedded			false # embedded processor?
-partition.feature_size		16e-9 # technology node in meters
-partition.temperature			300 # temperature in Kelvin
-partition.component_type		core # core, uncore, llc(non-private caches)
-partition.clock_frequency		1.000000e+09 # clock frequency in Hz
-partition.longer_channel_device		false
-partition.core_type			OOO
-partition.wire_type			global
-partition.device_type			hp
-partition.interconnect_projection	aggressive
-partition.wiring_type			global
-partition.end

-partition.name		CORE_DIE:SCH0
-partition.x_left	1.48e-3
-partition.y_bottom	0.0
-partition.layer	0
-partition.width	1.20e-3
-partition.length	0.62e-3
-partition.die		CORE_DIE
-partition.opt_for_clk			true # optimize the result on clock
-partition.embedded			false # embedded processor?
-partition.feature_size		16e-9 # technology node in meters
-partition.temperature			300 # temperature in Kelvin
-partition.component_type		core # core, uncore, llc(non-private caches)
-partition.clock_frequency		1.000000e+09 # clock frequency in Hz
-partition.longer_channel_device		false
-partition.core_type			OOO
-partition.wire_type			global
-partition.device_type			hp
-partition.interconnect_projection	aggressive
-partition.wiring_type			global
-partition.end

-partition.name		CORE_DIE:FPU0
-partition.x_left	0.0
-partition.y_bottom	0.0
-partition.layer	0
-partition.width	0.61e-3
-partition.length	1.48e-3
-partition.die		CORE_DIE
-partition.opt_for_clk			true # optimize the result on clock
-partition.embedded			false # embedded processor?
-partition.feature_size		16e-9 # technology node in meters
-partition.temperature			300 # temperature in Kelvin
-partition.component_type		core # core, uncore, llc(non-private caches)
-partition.clock_frequency		1.000000e+09 # clock frequency in Hz
-partition.longer_channel_device		false
-partition.core_type			OOO
-partition.wire_type			global
-partition.device_type			hp
-partition.interconnect_projection	aggressive
-partition.wiring_type			global
-partition.end

-partition.name		CORE_DIE:INT0
-partition.x_left	0.0
-partition.y_bottom	0.61e-3
-partition.layer	0
-partition.width	0.86e-3
-partition.length	1.48e-3
-partition.die		CORE_DIE
-partition.opt_for_clk			true # optimize the result on clock
-partition.embedded			false # embedded processor?
-partition.feature_size		16e-9 # technology node in meters
-partition.temperature			300 # temperature in Kelvin
-partition.component_type		core # core, uncore, llc(non-private caches)
-partition.clock_frequency		1.000000e+09 # clock frequency in Hz
-partition.longer_channel_device		false
-partition.core_type			OOO
-partition.wire_type			global
-partition.device_type			hp
-partition.interconnect_projection	aggressive
-partition.wiring_type			global
-partition.end

-partition.name		CORE_DIE:MEM0
-partition.x_left	0.0
-partition.y_bottom	1.47e-3
-partition.layer	0
-partition.width	0.63e-3
-partition.length	1.48e-3
-partition.die		CORE_DIE
-partition.opt_for_clk			true # optimize the result on clock
-partition.embedded			false # embedded processor?
-partition.feature_size		16e-9 # technology node in meters
-partition.temperature			300 # temperature in Kelvin
-partition.component_type		core # core, uncore, llc(non-private caches)
-partition.clock_frequency		1.000000e+09 # clock frequency in Hz
-partition.longer_channel_device		false
-partition.core_type			OOO
-partition.wire_type			global
-partition.device_type			hp
-partition.interconnect_projection	aggressive
-partition.wiring_type			global
-partition.end

# CORE0 ENDS

# CORE0 DESCRIPTION BEGINS

-module.name			core:instruction_buffer:0
-module.partition		CORE_DIE:FRT0
-module.energy_library		McPAT
-module.energy_model		array
-module.energy_submodel		ram
-module.line_width		16
-module.output_width		128
-module.size			256 # 16 entries
-module.rw_ports		4 # decode width
-module.access_mode		normal
-module.end

-module.name			core:BTB:0
-module.partition		CORE_DIE:FRT0
-module.energy_library		McPAT
-module.energy_model		array
-module.energy_submodel		cache
-module.line_width		4
-module.output_width		32
-module.assoc			2
-module.size			5120 # 1024(L1) + 4096(L2)
-module.tag_width		69
-module.rw_ports		1
-module.rd_ports		1
-module.wr_ports		1
-module.cycle_time		1
-module.access_time		3
-module.access_mode		normal
-module.TDP_duty_cycle.write	0.000000
-module.end

-module.name			core:chooser:0
-module.partition		CORE_DIE:FRT0
-module.energy_library		McPAT
-module.energy_model		array
-module.energy_submodel		ram
-module.line_width		1
-module.output_width		8
-module.size			4096
-module.tag_width		69
-module.rd_ports		1
-module.wr_ports		1
-module.access_mode		fast
-module.TDP_duty_cycle.write	0.000000
-module.end

-module.name			core:global_predictor:0
-module.partition		CORE_DIE:FRT0
-module.energy_library		McPAT
-module.energy_model		array
-module.energy_submodel		ram
-module.line_width		1
-module.output_width		8
-module.size			4096
-module.tag_width		69
-module.rd_ports		1
-module.wr_ports		1
-module.access_mode		fast
-module.TDP_duty_cycle.write	0.000000
-module.end

-module.name			core:L1_local_predictor:0
-module.partition		CORE_DIE:FRT0
-module.energy_library		McPAT
-module.energy_model		array
-module.energy_submodel		ram
-module.line_width		2
-module.output_width		16
-module.size			2048
-module.tag_width		69
-module.rd_ports		1
-module.wr_ports		1
-module.access_mode		fast
-module.TDP_duty_cycle.write	0.000000
-module.end

-module.name			core:L2_local_predictor:0
-module.partition		CORE_DIE:FRT0
-module.energy_library		McPAT
-module.energy_model		array
-module.energy_submodel		ram
-module.line_width		1
-module.output_width		8
-module.size			1024
-module.tag_width		69
-module.rd_ports		1
-module.wr_ports		1
-module.access_mode		fast
-module.TDP_duty_cycle.write	0.000000
-module.end

-module.name			core:RAS:0
-module.partition		CORE_DIE:FRT0
-module.energy_library		McPAT
-module.energy_model		array
-module.energy_submodel		ram
-module.line_width		8
-module.output_width		64
-module.size			128 # 16 entries
-module.tag_width		69
-module.rd_ports		1
-module.wr_ports		1
-module.access_mode		fast
-module.TDP_duty_cycle.write	0.000000
-module.end

-module.name			core:ITLB:0
-module.partition		CORE_DIE:FRT0
-module.energy_library		McPAT
-module.energy_model		array
-module.energy_submodel		cache
-module.line_width		5
-module.output_width		40
-module.assoc			0
-module.size			640
-module.tag_width		57
-module.rd_ports		1
-module.wr_ports		1
-module.search_ports		1
-module.cycle_time		1
-module.access_time		2
-module.access_mode		normal
-module.TDP_duty_cycle.read	0.000000
-module.TDP_duty_cycle.write	0.000000
-module.end

-module.name			core:IL1:0
-module.partition		CORE_DIE:FRT0
-module.energy_library		McPAT
-module.energy_model		array
-module.energy_submodel		cache
-module.line_width		32 # in bytes
-module.output_width		256 # in bits
-module.assoc			4
-module.size			32768 # 32KB (256 sets)
-module.tag_width		44 # phy_addr(52 bits) - log2(sets) - log2(line_width) + tag_overhead(5 bits)
-module.rw_ports		1
-module.cycle_time		1
-module.access_time		4
-module.access_mode		normal
-module.TDP_duty_cycle.write	0.000000
-module.end

-module.name		core:IL1:MissBuffer:0
-module.partition		CORE_DIE:FRT0
-module.energy_library		McPAT
-module.energy_model		array
-module.energy_submodel		cache
-module.line_width		40
-module.output_width		320
-module.assoc		0
-module.size		320
-module.tag_width		57 # phy_addr (52 bits) + tag_overhead (5 bits)
-module.rw_ports		1
-module.search_ports		1
-module.cycle_time		1
-module.access_time		4
-module.access_mode		normal
-module.TDP_duty_cycle.read		0.000000
-module.end

-module.name		core:IL1:FillBuffer:0
-module.partition		CORE_DIE:FRT0
-module.energy_library		McPAT
-module.energy_model		array
-module.energy_submodel		cache
-module.line_width		32
-module.output_width		256
-module.assoc		0
-module.size		256
-module.tag_width		57
-module.rw_ports		1
-module.search_ports		1
-module.cycle_time		1
-module.access_time		4
-module.access_mode		normal
-module.TDP_duty_cycle.read		0.000000
-module.end

-module.name		core:IL1:PrefetchBuffer:0
-module.partition		CORE_DIE:FRT0
-module.energy_library		McPAT
-module.energy_model		array
-module.energy_submodel		cache
-module.line_width		32
-module.output_width		256
-module.assoc		0
-module.size		256
-module.tag_width		57
-module.rw_ports		1
-module.search_ports		1
-module.cycle_time		1
-module.access_time		4
-module.access_mode		normal
-module.TDP_duty_cycle.read		0.000000
-module.end

-module.name			core:instruction_decoder:0
-module.partition		CORE_DIE:FRT0
-module.energy_library		McPAT
-module.energy_model		instruction_decoder
-module.x86			true
-module.opcode			16
-module.scaling			4.000000
-module.end

-module.name			core:operand_decoder:0
-module.partition		CORE_DIE:FRT0
-module.energy_library		McPAT
-module.energy_model		instruction_decoder
-module.x86			true
-module.opcode			4
-module.scaling			4.000000
-module.end

-module.name			core:uop_sequencer:0
-module.partition		CORE_DIE:FRT0
-module.energy_library		McPAT
-module.energy_model		instruction_decoder
-module.x86			true
-module.opcode			8
-module.scaling			4.000000
-module.end

-module.name			core:uop_queue:0
-module.partition		CORE_DIE:FRT0
-module.energy_library		McPAT
-module.energy_model		array
-module.energy_submodel		ram
-module.line_width		16
-module.output_width		128
-module.size			384 # 24 entries
-module.rw_ports		6 # issue width
-module.access_mode		normal
-module.end

-module.name			core:RS:0
-module.partition		CORE_DIE:SCH0
-module.energy_library		McPAT
-module.energy_model		array
-module.energy_submodel		cache
-module.line_width		16
-module.output_width		128
-module.assoc			0
-module.size			576 # 36 entries
-module.tag_width		7
-module.rd_ports		6 # issue width
-module.wr_ports		6 # issue width
-module.search_ports		6 # issue width
-module.cycle_time		2
-module.access_time		2
-module.access_mode		normal
-module.TDP_duty_cycle.read	0.666667
-module.TDP_duty_cycle.write	0.666667
-module.TDP_duty_cycle.search	0.666667
-module.end

-module.name			core:ROB:0
-module.partition		CORE_DIE:SCH0
-module.energy_library		McPAT
-module.energy_model		array
-module.energy_submodel		ram
-module.line_width		18
-module.output_width		144
-module.size			2304 # 128 entries
-module.tag_width		14
-module.rd_ports		6 # issue width
-module.wr_ports		6 # issue width
-module.access_mode		sequential
-module.TDP_duty_cycle.read	0.666667
-module.TDP_duty_cycle.write	0.666667
-module.end

-module.name			core:issue_select:0
-module.partition		CORE_DIE:SCH0
-module.energy_library		McPAT
-module.energy_model		selection_logic
-module.selection_input		36
-module.selection_output	6
-module.TDP_duty_cycle.read	0.500000
-module.scaling			2.000000
-module.end

#general purpose
-module.name			core:GPREG:0
-module.partition		CORE_DIE:SCH0
-module.energy_library		McPAT
-module.energy_model		array
-module.energy_submodel		ram
-module.line_width		8 # 64-bit data
-module.sets			16 # 16 GPRs
-module.rw_ports		1
-module.rd_ports		8 # 2*issue width
-module.wr_ports		4 # commit width
-module.access_mode		sequential
-module.area_scaling		1.1 # McPAT overhead number
-module.end

#other (segment/control) registers
-module.name			core:SCFREG:0
-module.partition		CORE_DIE:SCH0
-module.energy_library		McPAT
-module.energy_model		array
-module.energy_submodel		ram
-module.line_width		8 # 64-bit data
-module.sets			17 # 8 SEGs + 8 CRs + 1 FLAG
-module.rw_ports		1
-module.rd_ports		8 # issue width
-module.wr_ports		4 # commit width
-module.access_mode		sequential
-module.area_scaling		1.1 # McPAT overhead number
-module.end

#floating-point registers
-module.name			core:FPREG:0
-module.partition		CORE_DIE:SCH0
-module.energy_library		McPAT
-module.energy_model		array
-module.energy_submodel		ram
-module.line_width		10 # 80-bit data
-module.sets			16 # 16 FPs
-module.rw_ports		1
-module.rd_ports		4 # 2*fp_issue width
-module.wr_ports		4 # commit width
-module.access_mode		sequential
-module.area_scaling		1.1 # McPAT overhead number
-module.end

-module.name			core:ALU:0
-module.partition		CORE_DIE:INT0
-module.energy_library		McPAT
-module.energy_model		functional_unit
-module.energy_submodel		alu
-module.with_clock_grid		false
-module.scaling			6.000000
-module.end

-module.name			core:FPU:0
-module.partition		CORE_DIE:FPU0
-module.energy_library		McPAT
-module.energy_model		functional_unit
-module.energy_submodel		fpu
-module.with_clock_grid		false
-module.scaling			2.000000
-module.TDP_duty_cycle.read		0.300000
-module.end

-module.name			core:MUL:0
-module.partition		CORE_DIE:FPU0
-module.energy_library		McPAT
-module.energy_model		functional_unit
-module.energy_submodel		mul
-module.with_clock_grid		false
-module.scaling			1.000000
-module.TDP_duty_cycle.read		0.300000
-module.end

#register renaming table
-module.name					core:RAT:0
-module.partition				CORE_DIE:SCH0
-module.energy_library		McPAT
-module.energy_model		array
-module.energy_submodel		cache
-module.line_width		29
-module.output_width		8
-module.assoc		0
-module.sets		32
-module.tag_width		64
-module.rw_ports		1
-module.rd_ports		8
-module.wr_ports		4
-module.search_ports		4
-module.access_mode		fast
-module.TDP_duty_cycle.search		0.0
-module.adjust_area true
-module.end

-module.name		core:freelist:0
-module.partition		CORE_DIE:SCH0
-module.energy_library		McPAT
-module.energy_model		array
-module.energy_submodel		ram
-module.line_width		1
-module.output_width		8
-module.size			128 # ROB entries
-module.tag_width		64
-module.rw_ports		1
-module.rd_ports		4
-module.wr_ports		7
-module.access_mode		sequential
-module.TDP_duty_cycle.write		0.571429
-module.end

-module.name			core:dependency_check:0
-module.partition		CORE_DIE:SCH0
-module.energy_library		McPAT
-module.energy_model		dependency_check_logic
-module.compare_bits		8 # TODO: check logic.cc:225-228
-module.decode_width		4
-module.TDP_duty_cycle.read	2.000000
-module.scaling			2
-module.end

-module.name		core:DL1:0
-module.partition		CORE_DIE:MEM0
-module.energy_library		McPAT
-module.energy_model		array
-module.energy_submodel		cache
-module.line_width		64
-module.output_width		512
-module.assoc		8
-module.size		131072 # 128KB (256 sets)
-module.tag_width		18
-module.rw_ports		2
-module.cycle_time		1
-module.access_time		4
-module.access_mode		normal
-module.TDP_duty_cycle.read		0.335000
-module.TDP_duty_cycle.write		0.165000
-module.end

-module.name		core:DL1:MissBuffer:0
-module.partition		CORE_DIE:MEM0
-module.energy_library		McPAT
-module.energy_model		array
-module.energy_submodel		cache
-module.line_width		40
-module.output_width		320
-module.assoc		0
-module.size		320
-module.tag_width		57
-module.rw_ports		2
-module.search_ports		2
-module.cycle_time		1
-module.access_time		2
-module.access_mode		normal
-module.TDP_duty_cycle.read		0.000000
-module.end

-module.name		core:DL1:FillBuffer:0
-module.partition		CORE_DIE:MEM0
-module.energy_library		McPAT
-module.energy_model		array
-module.energy_submodel		cache
-module.line_width		32
-module.output_width		256
-module.assoc		0
-module.size		256
-module.tag_width		57
-module.rw_ports		2
-module.search_ports		2
-module.cycle_time		1
-module.access_time		2
-module.access_mode		normal
-module.TDP_duty_cycle.read		0.000000
-module.end

-module.name		core:DL1:PrefetchBuffer:0
-module.partition		CORE_DIE:MEM0
-module.energy_library		McPAT
-module.energy_model		array
-module.energy_submodel		cache
-module.line_width		32
-module.output_width		256
-module.assoc		0
-module.size		256
-module.tag_width		57
-module.rw_ports		2
-module.search_ports		2
-module.cycle_time		1
-module.access_time		2
-module.access_mode		normal
-module.TDP_duty_cycle.read		0.000000
-module.end

-module.name		core:DL1:WBB:0
-module.partition		CORE_DIE:MEM0
-module.energy_library		McPAT
-module.energy_model		array
-module.energy_submodel		cache
-module.line_width		32
-module.output_width		256
-module.assoc		0
-module.size		256
-module.tag_width		57
-module.rw_ports		2
-module.search_ports		2
-module.cycle_time		1
-module.access_time		2
-module.access_mode		normal
-module.TDP_duty_cycle.read		0.000000
-module.end

-module.name		core:StoreQueue:0
-module.partition		CORE_DIE:MEM0
-module.energy_library		McPAT
-module.energy_model		array
-module.energy_submodel		cache
-module.line_width		8
-module.output_width		64
-module.assoc		0
-module.size		384 # 48 entries
-module.tag_width		85
-module.rd_ports		2
-module.wr_ports		2
-module.search_ports		2
-module.access_mode		sequential
-module.TDP_duty_cycle.read		0.500000
-module.TDP_duty_cycle.write		0.500000
-module.TDP_duty_cycle.search		0.500000
-module.area_scaling		1.210000
-module.end

-module.name		core:LoadQueue:0
-module.partition		CORE_DIE:MEM0
-module.energy_library		McPAT
-module.energy_model		array
-module.energy_submodel		cache
-module.line_width		8
-module.output_width		64
-module.assoc		0
-module.size		768 # 96 entries
-module.tag_width		85
-module.rd_ports		2
-module.wr_ports		2
-module.search_ports		2
-module.access_mode		sequential
-module.TDP_duty_cycle.read		0.500000
-module.TDP_duty_cycle.write		0.500000
-module.TDP_duty_cycle.search		0.500000
-module.area_scaling		1.100000
-module.end

-module.name		core:DTLB:0
-module.partition		CORE_DIE:MEM0
-module.energy_library		McPAT
-module.energy_model		array
-module.energy_submodel		cache
-module.line_width		5
-module.output_width		40
-module.assoc		0
-module.size		640
-module.tag_width		57
-module.rd_ports		2
-module.wr_ports		2
-module.search_ports		2
-module.cycle_time		1
-module.access_time		2
-module.access_mode		normal
-module.TDP_duty_cycle.read		0.000000
-module.TDP_duty_cycle.write		0.000000
-module.TDP_duty_cycle.search		0.500000
-module.end

#bypass interconnect
-module.name			core:ALU_bypass:0
-module.partition		CORE_DIE:INT0
-module.energy_library		McPAT
-module.energy_model		interconnect
-module.data			72 # data(64bit) + tag(8bit)
-module.connect			core:RS:0
-module.connect			core:ROB:0
-module.connect			core:ALU:0
-module.end

#bypass interconnect
-module.name			core:FPU_bypass:0
-module.partition		CORE_DIE:FPU0
-module.energy_library		McPAT
-module.energy_model		interconnect
-module.data			88 # data(80bit) + tag(8bit)
-module.connect			core:RS:0
-module.connect			core:ROB:0
-module.connect			core:FPU:0
-module.end

#bypass interconnect
-module.name			core:MUL_bypass:0
-module.partition		CORE_DIE:FPU0
-module.energy_library		McPAT
-module.energy_model		interconnect
-module.data			72 # data(64bit) + tag(8bit)
-module.connect			core:RS:0
-module.connect			core:ROB:0
-module.connect			core:MUL:0
-module.end

#bypass interconnect
-module.name			core:LD_bypass:0
-module.partition		CORE_DIE:MEM0
-module.energy_library		McPAT
-module.energy_model		interconnect
-module.data			72 # data(64bit) + tag(8bit)
-module.connect			core:RS:0
-module.connect			core:LoadQueue:0
-module.end

// microarchitecture signaling wires and latches
#program counter
-module.name					core:PC:0
-module.partition				CORE_DIE:FRT0
-module.energy_library				McPAT
-module.energy_model				pipeline
-module.pipeline_stages				1
-module.per_stage_vector			128 # current PC(64 bits) + next PC(64 bits)
-module.end

#latch: instruction queue to decoder
-module.name					core:pipe:IB2ID:0
-module.partition				CORE_DIE:FRT0
-module.energy_library				McPAT
-module.energy_model				pipeline
-module.pipeline_stages				1
-module.per_stage_vector			192 # instruction length (16 bytes) + PC
-module.scaling					4 # decode width
-module.end

#latch: instruction decoder to uopQ
-module.name					core:pipe:ID2uQ:0
-module.partition				CORE_DIE:FRT0
-module.energy_library				McPAT
-module.energy_model				pipeline
-module.pipeline_stages				1
-module.per_stage_vector		  192 # instruction length (16 bytes) + PC
-module.scaling					4 # decode width
-module.end

#latch: uopQ to scheduler stage 1 (renaming)
-module.name					core:pipe:uQ2RR:0
-module.partition				CORE_DIE:SCH0
-module.energy_library				McPAT
-module.energy_model				pipeline
-module.pipeline_stages				1
-module.per_stage_vector			192 # instruction length (16 bytes) + PC
-module.scaling					6 # (peak) issue width
-module.end

#latch: uopQ to scheduler stage 2-1 (issue/schedule)
-module.name					core:pipe:RR2SCH:0
-module.partition				CORE_DIE:SCH0
-module.energy_library				McPAT
-module.energy_model				pipeline
-module.pipeline_stages				1
-module.per_stage_vector			216 # instruction length (16 bytes) + 3*operand tags(8 bits) + PC
-module.scaling					6 # issue width
-module.end

#latch: issue/schedule to execution payload/dispatch
-module.name					core:pipe:SCH2EXP:0
-module.partition				CORE_DIE:SCH0
-module.energy_library				McPAT
-module.energy_model				pipeline
-module.pipeline_stages				1
-module.per_stage_vector			152 # instruction length (16 bytes) + 3*operand tags(8 bits)
-module.scaling					6 # issue width
-module.end

#latch: execution payload/dispatch to execution units
-module.name					core:pipe:EXP2ALU:0
-module.partition				CORE_DIE:INT0
-module.energy_library				McPAT
-module.energy_model				pipeline
-module.pipeline_stages				1
-module.per_stage_vector			264 # instruction length (16 bytes) + 2*data width (64 bits) + output operand tag (8 bits)
-module.scaling					4 # execution width
-module.end

#latch: execution payload/dispatch to execution units
-module.name					core:pipe:EXP2FPU:0
-module.partition				CORE_DIE:FPU0
-module.energy_library				McPAT
-module.energy_model				pipeline
-module.pipeline_stages				1
-module.per_stage_vector			264 # instruction length (16 bytes) + 2*data width (64 bits) + output operand tag (8 bits)
-module.scaling					2 # execution width
-module.end

#latch: execution units to result (ROB/bypass)
-module.name					core:pipe:ALU2ROB:0
-module.partition				CORE_DIE:INT0
-module.energy_library				McPAT
-module.energy_model				pipeline
-module.pipeline_stages				1
-module.per_stage_vector			72 # data width (64 bits) + output operand tag (8 bits)
-module.scaling					4 # execution width
-module.end

#latch: execution units to result (ROB/bypass)
-module.name					core:pipe:FPU2ROB:0
-module.partition				CORE_DIE:FPU0
-module.energy_library				McPAT
-module.energy_model				pipeline
-module.pipeline_stages				1
-module.per_stage_vector			72 # data width (64 bits) + output operand tag (8 bits)
-module.scaling					2 # execution width
-module.end

#latch: commit and writeback
-module.name					core:pipe:ROB2CM:0
-module.partition				CORE_DIE:SCH0
-module.energy_library				McPAT
-module.energy_model				pipeline
-module.pipeline_stages				1
-module.per_stage_vector			130 # data width (64 bits) + address width (64 bits) + output operand tag (8 bits)
-module.scaling					4 # commit width
-module.end

-module.name		core:DL2:0
-module.partition		DL2_DIE:SPOT0
-module.energy_library		McPAT
-module.energy_model		array
-module.energy_submodel		cache
-module.line_width		128
-module.output_width		1024
-module.assoc		16
-module.size		2097152 # 8MB (1024 sets)
-module.tag_width		15
-module.rw_ports		1
-module.cycle_time		4
-module.access_time		35
-module.access_mode		sequential
-module.TDP_duty_cycle.read		0.335000
-module.TDP_duty_cycle.write		0.165000
-module.end

-module.name		core:undiffCore:Frontend:0
-module.partition		CORE_DIE:FRT0
-module.energy_library		McPAT
-module.energy_model		undifferentiated_core
-module.pipeline_stages		31
-module.hthreads		1
-module.issue_width		4
-module.scaling	0.07
-module.end

-module.name		core:undiffCore:Schedule:0
-module.partition		CORE_DIE:SCH0
-module.energy_library		McPAT
-module.energy_model		undifferentiated_core
-module.pipeline_stages		31
-module.hthreads		1
-module.issue_width		4
-module.scaling	0.09
-module.end

-module.name		core:undiffCore:Execute:INT:0
-module.partition		CORE_DIE:INT0
-module.energy_library		McPAT
-module.energy_model		undifferentiated_core
-module.pipeline_stages		31
-module.hthreads		1
-module.issue_width		4
-module.scaling	0.15
-module.end

-module.name		core:undiffCore:Execute:FP:0
-module.partition		CORE_DIE:FPU0
-module.energy_library		McPAT
-module.energy_model		undifferentiated_core
-module.pipeline_stages		31
-module.hthreads		1
-module.issue_width		4
-module.scaling	0.10
-module.end

-module.name		core:undiffCore:Memory:DL1:0
-module.partition		CORE_DIE:MEM0
-module.energy_library		McPAT
-module.energy_model		undifferentiated_core
-module.pipeline_stages		31
-module.hthreads		1
-module.issue_width		4
-module.scaling	0.11
-module.end

-module.name		core:undiffCore:Memory:DL2:0
-module.partition		DL2_DIE:SPOT0
-module.energy_library		McPAT
-module.energy_model		undifferentiated_core
-module.pipeline_stages		31
-module.hthreads		1
-module.issue_width		4
-module.scaling	0.48
-module.end

# CORE0 DESCRIPTION ENDS