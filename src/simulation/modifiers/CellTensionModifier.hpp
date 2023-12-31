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

#ifndef CELLTENSIONMODIFIER_HPP_
#define CELLTENSIONMODIFIER_HPP_

#include "ChasteSerialization.hpp"
#include <boost/serialization/base_object.hpp>

#include "AbstractCellBasedSimulationModifier.hpp"
// #include "VertexBasedCellPopulation.hpp"

/**
 * A modifier class in which the average self propulstion angle in
 * neighbouring cells are computed and stored in CellData.
 */
template<unsigned DIM>
class CellTensionModifier : public AbstractCellBasedSimulationModifier<DIM,DIM>
{
    /** Needed for serialization. */
    friend class boost::serialization::access;
    /**
     * Boost Serialization method for archiving/checkpointing.
     * Archives the object and its member variables.
     *
     * @param archive  The boost archive.
     * @param version  The current version of this class.
     */
    template<class Archive>
    void serialize(Archive & archive, const unsigned int version)
    {
        archive & boost::serialization::base_object<AbstractCellBasedSimulationModifier<DIM,DIM> >(*this);
	archive & mKA;
        archive & mKP;
        archive & mP0;

    }

protected:

    /**
     * Cell deformation energy parameter. Has units of kg s^-2 (cell size at equilibrium rest length)^-1.
     */
    double mKA;

    /**
     * Cell membrane energy parameter. Has units of kg s^-2 (cell size at equilibrium rest length).
     */
    double mKP;

    /**
    * Preferred perimeter. Has units of cell size at equilibrium rest length.
    */
    double mP0;

public:

    /**
     * Default constructor.
     */
    CellTensionModifier();

    /**
     * Destructor.
     */
    virtual ~CellTensionModifier();

    /**
     * Overridden UpdateAtEndOfTimeStep() method.
     *
     * Specifies what to do in the simulation at the end of each time step.
     *
     * @param rCellPopulation reference to the cell population
     */
    virtual void UpdateAtEndOfTimeStep(AbstractCellPopulation<DIM,DIM>& rCellPopulation);
    // virtual void UpdateAtEndOfTimeStep(VertexBasedCellPopulation<DIM>& rCellPopulation);

    /**
     * Overridden SetupSolve() method.
     *
     * Specifies what to do in the simulation before the start of the time loop.
     *
     * @param rCellPopulation reference to the cell population
     * @param outputDirectory the output directory, relative to where Chaste output is stored
     */
    virtual void SetupSolve(AbstractCellPopulation<DIM,DIM>& rCellPopulation, std::string outputDirectory);
  // virtual void SetupSolve(VertexBasedCellPopulation<DIM>& rCellPopulation, std::string outputDirectory);

    /**
     * @return mKA
     */
    double GetKA();

    /**
     * @return mKP
     */
    double GetKP();

    /**
     * @return mP0
     */
    double GetP0();

     /**
     * Set mKA.
     *
     * @param KA the new value of mKA
     */
    void SetKA(double KA);

    /**
     * Set mKP.
     *
     * @param KP the new value of mKP
     */
    void SetKP(double KP);

    /**
     * Set mP0.
     *
     * @param P0 the new value of mP0
     */
    void SetP0(double P0);

    /**
     * Helper method to compute the average self propulsion angle in each cell's neighbours and store these in the CellData.
     *
     * Note: If using a CaBasedCellPopulation, we assume a Moore neighbourhood and unit carrying capacity.
     * If a cell has no neighbours (such as an isolated cell in a CaBasedCellPopulation), we store the
     * value -1 in the CellData.
     *
     * @param rCellPopulation reference to the cell population
     */
    void UpdateCellData(AbstractCellPopulation<DIM,DIM>& rCellPopulation);
    // void UpdateCellData(VertexBasedCellPopulation<DIM>& rCellPopulation);

    /**
     * Overridden OutputSimulationModifierParameters() method.
     * Output any simulation modifier parameters to file.
     *
     * @param rParamsFile the file stream to which the parameters are output
     */
    void OutputSimulationModifierParameters(out_stream& rParamsFile);
};

#include "SerializationExportWrapper.hpp"
EXPORT_TEMPLATE_CLASS_SAME_DIMS(CellTensionModifier)

#endif /*CELLTENSIONMODIFIER_HPP_*/
