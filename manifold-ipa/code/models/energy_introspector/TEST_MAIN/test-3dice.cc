#include <iostream>
#include <assert.h>
#include <stdlib.h>

#include "../energy_introspector.h"
using namespace EI;

int main(int argc, char* argv[])
{
  energy_introspector_t *energy_introspector = new energy_introspector_t((char*)"CONFIG/3DICE/3d-ice_test.config");

  power_t dl2_die_spot0[5], dl2_die_spot1[5], dl2_die_spot2[5], dl2_die_spot3[5];//, dl2_die_spot4[5], dl2_die_spot6[5], dl2_die_spot8[5], dl2_die_spot9[5], dl2_die_spot12[5];
  power_t core0_frt[5], core0_fpu[5], core0_int[5], core0_sch[5], core0_mem[5];
  power_t core1_frt[5], core1_fpu[5], core1_int[5], core1_sch[5], core1_mem[5];
  power_t core2_frt[5], core2_fpu[5], core2_int[5], core2_sch[5], core2_mem[5];
  power_t core3_frt[5], core3_fpu[5], core3_int[5], core3_sch[5], core3_mem[5];
/*  power_t core4_frt[5], core4_fpu[5], core4_int[5], core4_sch[5], core4_mem[5];
  power_t core6_frt[5], core6_fpu[5], core6_int[5], core6_sch[5], core6_mem[5];
  power_t core8_frt[5], core8_fpu[5], core8_int[5], core8_sch[5], core8_mem[5];
  power_t core9_frt[5], core9_fpu[5], core9_int[5], core9_sch[5], core9_mem[5];
  power_t core12_frt[5], core12_fpu[5], core12_int[5], core12_sch[5], core12_mem[5]; 
*/
  if (argc < 2)
  {
    cout << "Requires 2 arguments: SPOTPOWER and COREPOWER. COREPOWER*5 = TOTAL CORE POWER" << endl;
    cout << "Usage: ./test-3dice SPOTPOWER COREPOWER" << endl;
    exit(0);
  }

  double COREPOWER = atof(argv[2]);
  double SPOTPOWER = atof(argv[1]);
  double core_temp = 0.0, core_temp_acc = 0.0;

  cout << "COREPOWER= " << COREPOWER << endl;
  cout << "SPOTPOWER= " << SPOTPOWER << endl;

  for(int i = 0; i < 5; i++)
  {
    dl2_die_spot0[i].total = SPOTPOWER;
    dl2_die_spot1[i].total = SPOTPOWER;
    dl2_die_spot2[i].total = SPOTPOWER;
    dl2_die_spot3[i].total = SPOTPOWER;
/*    dl2_die_spot4[i].total = SPOTPOWER;
    dl2_die_spot6[i].total = SPOTPOWER;
    dl2_die_spot8[i].total = SPOTPOWER;
    dl2_die_spot9[i].total = SPOTPOWER;
    dl2_die_spot12[i].total = SPOTPOWER;*/
/*
    core0_frt[i].total = COREPOWER; core0_fpu[i].total = COREPOWER; core0_int[i].total = COREPOWER; core0_sch[i].total = COREPOWER; core0_mem[i].total = COREPOWER;
    core1_frt[i].total = COREPOWER; core1_fpu[i].total = COREPOWER; core1_int[i].total = COREPOWER; core1_sch[i].total = COREPOWER; core1_mem[i].total = COREPOWER;
    core2_frt[i].total = COREPOWER; core2_fpu[i].total = COREPOWER; core2_int[i].total = COREPOWER; core2_sch[i].total = COREPOWER; core2_mem[i].total = COREPOWER;
    core3_frt[i].total = COREPOWER; core3_fpu[i].total = COREPOWER; core3_int[i].total = COREPOWER; core3_sch[i].total = COREPOWER; core3_mem[i].total = COREPOWER;
    core4_frt[i].total = COREPOWER; core4_fpu[i].total = COREPOWER; core4_int[i].total = COREPOWER; core4_sch[i].total = COREPOWER; core4_mem[i].total = COREPOWER;
    core6_frt[i].total = COREPOWER; core6_fpu[i].total = COREPOWER; core6_int[i].total = COREPOWER; core6_sch[i].total = COREPOWER; core6_mem[i].total = COREPOWER;
    core8_frt[i].total = COREPOWER; core8_fpu[i].total = COREPOWER; core8_int[i].total = COREPOWER; core8_sch[i].total = COREPOWER; core8_mem[i].total = COREPOWER;
    core9_frt[i].total = COREPOWER; core9_fpu[i].total = COREPOWER; core9_int[i].total = COREPOWER; core9_sch[i].total = COREPOWER; core9_mem[i].total = COREPOWER;
    core12_frt[i].total = COREPOWER; core12_fpu[i].total = COREPOWER; core12_int[i].total = COREPOWER; core12_sch[i].total = COREPOWER; core12_mem[i].total = COREPOWER;
*/
  }
/*  
  dl2_die_spot0[0].total = SPOTPOWER;
  dl2_die_spot0[1].total = SPOTPOWER;
  dl2_die_spot0[2].total = SPOTPOWER;
  dl2_die_spot0[3].total = SPOTPOWER;
  dl2_die_spot0[4].total = SPOTPOWER;

  dl2_die_spot1[0].total = SPOTPOWER;
  dl2_die_spot1[1].total = SPOTPOWER;
  dl2_die_spot1[2].total = SPOTPOWER;
  dl2_die_spot1[3].total = SPOTPOWER;
  dl2_die_spot1[4].total = SPOTPOWER;

  dl2_die_spot2[0].total = SPOTPOWER;
  dl2_die_spot2[1].total = SPOTPOWER;
  dl2_die_spot2[2].total = SPOTPOWER;
  dl2_die_spot2[3].total = SPOTPOWER;
  dl2_die_spot2[4].total = SPOTPOWER;

  dl2_die_spot3[0].total = SPOTPOWER;
  dl2_die_spot3[1].total = SPOTPOWER;
  dl2_die_spot3[2].total = SPOTPOWER;
  dl2_die_spot3[3].total = SPOTPOWER;
  dl2_die_spot3[4].total = SPOTPOWER;

  dl2_die_spot4[0].total = SPOTPOWER;
  dl2_die_spot4[1].total = SPOTPOWER;
  dl2_die_spot4[2].total = SPOTPOWER;
  dl2_die_spot4[3].total = SPOTPOWER;
  dl2_die_spot4[4].total = SPOTPOWER;

  dl2_die_spot6[0].total = SPOTPOWER;
  dl2_die_spot6[1].total = SPOTPOWER;
  dl2_die_spot6[2].total = SPOTPOWER;
  dl2_die_spot6[3].total = SPOTPOWER;
  dl2_die_spot6[4].total = SPOTPOWER;

  dl2_die_spot8[0].total = SPOTPOWER;
  dl2_die_spot8[1].total = SPOTPOWER;
  dl2_die_spot8[2].total = SPOTPOWER;
  dl2_die_spot8[3].total = SPOTPOWER;
  dl2_die_spot8[4].total = SPOTPOWER;

  dl2_die_spot9[0].total = SPOTPOWER;
  dl2_die_spot9[1].total = SPOTPOWER;
  dl2_die_spot9[2].total = SPOTPOWER;
  dl2_die_spot9[3].total = SPOTPOWER;
  dl2_die_spot9[4].total = SPOTPOWER;

  dl2_die_spot12[0].total = SPOTPOWER;
  dl2_die_spot12[1].total = SPOTPOWER;
  dl2_die_spot12[2].total = SPOTPOWER;
  dl2_die_spot12[3].total = SPOTPOWER;
  dl2_die_spot12[4].total = SPOTPOWER;
*/
  core0_frt[0].total = COREPOWER; core0_fpu[0].total = COREPOWER; core0_int[0].total = COREPOWER; core0_sch[0].total = COREPOWER; core0_mem[0].total = COREPOWER;
  core1_frt[0].total = COREPOWER; core1_fpu[0].total = COREPOWER; core1_int[0].total = COREPOWER; core1_sch[0].total = COREPOWER; core1_mem[0].total = COREPOWER;
  core2_frt[0].total = COREPOWER; core2_fpu[0].total = COREPOWER; core2_int[0].total = COREPOWER; core2_sch[0].total = COREPOWER; core2_mem[0].total = COREPOWER;
  core3_frt[0].total = COREPOWER; core3_fpu[0].total = COREPOWER; core3_int[0].total = COREPOWER; core3_sch[0].total = COREPOWER; core3_mem[0].total = COREPOWER;
/*  core4_frt[0].total = COREPOWER; core4_fpu[0].total = COREPOWER; core4_int[0].total = COREPOWER; core4_sch[0].total = COREPOWER; core4_mem[0].total = COREPOWER;
  core6_frt[0].total = COREPOWER; core6_fpu[0].total = COREPOWER; core6_int[0].total = COREPOWER; core6_sch[0].total = COREPOWER; core6_mem[0].total = COREPOWER;
  core8_frt[0].total = COREPOWER; core8_fpu[0].total = COREPOWER; core8_int[0].total = COREPOWER; core8_sch[0].total = COREPOWER; core8_mem[0].total = COREPOWER;
  core9_frt[0].total = COREPOWER; core9_fpu[0].total = COREPOWER; core9_int[0].total = COREPOWER; core9_sch[0].total = COREPOWER; core9_mem[0].total = COREPOWER;
  core12_frt[0].total = COREPOWER; core12_fpu[0].total = COREPOWER; core12_int[0].total = COREPOWER; core12_sch[0].total = COREPOWER; core12_mem[0].total = COREPOWER;*/

  core0_frt[1].total = COREPOWER; core0_fpu[1].total = COREPOWER; core0_int[1].total = COREPOWER; core0_sch[1].total = COREPOWER; core0_mem[1].total = COREPOWER;
  core1_frt[1].total = COREPOWER; core1_fpu[1].total = COREPOWER; core1_int[1].total = COREPOWER; core1_sch[1].total = COREPOWER; core1_mem[1].total = COREPOWER;
  core2_frt[1].total = COREPOWER; core2_fpu[1].total = COREPOWER; core2_int[1].total = COREPOWER; core2_sch[1].total = COREPOWER; core2_mem[1].total = COREPOWER;
  core3_frt[1].total = COREPOWER; core3_fpu[1].total = COREPOWER; core3_int[1].total = COREPOWER; core3_sch[1].total = COREPOWER; core3_mem[1].total = COREPOWER;
/*  core4_frt[1].total = COREPOWER; core4_fpu[1].total = COREPOWER; core4_int[1].total = COREPOWER; core4_sch[1].total = COREPOWER; core4_mem[1].total = COREPOWER;
  core6_frt[1].total = COREPOWER; core6_fpu[1].total = COREPOWER; core6_int[1].total = COREPOWER; core6_sch[1].total = COREPOWER; core6_mem[1].total = COREPOWER;
  core8_frt[1].total = COREPOWER; core8_fpu[1].total = COREPOWER; core8_int[1].total = COREPOWER; core8_sch[1].total = COREPOWER; core8_mem[1].total = COREPOWER;
  core9_frt[1].total = COREPOWER; core9_fpu[1].total = COREPOWER; core9_int[1].total = COREPOWER; core9_sch[1].total = COREPOWER; core9_mem[1].total = COREPOWER;
  core12_frt[1].total = COREPOWER; core12_fpu[1].total = COREPOWER; core12_int[1].total = COREPOWER; core12_sch[1].total = COREPOWER; core12_mem[1].total = COREPOWER;*/

  core0_frt[2].total = COREPOWER; core0_fpu[2].total = COREPOWER; core0_int[2].total = COREPOWER; core0_sch[2].total = COREPOWER; core0_mem[2].total = COREPOWER;
  core1_frt[2].total = COREPOWER; core1_fpu[2].total = COREPOWER; core1_int[2].total = COREPOWER; core1_sch[2].total = COREPOWER; core1_mem[2].total = COREPOWER;
  core2_frt[2].total = COREPOWER; core2_fpu[2].total = COREPOWER; core2_int[2].total = COREPOWER; core2_sch[2].total = COREPOWER; core2_mem[2].total = COREPOWER;
  core3_frt[2].total = COREPOWER; core3_fpu[2].total = COREPOWER; core3_int[2].total = COREPOWER; core3_sch[2].total = COREPOWER; core3_mem[2].total = COREPOWER;
/*  core4_frt[2].total = COREPOWER; core4_fpu[2].total = COREPOWER; core4_int[2].total = COREPOWER; core4_sch[2].total = COREPOWER; core4_mem[2].total = COREPOWER;
  core6_frt[2].total = COREPOWER; core6_fpu[2].total = COREPOWER; core6_int[2].total = COREPOWER; core6_sch[2].total = COREPOWER; core6_mem[2].total = COREPOWER;
  core8_frt[2].total = COREPOWER; core8_fpu[2].total = COREPOWER; core8_int[2].total = COREPOWER; core8_sch[2].total = COREPOWER; core8_mem[2].total = COREPOWER;
  core9_frt[2].total = COREPOWER; core9_fpu[2].total = COREPOWER; core9_int[2].total = COREPOWER; core9_sch[2].total = COREPOWER; core9_mem[2].total = COREPOWER;
  core12_frt[2].total = COREPOWER; core12_fpu[2].total = COREPOWER; core12_int[2].total = COREPOWER; core12_sch[2].total = COREPOWER; core12_mem[2].total = COREPOWER;*/

  core0_frt[3].total = COREPOWER; core0_fpu[3].total = COREPOWER; core0_int[3].total = COREPOWER; core0_sch[3].total = COREPOWER; core0_mem[3].total = COREPOWER;
  core1_frt[3].total = COREPOWER; core1_fpu[3].total = COREPOWER; core1_int[3].total = COREPOWER; core1_sch[3].total = COREPOWER; core1_mem[3].total = COREPOWER;
  core2_frt[3].total = COREPOWER; core2_fpu[3].total = COREPOWER; core2_int[3].total = COREPOWER; core2_sch[3].total = COREPOWER; core2_mem[3].total = COREPOWER;
  core3_frt[3].total = COREPOWER; core3_fpu[3].total = COREPOWER; core3_int[3].total = COREPOWER; core3_sch[3].total = COREPOWER; core3_mem[3].total = COREPOWER;
/*  core4_frt[3].total = COREPOWER; core4_fpu[3].total = COREPOWER; core4_int[3].total = COREPOWER; core4_sch[3].total = COREPOWER; core4_mem[3].total = COREPOWER;
  core6_frt[3].total = COREPOWER; core6_fpu[3].total = COREPOWER; core6_int[3].total = COREPOWER; core6_sch[3].total = COREPOWER; core6_mem[3].total = COREPOWER;
  core8_frt[3].total = COREPOWER; core8_fpu[3].total = COREPOWER; core8_int[3].total = COREPOWER; core8_sch[3].total = COREPOWER; core8_mem[3].total = COREPOWER;
  core9_frt[3].total = COREPOWER; core9_fpu[3].total = COREPOWER; core9_int[3].total = COREPOWER; core9_sch[3].total = COREPOWER; core9_mem[3].total = COREPOWER;
  core12_frt[3].total = COREPOWER; core12_fpu[3].total = COREPOWER; core12_int[3].total = COREPOWER; core12_sch[3].total = COREPOWER; core12_mem[3].total = COREPOWER;*/

  core0_frt[4].total = COREPOWER; core0_fpu[4].total = COREPOWER; core0_int[4].total = COREPOWER; core0_sch[4].total = COREPOWER; core0_mem[4].total = COREPOWER;
  core1_frt[4].total = COREPOWER; core1_fpu[4].total = COREPOWER; core1_int[4].total = COREPOWER; core1_sch[4].total = COREPOWER; core1_mem[4].total = COREPOWER;
  core2_frt[4].total = COREPOWER; core2_fpu[4].total = COREPOWER; core2_int[4].total = COREPOWER; core2_sch[4].total = COREPOWER; core2_mem[4].total = COREPOWER;
  core3_frt[4].total = COREPOWER; core3_fpu[4].total = COREPOWER; core3_int[4].total = COREPOWER; core3_sch[4].total = COREPOWER; core3_mem[4].total = COREPOWER;
/*  core4_frt[4].total = COREPOWER; core4_fpu[4].total = COREPOWER; core4_int[4].total = COREPOWER; core4_sch[4].total = COREPOWER; core4_mem[4].total = COREPOWER;
  core6_frt[4].total = COREPOWER; core6_fpu[4].total = COREPOWER; core6_int[4].total = COREPOWER; core6_sch[4].total = COREPOWER; core6_mem[4].total = COREPOWER;
  core8_frt[4].total = COREPOWER; core8_fpu[4].total = COREPOWER; core8_int[4].total = COREPOWER; core8_sch[4].total = COREPOWER; core8_mem[4].total = COREPOWER;
  core9_frt[4].total = COREPOWER; core9_fpu[4].total = COREPOWER; core9_int[4].total = COREPOWER; core9_sch[4].total = COREPOWER; core9_mem[4].total = COREPOWER;
  core12_frt[4].total = COREPOWER; core12_fpu[4].total = COREPOWER; core12_int[4].total = COREPOWER; core12_sch[4].total = COREPOWER; core12_mem[4].total = COREPOWER;*/

  for(int i = 0; i < 5; i++)
  {
    // DL2_DIE
    energy_introspector->push_data<power_t>((double)i*0.02+0.02,0.02,"partition","DL2_DIE:SPOT0","power",dl2_die_spot0[i]);
    energy_introspector->push_data<power_t>((double)i*0.02+0.02,0.02,"partition","DL2_DIE:SPOT1","power",dl2_die_spot1[i]);
    energy_introspector->push_data<power_t>((double)i*0.02+0.02,0.02,"partition","DL2_DIE:SPOT2","power",dl2_die_spot2[i]);
    energy_introspector->push_data<power_t>((double)i*0.02+0.02,0.02,"partition","DL2_DIE:SPOT3","power",dl2_die_spot3[i]);
/*    energy_introspector->push_data<power_t>((double)i*0.02+0.02,0.02,"partition","DL2_DIE:SPOT4","power",dl2_die_spot4[i]);
    energy_introspector->push_data<power_t>((double)i*0.02+0.02,0.02,"partition","DL2_DIE:SPOT6","power",dl2_die_spot6[i]);
    energy_introspector->push_data<power_t>((double)i*0.02+0.02,0.02,"partition","DL2_DIE:SPOT8","power",dl2_die_spot8[i]);
    energy_introspector->push_data<power_t>((double)i*0.02+0.02,0.02,"partition","DL2_DIE:SPOT9","power",dl2_die_spot9[i]);
    energy_introspector->push_data<power_t>((double)i*0.02+0.02,0.02,"partition","DL2_DIE:SPOT12","power",dl2_die_spot12[i]);*/

    // CORE_DIE
    energy_introspector->push_data<power_t>((double)i*0.02+0.02,0.02,"partition","CORE_DIE:FRT0","power",core0_frt[i]);
    energy_introspector->push_data<power_t>((double)i*0.02+0.02,0.02,"partition","CORE_DIE:FPU0","power",core0_fpu[i]);
    energy_introspector->push_data<power_t>((double)i*0.02+0.02,0.02,"partition","CORE_DIE:INT0","power",core0_int[i]);
    energy_introspector->push_data<power_t>((double)i*0.02+0.02,0.02,"partition","CORE_DIE:SCH0","power",core0_sch[i]);
    energy_introspector->push_data<power_t>((double)i*0.02+0.02,0.02,"partition","CORE_DIE:MEM0","power",core0_mem[i]);

    energy_introspector->push_data<power_t>((double)i*0.02+0.02,0.02,"partition","CORE_DIE:FRT1","power",core1_frt[i]);
    energy_introspector->push_data<power_t>((double)i*0.02+0.02,0.02,"partition","CORE_DIE:FPU1","power",core1_fpu[i]);
    energy_introspector->push_data<power_t>((double)i*0.02+0.02,0.02,"partition","CORE_DIE:INT1","power",core1_int[i]);
    energy_introspector->push_data<power_t>((double)i*0.02+0.02,0.02,"partition","CORE_DIE:SCH1","power",core1_sch[i]);
    energy_introspector->push_data<power_t>((double)i*0.02+0.02,0.02,"partition","CORE_DIE:MEM1","power",core1_mem[i]);

    energy_introspector->push_data<power_t>((double)i*0.02+0.02,0.02,"partition","CORE_DIE:FRT2","power",core2_frt[i]);
    energy_introspector->push_data<power_t>((double)i*0.02+0.02,0.02,"partition","CORE_DIE:FPU2","power",core2_fpu[i]);
    energy_introspector->push_data<power_t>((double)i*0.02+0.02,0.02,"partition","CORE_DIE:INT2","power",core2_int[i]);
    energy_introspector->push_data<power_t>((double)i*0.02+0.02,0.02,"partition","CORE_DIE:SCH2","power",core2_sch[i]);
    energy_introspector->push_data<power_t>((double)i*0.02+0.02,0.02,"partition","CORE_DIE:MEM2","power",core2_mem[i]);

    energy_introspector->push_data<power_t>((double)i*0.02+0.02,0.02,"partition","CORE_DIE:FRT3","power",core3_frt[i]);
    energy_introspector->push_data<power_t>((double)i*0.02+0.02,0.02,"partition","CORE_DIE:FPU3","power",core3_fpu[i]);
    energy_introspector->push_data<power_t>((double)i*0.02+0.02,0.02,"partition","CORE_DIE:INT3","power",core3_int[i]);
    energy_introspector->push_data<power_t>((double)i*0.02+0.02,0.02,"partition","CORE_DIE:SCH3","power",core3_sch[i]);
    energy_introspector->push_data<power_t>((double)i*0.02+0.02,0.02,"partition","CORE_DIE:MEM3","power",core3_mem[i]);
/*
    energy_introspector->push_data<power_t>((double)i*0.02+0.02,0.02,"partition","CORE_DIE:FRT4","power",core4_frt[i]);
    energy_introspector->push_data<power_t>((double)i*0.02+0.02,0.02,"partition","CORE_DIE:FPU4","power",core4_fpu[i]);
    energy_introspector->push_data<power_t>((double)i*0.02+0.02,0.02,"partition","CORE_DIE:INT4","power",core4_int[i]);
    energy_introspector->push_data<power_t>((double)i*0.02+0.02,0.02,"partition","CORE_DIE:SCH4","power",core4_sch[i]);
    energy_introspector->push_data<power_t>((double)i*0.02+0.02,0.02,"partition","CORE_DIE:MEM4","power",core4_mem[i]);

    energy_introspector->push_data<power_t>((double)i*0.02+0.02,0.02,"partition","CORE_DIE:FRT6","power",core6_frt[i]);
    energy_introspector->push_data<power_t>((double)i*0.02+0.02,0.02,"partition","CORE_DIE:FPU6","power",core6_fpu[i]);
    energy_introspector->push_data<power_t>((double)i*0.02+0.02,0.02,"partition","CORE_DIE:INT6","power",core6_int[i]);
    energy_introspector->push_data<power_t>((double)i*0.02+0.02,0.02,"partition","CORE_DIE:SCH6","power",core6_sch[i]);
    energy_introspector->push_data<power_t>((double)i*0.02+0.02,0.02,"partition","CORE_DIE:MEM6","power",core6_mem[i]);

    energy_introspector->push_data<power_t>((double)i*0.02+0.02,0.02,"partition","CORE_DIE:FRT8","power",core8_frt[i]);
    energy_introspector->push_data<power_t>((double)i*0.02+0.02,0.02,"partition","CORE_DIE:FPU8","power",core8_fpu[i]);
    energy_introspector->push_data<power_t>((double)i*0.02+0.02,0.02,"partition","CORE_DIE:INT8","power",core8_int[i]);
    energy_introspector->push_data<power_t>((double)i*0.02+0.02,0.02,"partition","CORE_DIE:SCH8","power",core8_sch[i]);
    energy_introspector->push_data<power_t>((double)i*0.02+0.02,0.02,"partition","CORE_DIE:MEM8","power",core8_mem[i]);

    energy_introspector->push_data<power_t>((double)i*0.02+0.02,0.02,"partition","CORE_DIE:FRT9","power",core9_frt[i]);
    energy_introspector->push_data<power_t>((double)i*0.02+0.02,0.02,"partition","CORE_DIE:FPU9","power",core9_fpu[i]);
    energy_introspector->push_data<power_t>((double)i*0.02+0.02,0.02,"partition","CORE_DIE:INT9","power",core9_int[i]);
    energy_introspector->push_data<power_t>((double)i*0.02+0.02,0.02,"partition","CORE_DIE:SCH9","power",core9_sch[i]);
    energy_introspector->push_data<power_t>((double)i*0.02+0.02,0.02,"partition","CORE_DIE:MEM9","power",core9_mem[i]);

    energy_introspector->push_data<power_t>((double)i*0.02+0.02,0.02,"partition","CORE_DIE:FRT12","power",core12_frt[i]);
    energy_introspector->push_data<power_t>((double)i*0.02+0.02,0.02,"partition","CORE_DIE:FPU12","power",core12_fpu[i]);
    energy_introspector->push_data<power_t>((double)i*0.02+0.02,0.02,"partition","CORE_DIE:INT12","power",core12_int[i]);
    energy_introspector->push_data<power_t>((double)i*0.02+0.02,0.02,"partition","CORE_DIE:SCH12","power",core12_sch[i]);
    energy_introspector->push_data<power_t>((double)i*0.02+0.02,0.02,"partition","CORE_DIE:MEM12","power",core12_mem[i]);
*/
    energy_introspector->compute_temperature((double)i*0.02+0.02,0.02,"3DICE");

    grid_t<double> thermal_grid = energy_introspector->pull_data<grid_t<double> >((double)i*0.02+0.02,"package","3DICE","thermal_grid");

    for(int row_index = 0; row_index < 100; row_index++)
    {
      for(int col_index = 0; col_index < 100; col_index++)
        cout << thermal_grid.pull(col_index,row_index,0) << "  ";

      cout << endl;
    }

    cout << "SPOT0\t" << energy_introspector->pull_data<double>((double)i*0.02+0.02,"partition","DL2_DIE:SPOT0","temperature") << "\t";
    cout << "SPOT1\t" << energy_introspector->pull_data<double>((double)i*0.02+0.02,"partition","DL2_DIE:SPOT1","temperature") << "\t";
    cout << "SPOT2\t" << energy_introspector->pull_data<double>((double)i*0.02+0.02,"partition","DL2_DIE:SPOT2","temperature") << "\t";
    cout << "SPOT3\t" << energy_introspector->pull_data<double>((double)i*0.02+0.02,"partition","DL2_DIE:SPOT3","temperature") << "\n";
/*    cout << "SPOT4\t" << energy_introspector->pull_data<double>((double)i*0.02+0.02,"partition","DL2_DIE:SPOT4","temperature") << "\t";
    cout << "SPOT6\t" << energy_introspector->pull_data<double>((double)i*0.02+0.02,"partition","DL2_DIE:SPOT6","temperature") << "\t";
    cout << "SPOT8\t" << energy_introspector->pull_data<double>((double)i*0.02+0.02,"partition","DL2_DIE:SPOT8","temperature") << "\t";
    cout << "SPOT9\t" << energy_introspector->pull_data<double>((double)i*0.02+0.02,"partition","DL2_DIE:SPOT9","temperature") << "\t";
    cout << "SPOT12\t" << energy_introspector->pull_data<double>((double)i*0.02+0.02,"partition","DL2_DIE:SPOT12","temperature") << "\n";    */

    core_temp = energy_introspector->pull_data<double>((double)i*0.02+0.02,"partition","CORE_DIE:FRT0","temperature");
    core_temp_acc += core_temp;
	cout << "FRT0\t" << core_temp << "\t";
    core_temp = energy_introspector->pull_data<double>((double)i*0.02+0.02,"partition","CORE_DIE:FPU0","temperature");
    core_temp_acc += core_temp;
	cout << "FPU0\t" << core_temp << "\t";
	core_temp = energy_introspector->pull_data<double>((double)i*0.02+0.02,"partition","CORE_DIE:INT0","temperature");
    core_temp_acc += core_temp;
	cout << "INT0\t" << core_temp << "\t";
	core_temp = energy_introspector->pull_data<double>((double)i*0.02+0.02,"partition","CORE_DIE:SCH0","temperature");
    core_temp_acc += core_temp;
	cout << "SCH0\t" << core_temp << "\t";
	core_temp = energy_introspector->pull_data<double>((double)i*0.02+0.02,"partition","CORE_DIE:MEM0","temperature");
    core_temp_acc += core_temp;
	cout << "MEM0\t" << core_temp << "\tAVG0\t" << core_temp_acc/5 <<"\n";
	core_temp_acc = 0.0;


    core_temp = energy_introspector->pull_data<double>((double)i*0.02+0.02,"partition","CORE_DIE:FRT1","temperature");
    core_temp_acc += core_temp;
	cout << "FRT1\t" << core_temp << "\t";
    core_temp = energy_introspector->pull_data<double>((double)i*0.02+0.02,"partition","CORE_DIE:FPU1","temperature");
    core_temp_acc += core_temp;
	cout << "FPU1\t" << core_temp << "\t";
	core_temp = energy_introspector->pull_data<double>((double)i*0.02+0.02,"partition","CORE_DIE:INT1","temperature");
    core_temp_acc += core_temp;
	cout << "INT1\t" << core_temp << "\t";
	core_temp = energy_introspector->pull_data<double>((double)i*0.02+0.02,"partition","CORE_DIE:SCH1","temperature");
    core_temp_acc += core_temp;
	cout << "SCH1\t" << core_temp << "\t";
	core_temp = energy_introspector->pull_data<double>((double)i*0.02+0.02,"partition","CORE_DIE:MEM1","temperature");
    core_temp_acc += core_temp;
	cout << "MEM1\t" << core_temp << "\tAVG1\t" << core_temp_acc/5 <<"\n";
	core_temp_acc = 0.0;


    core_temp = energy_introspector->pull_data<double>((double)i*0.02+0.02,"partition","CORE_DIE:FRT2","temperature");
    core_temp_acc += core_temp;
	cout << "FRT2\t" << core_temp << "\t";
    core_temp = energy_introspector->pull_data<double>((double)i*0.02+0.02,"partition","CORE_DIE:FPU2","temperature");
    core_temp_acc += core_temp;
	cout << "FPU2\t" << core_temp << "\t";
	core_temp = energy_introspector->pull_data<double>((double)i*0.02+0.02,"partition","CORE_DIE:INT2","temperature");
    core_temp_acc += core_temp;
	cout << "INT2\t" << core_temp << "\t";
	core_temp = energy_introspector->pull_data<double>((double)i*0.02+0.02,"partition","CORE_DIE:SCH2","temperature");
    core_temp_acc += core_temp;
	cout << "SCH2\t" << core_temp << "\t";
	core_temp = energy_introspector->pull_data<double>((double)i*0.02+0.02,"partition","CORE_DIE:MEM2","temperature");
    core_temp_acc += core_temp;
	cout << "MEM2\t" << core_temp << "\tAVG2\t" << core_temp_acc/5 <<"\n";
	core_temp_acc = 0.0;


    core_temp = energy_introspector->pull_data<double>((double)i*0.02+0.02,"partition","CORE_DIE:FRT3","temperature");
    core_temp_acc += core_temp;
	cout << "FRT3\t" << core_temp << "\t";
    core_temp = energy_introspector->pull_data<double>((double)i*0.02+0.02,"partition","CORE_DIE:FPU3","temperature");
    core_temp_acc += core_temp;
	cout << "FPU3\t" << core_temp << "\t";
	core_temp = energy_introspector->pull_data<double>((double)i*0.02+0.02,"partition","CORE_DIE:INT3","temperature");
    core_temp_acc += core_temp;
	cout << "INT3\t" << core_temp << "\t";
	core_temp = energy_introspector->pull_data<double>((double)i*0.02+0.02,"partition","CORE_DIE:SCH3","temperature");
    core_temp_acc += core_temp;
	cout << "SCH3\t" << core_temp << "\t";
	core_temp = energy_introspector->pull_data<double>((double)i*0.02+0.02,"partition","CORE_DIE:MEM3","temperature");
    core_temp_acc += core_temp;
	cout << "MEM3\t" << core_temp << "\tAVG3\t" << core_temp_acc/5 <<"\n";
	core_temp_acc = 0.0;
/*
    core_temp = energy_introspector->pull_data<double>((double)i*0.02+0.02,"partition","CORE_DIE:FRT4","temperature");
    core_temp_acc += core_temp;
	cout << "FRT4\t" << core_temp << "\t";
    core_temp = energy_introspector->pull_data<double>((double)i*0.02+0.02,"partition","CORE_DIE:FPU4","temperature");
    core_temp_acc += core_temp;
	cout << "FPU4\t" << core_temp << "\t";
	core_temp = energy_introspector->pull_data<double>((double)i*0.02+0.02,"partition","CORE_DIE:INT4","temperature");
    core_temp_acc += core_temp;
	cout << "INT4\t" << core_temp << "\t";
	core_temp = energy_introspector->pull_data<double>((double)i*0.02+0.02,"partition","CORE_DIE:SCH4","temperature");
    core_temp_acc += core_temp;
	cout << "SCH4\t" << core_temp << "\t";
	core_temp = energy_introspector->pull_data<double>((double)i*0.02+0.02,"partition","CORE_DIE:MEM4","temperature");
    core_temp_acc += core_temp;
	cout << "MEM4\t" << core_temp << "\tAVG4\t" << core_temp_acc/5 <<"\n";
	core_temp_acc = 0.0;

    core_temp = energy_introspector->pull_data<double>((double)i*0.02+0.02,"partition","CORE_DIE:FRT6","temperature");
    core_temp_acc += core_temp;
	cout << "FRT6\t" << core_temp << "\t";
    core_temp = energy_introspector->pull_data<double>((double)i*0.02+0.02,"partition","CORE_DIE:FPU6","temperature");
    core_temp_acc += core_temp;
	cout << "FPU6\t" << core_temp << "\t";
	core_temp = energy_introspector->pull_data<double>((double)i*0.02+0.02,"partition","CORE_DIE:INT6","temperature");
    core_temp_acc += core_temp;
	cout << "INT6\t" << core_temp << "\t";
	core_temp = energy_introspector->pull_data<double>((double)i*0.02+0.02,"partition","CORE_DIE:SCH6","temperature");
    core_temp_acc += core_temp;
	cout << "SCH6\t" << core_temp << "\t";
	core_temp = energy_introspector->pull_data<double>((double)i*0.02+0.02,"partition","CORE_DIE:MEM6","temperature");
    core_temp_acc += core_temp;
	cout << "MEM6\t" << core_temp << "\tAVG6\t" << core_temp_acc/5 <<"\n";
	core_temp_acc = 0.0;

    core_temp = energy_introspector->pull_data<double>((double)i*0.02+0.02,"partition","CORE_DIE:FRT8","temperature");
    core_temp_acc += core_temp;
	cout << "FRT8\t" << core_temp << "\t";
    core_temp = energy_introspector->pull_data<double>((double)i*0.02+0.02,"partition","CORE_DIE:FPU8","temperature");
    core_temp_acc += core_temp;
	cout << "FPU8\t" << core_temp << "\t";
	core_temp = energy_introspector->pull_data<double>((double)i*0.02+0.02,"partition","CORE_DIE:INT8","temperature");
    core_temp_acc += core_temp;
	cout << "INT8\t" << core_temp << "\t";
	core_temp = energy_introspector->pull_data<double>((double)i*0.02+0.02,"partition","CORE_DIE:SCH8","temperature");
    core_temp_acc += core_temp;
	cout << "SCH8\t" << core_temp << "\t";
	core_temp = energy_introspector->pull_data<double>((double)i*0.02+0.02,"partition","CORE_DIE:MEM8","temperature");
    core_temp_acc += core_temp;
	cout << "MEM8\t" << core_temp << "\tAVG8\t" << core_temp_acc/5 <<"\n";
	core_temp_acc = 0.0;

    core_temp = energy_introspector->pull_data<double>((double)i*0.02+0.02,"partition","CORE_DIE:FRT9","temperature");
    core_temp_acc += core_temp;
	cout << "FRT9\t" << core_temp << "\t";
    core_temp = energy_introspector->pull_data<double>((double)i*0.02+0.02,"partition","CORE_DIE:FPU9","temperature");
    core_temp_acc += core_temp;
	cout << "FPU9\t" << core_temp << "\t";
	core_temp = energy_introspector->pull_data<double>((double)i*0.02+0.02,"partition","CORE_DIE:INT9","temperature");
    core_temp_acc += core_temp;
	cout << "INT9\t" << core_temp << "\t";
	core_temp = energy_introspector->pull_data<double>((double)i*0.02+0.02,"partition","CORE_DIE:SCH9","temperature");
    core_temp_acc += core_temp;
	cout << "SCH9\t" << core_temp << "\t";
	core_temp = energy_introspector->pull_data<double>((double)i*0.02+0.02,"partition","CORE_DIE:MEM9","temperature");
    core_temp_acc += core_temp;
	cout << "MEM9\t" << core_temp << "\tAVG9\t" << core_temp_acc/5 <<"\n";
	core_temp_acc = 0.0;

    core_temp = energy_introspector->pull_data<double>((double)i*0.02+0.02,"partition","CORE_DIE:FRT12","temperature");
    core_temp_acc += core_temp;
	cout << "FRT12\t" << core_temp << "\t";
    core_temp = energy_introspector->pull_data<double>((double)i*0.02+0.02,"partition","CORE_DIE:FPU12","temperature");
    core_temp_acc += core_temp;
	cout << "FPU12\t" << core_temp << "\t";
	core_temp = energy_introspector->pull_data<double>((double)i*0.02+0.02,"partition","CORE_DIE:INT12","temperature");
    core_temp_acc += core_temp;
	cout << "INT12\t" << core_temp << "\t";
	core_temp = energy_introspector->pull_data<double>((double)i*0.02+0.02,"partition","CORE_DIE:SCH12","temperature");
    core_temp_acc += core_temp;
	cout << "SCH12\t" << core_temp << "\t";
	core_temp = energy_introspector->pull_data<double>((double)i*0.02+0.02,"partition","CORE_DIE:MEM12","temperature");
    core_temp_acc += core_temp;
	cout << "MEM12\t" << core_temp << "\tAVG12\t" << core_temp_acc/5 <<"\n";
	core_temp_acc = 0.0;
*/
  }
  return 0;
}
