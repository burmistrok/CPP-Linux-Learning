// dbus_client.c

#include <glib.h>
#include <gio/gio.h>

static void on_method_return(GDBusProxy *proxy, GAsyncResult *res, gpointer user_data) {
    GVariant *result;
    GError *error = NULL;

    // Get the result of the method call
    result = g_dbus_proxy_call_finish(proxy, res, &error);

    if (error != NULL) {
        g_printerr("Error calling D-Bus method: %s\n", error->message);
        g_error_free(error);
    } else {
        const gchar *message;

        // Extract the message from the result
        g_variant_get(result, "(s)", &message);
        g_print("Received message from D-Bus service: %s\n", message);

        // Cleanup
        g_variant_unref(result);
    }

    // Quit the main loop
    g_main_loop_quit((GMainLoop *)user_data);
}

int main() {
    GMainLoop *loop;
    GError *error = NULL;

    // Initialize GLib
    g_type_init();
    loop = g_main_loop_new(NULL, FALSE);

    // Create a new D-Bus proxy
    GDBusProxy *proxy = g_dbus_proxy_new_for_bus_sync(
        G_BUS_TYPE_SESSION,
        G_DBUS_PROXY_FLAGS_NONE,
        NULL,  // GDBusInterfaceInfo
        "org.example.ExampleService",
        "/org/example/ExampleObject",
        "org.example.ExampleInterface",
        NULL,  // GCancellable
        &error
    );

    if (error != NULL) {
        g_printerr("Error creating D-Bus proxy: %s\n", error->message);
        g_error_free(error);
        return 1;
    }

    // Call the D-Bus method
    g_dbus_proxy_call(proxy,
                      "GetMessage",
                      NULL,  // GVariant (parameters)
                      G_DBUS_CALL_FLAGS_NONE,
                      -1,  // timeout (default)
                      NULL,  // GCancellable
                      on_method_return,
                      loop);

    // Run the main loop
    g_main_loop_run(loop);

    // Cleanup
    g_object_unref(proxy);
    g_main_loop_unref(loop);

    return 0;
}
