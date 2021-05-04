#if PYTHON_SUPPORT
#include "plugin.h"
#include <Python.h>

void init_python_module();
void initpy();

class gil_scoped_acquire
{
    PyGILState_STATE state;
    bool reset;

public:
    gil_scoped_acquire()
    {
        reset = false;
        state = PyGILState_Ensure();
    }

    ~gil_scoped_acquire()
    {
        if (!reset)
            PyGILState_Release(state);
    }
};

ssize_t idaapi on_python_loaded(void*, int notification_code, va_list va)
{
    auto info = va_arg(va, plugin_info_t*);

    if (notification_code == ui_plugin_loaded && !strcmp(info->org_name, "IDAPython"))
    {
        initpy();
        unhook_from_notification_point(HT_UI, on_python_loaded, nullptr);
    }

    return 0;
}

void initpy()
{
    if (!Py_IsInitialized())
        hook_to_notification_point(HT_UI, on_python_loaded, nullptr);
    else
    {
        gil_scoped_acquire gil;
        init_python_module();
    }
}

#endif