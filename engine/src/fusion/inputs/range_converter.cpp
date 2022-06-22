//
// Input Mapping Demo
// http://scribblings-by-apoch.googlecode.com/
//
// Wrapper class for converting raw input range values to sensitivity-calibrated range values
//

#include "range_converter.hpp"
#include "file.hpp"

using namespace fe;

/**
 * Construct the converter and load the conversion table provided
 */
RangeConverter::RangeConverter(std::wifstream& file) {
    if (!file)
        throw std::runtime_error("Invalid file stream provided to RangeConverter constructor");

    auto numconversions = attemptRead<unsigned>(file);
    for (unsigned i = 0; i < numconversions; ++i) {
        auto range = static_cast<Range>(attemptRead<unsigned>(file));
        Converter converter;
        converter.minimumInput = attemptRead<double>(file);
        converter.maximumInput = attemptRead<double>(file);
        converter.minimumOutput = attemptRead<double>(file);
        converter.maximumOutput = attemptRead<double>(file);

        if ((converter.maximumInput < converter.minimumInput) || (converter.maximumOutput < converter.minimumOutput))
            throw std::runtime_error("Invalid input range conversion");

        conversionMap.insert(std::make_pair(range, converter));
    }
}