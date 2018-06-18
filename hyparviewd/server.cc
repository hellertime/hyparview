// stdlib++
#include <iostream>

// seastar headers
#include <core/app-template.hh>

// smf headers
#include <smf/rpc_server.h>

// smfc generated headers
#include "hyparview_protocol.smf.fb.h"

int main(int argc, char **argv) {
    namespace po = boost::program_options;
    seastar::distributed<smf::rpc_server> rpc;
    seastar::app_template app;

    smf::rpc_server_args server_args;
    server_args.ip       = "0.0.0.0";
    server_args.rpc_port = 6789;

    return app.run(argc, argv, [&] () -> seastar::future<int> {
        seastar::engine().at_exit([&] { return rpc.stop(); });

        return rpc.start(server_args)
            .then([&rpc] {
                return rpc.invoke_on_all(&smf::rpc_server::start);
            })
            .then([] {
                return seastar::make_ready_future<int>(0);
            });
    });
}
