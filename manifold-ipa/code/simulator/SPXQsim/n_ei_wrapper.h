/*
 * n_ei_wrapper.h
 *
 *  Created on: Aug 16, 2017
 *      Author: karthik
 */

#ifndef N_EI_WRAPPER_H_
#define N_EI_WRAPPER_H_

#include <vector>
#include <numeric> //For accumulate
#include <algorithm> //For max_element
#include "kernel/component.h"
#include "kernel/clock.h"


#include "../../models/cache/mcp-cache/hash_table.h"

#include "../../models/cache/mcp-cache/cache-counters.h"
#include "../../models/cache/mcp-cache/mux_demux.h"
#include "../../models/processor/spx/pipeline.h"
#include "../../models/processor/spx/core.h"
#include "../../models/memory/DRAMSim2/dram_sim.h"


namespace manifold {
namespace n_ei_wrapper {
typedef unsigned int tick_t;

class n_ei_wrapper_t : public manifold::kernel::Component
{
public:
  n_ei_wrapper_t(manifold::kernel::Clock* clk, double supply_voltage, vector<manifold::spx::spx_core_t*> p_core, manifold::spx::pipeline_counter_t* proc_cnt,
		  manifold::mcp_cache_namespace::L1_counter_t* c1_cnt, manifold::mcp_cache_namespace::L2_counter_t* c2_cnt,
		  manifold::mcp_cache_namespace::LLP_cache* p_l1, manifold::mcp_cache_namespace::LLS_cache* p_l2, manifold::dramsim::Dram_sim* mc,
		  double sampling_period, int num_nodes, int uid);

  ~n_ei_wrapper_t();

  // manifold component functions
  void tick();
  void swap_cores(int src, int dst)
  {
	  int temp;
	  temp = p_cores_global[src]->get_core_id();
	  p_cores_global[src]->set_core_id(p_cores_global[dst]->get_core_id());
	  p_cores_global[dst]->set_core_id(temp);
  }
  uint64_t sam_cycle;
  tick_t slack_cycle;
  double init_vdd;
  int num_samples;
  vector<manifold::spx::spx_core_t*> p_cores_global;

private:
  manifold::kernel::Clock *clock;

  manifold::spx::pipeline_counter_t *p_cnt;
  manifold::mcp_cache_namespace::L1_counter_t *l1_cnt;
  manifold::mcp_cache_namespace::L2_counter_t *l2_cnt;
  manifold::mcp_cache_namespace::LLP_cache *p_l1cache;
  manifold::mcp_cache_namespace::LLS_cache *p_l2cache;
  manifold::dramsim::Dram_sim *mem_ctrl;

  double V_Old;
  int id;
};

} // namespace n_ei_wrapper
} // namespace manifold



#endif /* N_EI_WRAPPER_H_ */
