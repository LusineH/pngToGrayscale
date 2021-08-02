#include "png_writer.h"

PngWriter::PngWriter()
    : _filepath("")
{}

//
/// Create and configure the pipeline․
//
bool PngWriter::setupPipeline(const std::string &filename)
{
    //
    // Construct gstreamer pipeline.
    // We use appsrc element to push data to the pipeline.
    // pngenc encodes png data.
    //
    _gst_pipeline = gst_parse_launch("appsrc name=appsrc ! pngenc ! filesink name=filesink", NULL);
    if (!_gst_pipeline)
    {
        reportError("Pipeline could not be created.");
        return false;
    }

    // We set the output filename to the filesink element.
    GstElement *filesink = gst_bin_get_by_name(GST_BIN(_gst_pipeline), "filesink");
    g_object_set(filesink, "location", filename.c_str(), NULL);
    gst_object_unref(filesink);


    //
    // We use o push data to the pipeline.
    //
    _src_element = gst_bin_get_by_name(GST_BIN(_gst_pipeline), "appsrc");
    //
    // Describing the input buffer type.
    //
    // TODO: Get width & height from input.
    GstCaps* caps = gst_caps_new_simple ("video/x-raw",
            "format", G_TYPE_STRING, "RGB",
            "width", G_TYPE_INT, 500,
            "height", G_TYPE_INT, 500,
            "framerate", GST_TYPE_FRACTION, 0, 1,
            NULL);
    g_object_set (G_OBJECT(_src_element), "caps", caps, NULL);

    return true;
}

bool PngWriter::pushBuffer(GstBuffer *buffer)
{
    if (nullptr == buffer) {
        return false;
    }

    GstFlowReturn ret;
    // Push buffer into appsrc element through \"push-buffer\" signal.
    g_signal_emit_by_name(_src_element, "push-buffer", buffer, &ret);

    if (ret != GST_FLOW_OK)
    {
        // something wrong, stop pushing.
        reportError("Something went wrong when pushing buffer.\n");
        return false;
    }
    return true;
}

//
/// Change the pipeline state, and emit error if failed.
//
bool PngWriter::play()
{
    GstStateChangeReturn ret = gst_element_set_state(_gst_pipeline, GST_STATE_PLAYING);

    if (ret == GST_STATE_CHANGE_FAILURE)
    {
        reportError("Unable to set pipeline to the playing state.");
        return false;
    }
    return true;
}

//
/// Reports error message.
//
void PngWriter::reportError(const gchar * text)
{
    std::string error_msg = std::string("PngWriter: Error: ") + std::string(text) + "\n";
    g_printerr("%s", error_msg.c_str());
}
