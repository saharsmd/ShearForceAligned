#ifndef SINUSOIDALSHEARFORCE_HPP_
#define SINUSOIDALSHEARFORCE_HPP_

#include "ChasteSerialization.hpp"
#include <boost/serialization/base_object.hpp>
#include "Exception.hpp"

#include "AbstractForce.hpp"
#include "VertexBasedCellPopulation.hpp"

template<unsigned DIM>

class SinusoidalShearForce : public AbstractForce<DIM>
{
private:

  double mF1;

    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & archive, const unsigned int version)
    {
        archive & boost::serialization::base_object<AbstractForce<DIM> >(*this);
        archive & mF1;
    }

public:

    /**
     * Constructor.
     */
    SinusoidalShearForce();

    ~SinusoidalShearForce();

    void SetF1(double force);

    double GetF1();

    void AddForceContribution(AbstractCellPopulation<DIM>& rCellPopulation);

    void OutputForceParameters(out_stream& rParamsFile);

};

#include "SerializationExportWrapper.hpp"
EXPORT_TEMPLATE_CLASS_SAME_DIMS(SinusoidalShearForce)

#endif /*SINUSOIDALSHEARFORCE_HPP_*/
