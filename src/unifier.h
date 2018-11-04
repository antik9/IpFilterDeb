#pragma once

#include <fstream>
#include <vector>

namespace realty
{
    struct
    SampleUnifier
    {
        double min_attrs[7];
        double max_attrs[7];

        SampleUnifier       ( ) : stored ( false ) { };

        void
        unify               ( std::vector<std::vector<double>>& samples, int idx );

        void
        set_stored          ( );

        void
        save_to_file        ( std::string filename );

        void
        restore_from_file   ( std::string filename );

    private:
        bool stored;
    };
}
