// JSynth.cpp : Defines the entry point for the application.
//

#include <q/support/literals.hpp>
#include <q/synth/sin.hpp>
#include <q_io/audio_stream.hpp>

///////////////////////////////////////////////////////////////////////////////
// Synthesize a 440 Hz sine wave.
///////////////////////////////////////////////////////////////////////////////

namespace q = cycfi::q;
using namespace q::literals;

struct synth : q::port_audio_stream
{
    synth(q::frequency freq)
        : port_audio_stream(0, 2)
        , phase(freq, this->sampling_rate())
    {}

    void process(out_channels const& out)
    {
        auto left = out[0];
        auto right = out[1];
        for (auto frame : out.frames())
        {
            // Synthesize the sin wave
            right[frame] = left[frame] = q::sin(phase++);
        }
    }

    q::phase_iterator phase;   // The phase iterator
};

int main()
{
    synth my_synth{ 880_Hz };

    my_synth.start();
    q::sleep(5_s);
    my_synth.stop();

    return 0;
}