/*
 * dvfs.h
 *
 *  Created on: Jul 21, 2014
 *      Author: karthik
 */

#ifndef DVFS_H_
#define DVFS_H_


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
#define CoreNum 16                        // Number of cores
//#define NumIter 10000000   // Total number of time steps
#define fmax 1.8e9                       // Max frequency of core
#define fmin 0.5e9                         // Min frequency of core
//#define stepsize 0.7                       // Step Size for the update equation
#define VarSize 10                        // Array size to calculate the peak to peak variation


//#define c1 303.5091
#define FIXEDGAIN 0
#define USE_CONTROLLER 1
#ifdef USE_CONTROLLER
#define C_ENABLED 1
#define C_DISABLED 0
#else
#define C_ENABLED 0
#define C_DISABLED 1
#endif
//f(MHz) Vdd(V)
//500	0.49705
//600	0.5163
//700	0.53555
//800	0.5548
//900	0.57405
//1000	0.5933
//1100	0.61255
//1200	0.6318
//1300	0.65105
//1400	0.6703
//1500	0.68955

#endif /* DVFS_H_ */
