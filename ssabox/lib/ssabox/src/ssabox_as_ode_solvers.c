/*
 * ProviewR   Open Source Process Control.
 * Copyright (C) 2005-2025 SSAB EMEA AB.
 *
 * This file is part of ProviewR.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ProviewR. If not, see <http://www.gnu.org/licenses/>
 *
 * Linking ProviewR statically or dynamically with other modules is
 * making a combined work based on ProviewR. Thus, the terms and
 * conditions of the GNU General Public License cover the whole
 * combination.
 *
 * In addition, as a special exception, the copyright holders of
 * ProviewR give you permission to, from the build function in the
 * ProviewR Configurator, combine ProviewR with modules generated by the
 * ProviewR PLC Editor to a PLC program, regardless of the license
 * terms of these modules. You may copy and distribute the resulting
 * combined work under the terms of your choice, provided that every
 * copy of the combined work is accompanied by a complete copy of
 * the source code of ProviewR (the version used to produce the
 * combined work), being distributed under the terms of the GNU
 * General Public License plus this exception.
 **/

/*2005-02-13      File: AS_ODE_Solvers.c
* Author: Jonas Haulin   email: joha7211@student.uu.se
* Contains functions that will integrate the discretized DE:s for sway angle,
* trolley position and cable length
*/

#include "ssabox_as_ode_solvers.h"

void AS_thetaIntegrator(
    double* thetam, double* thetac, double Lc, double DLc, double ac, double dt)
{
  /* Function: thetaIntegrator
   * Simulation of sway of crane load.
   * One-dimensional traversal and sway. Variable pendulum length, L
   * Lagrange's equation: D^2(theta)*L + 2*D(L)*D(theta) + D^2(x)*cos(theta) +
   * g*sin(theta) = 0
   *
   * Integrates Lagrange's equation and stores the current sway angle in thetac
   *
   * Variables
   * thetam - sway angle at time n-1
   * thetac - sway angle at time n
   * thetap - sway angle at time n+1 */

  /*Solving the discretized ODE for thetap*/
  double thetap
      = (2.0 * (*thetac) * Lc + (*thetam) * (dt * DLc - Lc)
            - (dt * dt) * (ac * cos(*thetac) + AS_GRAV_ACCEL * sin(*thetac)))
      / (dt * DLc + Lc);
  *thetam = *thetac;
  *thetac = thetap;
}

void AS_xIntegrator(double* xm, double* xc, double ac, double dt)
{
  /* Calculates the position at time n+1 from acceleration and x(n), x(n-1) */

  double xp = 2.0 * (*xc) - *xm + ac * (dt * dt);
  *xm = *xc;
  *xc = xp;
}

void AS_xIntegratorUA(double* xm, double* xc, double uc, double ac, double dt)
{
  /* Calculates the position at time n+1 from velocity and x(n) */

  double xp = *xc + uc * dt + 0.5 * ac * (dt * dt);
  *xm = *xc;
  *xc = xp;
}

void AS_xcIntegratorU(double* xc, double uc, double dt)
{
  *xc += uc * dt;
}

void AS_xIntegrator2U(double* xm, double* xc, double um, double dt)
{
  double xp = *xm + 2.0 * um * dt;
  *xm = *xc;
  *xc = xp;
}

void AS_uIntegratorFwd(double* uc, double ac, double dt)
{
  /* Calculates velocity at time n+1 from uc and ac with a forward difference
   * approximation of the derivative */

  *uc += ac * dt;
}

void AS_LIntegratorFwd(double* Lc, double DLc, double dt)
{
  *Lc += DLc * dt;
}
