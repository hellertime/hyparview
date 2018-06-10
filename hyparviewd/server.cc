// stdlib++
#include <iostream>

// seastar headers
#include <core/app-template.hh>

// smf headers
#include <smf/rpc_server.h>

// smfc generated headers
#include "hyparview_message.smf.fb.h"
#include "hyparview_protocol.smf.fb.h"

int main(int argc, char **argv) {
    namespace po = boost::program_options;

    seastar::app_template app;
    app.add_options()
        ("ip", po::value<std::string>()->default_value("127.0.0.1"), "hypaviewd server ip address")
        ("port", po::value<uint16_t>()->default_value(12345), "hyparviewd server port")
        ;

    return app.run_deprecated(argc, argv, [&] {
        auto& config = app.configuration();
        return seastar::make_ready_future<>();
    });
}
