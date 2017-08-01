/*
 * dram_print_power.h
 *
 *  Created on: Jul 21, 2017
 *      Author: karthik
 */

#ifndef DRAM_PRINT_POWER_H_
#define DRAM_PRINT_POWER_H_

#include <vector>
#include <numeric> //For accumulate
#include <algorithm> //For max_element
#include "kernel/component.h"
#include "kernel/clock.h"
//#include "dvfs.h"

#include "../../models/cache/mcp-cache/hash_table.h"

#include "../../models/energy_introspector/energy_introspector.h"
#include "../../models/cache/mcp-cache/cache-counters.h"
#include "../../models/cache/mcp-cache/mux_demux.h"
#include "../../models/processor/spx/pipeline.h"
#include "../../models/processor/spx/core.h"
#include "../../models/memory/DRAMSim2/dram_sim.h"

void dram_print(manifold::dramsim::Dram_sim* mc, int i);



#endif /* DRAM_PRINT_POWER_H_ */
