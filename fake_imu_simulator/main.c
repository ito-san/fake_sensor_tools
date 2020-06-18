
/**
 * @file main.c
 * @brief Main program
 */

// Suppressing deprecated warnings
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#include <gtk/gtk.h>
#pragma GCC diagnostic warning "-Wdeprecated-declarations"

#include <interface.h>

/**
 * @brief pointers to widgets
 */
typedef struct
{
  GtkWidget * hbr_header;  //!< @brief GtkHeaderBar
  GtkWidget * sbar_list;   //!< @brief GtkStackSidebar
  GtkWidget * stk_base;    //!< @brief GtkStack

  GtkWidget * grd_general;        //!< @brief GtkGrid
  GtkWidget * txt_device_name;    //!< @brief GtkEntry
  GtkWidget * sw_serial_port;     //!< @brief GtkSwitch
  GtkWidget * sw_checksum_error;  //!< @brief GtkSwitch
  GtkWidget * sw_debug_output;    //!< @brief GtkSwitch

  GtkWidget * grd_bin;        //!< @brief GtkGrid
  GtkWidget * file_log_file;  //!< @brief GtkFileChooserButton
} Widgets;

void initGeneral(GtkBuilder * b, Widgets * w)
{
  w->grd_general = GTK_WIDGET(gtk_builder_get_object(b, "grd_general"));
  w->txt_device_name = GTK_WIDGET(gtk_builder_get_object(b, "txt_device_name"));
  w->sw_serial_port = GTK_WIDGET(gtk_builder_get_object(b, "sw_serial_port"));
  w->sw_checksum_error = GTK_WIDGET(gtk_builder_get_object(b, "sw_checksum_error"));
  w->sw_debug_output = GTK_WIDGET(gtk_builder_get_object(b, "sw_debug_output"));

  // Adds a child to stack
  gtk_stack_add_named(GTK_STACK(w->stk_base), w->grd_general, "General");
  // Sets one or more child properties for child and container
  gtk_container_child_set(GTK_CONTAINER(w->stk_base), w->grd_general, "title", "General", NULL);

  // Set the text in the widget
  gtk_entry_set_text(GTK_ENTRY(w->txt_device_name), getDeviceName());
}

void initBIN(GtkBuilder * b, Widgets * w)
{
  // Get the object
  w->grd_bin = GTK_WIDGET(gtk_builder_get_object(b, "grd_bin"));
  w->file_log_file = GTK_WIDGET(gtk_builder_get_object(b, "file_log_file"));

  // Adds a child to stack
  gtk_stack_add_named(GTK_STACK(w->stk_base), w->grd_bin, "BIN");
  // Sets one or more child properties for child and container
  gtk_container_child_set(GTK_CONTAINER(w->stk_base), w->grd_bin, "title", "BIN", NULL);

  // Set filename as the current filename for the file chooser
  gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(w->file_log_file), getLogFile());
}

int main(int argc, char * argv[])
{
  GtkBuilder * builder;  // Build an interface from an XML UI definition
  GtkWidget * window;    // Base class for all widgets

  // Allocate a block of memory from the slice allocator
  Widgets * widgets = g_slice_new(Widgets);

  // Load data from ini file
  loadIniFile();

  // Initialize everything needed to operate the toolkit
  gtk_init(&argc, &argv);
  // Build the GtkBuilder UI definition
  builder = gtk_builder_new_from_file("fake_imu_simulator.glade");
  // Get the object
  window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));
  widgets->hbr_header = GTK_WIDGET(gtk_builder_get_object(builder, "hbr_header"));
  widgets->sbar_list = GTK_WIDGET(gtk_builder_get_object(builder, "sbar_list"));
  widgets->stk_base = GTK_WIDGET(gtk_builder_get_object(builder, "stk_base"));

  // General
  initGeneral(builder, widgets);
  // BIN
  initBIN(builder, widgets);

  // Set a position constraint for this window
  gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER_ALWAYS);

  // Connect handlers to the named signals
  gtk_builder_connect_signals(builder, widgets);

  // Decreases the reference count of object
  g_object_unref(builder);
  // Flag a widget to be displayed
  gtk_widget_show(window);
  // Run the main loop
  gtk_main();

  // Free a block of memory that has been allocated from the slice allocator
  g_slice_free(Widgets, widgets);

  return 0;
}

/**
 * @brief Signals that all holders of a reference to the widget should release the reference that they hold
 * @param [in] object the object which received the signal
 * @param [in] user_data user data set when the signal handler was connected
 */
void on_window_destroy(GtkWidget * object, gpointer user_data)
{
  // Make the innermost invocation of the main loop return
  gtk_main_quit();

  // Save data to ini file
  saveIniFile();
}

/**
 * @brief Emitted when widget is going to be mapped, that is when the widget is visible
 * @param [in] widget the object which received the signal
 * @param [in] user_data user data set when the signal handler was connected
 */
void on_grd_map(GtkWidget * widget, gpointer user_data)
{
  gtk_header_bar_set_title(GTK_HEADER_BAR(user_data), gtk_widget_get_name(widget));
}

/**
 * @brief Emitted at the end of a single user-visible operation on the contents
 * @param [in] editable the object which received the signal
 * @param [in] user data set when the signal handler was connected
 */
void on_txt_device_name_changed(GtkEditable * editable, gpointer user_data)
{
  // Retrieve the contents of the entry widget
  // and set device name for saving it to ini file
  setDeviceName(gtk_entry_get_text(GTK_ENTRY(editable)));
}

/**
 * @brief Emitted when there is a change in the set of selected files
 * @param [in] chooser the object which received the signal
 * @param [in] user data set when the signal handler was connected
 */
void on_file_log_file_selection_changed(GtkFileChooser * chooser, gpointer user_data)
{
  // Get the filename for the currently selected file in the file selector
  // and set path of log file for saving it to ini file
  setLogFile(gtk_file_chooser_get_filename(chooser));
}

/**
 * @brief Emitted when the user changes the switch position
 * @param [in] widget the object on which the signal was emitted
 * @param [in] state the new state of the switch
 * @param [in] user_data user data set when the signal handler was connected
 * @return TRUE to stop the signal emission
 */
gboolean on_sw_serial_port_state_set(GtkSwitch * widget, gboolean state, gpointer user_data)
{
  if (state) {
    // Start serial port communication
    if (start() == 0) {
      // Set the sensitivity of a widget
      gtk_widget_set_sensitive(user_data, TRUE);
    }
  } else {
    // Set the sensitivity of a widget
    gtk_widget_set_sensitive(user_data, FALSE);
    // Stop serial port communication
    stop();
  }
  return FALSE;
}

/**
 * @brief Emitted when the user changes the switch position
 * @param [in] widget the object on which the signal was emitted
 * @param [in] state the new state of the switch
 * @param [in] user_data user data set when the signal handler was connected
 * @return TRUE to stop the signal emission
 */
gboolean on_sw_checksum_error_state_set(GtkSwitch * widget, gboolean state, gpointer user_data)
{
  // Generate checksum error or not
  setChecksumError(state);
  return FALSE;
}

/**
 * @brief Emitted when the user changes the switch position
 * @param [in] widget the object on which the signal was emitted
 * @param [in] state the new state of the switch
 * @param [in] user_data user data set when the signal handler was connected
 * @return TRUE to stop the signal emission
 */
gboolean on_sw_debug_output_state_set(GtkSwitch * widget, gboolean state, gpointer user_data)
{
  // Show debug output or not
  setDebugOutput(state);
  return FALSE;
}
