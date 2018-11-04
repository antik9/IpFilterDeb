#pragma once

#include <string>
#include <vector>

namespace realty
{
    void
    read_double ( std::string& buffer, double& value );

    std::vector<std::vector<double>>
    get_raw_data ( );
}
