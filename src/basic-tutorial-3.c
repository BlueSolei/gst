
#include <gst/gst.h>

typedef struct _Graph
{
  GstElement* pipeline;
  GstElement* source;
  GstElement* convert;
  GstElement* sink;
} Graph;

static void pad_added_handler(GstElement* element, GstPad* pad, Graph* this);

int basic_tutorial_3(int argc, char** argv)
{
  Graph graph;
  GstBus* bus;
  GstMessage* msg;
  GstStateChangeReturn ret;
  gboolean terminate = FALSE;

  gst_init(&argc, &argv);

  graph.pipeline = gst_pipeline_new("test-pipeline");
  graph.source = gst_element_factory_make("uridecodebin", "source");
  graph.convert = gst_element_factory_make("audioconvert", "convert");
  graph.sink = gst_element_factory_make("autoaudiosink", "sink");

  if (!graph.pipeline || !graph.source || !graph.convert || !graph.sink)
  {
    g_printerr("Failed to create graph's elements\n");
    return -1;
  }

  gst_bin_add_many(GST_BIN(graph.pipeline), graph.source, graph.convert, graph.sink, NULL);
  if (!gst_element_link(graph.convert, graph.sink))
  {
    g_printerr("Failed to link converter --> sink\n");
    g_object_unref(graph.pipeline);
    return -1;
  }

  g_object_set(
      graph.source, "uri",
      "https://www.freedesktop.org/software/gstreamer-sdk/data/media/sintel_trailer-480p.webm",
      NULL);

  g_signal_connect(graph.source, "pad-added", (GCallback) pad_added_handler, &graph);

  ret = gst_element_set_state(graph.pipeline, GST_STATE_PLAYING);
  if (ret == GST_STATE_CHANGE_FAILURE)
  {
    g_printerr("Failed to start play\n");
    g_object_unref(graph.pipeline);
    return -1;
  }

  bus = gst_element_get_bus(graph.pipeline);
  do
  {
    msg = gst_bus_timed_pop_filtered(
        bus, GST_CLOCK_TIME_NONE,
        (uint) GST_MESSAGE_STATE_CHANGED | (uint) GST_MESSAGE_EOS | (uint) GST_MESSAGE_ERROR);

    if (msg == NULL)
      continue;

    GError* err;
    gchar* debug_info;

    switch (GST_MESSAGE_TYPE(msg))
    {
      case GST_MESSAGE_ERROR:
        gst_message_parse_error(msg, &err, &debug_info);
        g_printerr("Error received from element %s: '%s'\n", GST_OBJECT_NAME(msg->src),
                   err->message);
        g_printerr("Debugging info: '%s'\n", debug_info ? debug_info : "<null>");
        g_clear_error(&err);
        g_free(debug_info);
        terminate = TRUE;
        break;
      case GST_MESSAGE_EOS:
        g_print("End of stream reached\n");
        terminate = TRUE;
        break;
      case GST_MESSAGE_STATE_CHANGED:
        if (GST_MESSAGE_SRC(msg) == GST_OBJECT(graph.pipeline))
        {
          GstState old_state, new_state, pending_state;
          gst_message_parse_state_changed(msg, &old_state, &new_state, &pending_state);
          g_print("Pipeline state changed. %s --> %s\n", gst_element_state_get_name(old_state),
                  gst_element_state_get_name(new_state));
        }
        break;
      default:
        g_printerr("Unexpected message type\n");
        break;
    }
    gst_message_unref(msg);
  } while (!terminate);

  g_object_unref(bus);
  gst_element_set_state(graph.pipeline, GST_STATE_NULL);
  g_object_unref(graph.pipeline);
  return 0;
}

static void pad_added_handler(GstElement* element, GstPad* pad, Graph* this)
{
  GstPad* sink_pad = gst_element_get_static_pad(this->convert, "sink");
  GstPadLinkReturn ret;
  GstCaps* new_pad_caps = NULL;
  GstStructure* new_pad_struct = NULL;
  const gchar* new_pad_type = NULL;
  g_print("Received new pad '%s' from '%s':\n", GST_PAD_NAME(pad), GST_ELEMENT_NAME(element));

  if (gst_pad_is_linked(sink_pad))
  {
    g_print("Converter is already linked. ignoring this pad.\n");
    return;
  }

  // check the new pad's type
  new_pad_caps = gst_pad_get_current_caps(pad);
  new_pad_struct = gst_caps_get_structure(new_pad_caps, 0);
  new_pad_type = gst_structure_get_name(new_pad_struct);
  if (!g_str_has_prefix(new_pad_type, "audio/x-raw"))
  {
    g_print("Pad has type '%s', which is not audio. ignoring.\n", new_pad_type);
    goto exit;
  }

  // link the new pad to the converter
  ret = gst_pad_link(pad, sink_pad);
  if (GST_PAD_LINK_FAILED(ret))
  {
    g_printerr("Pad type is '%s' but link failed :-(\n", new_pad_type);
  }
  else
  {
    g_print("Link succeeded (pad type '%s')\n", new_pad_type);
  }

exit:
  if (new_pad_caps)
    gst_caps_unref(new_pad_caps);

  gst_object_unref(sink_pad);
}