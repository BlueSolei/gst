#include <gst/gst.h>

int basic_tutorial_2(int argc, char **argv)
{
  GstElement *pipeline, *source, *sink, *volume;
  GstBus *bus;
  GstMessage *msg;
  GstStateChangeReturn ret;

  gst_init(&argc, &argv);

  // create the elements
  source = gst_element_factory_make("audiotestsrc", "source");
  volume = gst_element_factory_make("volume", "volume");
  sink = gst_element_factory_make("autoaudiosink", "sink");

  // create the empty pipeline
  pipeline = gst_pipeline_new("test-pipeline");
  if (!source || !sink || !pipeline)
  {
    g_printerr("Not all elements could be created\n");
    return -1;
  }

  g_object_set(volume, "volume", 0.7, NULL);
  gdouble v = 0.0;
  g_object_get(G_OBJECT(volume), "volume", &v);
  g_print("Volume level is %lf", v);

  // build the pipline
  gst_bin_add_many(GST_BIN(pipeline), source, volume, sink, NULL);
  if (!gst_element_link(source, volume) || !gst_element_link(volume, sink))
  {
    g_printerr("Source --> volume --> Sink failed to link\n");
    gst_object_unref(pipeline);
    return -1;
  }

  ret = gst_element_set_state(pipeline, GST_STATE_PLAYING);
  if (ret == GST_STATE_CHANGE_FAILURE)
  {
    g_printerr("Failed to set pipeline to PLAY state\n");
    gst_object_unref(pipeline);
    return -1;
  }

  bus = gst_element_get_bus(pipeline);
  msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE,
                                   (uint) GST_MESSAGE_ERROR | (uint) GST_MESSAGE_EOS);

  // parse message
  if (msg != NULL)
  {
    GError *err = NULL;
    gchar *debug_info = NULL;
    switch (GST_MESSAGE_TYPE(msg))
    {
      case GST_MESSAGE_ERROR:
        gst_message_parse_error(msg, &err, &debug_info);
        g_printerr("Error received from element '%s', error: '%s'\n", GST_OBJECT_NAME(msg->src),
                   err->message);
        g_printerr("Debugging info: '%s'\n", debug_info ? debug_info : "none");
        g_clear_error(&err);
        g_free(debug_info);
        break;
      case GST_MESSAGE_EOS:
        g_print("End-Of-Stream reached\n");
        break;
      default:
        g_printerr("Unexpected message received\n");
        break;
    }
    gst_message_unref(msg);
  }

  gst_object_unref(bus);
  gst_element_set_state(pipeline, GST_STATE_NULL);
  gst_object_unref(pipeline);

  return 0;
}