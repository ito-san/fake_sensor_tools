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
  GtkWidget * w_txt_device_name;        //!< @brief GtkEntry
  GtkWidget * w_file_ubx_log_file;      //!< @brief GtkFileChooserButton
  GtkWidget * w_file_ubx_pvt_log_file;  //!< @brief GtkFileChooserButton
  GtkWidget * w_file_nmea_log_file;     //!< @brief GtkFileChooserButton
  GtkWidget * w_sw_serial_port;         //!< @brief GtkSwitch
  GtkWidget * w_sw_checksum_error;      //!< @brief GtkSwitch
  GtkWidget * w_sw_debug_output;        //!< @brief GtkSwitch
} Widgets;

/**
 * @brief Main function
 * @param [in] argc the count of command line arguments
 * @param [in] argv the command line arguments
 */
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
  builder = gtk_builder_new_from_file("fake_gnss_simulator.glade");
  // Get the object
  window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));
  widgets->w_txt_device_name = GTK_WIDGET(gtk_builder_get_object(builder, "txt_device_name"));
  widgets->w_file_ubx_log_file = GTK_WIDGET(gtk_builder_get_object(builder, "file_ubx_log_file"));
  widgets->w_file_ubx_pvt_log_file =
    GTK_WIDGET(gtk_builder_get_object(builder, "file_ubx_pvt_log_file"));
  widgets->w_file_nmea_log_file = GTK_WIDGET(gtk_builder_get_object(builder, "file_nmea_log_file"));
  widgets->w_sw_serial_port = GTK_WIDGET(gtk_builder_get_object(builder, "sw_serial_port"));
  widgets->w_sw_checksum_error = GTK_WIDGET(gtk_builder_get_object(builder, "sw_checksum_error"));
  widgets->w_sw_debug_output = GTK_WIDGET(gtk_builder_get_object(builder, "sw_debug_output"));

  // Set a position constraint for this window
  gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER_ALWAYS);

  // Set the text in the widget
  gtk_entry_set_text(GTK_ENTRY(widgets->w_txt_device_name), getDeviceName());
  // Set filename as the current filename for the file chooser
  gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(widgets->w_file_ubx_log_file), getUBXLogFile());
  gtk_file_chooser_set_filename(
    GTK_FILE_CHOOSER(widgets->w_file_ubx_pvt_log_file), getUBXPVTLogFile());
  gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(widgets->w_file_nmea_log_file), getNMEALogFile());

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
void on_file_ubx_log_file_selection_changed(GtkFileChooser * chooser, gpointer user_data)
{
  // Get the filename for the currently selected file in the file selector
  // and set path of ubx log file for saving it to ini file
  setUBXLogFile(gtk_file_chooser_get_filename(chooser));
}

/**
 * @brief Emitted when there is a change in the set of selected files
 * @param [in] chooser the object which received the signal
 * @param [in] user data set when the signal handler was connected
 */
void on_file_ubx_pvt_log_file_selection_changed(GtkFileChooser * chooser, gpointer user_data)
{
  // Get the filename for the currently selected file in the file selector
  // and set path of ubx pvt log file for saving it to ini file
  setUBXPVTLogFile(gtk_file_chooser_get_filename(chooser));
}

/**
 * @brief Emitted when there is a change in the set of selected files
 * @param [in] chooser the object which received the signal
 * @param [in] user data set when the signal handler was connected
 */
void on_file_nmea_log_file_selection_changed(GtkFileChooser * chooser, gpointer user_data)
{
  // Get the filename for the currently selected file in the file selector
  // and set path of nmea log file for saving it to ini file
  setNMEALogFile(gtk_file_chooser_get_filename(chooser));
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
