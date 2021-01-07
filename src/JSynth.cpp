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

namespace q = cycfi::q;
using namespace q::literals;

struct synth : q::port_audio_stream
{
    synth() : port_audio_stream(0, 2) 
    {}

    void process(out_channels const& out)
    {
        auto left = out[0];
        auto right = out[1];
        for (auto frame : out.frames())
        {
            right[frame] = 0;
            // Synthesize the sin wave
            for (auto voice : keys)
            {
                right[frame] += q::saw(keys[voice.first]++);
            }
            right[frame] /= MAX_VOICES;
            left[frame] = right[frame];
        }
        process_remove();
    }        

    void add_voice(uint8_t key) 
    {
        if (keys.size() == MAX_VOICES) 
        {
            return;
        }
        keys[key] = q::phase_iterator(q::midi::note_frequency(key), this->sampling_rate());
    }

    void remove_voice(uint8_t key)
    {
        if (keys.find(key) == keys.end()) 
        {
            return;
        }
        to_delete.insert(key);
    }

    void process_remove()
    {
        for (auto elem : to_delete)
        {
            keys.erase(elem);
        }
        to_delete.clear();
    }

    std::unordered_map<uint8_t, q::phase_iterator> keys; // Current Voice storage.
    std::set<uint8_t> to_delete; // Delete buffer to avoid deleting mid iteration.
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

        this->_synth->add_voice(msg.key());
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

        this->_synth->remove_voice(msg.key());
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