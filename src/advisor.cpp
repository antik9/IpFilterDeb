#include <algorithm>
#include <fstream>
#include <string.h>

#include <dlib/clustering.h>
#include <dlib/rand.h>
#include <dlib/serialize.h>

#include "advisor.h"
#include "read.h"

using namespace dlib;

namespace realty
{
    Advisor::Advisor     ( std::string filename ) :
        __filename ( filename ),
        __iter_idx ( 0 ),
        __model ( ( kcentroid<kernel_type> ( kernel_type ( .1 ), .01, 1024 ) ) )
    { }

    void
    Advisor::give_advice ( )
    {
        auto& input_data = __samples[__iter_idx++];
        auto working_data = input_data;
        std::vector<std::vector<double>> working_vec { working_data };
        sample_type input_matrix;
        for ( int i = 0; i < 7; ++i )
        {
            __unifier.unify ( working_vec, i );
            input_matrix ( i ) = working_vec[0][i];
        }

        int cluster = __model ( input_matrix );
        std::sort (
            __cluster_to_samples[cluster].begin ( ),
            __cluster_to_samples[cluster].end ( ),
            [&] ( std::vector<double>& lhs, std::vector<double>& rhs )
            {
                double distance_lhs = ( lhs[0] - input_data[0] ) * ( lhs[0] - input_data[0] )
                    + ( lhs[1] - input_data[1] ) * ( lhs[1] - input_data[1] );
                double distance_rhs = ( rhs[0] - input_data[0] ) * ( rhs[0] - input_data[0] )
                    + ( rhs[1] - input_data[1] ) * ( rhs[1] - input_data[1] );
                return distance_lhs < distance_rhs;
            }
        );
        std::cout << std::endl;
        for ( auto& sample: __cluster_to_samples[cluster] )
        {
            std::cout
                << sample[0] << ";"
                << sample[1] << ";"
                << sample[2] << ";"
                << sample[3] << ";"
                << sample[4] << ";"
                << sample[5] << ";"
                << sample[6] << std::endl;
        }
    }

    bool
    Advisor::has_advice ( )
    {
        return __iter_idx < __samples.size ( );
    }

    void
    Advisor::read_data   ( )
    {
        __samples = realty::get_raw_data ( );
    }

    void
    Advisor::restore    ( )
    {
        restore_meta    ( );
        restore_model   ( );
        restore_unifier ( );
        restore_sample  ( );
    }

    void
    Advisor::save       ( )
    {
        save_model      ( );
        save_unifier    ( );
        save_sample     ( );
        save_meta       ( );
    }

    void
    Advisor::train       ( unsigned short number_of_clusters )
    {
        auto raw_samples = __samples;
        __unifier = realty::SampleUnifier ( );

        for ( int i = 0; i < 7; ++i )
        {
            __unifier.unify ( raw_samples, i );
        }

        std::vector<sample_type> samples;
        samples.reserve ( __samples.size ( ) );
        std::vector<sample_type> initial_centers;

        sample_type m;
        dlib::rand rnd;

        for ( auto& sample: raw_samples )
        {
            for ( int i = 0; i < 7; ++i )
            {
                m ( i ) = sample[i];
            }
            samples.push_back ( m );
        }

        __model.set_number_of_centers ( number_of_clusters );
        pick_initial_centers (
                number_of_clusters, initial_centers, samples, __model.get_kernel ( ) );
        __model.train ( samples, initial_centers );

        for ( int i = 0; i < __samples.size ( ); ++i )
        {
            int cluster = __model ( samples[i] );
            __cluster_to_samples[cluster].emplace_back ( __samples[i] );
        }
    }

    void
    Advisor::restore_meta ( )
    {
        std::ifstream input_file ( __filename + ".meta.dat" );
        input_file.read ( ( char * ) &__meta, sizeof ( __meta ) );
        input_file.close ( );
        std::cout << __meta.model_filename << " " << __meta.data_filename << " "
            << __meta.unifier_filename << " " << __meta.sample_size << std::endl;
    }

    void
    Advisor::restore_model ( )
    {
        deserialize ( __meta.model_filename ) >> __model;
    }

    void
    Advisor::restore_sample ( )
    {
        DataRow row;
        std::ifstream input_file ( __meta.data_filename );
        for ( int i = 0; i < __meta.sample_size; ++i )
        {
            input_file.read ( ( char * ) &row, sizeof ( row ) );
            std::vector<double> raw_sample ( 7 );
            for ( int i = 0; i < 7; ++i )
            {
                raw_sample[i] = row.row[i];
            }
            __cluster_to_samples[row.cluster].emplace_back ( raw_sample );
        }
        input_file.close ( );
    }

    void
    Advisor::restore_unifier ( )
    {
        __unifier.restore_from_file ( __meta.unifier_filename );
    }

    void
    Advisor::save_meta ( )
    {
        auto filename = __filename + ".meta.dat";
        std::ofstream output_file ( filename, std::ios::binary );
        output_file.write ( ( char* ) &__meta, sizeof ( __meta ) );
        output_file.close ( );
    }

    void
    Advisor::save_model ( )
    {
        memcpy (
            __meta.model_filename,
            ( __filename + ".dat" ).c_str ( ),
            __filename.size ( ) + 4
        );

        serialize ( __meta.model_filename ) << __model;
    }

    void
    Advisor::save_sample ( )
    {
        memcpy (
            __meta.data_filename,
            ( __filename + ".sample.dat" ).c_str ( ),
            __filename.size ( ) + 11
        );
        __meta.sample_size = sizeof ( __samples.size ( ) );
        DataRow row;

        std::ofstream output_file ( __meta.data_filename, std::ios::binary );

        for ( auto& key_value: __cluster_to_samples )
        {
            for ( auto& sample: key_value.second )
            {
                for ( int i = 0; i < 7; ++i )
                {
                    row.row[i] = sample[i];
                    row.cluster = key_value.first;
                    output_file.write ( ( char* ) &row, sizeof ( row ) );
                }
            }
        }
        output_file.close ( );
    }

    void
    Advisor::save_unifier ( )
    {
        memcpy (
            __meta.unifier_filename,
            ( __filename + ".unifier.dat" ).c_str ( ),
            __filename.size ( ) + 12
        );

        __unifier.save_to_file ( __meta.unifier_filename );
    }
}
