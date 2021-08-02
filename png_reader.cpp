#include "png_reader.h"

PngReader::PngReader()
    : _filepath("")
{}

//
/// Load the media file.
//
bool PngReader::loadMedia(const std::string &file)
{
    _filepath = file;

    // Initialize gstreamer if not initialized yet.
    if (!gst_is_initialized())
    {
        gst_init(0, 0);
    }
    // Create and configure gstreamer pipeline․
    return setupPipeline(file);
}

//
/// Create and configure the pipeline․
//
bool PngReader::setupPipeline(const std::string &filename)
{
    //
    // Construct gstreamer pipeline.
    // pngdec decodes png image data from a file into raw media.
    // videoconvert converts the frame to RGB format.
    // And we use appsink element at the end to pull out data from pipeline.
    //
    _gst_pipeline = gst_parse_launch("filesrc name=filesrc ! pngdec ! videoconvert ! videoscale !" \
                    "video/x-raw,format=RGB, width=500, height=500 ! appsink name=appsink", NULL);

    if (!_gst_pipeline)
    {
        reportError("Pipeline could not be created.");
        return false;
    }

    // We set the input filename to the filesrc element.
    GstElement *filesrc = gst_bin_get_by_name(GST_BIN(_gst_pipeline), "filesrc");
    g_object_set(filesrc, "location", filename.c_str(), NULL);
    gst_object_unref(filesrc);

    //
    // We use appsink element to pull out data from the pipeline.
    //
    _sink_element = gst_bin_get_by_name(GST_BIN(_gst_pipeline), "appsink");
    return true;
}

//
/// Helper function to pull sample from pipeline.
//
GstSample* PngReader::pullSample()
{
    // This function blocks until a sample or EOS becomes available or
    // the appsink element is set to the READY/NULL state or the timeout expires.
    // This function will only return samples when the appsink is in the PLAYING state.
    GstSample* sample = gst_app_sink_try_pull_sample(GST_APP_SINK(_sink_element), GST_CLOCK_TIME_NONE);
    return sample;
}

//
/// Change the pipeline state, and emit error if failed.
//
bool PngReader::play()
{
    GstStateChangeReturn ret = gst_element_set_state(_gst_pipeline, GST_STATE_PLAYING);

    if (ret == GST_STATE_CHANGE_FAILURE)
    {
        reportError("Unable to set the pipeline to the playing state.");
        return false;
    }
    return true;
}

//
/// Reports error message.
//
void PngReader::reportError(const gchar * text)
{
    std::string error_msg = std::string("PngReader: Error: ") + std::string(text) + "\n";
    g_printerr("%s", error_msg.c_str());
}
