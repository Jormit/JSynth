/************************************************
 *  JSynth.cpp
 ***********************************************/

#include <iostream>
#include <stdio.h>
#include <vector>
#include <q/support/literals.hpp>
#include <q_io/midi_stream.hpp>
#include <q_io/audio_stream.hpp>
#include <q/fx/lowpass.hpp>
#include <q/fx/waveshaper.hpp>

#include "config.hpp"
#include "osc.hpp"
#include "display.hpp"

namespace q = cycfi::q;
using namespace q::literals;

struct synth : q::port_audio_stream
{
    synth() : port_audio_stream(0, 2), _osc(NUM_OSCILLATORS, MAX_VOICES, OSC_TYPES, OSC_TUNE,
        this->sampling_rate(), ENV1_PARAM, ENV2_PARAM, FILTER_CUTOFF, FILTER_Q, FILTER_ENV_MOD)
    {}

    void process(out_channels const& out)
    {
        auto left = out[0];
        auto right = out[1];
        for (auto frame : out.frames())
        {
            auto val = clip_soft(_osc.process_frame());
            left[frame] = right[frame] = val;
        }
                   
        _osc.process_remove();        
        _osc.process_add();      
        _osc.process_filters();         
    }
    osc _osc;
    q::soft_clip clip_soft;
};

struct midi_processor : q::midi::processor
{
    using q::midi::processor::operator();

    synth* _synth;

    midi_processor(synth* _synth) 
    {
        this->_synth = _synth;
    }

    void operator()(q::midi::note_on msg, std::size_t time)
    {
        if (MIDI_DEBUG)
        {
            std::cout
                << "Note On  {"
                << "Channel: " << int(msg.channel())
                << ", Key: " << int(msg.key())
                << ", Velocity: " << int(msg.velocity())
                << '}' << std::endl;
        }    

        this->_synth->_osc.add_voice(msg.key());
    }

    void operator()(q::midi::note_off msg, std::size_t time)
    {
        if (MIDI_DEBUG)
        {
            std::cout
                << "Note Off {"
                << "Channel: " << int(msg.channel())
                << ", Key: " << int(msg.key())
                << ", Velocity: " << int(msg.velocity())
                << '}' << std::endl;
        }       

        this->_synth->_osc.release_voice(msg.key());
    }
};

int main()
{
    // Select midi device.
    q::midi_input_stream::set_default_device(MIDI_DEVICE);

    synth synth{};
    synth.start();
    display_config();

    // Begin MIDI processing loop.
    q::midi_input_stream stream;
    if (stream.is_valid())
    {
        while (true)            
            stream.process(midi_processor(&synth));
    }
    return 0;
}