/*
 * dvfs.h
 *
 *  Created on: Jul 21, 2014
 *      Author: karthik
 */

#ifndef DVFS_H_
#define DVFS_H_

#include<math.h>

/*---------------- TEMPERATURE REGULATOR PARAMETERS (2014 Paper) ---------------------*/
/* Initial Conditions
Parameters for the LTI model */
//#define a -384.61              // Should be conductances
//#define b 916.52               // Should be capacitances

/* Euler integration parameters */
#define deltat 1e-6   // for the LTI model
#define deltat1 1e-3       // for the forward integration of dT_dPT

/* Voltage to Frequency equation parameters */
#define VFSlope 1.925e-4
#define V0 0.4008
//#define phi_Old 1e9

/* Technology parameters for the core */
#define kb 1.38e-23                      // Boltzmann's constant
#define eta 1                            // Ideality factor
#define threshold_voltage 0.2            // Threshold voltage of the transistor
#define V_off 0                          // Transistor switch off voltage parameter
#define q1 1.6e-19                        // Charge on an electron
#define N 100                            // Number of iterations until control kicks in a.k.a. Control cycle
#define TRANSIENT_TIME 50                             // Number of iterations  until transient dies out

//#define NumIter 10000000   // Total number of time steps
//#define stepsize 0.7                       // Step Size for the update equation
#define VarSize 10                        // Array size to calculate the peak to peak variation
#define FIXEDGAIN 0
/*---------------- TEMPERATURE REGULATOR PARAMETERS (2014 Paper) ---------------------*/


/*---------------- TRINIY PARAMETERS ---------------------*/
#define CoreNum 16                        // Number of cores
#define fmax 1.5e9                       // Max frequency of core
#define fmin 0.5e9                         // Min frequency of core

#define USE_CONTROLLER 1
#if USE_CONTROLLER
#define C_ENABLED 1
#define C_DISABLED 0
#else
#define C_ENABLED 0
#define C_DISABLED 1
#endif

#define T_MAX 354
// Temperature Model Parameters
#define A1 0.9998
#define B1 -8.46
#define C1 -37
#define D1 0.0002*T_MAX

// Static Power Model Parameters
#define BETA -0.42675
#define GAMMA 0.000674
#define DELTA 0.001618
#define EPSILON -0.09038

#define NUM_AVAIL_FREQS 21
#define SATURATION_LIMIT 10			// TODO: This parameter should come from config file
#define IPC_MAX 4					// ISSUE WIDTH
#define CONTROL_CYCLE 0.001			// TODO: This parameter should come from config file
/*---------------- TRINIY PARAMETERS ---------------------*/
#endif /* DVFS_H_ */
