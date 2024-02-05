// dbus_client.cpp

#include <glibmm.h>
#include <giomm.h>

int main() {
    // Initialize GLib
    Glib::init();

    // Create a D-Bus proxy
    Glib::RefPtr<Gio::DBus::Connection> bus = Gio::DBus::get_system()->get_bus();
    Glib::RefPtr<Gio::DBus::Proxy> proxy = Gio::DBus::Proxy::create_for_bus_sync(
        Gio::DBus::BUS_TYPE_SYSTEM,
        Gio::DBus::Proxy::FLAGS_NONE,
        nullptr,  // Glib::RefPtr<Gio::DBus::InterfaceInfo>
        "org.example.ExampleService",
        "/org/example/ExampleObject",
        "org.example.ExampleInterface");

    // Call the D-Bus method
    Glib::RefPtr<Gio::DBus::MethodInvocation> invocation = proxy->call_method("GetMessage", Glib::Variant<Glib::ustring>());

    // Wait for the result
    Glib::Variant<Glib::ustring> result;
    invocation->wait_for_reply(result);

    // Print the received message
    std::cout << "Received message from D-Bus service: " << result.get() << std::endl;

    return 0;
}
