/*
   The Energy Introspector (EI) is the simulation interface and does not possess 
   the copyrights of the following tools: 
      IntSim: Microelectronics Research Center, Georgia Tech
      DSENT: Computer Science and Artificial Intelligence Lab, MIT
      McPAT: Hewlett-Packard Labs
      3D-ICE: Embedded Systems Lab, EPFL
      HotSpot: Laboratory for Computer Architecture at Virginia, Univ. of Virginia
  
   The EI does not guarantee the exact same functionality, performance, or result
   of the above tools distributed by the original developers.
  
   The Use, modification, and distribution of the EI is subject to the policy of
   the Semiconductor Research Corporation (Task 2084.001).
  
   Copyright 2012
   William Song and Sudhakar Yalamanchili
   Georgia Institute of Technology, Atlanta, GA 30332
*/

#ifndef THERMALLIB_3DICE_H
#define THERMALLIB_3DICE_H

#include "energy_introspector.h"

extern "C" {
#include "THERMALLIB_3DICE/3d-ice/include/analysis.h"
#include "THERMALLIB_3DICE/3d-ice/include/channel.h"
#include "THERMALLIB_3DICE/3d-ice/include/conventional_heat_sink.h"
#include "THERMALLIB_3DICE/3d-ice/include/die.h"
#include "THERMALLIB_3DICE/3d-ice/include/dimensions.h"
#include "THERMALLIB_3DICE/3d-ice/include/floorplan.h"
#include "THERMALLIB_3DICE/3d-ice/include/floorplan_element.h"
#include "THERMALLIB_3DICE/3d-ice/include/ic_element.h"
#include "THERMALLIB_3DICE/3d-ice/include/inspection_point.h"
#include "THERMALLIB_3DICE/3d-ice/include/layer.h"
#include "THERMALLIB_3DICE/3d-ice/include/macros.h"
#include "THERMALLIB_3DICE/3d-ice/include/material.h"
#include "THERMALLIB_3DICE/3d-ice/include/network_message.h"
#include "THERMALLIB_3DICE/3d-ice/include/powers_queue.h"
#include "THERMALLIB_3DICE/3d-ice/include/stack_element.h"
#include "THERMALLIB_3DICE/3d-ice/include/system_matrix.h"
#include "THERMALLIB_3DICE/3d-ice/include/thermal_cell.h"
#include "THERMALLIB_3DICE/3d-ice/include/thermal_data.h"
#include "THERMALLIB_3DICE/3d-ice/include/types.h"
}

namespace EI {

  class THERMALLIB_3DICE : public thermal_library_t
  {
  public:
    THERMALLIB_3DICE(string component_name, parameters_t *parameters, energy_introspector_t *ei);
    ~THERMALLIB_3DICE() {}
    
    virtual void compute_temperature(double time_tick, double period);
    virtual grid_t<double> get_thermal_grid(void);
    virtual double get_partition_temperature(string partition_name, int type);
    virtual double get_point_temperature(double x, double y, int layer);
    virtual int get_partition_layer_index(string partition_name);
    virtual void deliver_power(string partition_name, power_t partition_power);
    
  private:
    StackDescription stkd;
    Analysis analysis;
    ThermalData tdata; 
    
    SimResult_t (*emulate) (ThermalData*, StackDescription*, Analysis*);
    SimResult_t sim_result;
    Error_t error; 
    
    int thermal_grid_mapping;
    enum ICE_THERMAL_GRID_MAPPING
    {
      MAX_TEMP = 0, 
      MIN_TEMP, 
      AVG_TEMP,      
      CENTER_TEMP,
      NUM_THERMAL_GRID_MAPPINGS,
      NO_THERMAL_GRID_MAPPING      
    };
  };
  
  #ifndef INT32_MAX
  #define INT32_MAX 2147483647
  #endif
  
} // namespace EI
  
#endif
