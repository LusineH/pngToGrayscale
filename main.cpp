#include "png_reader.h"
#include "png_writer.h"
#include <stdio.h>
#include <string.h>
#include <gst/gst.h>
#include <gst/video/video.h>
#include <fstream>
#include <sys/time.h>
#include <unistd.h>

//
/// Some helper functions.
//
bool is_file_exist(const char *fileName)
{
    std::ifstream infile(fileName);
    return infile.good();
}

// TODO: padding! If there is padding present, then first determine padding and access in different way.
bool rgb2gray(unsigned char *src, unsigned char *dest, int width, int height)
{
    int r, g, b;
    for (int i=0;i<width*height;++i)
    {
        r = *src++; // load red
        g = *src++; // load green
        b = *src++; // load blue
        // Build weighted average.
        *dest++ = (r * 76 + g * 150 + b * 30) >>8;
        *dest++ = (r * 76 + g * 150 + b * 30) >>8;
        *dest++ = (r * 76 + g * 150 + b * 30) >>8;
    }
    return true;
}

//
/// Converts input rgb buffer to grayscale and returns it.
//
GstBuffer* getGrayBuffer(GstSample* rgb_frame)
{
    GstBuffer *out_buf = nullptr;
    if (rgb_frame)
    {
        // We have a valid sample do things with the image here.
        GstBuffer *buffer = gst_sample_get_buffer(rgb_frame);
        out_buf = gst_buffer_copy(buffer);

        GstMapInfo info; // contains the actual image
        if (gst_buffer_map(buffer, &info, GST_MAP_READ))
        {
            GstVideoInfo* video_info = gst_video_info_new();
            if (!gst_video_info_from_caps(video_info, gst_sample_get_caps(rgb_frame)))
            {
                // Could not parse video info (should not happen).
                g_printerr("Failed to parse video info\n");
                return NULL;
            }
            // Get a pointer to the image data.
            unsigned char* data = info.data;
            gint height = video_info->height;
            gint width = video_info->width;
            gint size = video_info->size;
            g_print("Info: heigh %d width %d size %d\n", height, width, size);

            // Convert the input rgb buffer to grayscale.
            unsigned char *dest = new unsigned char[size];
            rgb2gray(data, dest, width, height );

            // Return the output GstBuffer.
            gst_buffer_fill(out_buf, 0, dest, size);
        }
    }
    return out_buf;
}


int main(int argc, char **argv)
{
    if (argc != 3)
    {
        g_printerr("Usage: %s <input> <output>\n", argv[0]);
        return -1;
    }

    // Initialize GStreamer.
    gst_init (NULL, NULL);

    // Validate input argument.
    std::string input_filename = argv[1];
    if (!is_file_exist(input_filename.c_str()))
    {
        g_printerr("Invalid input file %s\n", input_filename.c_str());
        return -1;
    }

    // PngReader reads the input file and constructs a pipeline to decode the input image.
    PngReader pngReader;
    if (!pngReader.loadMedia(input_filename))
        return -1;
    if (!pngReader.play())
        return -1;

    // PngWriter constructs a pipeline to encode the input buffer as png file.
    PngWriter pngWriter;
    if (!pngWriter.setupPipeline(argv[2]))
        return -1;
    if (!pngWriter.play())
        return -1;

    // Get the frame from reader.
    GstSample* frame = pngReader.pullSample();
    if (!frame)
    {
        g_printerr("Couldn't read the frame\n");
        return -1;
    }

    // Convert decoded frame from rgb to grayscale.
    GstBuffer *buffer = getGrayBuffer(frame);
    if (buffer)
    {
        // Give the converted grayscale frame to encoder pipeline.
        // This will save the frame to output buffer.
        pngWriter.pushBuffer(buffer);
    }

    sleep(1);
    return 0;
}