#pragma once

#include <lo/lo.h>
#include <functional>
#include <string>

class NSMClient
{
public:
    using CallbackOpen = std::function<bool(const std::string&)>;
    using CallbackSave = std::function<bool()>;
    using CallbackShow = std::function<void(bool)>;

    NSMClient();
    ~NSMClient();

    bool init(const std::string& appName);

    void setOpenCallback(CallbackOpen cb);
    void setSaveCallback(CallbackSave cb);
    void setShowCallback(CallbackShow cb);

    void poll();

private:
    static int osc_handler(const char *path, const char *types,
                           lo_arg **argv, int argc,
                           lo_message msg, void *user_data);

    lo_server_thread server = nullptr;
    std::string nsm_url;

    CallbackOpen cb_open;
    CallbackSave cb_save;
    CallbackShow cb_show;
};

