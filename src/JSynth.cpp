// JSynth.cpp : Defines the entry point for the application.
//

#include <iostream>
#include <set>
#include <unordered_map>
#include <q/support/literals.hpp>
#include <q/synth/sin.hpp>
#include <q/synth/saw.hpp>
#include <q_io/midi_stream.hpp>
#include <q_io/audio_stream.hpp>

#include "config.hpp"
#include "osc.hpp"

namespace q = cycfi::q;
using namespace q::literals;

struct synth : q::port_audio_stream
{
    synth() : port_audio_stream(0, 2),
        _osc{ osc(this->sampling_rate(), 1.0f, ENVELOPE_PARAM, SAW),
        osc(this->sampling_rate(), 1.02f, ENVELOPE_PARAM, NONE) }
    {}

    void process(out_channels const& out)
    {
        auto left = out[0];
        auto right = out[1];
        for (auto frame : out.frames())
        {
            float next_out = 0.0f;
            for (int i = 0; i < NUM_OSCILLATORS; i++) {
                next_out += _osc[i].process_frame();
            }
            left[frame] = right[frame] = next_out / NUM_OSCILLATORS;
        }
        for (int i = 0; i < NUM_OSCILLATORS; i++) {
            _osc[i].process_remove();
        }        
    }



    osc _osc[NUM_OSCILLATORS];
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

        for (int i = 0; i < NUM_OSCILLATORS; i++) {
            this->_synth->_osc[i].add_voice(msg.key());
        }
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

        for (int i = 0; i < NUM_OSCILLATORS; i++) {
            this->_synth->_osc[i].remove_voice(msg.key());
        }
    }
};

int main()
{
    // Select midi device.
    q::midi_input_stream::set_default_device(MIDI_DEVICE);

    synth synth{};
    synth.start();

    // Begin MIDI processing loop.
    q::midi_input_stream stream;
    if (stream.is_valid())
    {
        while (true)
            stream.process(midi_processor(&synth));
    }
    return 0;
}