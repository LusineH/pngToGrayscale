#ifndef png_reader_h
#define png_reader_h

#include <gst/gst.h>
#include <gst/app/gstappsink.h>
#include <string>

//
// Decodes png images.
//
class PngReader
{

public:
    PngReader();

    ~PngReader()
    {}
    //
    /// Load the media file.
    //
    bool loadMedia(const std::string &file);

    //
    /// Helper function to pull sample from pipeline.
    //
    //
    /// This function blocks until a sample or EOS becomes available or
    /// the appsink element is set to the READY/NULL state.
    /// @return a GstSample or NULL when the appsink is stopped or EOS or the timeout expires.
    //
    GstSample* pullSample();

    //
    /// Play the pipeline.
    //
    bool play();

private:
    //
    /// Create and configure gstreamer pipeline․
    //
    bool setupPipeline(const std::string &file);
    //
    /// Reports error message.
    //
    void reportError(const gchar * text);

private:
    //
    /// File name.
    //
    std::string _filepath;
    //
    /// The object represents gstreamer pipeline.
    //
    GstElement *_gst_pipeline = nullptr;
    //
    /// We need sink element to pull out data from the pipeline.
    //
    GstElement *_sink_element = nullptr;
}; // class PngReader

#endif // png_reader_h
