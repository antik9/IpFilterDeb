#include "unifier.h"

namespace realty
{
    void
    SampleUnifier::unify ( std::vector<std::vector<double>>& samples, int idx )
    {
        double min, max;
        if ( stored )
        {
            min = min_attrs[idx];
            max = max_attrs[idx];
        }
        else
        {
            min = samples[0][idx];
            max = samples[0][idx];
            for ( auto& sample: samples )
            {
                min = min < sample[idx] ? min : sample[idx];
                max = max > sample[idx] ? max : sample[idx];
            }
        }

        min_attrs[idx] = min;
        max_attrs[idx] = max;

        double delta = max - min;

        for ( auto& sample: samples )
        {
            double unified_value = ( sample[idx] - min ) / delta;
            sample[idx] = ( unified_value > 0 ? ( unified_value < 1 ? unified_value : 1 ) : 0 );
        }
    }

    void
    SampleUnifier::set_stored ( )
    {
        stored = true;
    }

    void
    SampleUnifier::save_to_file ( std::string filename )
    {
        set_stored ( );
        std::ofstream output_file ( filename, std::ios::binary );
        output_file.write ( ( char* ) this, sizeof ( *this ) );
        output_file.close ( );
    }

    void
    SampleUnifier::restore_from_file ( std::string filename )
    {
        std::ifstream input_file ( filename, std::ios::binary );
        input_file.read ( ( char* ) this, sizeof ( *this ) );
    }
}
