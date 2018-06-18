// stdlib++
#include <iostream>

// linux
#include <arpa/inet.h>

// seastar headers
#include <core/app-template.hh>
#include <core/distributed.hh>
#include <core/sleep.hh>

// smf headers
#include <smf/rpc_generated.h>
#include <smf/rpc_recv_context.h>
#include <smf/rpc_server.h>

// smfc generated headers
#include "hyparview_message_generated.h"
#include "hyparview_protocol.smf.fb.h"

namespace hpvp = hyparview::protocol;
namespace hpvm = hyparview::message;

using join_msg_t = smf::rpc_typed_envelope<hpvm::Join>;
using ack_msg_t = smf::rpc_typed_envelope<hpvm::Ack>;

class hyparview_service : public hpvp::HyParView {
    virtual seastar::future<ack_msg_t> Join(smf::rpc_recv_typed_context<hpvm::Join> &&join_msg) final {
        ack_msg_t ack_msg;

        if (join_msg) {
            DLOG_DEBUG("Join({})", join_msg->node()->name()->str());
        }
        ack_msg.envelope.set_status(200);
        return seastar::make_ready_future<ack_msg_t>(std::move(ack_msg));
    }
};

int main(int argc, char **argv) {
    SET_LOG_LEVEL(seastar::log_level::trace);
    std::cout.setf(std::ios::unitbuf);
    DLOG_DEBUG("Starting up HyParView...");
    namespace po = boost::program_options;
    seastar::distributed<smf::rpc_server> rpc;
    seastar::app_template app;

    smf::rpc_server_args server_args;
    server_args.ip       = "0.0.0.0";
    server_args.rpc_port = 6789;
    server_args.flags |= smf::rpc_server_flags::rpc_server_flags_disable_http_server;

    // El Cheapo Client
    uint32_t ip;
    inet_pton(AF_INET, "127.0.0.1", &ip);
    seastar::shared_ptr<hpvp::HyParViewClient> client = 
        hpvp::HyParViewClient::make_shared(seastar::make_ipv4_address(ntohl(ip), 6789));
 
    return app.run(argc, argv, [&] () -> seastar::future<int> {
        seastar::engine().at_exit([&] { return rpc.stop(); });

        return rpc.start(server_args)
            .then([&rpc] {
                return rpc.invoke_on_all(
                    &smf::rpc_server::register_service<hyparview_service>);
            })
            .then([&rpc] {
                return rpc.invoke_on_all(&smf::rpc_server::start);
            })
            .then([&client] {
                // TODO: make this much better
                using namespace std::chrono_literals;
                return seastar::sleep(5s)
                    .then([&client] {
                        DLOG_DEBUG("Starting up client {}", client->server_addr);
                        return client->connect()
                            .then([&client] {
                                join_msg_t join_msg;
                                join_msg.data->node->name = seastar::sstring("IDENTITY");
                                return client->Join(join_msg.serialize_data());
                            })
                            .then([&client] (auto ack_msg) {
                                DLOG_DEBUG("Join->Ack()");
                                return seastar::now();
                            })
                            .then([&client] {
                                return client->stop();      
                            });
                    });
            })
            .then([] {
                return seastar::make_ready_future<int>(0);
            });
    });
}
