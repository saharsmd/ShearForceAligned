/*

Copyright (c) 2005-2019, University of Oxford.
All rights reserved.

University of Oxford means the Chancellor, Masters and Scholars of the
University of Oxford, having an administrative office at Wellington
Square, Oxford OX1 2JD, UK.

This file is part of Chaste.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
 * Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.
 * Neither the name of the University of Oxford nor the names of its
   contributors may be used to endorse or promote products derived from this
   software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#include "ErkPropulsionOdeSystemVelocityAlignment.hpp"
#include "CellwiseOdeSystemInformation.hpp"

ErkPropulsionOdeSystemVelocityAlignment::ErkPropulsionOdeSystemVelocityAlignment(std::vector<double> stateVariables)
  : AbstractOdeSystem(3)    // with n=3 variables
{
    mpSystemInfo.reset(new CellwiseOdeSystemInformation<ErkPropulsionOdeSystemVelocityAlignment>);

    /**
     * The state variables are as follows:
     *
     * 0 - Self propulsion angle theta for this cell
     * 1 - Erk activity for this cell
     * 2 - Rest area for this cell
     *
     * We store the last state variable so that it can be written
     * to file at each time step alongside the others, and visualized.
     */

    SetDefaultInitialCondition(1, 1.0); // soon overwritten    // Theta (propulsion angle)
    SetDefaultInitialCondition(1, 1.0); // soon overwritten    // Erk
    SetDefaultInitialCondition(1, 1.0); // soon overwritten    // Target Area

    this->mParameters.push_back(1.0);    // Default cell area. Soon overwritten
    this->mParameters.push_back(1.0);    // Default taul. Soon overwritten
    this->mParameters.push_back(1.0);    // Default alpha. Soon overwritten
    this->mParameters.push_back(1.0);    // Default beta. Soon overwritten
    this->mParameters.push_back(0.1);    // Default Eta Std. Soon overwritten
    this->mParameters.push_back(0.01);    // Default dt_ode. Soon overwritten

    // TODO: Need access to instantaneous velocity
    this->mParameters.push_back(0.0);    // Default angle of cell velocity theta_vi
    // TODO: Need a parameter K for strength of alignment K*sin(theta_iv - theta_i)
    this->mParameters.push_back(0.0);    // Default strength of alignment K

    if (stateVariables != std::vector<double>())
    {
        SetStateVariables(stateVariables);
    }
}

ErkPropulsionOdeSystemVelocityAlignment::~ErkPropulsionOdeSystemVelocityAlignment()
{
}

void ErkPropulsionOdeSystemVelocityAlignment::EvaluateYDerivatives(double time, const std::vector<double>& rY, std::vector<double>& rDY)
{
  double theta = rY[0];    // Angle of self-propulsion
  double erk = rY[1];
  double target_area = rY[2];
  double area = this->mParameters[0];

  double taul = this->mParameters[1];
  double a = this->mParameters[2];
  double b = this->mParameters[3];
  double eta_std = this->mParameters[4];    // persistence time is equal to sqrt(1/eta_std)
  // TODO Figure out how to access the dt of the ode/srn model and use
  // that instead of assigning to each cell's CellData.
  double dt_ode = this->mParameters[5];

  double theta_vi = this->mParameters[6];    // Angle of instantaneous cell velocity
  double K = this->mParameters[7];    // Strength of alignment K*sin(theta_vi - theta_i)

  // Random kicks in angle. Stochastic differential equation so dtheta
  // should scale like sqrt(dt/tp) -> dtheta/dt needs to be multiplied
  // by 1/sqrt(tp*dt) here. The factor of sqrt(2) ensures the correct
  // persistence time tp = 2/<eta^2> - see eq4 in Boocock et al (2023)
  // 10.1101/2023.03.24.534111.
  rDY[0] = eta_std*sqrt(2)*sqrt(1/dt_ode)*(RandomNumberGenerator::Instance()->StandardNormalRandomDeviate()) + K*sin(theta_vi - theta);    // d[theta]/dt

  // -E^3 stabilizing non-linearity
  rDY[1] = -erk - pow(erk, 3.0) + b*(area-1.0);    // d[Erk]/dt
  rDY[2] = ((1.0-target_area) - a*erk) / taul;    // d[TargetArea]/dt
}

template<>
void CellwiseOdeSystemInformation<ErkPropulsionOdeSystemVelocityAlignment>::Initialise()
{
    this->mVariableNames.push_back("Theta");
    this->mVariableUnits.push_back("non-dim");
    this->mInitialConditions.push_back(0.0); // will be filled in later

    this->mVariableNames.push_back("Erk");
    this->mVariableUnits.push_back("non-dim");
    this->mInitialConditions.push_back(0.0); // will be filled in later

    this->mVariableNames.push_back("Target Area");
    this->mVariableUnits.push_back("non-dim");
    this->mInitialConditions.push_back(0.0); // will be filled in later

    // If this is ever not the first parameter change the line
    this->mParameterNames.push_back("Cell Area");
    this->mParameterUnits.push_back("non-dim");

    this->mParameterNames.push_back("taul");
    this->mParameterUnits.push_back("non-dim");

    this->mParameterNames.push_back("alpha");
    this->mParameterUnits.push_back("non-dim");

    this->mParameterNames.push_back("beta");
    this->mParameterUnits.push_back("non-dim");

    this->mParameterNames.push_back("Eta Std");
    this->mParameterUnits.push_back("non-dim");

    this->mParameterNames.push_back("dt_ode");
    this->mParameterUnits.push_back("non-dim");

    // Instantaneous angle of cell velocity
    this->mParameterNames.push_back("theta_vi");
    this->mParameterUnits.push_back("non-dim");

    // Strength of alignment K*sin(theta_vi - theta_i)
    this->mParameterNames.push_back("K");
    this->mParameterUnits.push_back("non-dim");

    this->mInitialised = true;
}

// Serialization for Boost >= 1.36
#include "SerializationExportWrapperForCpp.hpp"
CHASTE_CLASS_EXPORT(ErkPropulsionOdeSystemVelocityAlignment)
