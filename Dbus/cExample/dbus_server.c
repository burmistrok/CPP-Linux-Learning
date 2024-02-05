// dbus_server.c

#include <glib.h>
#include <gio/gio.h>

GDBusNodeInfo *introspection_data = NULL;

static gboolean handle_get_message(GDBusConnection *connection,
                                   const gchar *sender,
                                   const gchar *object_path,
                                   const gchar *interface_name,
                                   const gchar *method_name,
                                   GVariant *parameters,
                                   GDBusMethodInvocation *invocation,
                                   gpointer user_data) {
    g_print("Received method call: %s\n", method_name);

    // You can implement more complex logic here

    // Return a message to the client
    GVariant *result = g_variant_new("(s)", "Hello from D-Bus!");
    g_dbus_method_invocation_return_value(invocation, result);

    return TRUE;
}

static const GDBusInterfaceVTable interface_vtable = {
    .method_call = handle_get_message,
    NULL,
    NULL
};

static void on_bus_acquired(GDBusConnection *connection, const gchar *name, gpointer user_data) {
    guint registration_id;
    GError *error = NULL;

    // Create a new interface for the service
    GDBusInterfaceInfo *interface_info = g_dbus_node_info_lookup_interface(introspection_data, "org.example.ExampleInterface");

    // Register the interface on the object path
    registration_id = g_dbus_connection_register_object(connection,
                                                        "/org/example/ExampleObject",
                                                        interface_info,
                                                        &interface_vtable,
                                                        NULL,  // user_data
                                                        NULL,  // user_data_free_func
                                                        &error);

    if (error != NULL) {
        g_printerr("Error registering D-Bus object: %s\n", error->message);
        g_error_free(error);
    } else {
        g_print("D-Bus service is running...\n");
    }
}

static void on_name_lost(GDBusConnection *connection, const gchar *name, gpointer user_data) {
    g_printerr("Lost D-Bus service name: %s\n", name);
    g_main_loop_quit((GMainLoop *)user_data);
}

int main() {
    GMainLoop *loop;
    guint owner_id;

    // Initialize GLib and create a new D-Bus server
    g_type_init();
    loop = g_main_loop_new(NULL, FALSE);

    // Load D-Bus introspection data
    introspection_data = g_dbus_node_info_new_for_xml(
        "<node>"
        "  <interface name='org.example.ExampleInterface'>"
        "    <method name='GetMessage'>"
        "      <arg type='s' name='message' direction='out'/>"
        "    </method>"
        "  </interface>"
        "</node>",
        NULL
    );

    // Acquire the D-Bus service name
    owner_id = g_bus_own_name(G_BUS_TYPE_SESSION,
                              "org.example.ExampleService",
                              G_BUS_NAME_OWNER_FLAGS_NONE,
                              on_bus_acquired,
                              NULL,  // on_name_acquired
                              on_name_lost,
                              loop,
                              NULL); // user_data

    // Run the main loop
    g_main_loop_run(loop);

    // Cleanup
    g_dbus_node_info_unref(introspection_data);
    g_bus_unown_name(owner_id);
    g_main_loop_unref(loop);

    return 0;
}
