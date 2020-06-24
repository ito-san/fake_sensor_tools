
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

  GtkWidget * grd_general;      //!< @brief GtkGrid
  GtkWidget * txt_address;      //!< @brief GtkEntry
  GtkWidget * sw_server;        //!< @brief GtkSwitch
  GtkWidget * sw_debug_output;  //!< @brief GtkSwitch

  GtkWidget * grd_info;        //!< @brief GtkGrid
  GtkWidget * file_info_json;  //!< @brief GtkFileChooserButton

  GtkWidget * grd_diag;            //!< @brief GtkGrid
  GtkWidget * file_diag_json;      //!< @brief GtkFileChooserButton
  GtkAdjustment * adj_top_hv;      //!< @brief GtkAdjustment
  GtkAdjustment * adj_top_temp;    //!< @brief GtkAdjustment
  GtkAdjustment * adj_top_5v;      //!< @brief GtkAdjustment
  GtkAdjustment * adj_top_2_5v;    //!< @brief GtkAdjustment
  GtkAdjustment * adj_top_3_3v;    //!< @brief GtkAdjustment
  GtkAdjustment * adj_top_5v_raw;  //!< @brief GtkAdjustment
  GtkAdjustment * adj_top_vcc;     //!< @brief GtkAdjustment

  GtkAdjustment * adj_bot_i_out;  //!< @brief GtkAdjustment
  GtkAdjustment * adj_bot_1_2v;   //!< @brief GtkAdjustment
  GtkAdjustment * adj_bot_temp;   //!< @brief GtkAdjustment
  GtkAdjustment * adj_bot_5v;     //!< @brief GtkAdjustment
  GtkAdjustment * adj_bot_2_5v;   //!< @brief GtkAdjustment
  GtkAdjustment * adj_bot_3_3v;   //!< @brief GtkAdjustment
  GtkAdjustment * adj_bot_v_in;   //!< @brief GtkAdjustment
  GtkAdjustment * adj_bot_1_25v;  //!< @brief GtkAdjustment

  GtkWidget * grd_status;         //!< @brief GtkGrid
  GtkWidget * file_status_json;   //!< @brief GtkFileChooserButton
  GtkWidget * sw_motor_state;     //!< @brief GtkSwitch
  GtkAdjustment * adj_motor_rpm;  //!< @brief GtkAdjustment
  GtkWidget * sw_laser_state;     //!< @brief GtkSwitch

  GtkWidget * grd_settings;         //!< @brief GtkGrid
  GtkWidget * file_settings_json;   //!< @brief GtkFileChooserButton
  GtkAdjustment * adj_rpm_setting;  //!< @brief GtkAdjustment
} Widgets;

void initGeneral(GtkBuilder * b, Widgets * w)
{
  w->grd_general = GTK_WIDGET(gtk_builder_get_object(b, "grd_general"));
  w->txt_address = GTK_WIDGET(gtk_builder_get_object(b, "txt_address"));
  w->sw_server = GTK_WIDGET(gtk_builder_get_object(b, "sw_server"));
  w->sw_debug_output = GTK_WIDGET(gtk_builder_get_object(b, "sw_debug_output"));

  // Adds a child to stack
  gtk_stack_add_named(GTK_STACK(w->stk_base), w->grd_general, "General");
  // Sets one or more child properties for child and container
  gtk_container_child_set(GTK_CONTAINER(w->stk_base), w->grd_general, "title", "General", NULL);

  // Set the text in the widget
  gtk_entry_set_text(GTK_ENTRY(w->txt_address), getAddress());
}

void initInfo(GtkBuilder * b, Widgets * w)
{
  w->grd_info = GTK_WIDGET(gtk_builder_get_object(b, "grd_info"));
  w->file_info_json = GTK_WIDGET(gtk_builder_get_object(b, "file_info_json"));

  // Adds a child to stack
  gtk_stack_add_named(GTK_STACK(w->stk_base), w->grd_info, "Info");
  // Sets one or more child properties for child and container
  gtk_container_child_set(GTK_CONTAINER(w->stk_base), w->grd_info, "title", "Info", NULL);

  // Set filename as the current filename for the file chooser
  const char * file = getInfoJson();
  if (strlen(file) > 0) gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(w->file_info_json), file);
}

void initDiag(GtkBuilder * b, Widgets * w)
{
  w->grd_diag = GTK_WIDGET(gtk_builder_get_object(b, "grd_diag"));
  w->file_diag_json = GTK_WIDGET(gtk_builder_get_object(b, "file_diag_json"));
  w->adj_top_hv = GTK_ADJUSTMENT(gtk_builder_get_object(b, "adj_top_hv"));
  w->adj_top_temp = GTK_ADJUSTMENT(gtk_builder_get_object(b, "adj_top_temp"));
  w->adj_top_5v = GTK_ADJUSTMENT(gtk_builder_get_object(b, "adj_top_5v"));
  w->adj_top_2_5v = GTK_ADJUSTMENT(gtk_builder_get_object(b, "adj_top_2_5v"));
  w->adj_top_3_3v = GTK_ADJUSTMENT(gtk_builder_get_object(b, "adj_top_3_3v"));
  w->adj_top_5v_raw = GTK_ADJUSTMENT(gtk_builder_get_object(b, "adj_top_5v_raw"));
  w->adj_top_vcc = GTK_ADJUSTMENT(gtk_builder_get_object(b, "adj_top_vcc"));

  w->adj_bot_i_out = GTK_ADJUSTMENT(gtk_builder_get_object(b, "adj_bot_i_out"));
  w->adj_bot_1_2v = GTK_ADJUSTMENT(gtk_builder_get_object(b, "adj_bot_1_2v"));
  w->adj_bot_temp = GTK_ADJUSTMENT(gtk_builder_get_object(b, "adj_bot_temp"));
  w->adj_bot_5v = GTK_ADJUSTMENT(gtk_builder_get_object(b, "adj_bot_5v"));
  w->adj_bot_2_5v = GTK_ADJUSTMENT(gtk_builder_get_object(b, "adj_bot_2_5v"));
  w->adj_bot_3_3v = GTK_ADJUSTMENT(gtk_builder_get_object(b, "adj_bot_3_3v"));
  w->adj_bot_v_in = GTK_ADJUSTMENT(gtk_builder_get_object(b, "adj_bot_v_in"));
  w->adj_bot_1_25v = GTK_ADJUSTMENT(gtk_builder_get_object(b, "adj_bot_1_25v"));

  // Adds a child to stack
  gtk_stack_add_named(GTK_STACK(w->stk_base), w->grd_diag, "Diagnostics");
  // Sets one or more child properties for child and container
  gtk_container_child_set(GTK_CONTAINER(w->stk_base), w->grd_diag, "title", "Diagnostics", NULL);

  // Set filename as the current filename for the file chooser
  const char * file = getDiagJson();
  if (strlen(file) > 0) gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(w->file_diag_json), file);

  // Sets the GtkAdjustment value
  gtk_adjustment_set_value(w->adj_top_hv, getTopHv());
  gtk_adjustment_set_value(w->adj_top_temp, getTopTemp());
  gtk_adjustment_set_value(w->adj_top_5v, getTop5v());
  gtk_adjustment_set_value(w->adj_top_2_5v, getTop2_5v());
  gtk_adjustment_set_value(w->adj_top_3_3v, getTop3_3v());
  gtk_adjustment_set_value(w->adj_top_5v_raw, getTop5vRaw());
  gtk_adjustment_set_value(w->adj_top_vcc, getTopVcc());

  gtk_adjustment_set_value(w->adj_bot_i_out, getBottomIOut());
  gtk_adjustment_set_value(w->adj_bot_1_2v, getBottom1_2v());
  gtk_adjustment_set_value(w->adj_bot_temp, getBottomTemp());
  gtk_adjustment_set_value(w->adj_bot_5v, getBottom5v());
  gtk_adjustment_set_value(w->adj_bot_2_5v, getBottom2_5v());
  gtk_adjustment_set_value(w->adj_bot_3_3v, getBottom3_3v());
  gtk_adjustment_set_value(w->adj_bot_v_in, getBottomVIn());
  gtk_adjustment_set_value(w->adj_bot_1_25v, getBottom1_25v());
}

void initStatus(GtkBuilder * b, Widgets * w)
{
  w->grd_status = GTK_WIDGET(gtk_builder_get_object(b, "grd_status"));
  w->file_status_json = GTK_WIDGET(gtk_builder_get_object(b, "file_status_json"));
  w->sw_motor_state = GTK_WIDGET(gtk_builder_get_object(b, "sw_motor_state"));
  w->adj_motor_rpm = GTK_ADJUSTMENT(gtk_builder_get_object(b, "adj_motor_rpm"));
  w->sw_laser_state = GTK_WIDGET(gtk_builder_get_object(b, "sw_laser_state"));

  // Adds a child to stack
  gtk_stack_add_named(GTK_STACK(w->stk_base), w->grd_status, "Status");
  // Sets one or more child properties for child and container
  gtk_container_child_set(GTK_CONTAINER(w->stk_base), w->grd_status, "title", "Status", NULL);

  // Set filename as the current filename for the file chooser
  const char * file = getStatusJson();
  if (strlen(file) > 0) gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(w->file_status_json), file);

  // Sets the underlying state
  gtk_switch_set_state(GTK_SWITCH(w->sw_motor_state), getMotorState());
  gtk_switch_set_state(GTK_SWITCH(w->sw_laser_state), getLaserState());

  // Sets the GtkAdjustment value
  gtk_adjustment_set_value(w->adj_motor_rpm, getMotorRpm());
}

void initSettings(GtkBuilder * b, Widgets * w)
{
  w->grd_settings = GTK_WIDGET(gtk_builder_get_object(b, "grd_settings"));
  w->file_settings_json = GTK_WIDGET(gtk_builder_get_object(b, "file_settings_json"));
  w->adj_rpm_setting = GTK_ADJUSTMENT(gtk_builder_get_object(b, "adj_rpm_setting"));

  // Adds a child to stack
  gtk_stack_add_named(GTK_STACK(w->stk_base), w->grd_settings, "Settings");
  // Sets one or more child properties for child and container
  gtk_container_child_set(GTK_CONTAINER(w->stk_base), w->grd_settings, "title", "Settings", NULL);

  // Set filename as the current filename for the file chooser
  const char * file = getSettingsJson();
  if (strlen(file) > 0)
    gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(w->file_settings_json), file);

  // Sets the GtkAdjustment value
  gtk_adjustment_set_value(w->adj_rpm_setting, getRpmSetting());
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
  builder = gtk_builder_new_from_file("fake_velodyne_simulator.glade");
  // Get the object
  window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));
  widgets->hbr_header = GTK_WIDGET(gtk_builder_get_object(builder, "hbr_header"));
  widgets->sbar_list = GTK_WIDGET(gtk_builder_get_object(builder, "sbar_list"));
  widgets->stk_base = GTK_WIDGET(gtk_builder_get_object(builder, "stk_base"));

  // General
  initGeneral(builder, widgets);
  // Info
  initInfo(builder, widgets);
  // Diagnostics
  initDiag(builder, widgets);
  // Status
  initStatus(builder, widgets);
  // Settings
  initSettings(builder, widgets);

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

// General
/**
 * @brief Emitted at the end of a single user-visible operation on the contents
 * @param [in] editable the object which received the signal
 * @param [in] user data set when the signal handler was connected
 */
void on_txt_address_changed(GtkEditable * editable, gpointer user_data)
{
  // Retrieve the contents of the entry widget
  // and set device name for saving it to ini file
  setAddress(gtk_entry_get_text(GTK_ENTRY(editable)));
}

/**
 * @brief Emitted when the user changes the switch position
 * @param [in] widget the object on which the signal was emitted
 * @param [in] state the new state of the switch
 * @param [in] user_data user data set when the signal handler was connected
 * @return TRUE to stop the signal emission
 */
gboolean on_sw_server_state_set(GtkSwitch * widget, gboolean state, gpointer user_data)
{
  if (state) {
    // Start HTTP server
    start();
  } else {
    // Stop HTTP server
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
gboolean on_sw_debug_output_state_set(GtkSwitch * widget, gboolean state, gpointer user_data)
{
  // Show debug output or not
  setDebugOutput(state);
  return FALSE;
}

// Info
/**
 * @brief Emitted when there is a change in the set of selected files
 * @param [in] chooser the object which received the signal
 * @param [in] user data set when the signal handler was connected
 */
void on_file_info_json_selection_changed(GtkFileChooser * chooser, gpointer user_data)
{
  // Get the filename for the currently selected file in the file selector
  // and set path of info.json for saving it to ini file
  setInfoJson(gtk_file_chooser_get_filename(chooser));
}

// Diagnostics
/**
 * @brief Emitted when there is a change in the set of selected files
 * @param [in] chooser the object which received the signal
 * @param [in] user data set when the signal handler was connected
 */
void on_file_diag_json_selection_changed(GtkFileChooser * chooser, gpointer user_data)
{
  // Get the filename for the currently selected file in the file selector
  // and set path of diag.json for saving it to ini file
  setDiagJson(gtk_file_chooser_get_filename(chooser));
}

/**
 * @brief Emitted when the “value” property has been changed
 * @param [in] adjustment the object which received the signal
 * @param [in] user_data user data set when the signal handler was connected
 */
void on_adj_top_hv_value_changed(GtkAdjustment * adjustment, gpointer user_data)
{
  // Gets the current value of the adjustment
  gdouble value = gtk_adjustment_get_value(adjustment);
  // Set the high voltage bias to the APD (Avalanche photodiode)
  setTopHv(value);
}

/**
 * @brief Emitted when the “value” property has been changed
 * @param [in] adjustment the object which received the signal
 * @param [in] user_data user data set when the signal handler was connected
 */
void on_adj_top_temp_value_changed(GtkAdjustment * adjustment, gpointer user_data)
{
  // Gets the current value of the adjustment
  gdouble value = gtk_adjustment_get_value(adjustment);
  // Set the temperature of the top board
  setTopTemp(value);
}

/**
 * @brief Emitted when the “value” property has been changed
 * @param [in] adjustment the object which received the signal
 * @param [in] user_data user data set when the signal handler was connected
 */
void on_adj_top_5v_value_changed(GtkAdjustment * adjustment, gpointer user_data)
{
  // Gets the current value of the adjustment
  gdouble value = gtk_adjustment_get_value(adjustment);
  // Set the top board's 5.0 V rail
  setTop5v(value);
}

/**
 * @brief Emitted when the “value” property has been changed
 * @param [in] adjustment the object which received the signal
 * @param [in] user_data user data set when the signal handler was connected
 */
void on_adj_top_2_5v_value_changed(GtkAdjustment * adjustment, gpointer user_data)
{
  // Gets the current value of the adjustment
  gdouble value = gtk_adjustment_get_value(adjustment);
  // Set the top board's 2.5 V rail
  setTop2_5v(value);
}

/**
 * @brief Emitted when the “value” property has been changed
 * @param [in] adjustment the object which received the signal
 * @param [in] user_data user data set when the signal handler was connected
 */
void on_adj_top_3_3v_value_changed(GtkAdjustment * adjustment, gpointer user_data)
{
  // Gets the current value of the adjustment
  gdouble value = gtk_adjustment_get_value(adjustment);
  // Set the top board's 3.3 V rail
  setTop3_3v(value);
}

/**
 * @brief Emitted when the “value” property has been changed
 * @param [in] adjustment the object which received the signal
 * @param [in] user_data user data set when the signal handler was connected
 */
void on_adj_top_5v_raw_value_changed(GtkAdjustment * adjustment, gpointer user_data)
{
  // Gets the current value of the adjustment
  gdouble value = gtk_adjustment_get_value(adjustment);
  // Set the top board's unregulated 5.0 V rail
  setTop5vRaw(value);
}

/**
 * @brief Emitted when the “value” property has been changed
 * @param [in] adjustment the object which received the signal
 * @param [in] user_data user data set when the signal handler was connected
 */
void on_adj_top_vcc_value_changed(GtkAdjustment * adjustment, gpointer user_data)
{
  // Gets the current value of the adjustment
  gdouble value = gtk_adjustment_get_value(adjustment);
  // Set the top board's 1.2 V rail
  setTop5vVcc(value);
}

/**
 * @brief Emitted when the “value” property has been changed
 * @param [in] adjustment the object which received the signal
 * @param [in] user_data user data set when the signal handler was connected
 */
void on_adj_bot_i_out_value_changed(GtkAdjustment * adjustment, gpointer user_data)
{
  // Gets the current value of the adjustment
  gdouble value = gtk_adjustment_get_value(adjustment);
  // Set the sensor's input power line
  setBottomIOut(value);
}

/**
 * @brief Emitted when the “value” property has been changed
 * @param [in] adjustment the object which received the signal
 * @param [in] user_data user data set when the signal handler was connected
 */
void on_adj_bot_1_2v_value_changed(GtkAdjustment * adjustment, gpointer user_data)
{
  // Gets the current value of the adjustment
  gdouble value = gtk_adjustment_get_value(adjustment);
  // Set the bottom board's 1.2 V rail
  setBottom1_2v(value);
}

/**
 * @brief Emitted when the “value” property has been changed
 * @param [in] adjustment the object which received the signal
 * @param [in] user_data user data set when the signal handler was connected
 */
void on_adj_bot_temp_value_changed(GtkAdjustment * adjustment, gpointer user_data)
{
  // Gets the current value of the adjustment
  gdouble value = gtk_adjustment_get_value(adjustment);
  // Set the temperature of the bottom board
  setBottomTemp(value);
}

/**
 * @brief Emitted when the “value” property has been changed
 * @param [in] adjustment the object which received the signal
 * @param [in] user_data user data set when the signal handler was connected
 */
void on_adj_bot_5v_value_changed(GtkAdjustment * adjustment, gpointer user_data)
{
  // Gets the current value of the adjustment
  gdouble value = gtk_adjustment_get_value(adjustment);
  // Set the bottom board's 5.0 V rail
  setBottom5v(value);
}

/**
 * @brief Emitted when the “value” property has been changed
 * @param [in] adjustment the object which received the signal
 * @param [in] user_data user data set when the signal handler was connected
 */
void on_adj_bot_2_5v_value_changed(GtkAdjustment * adjustment, gpointer user_data)
{
  // Gets the current value of the adjustment
  gdouble value = gtk_adjustment_get_value(adjustment);
  // Set the bottom board's 2.5 V rail
  setBottom2_5v(value);
}

/**
 * @brief Emitted when the “value” property has been changed
 * @param [in] adjustment the object which received the signal
 * @param [in] user_data user data set when the signal handler was connected
 */
void on_adj_bot_3_3v_value_changed(GtkAdjustment * adjustment, gpointer user_data)
{
  // Gets the current value of the adjustment
  gdouble value = gtk_adjustment_get_value(adjustment);
  // Set the bottom board's 3.3 V rail
  setBottom3_3v(value);
}

/**
 * @brief Emitted when the “value” property has been changed
 * @param [in] adjustment the object which received the signal
 * @param [in] user_data user data set when the signal handler was connected
 */
void on_adj_bot_v_in_value_changed(GtkAdjustment * adjustment, gpointer user_data)
{
  // Gets the current value of the adjustment
  gdouble value = gtk_adjustment_get_value(adjustment);
  // Set the voltage of the input power
  setBottomVIn(value);
}

/**
 * @brief Emitted when the “value” property has been changed
 * @param [in] adjustment the object which received the signal
 * @param [in] user_data user data set when the signal handler was connected
 */
void on_adj_bot_1_25v_value_changed(GtkAdjustment * adjustment, gpointer user_data)
{
  // Gets the current value of the adjustment
  gdouble value = gtk_adjustment_get_value(adjustment);
  // Set the bottom board's 1.25 V rail
  setBottom1_25v(value);
}

// Status
/**
 * @brief Emitted when there is a change in the set of selected files
 * @param [in] chooser the object which received the signal
 * @param [in] user data set when the signal handler was connected
 */
void on_file_status_json_selection_changed(GtkFileChooser * chooser, gpointer user_data)
{
  // Get the filename for the currently selected file in the file selector
  // and set path of diag.json for saving it to ini file
  setStatusJson(gtk_file_chooser_get_filename(chooser));
}

/**
 * @brief Emitted when the user changes the switch position
 * @param [in] widget the object on which the signal was emitted
 * @param [in] state the new state of the switch
 * @param [in] user_data user data set when the signal handler was connected
 * @return TRUE to stop the signal emission
 */
gboolean on_sw_motor_state_state_set(GtkSwitch * widget, gboolean state, gpointer user_data)
{
  // Set motor state
  setMotorState(state);
  return FALSE;
}

/**
 * @brief Emitted when the “value” property has been changed
 * @param [in] adjustment the object which received the signal
 * @param [in] user_data user data set when the signal handler was connected
 */
void on_adj_motor_rpm_value_changed(GtkAdjustment * adjustment, gpointer user_data)
{
  // Gets the current value of the adjustment
  gdouble value = gtk_adjustment_get_value(adjustment);
  // Set motor rpm
  setMotorRpm(value);
}

/**
 * @brief Emitted when the user changes the switch position
 * @param [in] widget the object on which the signal was emitted
 * @param [in] state the new state of the switch
 * @param [in] user_data user data set when the signal handler was connected
 * @return TRUE to stop the signal emission
 */
gboolean on_sw_laser_state_state_set(GtkSwitch * widget, gboolean state, gpointer user_data)
{
  // Set laser state
  setLaserState(state);
  return FALSE;
}

// Settings
/**
 * @brief Emitted when there is a change in the set of selected files
 * @param [in] chooser the object which received the signal
 * @param [in] user data set when the signal handler was connected
 */
void on_file_settings_json_selection_changed(GtkFileChooser * chooser, gpointer user_data)
{
  // Get the filename for the currently selected file in the file selector
  // and set path of diag.json for saving it to ini file
  setSettingsJson(gtk_file_chooser_get_filename(chooser));
}

/**
 * @brief Emitted when the “value” property has been changed
 * @param [in] adjustment the object which received the signal
 * @param [in] user_data user data set when the signal handler was connected
 */
void on_adj_rpm_setting_value_changed(GtkAdjustment * adjustment, gpointer user_data)
{
  // Gets the current value of the adjustment
  gdouble value = gtk_adjustment_get_value(adjustment);
  // Set rpm setting
  setRpmSetting(value);
}
