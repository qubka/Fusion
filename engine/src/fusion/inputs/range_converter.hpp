//
// Input Mapping Demo
// http://scribblings-by-apoch.googlecode.com/
//
// Wrapper class for converting raw input range values to sensitivity-calibrated range values
//

#pragma once

#include "input_constants.hpp"

namespace fe {
    class RangeConverter {
        // Internal helpers
    private:
        struct Converter {
            double minimumInput;
            double maximumInput;

            double minimumOutput;
            double maximumOutput;

            template <typename RangeType>
            RangeType convert(RangeType invalue) const {
                auto v = static_cast<double>(invalue);
                if (v < minimumInput)
                    v = minimumInput;
                else if (v > maximumInput)
                    v = maximumInput;

                double interpolationfactor = (v - minimumInput) / (maximumInput - minimumInput);
                return static_cast<RangeType>((interpolationfactor * (maximumOutput - minimumOutput)) + minimumOutput);
            }
        };

        // Internal type shortcuts
    private:
        typedef std::map<Range, Converter> ConversionMapT;

        // Construction
    public:
        RangeConverter() = default;
        explicit RangeConverter(std::wifstream& file);
        ~RangeConverter() = default;

        // Conversion interface
    public:
        template <typename RangeType>
        RangeType convert(Range rangeid, RangeType invalue) const {
            auto it = conversionMap.find(rangeid);
            if (it == conversionMap.end())
                return invalue;
            return it->second.convert<RangeType>(invalue);
        }

        // Internal tracking
    private:
        ConversionMapT conversionMap;
    };
}