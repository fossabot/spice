/**
 * \file
 * This header defines  utilities to print some of spice's types to the console.
 */
#ifndef SPICE_PRINT
#define SPICE_PRINT

#import <vector>
#import <cmath>

#import "image.hpp"

namespace spice {
/**
 * This namespace exports utilities to print some of spice's types to the
 * console.
 */
namespace print {
    /**
     * Insert escape codes for terminals supporting 8 bpc colours into a string.
     *
     * \param str The string to surround with escape codes
     * \param foreground The text colour
     * \param background The background colour
     */
    template<typename T>
    std::string color_string(
        std::string const & str,
        color<T> const & foreground,
        color<T> const & background)
    {
        const float scaling_factor = 255 / (image<T>::intensity_range.max -
            image<T>::intensity_range.min);
        const float offset = 0 - image<T>::intensity_range.min;

        return std::string("\033[48;2;") +
        // set BG-colours
        std::to_string(static_cast<int>(
            std::floor(background[0] * scaling_factor + offset))) + ";" +
        std::to_string(static_cast<int>(
            std::floor(background[1] * scaling_factor + offset))) + ";" +
        std::to_string(static_cast<int>(
            std::floor(background[2] * scaling_factor + offset))) +
        // set FG-colours
        ";38;2;" +
        std::to_string(static_cast<int>(
            std::floor(foreground[0] * scaling_factor + offset))) + ";" +
        std::to_string(static_cast<int>(
            std::floor(foreground[1] * scaling_factor + offset))) + ";" +
        std::to_string(static_cast<int>(
            std::floor(foreground[2] * scaling_factor + offset))) + "m" +
        // add the actual string and the ending escape code
        str + "\033[0m";
    }

    /**
     * Prints a histogram to the provided stream.
     *
     * \param histogram The data to print
     * \param max_amplitude The length of the longest bar in single spaces
     * \param colors The colour to print each channel in. Note that superfluous
     * colours will be ignored and missing colours will be represented as grey.
     * \param stream The stream to insert the histogram into
     */
    void histogram(std::vector<std::vector<size_t>> const & histogram,
        size_t max_amplitude,
        std::vector<color<float>> colors,
        std::ostream & stream = std::cout)
    {
        // find the most common value class per channel
        std::vector<size_t> max_samples(histogram.size());
        for (size_t chan = 0; chan < histogram.size(); ++chan)
            max_samples[chan] = *std::max_element(
                std::begin(histogram[chan]), std::end(histogram[chan]));

        // find the most common value class overall
        size_t max_sample = *std::max_element(
            std::begin(max_samples), std::end(max_samples));

        // calculate the scaling factor
        const float scale = static_cast<float>(max_amplitude) / max_sample;

        // do the actual printing
        for (size_t sample = 0; sample < histogram[0].size(); ++sample) {
            for (size_t cell = 0; cell < max_amplitude; ++cell) {
                // calculate colour (sum of all active channels' colours divided
                // by number of channels) and print
                color<float> cell_color({0,0,0});
                for (size_t channel = 0; channel < histogram.size(); ++channel)
                {
                    if ((histogram[channel][sample] * scale) < cell) {
                        if (colors.size() > channel)
                            cell_color += colors[channel];
                        else
                            cell_color += color<float>({0.5, 0.5, 0.5});
                    }

                    // stream << " | ";
                    // for (size_t c = 0; c < cell_color.size(); ++c)
                    //     stream << std::to_string(cell_color[c]);
                }
                cell_color /= histogram.size();
                cell_color = color<float>({1, 1, 1}) - cell_color;
                stream << color_string("=", cell_color, cell_color);
            }
            stream << "\n";
        }
    }
}
}

#endif // SPICE_PRINT
