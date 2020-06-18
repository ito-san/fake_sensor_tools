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

  GtkWidget * grd_ubx_mon_hw;     //!< @brief GtkGrid
  GtkWidget * cmb_a_status;       //!< @brief GtkComboBoxText
  GtkWidget * cmb_jamming_state;  //!< @brief GtkComboBoxText

  GtkWidget * grd_ubx_mon_comms;  //!< @brief GtkGrid
  GtkWidget * cmb_port_id;        //!< @brief GtkComboBoxText
  GtkWidget * sw_port_enabled;    //!< @brief GtkSwitch
  GtkAdjustment * adj_tx_usage;   //!< @brief GtkAdjustment

  GtkWidget * grd_ubx_nav_state;    //!< @brief GtkGrid
  GtkWidget * cmb_spoof_det_state;  //!< @brief GtkComboBoxText

} Widgets;

static Widgets * widgets = NULL;  //!< @brief pointers to widgets

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

void initUbxMonHW(GtkBuilder * b, Widgets * w)
{
  // Get the object
  w->grd_ubx_mon_hw = GTK_WIDGET(gtk_builder_get_object(b, "grd_ubx_mon_hw"));
  w->cmb_a_status = GTK_WIDGET(gtk_builder_get_object(b, "cmb_a_status"));
  w->cmb_jamming_state = GTK_WIDGET(gtk_builder_get_object(b, "cmb_jamming_state"));

  // Adds a child to stack
  gtk_stack_add_named(GTK_STACK(w->stk_base), w->grd_ubx_mon_hw, "UBX-MON-HW");
  // Sets one or more child properties for child and container
  gtk_container_child_set(
    GTK_CONTAINER(w->stk_base), w->grd_ubx_mon_hw, "title", "UBX-MON-HW", NULL);

  // Appends text to the list of strings stored in combo_box
  gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(w->cmb_a_status), NULL, "INIT");
  gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(w->cmb_a_status), NULL, "DONTKNOW");
  gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(w->cmb_a_status), NULL, "OK");
  gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(w->cmb_a_status), NULL, "SHORT");
  gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(w->cmb_a_status), NULL, "OPEN");
  // Sets the active item of combo_box
  gtk_combo_box_set_active(GTK_COMBO_BOX(w->cmb_a_status), getAStatus());

  // Appends text to the list of strings stored in combo_box
  gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(w->cmb_jamming_state), NULL, "Unknown");
  gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(w->cmb_jamming_state), NULL, "OK");
  gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(w->cmb_jamming_state), NULL, "Warning");
  gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(w->cmb_jamming_state), NULL, "Critical");
  // Sets the active item of combo_box
  gtk_combo_box_set_active(GTK_COMBO_BOX(w->cmb_jamming_state), getJammingState());
}

void initUbxMonCOMMS(GtkBuilder * b, Widgets * w)
{
  // Get the object
  w->grd_ubx_mon_comms = GTK_WIDGET(gtk_builder_get_object(b, "grd_ubx_mon_comms"));
  w->cmb_port_id = GTK_WIDGET(gtk_builder_get_object(b, "cmb_port_id"));
  w->sw_port_enabled = GTK_WIDGET(gtk_builder_get_object(b, "sw_port_enabled"));
  w->adj_tx_usage = GTK_ADJUSTMENT(gtk_builder_get_object(b, "adj_tx_usage"));

  // Adds a child to stack
  gtk_stack_add_named(GTK_STACK(w->stk_base), w->grd_ubx_mon_comms, "UBX-MON-COMMS");
  // Sets one or more child properties for child and container
  gtk_container_child_set(
    GTK_CONTAINER(w->stk_base), w->grd_ubx_mon_comms, "title", "UBX-MON-COMMS", NULL);

  // Appends text to the list of strings stored in combo_box
  gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(w->cmb_port_id), NULL, "I2C");
  gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(w->cmb_port_id), NULL, "UART1");
  gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(w->cmb_port_id), NULL, "UART2");
  gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(w->cmb_port_id), NULL, "USB");
  gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(w->cmb_port_id), NULL, "SPI");

  // Set port id
  setPortId(PORT_ID_USB);
  // Sets the active item of combo_box
  gtk_combo_box_set_active(GTK_COMBO_BOX(w->cmb_port_id), getPortId());

  // Get port block
  PortBlock portBlock = getPortBlock();
  // Sets the underlying state
  gtk_switch_set_state(GTK_SWITCH(w->sw_port_enabled), portBlock.port_enabled);
  // Sets the GtkAdjustment value
  gtk_adjustment_set_value(w->adj_tx_usage, portBlock.tx_usage);
}

void initUbxNavSTATUS(GtkBuilder * b, Widgets * w)
{
  // Get the object
  w->grd_ubx_nav_state = GTK_WIDGET(gtk_builder_get_object(b, "grd_ubx_nav_state"));
  w->cmb_spoof_det_state = GTK_WIDGET(gtk_builder_get_object(b, "cmb_spoof_det_state"));

  // Adds a child to stack
  gtk_stack_add_named(GTK_STACK(w->stk_base), w->grd_ubx_nav_state, "UBX-NAV_STATUS");
  // Sets one or more child properties for child and container
  gtk_container_child_set(
    GTK_CONTAINER(w->stk_base), w->grd_ubx_nav_state, "title", "UBX-NAV_STATUS", NULL);

  // Appends text to the list of strings stored in combo_box
  gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(w->cmb_spoof_det_state), NULL, "Unknown");
  gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(w->cmb_spoof_det_state), NULL, "No spoofing");
  gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(w->cmb_spoof_det_state), NULL, "Spoofing");
  gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(w->cmb_spoof_det_state), NULL, "Multiple spoofing");
  // Sets the active item of combo_box
  gtk_combo_box_set_active(GTK_COMBO_BOX(w->cmb_spoof_det_state), getSpoofDetState());
}

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
  widgets = g_slice_new(Widgets);

  // Load data from ini file
  loadIniFile();

  // Initialize everything needed to operate the toolkit
  gtk_init(&argc, &argv);
  // Build the GtkBuilder UI definition
  builder = gtk_builder_new_from_file("fake_gnss_simulator.glade");
  // Get the object
  window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));
  widgets->hbr_header = GTK_WIDGET(gtk_builder_get_object(builder, "hbr_header"));
  widgets->sbar_list = GTK_WIDGET(gtk_builder_get_object(builder, "sbar_list"));
  widgets->stk_base = GTK_WIDGET(gtk_builder_get_object(builder, "stk_base"));

  // General
  initGeneral(builder, widgets);
  // UBX-MON-HW
  initUbxMonHW(builder, widgets);
  // UBX-MON-COMMS
  initUbxMonCOMMS(builder, widgets);
  // UBX-NAV_STATUS
  initUbxNavSTATUS(builder, widgets);

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
 * @param [in] user_data user data set when the signal handler was connected
 */
void on_txt_device_name_changed(GtkEditable * editable, gpointer user_data)
{
  // Retrieve the contents of the entry widget
  // and set device name for saving it to ini file
  setDeviceName(gtk_entry_get_text(GTK_ENTRY(editable)));
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
    start();
  } else {
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

// UBX-MON-HW
/**
 * @brief Emitted when the active item is changed
 * @param [in] widget the object on which the signal was emitted
 * @param [in] user_data user data set when the signal handler was connected
 */
void on_cmb_a_status_changed(GtkComboBox * widget, gpointer user_data)
{
  AStatus aStatus = A_STATUS_INIT;
  // Get  the currently active string in combo_box
  gchar * text = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(widget));
  if (!strcmp(text, "INIT"))
    aStatus = A_STATUS_INIT;
  else if (!strcmp(text, "DONTKNOW"))
    aStatus = A_STATUS_DONTKNOW;
  else if (!strcmp(text, "OK"))
    aStatus = A_STATUS_OK;
  else if (!strcmp(text, "SHORT"))
    aStatus = A_STATUS_SHORT;
  else if (!strcmp(text, "OPEN"))
    aStatus = A_STATUS_OPEN;

  setAStatus(aStatus);
}

/**
 * @brief Emitted when the active item is changed
 * @param [in] widget the object on which the signal was emitted
 * @param [in] user_data user data set when the signal handler was connected
 */
void on_cmb_jamming_state_changed(GtkComboBox * widget, gpointer user_data)
{
  JammingState jammingState = JAMMING_STATE_UNKNOWN;
  // Get  the currently active string in combo_box
  gchar * text = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(widget));
  if (!strcmp(text, "Unknown"))
    jammingState = JAMMING_STATE_UNKNOWN;
  else if (!strcmp(text, "OK"))
    jammingState = JAMMING_STATE_OK;
  else if (!strcmp(text, "Warning"))
    jammingState = JAMMING_STATE_WARNING;
  else if (!strcmp(text, "Critical"))
    jammingState = JAMMING_STATE_CRITICAL;

  // Set jammingState
  setJammingState(jammingState);
}

// UBX-MON-COMMS
/**
 * @brief Emitted when the active item is changed
 * @param [in] widget the object on which the signal was emitted
 * @param [in] user_data user data set when the signal handler was connected
 */
void on_cmb_port_id_changed(GtkComboBox * widget, gpointer user_data)
{
  PortId portId = PORT_ID_I2C;

  // Get  the currently active string in combo_box
  gchar * text = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(widget));
  if (!strcmp(text, "I2C"))
    portId = PORT_ID_I2C;
  else if (!strcmp(text, "UART1"))
    portId = PORT_ID_UART1;
  else if (!strcmp(text, "UART2"))
    portId = PORT_ID_UART2;
  else if (!strcmp(text, "USB"))
    portId = PORT_ID_USB;
  else if (!strcmp(text, "SPI"))
    portId = PORT_ID_SPI;

  // Set port id
  setPortId(portId);
  // Get port block
  PortBlock portBlock = getPortBlock();

  // Sets the underlying state
  gtk_switch_set_state(GTK_SWITCH(widgets->sw_port_enabled), portBlock.port_enabled);
  // Sets the GtkAdjustment value
  gtk_adjustment_set_value(widgets->adj_tx_usage, portBlock.tx_usage);
}

/**
 * @brief Emitted when the user changes the switch position
 * @param [in] widget the object on which the signal was emitted
 * @param [in] state the new state of the switch
 * @param [in] user_data user data set when the signal handler was connected
 * @return TRUE to stop the signal emission
 */
gboolean on_sw_port_enabled_state_set(GtkSwitch * widget, gboolean state, gpointer user_data)
{
  // Set port enabled
  setPortEnabled(state);
  return FALSE;
}

/**
 * @brief Emitted when the “value” property has been changed
 * @param [in] adjustment the object which received the signal
 * @param [in] user_data user data set when the signal handler was connected
 */
void on_adj_tx_usage_value_changed(GtkAdjustment * adjustment, gpointer user_data)
{
  // Gets the current value of the adjustment
  gdouble value = gtk_adjustment_get_value(adjustment);
  // Set txUsage
  setTxUsage(value);
}

// UBX-NAV-STATUS
/**
  * @brief Emitted when the active item is changed
  * @param [in] widget the object on which the signal was emitted
  * @param [in] user_data user data set when the signal handler was connected
  */
void on_cmb_spoof_det_state_changed(GtkComboBox * widget, gpointer user_data)
{
  SpoofDetState spoofDetState = SPOOF_DET_STATE_UNKNOWN;
  // Get  the currently active string in combo_box
  gchar * text = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(widget));
  if (!strcmp(text, "Unknown"))
    spoofDetState = SPOOF_DET_STATE_UNKNOWN;
  else if (!strcmp(text, "No spoofing"))
    spoofDetState = SPOOF_DET_STATE_NO_SPOOFING;
  else if (!strcmp(text, "Spoofing"))
    spoofDetState = SPOOF_DET_STATE_SPOOFING;
  else if (!strcmp(text, "Multiple spoofing"))
    spoofDetState = POOF_DET_STATE_MULTIPLE_SPOOFING;

  // Set spoofDetState
  setSpoofDetState(spoofDetState);
}
