#include "nsm_client.h"
#include <cstdlib>
#include <iostream>

NSMClient::NSMClient() {}

NSMClient::~NSMClient()
{
    if (server)
        lo_server_thread_free(server);
}

bool NSMClient::init(const std::string& appName)
{
    const char* url = getenv("NSM_URL");
    if (!url)
    {
        std::cerr << "NSM not available\n";
        return false;
    }

    nsm_url = url;

    server = lo_server_thread_new(nullptr, nullptr);
    lo_server_thread_add_method(server, nullptr, nullptr, osc_handler, this);

    lo_server_thread_start(server);

    std::cout << "NSM client started for " << appName << "\n";
    return true;
}

int NSMClient::osc_handler(const char *path, const char *types,
                           lo_arg **argv, int argc,
                           lo_message msg, void *user_data)
{
    auto* self = static_cast<NSMClient*>(user_data);

    std::string p(path);

    if (p == "/nsm/open")
    {
        const char* sessionPath = &argv[0]->s;
        if (self->cb_open)
            self->cb_open(sessionPath);
    }
    else if (p == "/nsm/save")
    {
        if (self->cb_save)
            self->cb_save();
    }
    else if (p == "/nsm/show")
    {
        if (self->cb_show)
            self->cb_show(true);
    }
    else if (p == "/nsm/hide")
    {
        if (self->cb_show)
            self->cb_show(false);
    }

    return 0;
}

void NSMClient::poll()
{
    if (server)
      lo_server_thread_start(server);
}
