#include "VideoHandler.hpp"

#include "wrapper/Pipeline.hpp"
#include "wrapper/VideoConvert.hpp"
#include "wrapper/AudioConvert.hpp"
#include "wrapper/Volume.hpp"
#include "wrapper/VideoScale.hpp"
#include "wrapper/Queue.hpp"
#include "wrapper/Decodebin.hpp"
#include "wrapper/FileSrc.hpp"
#include "wrapper/AutoAudioSink.hpp"
#include "wrapper/Element.hpp"
#include "wrapper/Pad.hpp"
#include "wrapper/Caps.hpp"
#include "wrapper/Capsfilter.hpp"
#include "customsink/XiboVideoSink.hpp"

#include "utils/Utilities.hpp"
#include <boost/format.hpp>

namespace ph = std::placeholders;

const int DEFAULT_VIDEO_BUFFER = 500;

VideoHandler::VideoHandler()
{
    if(!gst_plugin_register_static(GST_VERSION_MAJOR, GST_VERSION_MINOR, "xibovideosink", "Video Sink Plugin for gstreamer",
                                   pluginInit, "0.1", "GPL", "source", "package", "http://github.com/Stivius"))
    {
        throw std::runtime_error("XiboVideoSink was not registered");
    }

    m_videoWindow = std::make_unique<GtkDrawingAreaAdaptor>();
    m_pipeline = Gst::Pipeline::create("pipeline");
    m_source = Gst::FileSrc::create();
    m_decodebin = Gst::Decodebin::create();
    m_videoConverter = Gst::VideoConvert::create();
    m_audioConverter = Gst::AudioConvert::create();
    m_volume = Gst::Volume::create();
    m_videoScale = Gst::VideoScale::create();
    m_audioSink = Gst::AutoAudioSink::create();
    m_queue = Gst::Queue::create();
    m_videoSink = Gst::Element::create("xibovideosink");
    m_capsfilter = Gst::Capsfilter::create();

    checkGstElements();

    init();
}

IWidgetAdaptor& VideoHandler::videoWindow()
{
    return *m_videoWindow;
}

void VideoHandler::scale(double scaleX, double scaleY)
{
    setSize(static_cast<int>(width() * scaleX),
            static_cast<int>(height() * scaleY));
}

void VideoHandler::setSize(int width, int height)
{
    boost::format videoFmt{"video/x-raw, width = (int)%1%, height = (int)%2%"};

    m_capsfilter->setCaps(Gst::Caps::create((videoFmt % width % height).str()));
    m_videoWindow->setSize(width, height);
}

int VideoHandler::width() const
{
    return m_videoWindow->width();
}

int VideoHandler::height() const
{
    return m_videoWindow->height();
}

void VideoHandler::checkGstElements()
{
    if(!m_pipeline || !m_source || !m_decodebin || !m_videoConverter || !m_videoScale ||
        !m_videoSink || !m_audioConverter || !m_volume || !m_audioSink || !m_queue || !m_capsfilter)
    {
        throw std::runtime_error("[VideoHandler] One element could not be created");
    }
}

void VideoHandler::init()
{
    auto sink = GST_XIBOVIDEOSINK(m_videoSink->getHandler());
    gst_xibovideosink_set_handler(sink, &m_videoWindow->get());

    m_queue->setMaxSizeBuffers(DEFAULT_VIDEO_BUFFER);
    m_pipeline->addBusWatch(sigc::mem_fun(*this, &VideoHandler::busMessageWatch));

    m_pipeline->add(m_source)->add(m_decodebin)->add(m_videoConverter)->add(m_queue)->add(m_videoScale)->add(m_videoSink)->add(m_capsfilter)->add(m_audioConverter)->add(m_volume)->add(m_audioSink);
    m_source->link(m_decodebin);
    m_videoConverter->link(m_queue)->link(m_videoScale)->link(m_capsfilter)->link(m_videoSink);
    m_audioConverter->link(m_volume)->link(m_audioSink);

    m_decodebin->signalPadAdded().connect(sigc::mem_fun(*this, &VideoHandler::onPadAdded));
    m_decodebin->signalNoMorePads().connect(sigc::mem_fun(*this, &VideoHandler::noMorePads));
}

bool VideoHandler::busMessageWatch(const Gst::RefPtr<Gst::Message>& message)
{
    switch(message->type())
    {
        case Gst::MessageType::ERROR:
        {
            auto err = message->parseError();
            Utils::logger()->error("{}", err.getText());
            Utils::logger()->error("[VideoHandler] Debug details: {}", err.getDebugInfo());
            break;
        }
        case Gst::MessageType::EOS:
        {
            Utils::logger()->debug("[VideoHandler] End of stream");
            m_pipeline->setState(Gst::State::NULL_STATE);
            m_videoFinished.emit();
            break;
        }
        default:
            break;
    }
    return true;
}

void VideoHandler::noMorePads()
{
    auto pad = m_decodebin->getStaticPad("src_1");
    Utils::logger()->debug("[VideoHandler] No more pads");

    if(!pad)
    {
        m_audioConverter->setState(Gst::State::NULL_STATE);
        m_volume->setState(Gst::State::NULL_STATE);
        m_audioSink->setState(Gst::State::NULL_STATE);
        m_pipeline->remove(m_audioConverter)->remove(m_volume)->remove(m_audioSink);
    }
}

void VideoHandler::onPadAdded(const Gst::RefPtr<Gst::Pad>& pad)
{
    Gst::RefPtr<Gst::Pad> sinkpad;
    Utils::logger()->debug("[VideoHandler] Pad added");

    auto caps = pad->getCurrentCaps()->m_handler;

    if (gst_caps_is_any (caps)) {
        Utils::logger()->debug("ANY");
        return;
    }
    if (gst_caps_is_empty (caps)) {
        Utils::logger()->debug("EMPTY");
        return;
    }

    for (uint i = 0; i < gst_caps_get_size (caps); i++) {
        GstStructure *structure = gst_caps_get_structure (caps, i);

        Utils::logger()->debug("{}", gst_structure_get_name(structure));

//        gst_structure_foreach (structure, print_field, (gpointer) pfx);
    }

    // src_0 for video stream
    auto video_pad = m_decodebin->getStaticPad("src_0");
    // src1 for audio stream
    auto audio_pad = m_decodebin->getStaticPad("src_1");

    if(video_pad && !audio_pad)
    {
       Utils::logger()->debug("[VideoHandler] Video pad");

       auto caps = video_pad->getCurrentCaps();
       if(caps)
       {
           auto strct = caps->getStructure(0);
           Utils::logger()->info("[VideoHandler] width: {} height: {}", strct->getWidth(), strct->getHeight());
       }

       sinkpad = m_videoConverter->getStaticPad("sink");
       pad->link(sinkpad);
    }
    else if(audio_pad)
    {
       Utils::logger()->debug("[VideoHandler] Audio pad");
       sinkpad = m_audioConverter->getStaticPad("sink");
       pad->link(sinkpad);
    }
}

void VideoHandler::load(const FilePath& path)
{
    m_source->setLocation(path.string());
}

void VideoHandler::play()
{
    m_pipeline->setState(Gst::State::PLAYING);
    m_videoWindow->show();
}

void VideoHandler::stop()
{
    m_pipeline->setState(Gst::State::NULL_STATE);
    m_videoWindow->hide();
}

void VideoHandler::connect(OnVideoFinished callback)
{
    m_videoFinished.connect(callback);
}

void VideoHandler::setVolume(int volume)
{
    m_volume->setVolume(volume / static_cast<double>(MAX_VOLUME));
}
