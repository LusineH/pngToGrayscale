#ifndef png_writer_h
#define png_writer_h

#include <gst/gst.h>
#include <string>


//
/// Encodes png images.
//
class PngWriter
{
public:
    PngWriter();

    ~PngWriter()
    {}

    //
    /// Create and configure gstreamer pipeline․
    //
    bool setupPipeline(const std::string &file);

    //
    /// Helper function to push sample to pipeline.
    //
    bool pushBuffer(GstBuffer *buffer);

    //
    /// Play the pipeline.
    //
    bool play();

private:
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
    /// We need src element to push data to the pipeline.
    //
    GstElement *_src_element = nullptr;
}; // class PngWriter

#endif // png_writer_h
