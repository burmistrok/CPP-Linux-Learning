// dbus_server.cpp

#include <glibmm.h>
#include <giomm.h>

class ExampleService : public Glib::Object {
public:
    ExampleService() {
        // Register D-Bus interface and method
        Glib::RefPtr<Gio::DBus::NodeInfo> introspection_data = Gio::DBus::NodeInfo::create_for_xml(
            "<node>"
            "  <interface name='org.example.ExampleInterface'>"
            "    <method name='GetMessage'>"
            "      <arg type='s' name='message' direction='out'/>"
            "    </method>"
            "  </interface>"
            "</node>");

        Gio::DBus::add_interface_vtable(introspection_data->lookup_interface("org.example.ExampleInterface"),
                                        sigc::mem_fun(*this, &ExampleService::on_get_message));

        // Register D-Bus service
        bus = Gio::DBus::get_system()->get_bus();
        owner_id = bus->own_name("org.example.ExampleService",
                                Gio::DBus::NAME_OWNER_FLAGS_NONE,
                                sigc::mem_fun(*this, &ExampleService::on_bus_acquired),
                                sigc::mem_fun(*this, &ExampleService::on_name_lost));
    }

    virtual ~ExampleService() {
        // Release D-Bus service name
        bus->unown_name(owner_id);
    }

private:
    Glib::RefPtr<Gio::DBus::NodeInfo> introspection_data;
    Glib::RefPtr<Gio::DBus::Connection> bus;
    guint owner_id;

    void on_get_message(const Glib::RefPtr<Gio::DBus::Connection>& connection,
                        const Glib::ustring& sender,
                        const Glib::ustring& object_path,
                        const Glib::ustring& interface_name,
                        const Glib::ustring& method_name,
                        const Glib::VariantContainerBase& parameters,
                        const Glib::RefPtr<Gio::DBus::MethodInvocation>& invocation) {
        // You can implement more complex logic here

        // Return a message to the client
        Glib::Variant<Glib::ustring> result("Hello from D-Bus!");
        invocation->return_value(result);
    }

    void on_bus_acquired(const Glib::RefPtr<Gio::DBus::Connection>& connection,
                         const Glib::ustring& name) {
        // Register the object on the bus
        connection->register_object("/org/example/ExampleObject", introspection_data->lookup_interface("org.example.ExampleInterface"));
        std::cout << "D-Bus service is running..." << std::endl;
    }

    void on_name_lost(const Glib::RefPtr<Gio::DBus::Connection>& connection,
                      const Glib::ustring& name) {
        std::cerr << "Lost D-Bus service name: " << name << std::endl;
    }
};

int main() {
    // Initialize GLib
    Glib::init();

    // Create and run the D-Bus service
    ExampleService service;

    // Run the main loop
    Glib::MainLoop loop;
    loop.run();

    return 0;
}
