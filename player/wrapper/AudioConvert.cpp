#include "AudioConvert.hpp"

Gst::AudioConvert::AudioConvert()
{
    m_element = gst_element_factory_make("audioconvert", nullptr);
}

Gst::RefPtr<Gst::AudioConvert> Gst::AudioConvert::create()
{
    return std::shared_ptr<Gst::AudioConvert>(new Gst::AudioConvert);
}
