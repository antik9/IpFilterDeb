#include <dlib/clustering.h>
#include <map>
#include <string>

#include "unifier.h"

using namespace dlib;

namespace realty
{
    typedef matrix<double,7,1> sample_type;
    typedef radial_basis_kernel<sample_type> kernel_type;

    struct
    DataRow
    {
        double  row[7];
        int     cluster;
    };

    struct 
    Meta
    {
        char unifier_filename[64]   { '\0' };
        char model_filename[64]     { '\0' };
        char data_filename[64]      { '\0' };
        size_t sample_size;
    };

    struct
    Advisor
    {
        Advisor     ( std::string filename );

        void
        give_advice ( );

        bool
        has_advice  ( );

        void
        read_data   ( );

        void
        restore     ( );

        void
        save        ( );

        void
        train       ( unsigned short number_of_clusters );

    private:
        std::string                                         __filename;
        int                                                 __iter_idx;
        Meta                                                __meta;
        kkmeans<kernel_type>                                __model;
        std::map<int, std::vector<std::vector<double>>>     __cluster_to_samples;
        std::vector<std::vector<double>>                    __samples;
        realty::SampleUnifier                               __unifier;

        void
        restore_meta ( );

        void
        restore_model ( );

        void
        restore_sample ( );

        void
        restore_unifier ( );

        void
        save_meta ( );

        void
        save_model ( );

        void
        save_sample ( );

        void
        save_unifier ( );
    };
}
