#include "includes.h"
#include "plugin.h"

class Help : public Plugin {
    public:
        virtual const void Run() const;

        Help( const string& name, const uint_t& type );
        ~Help();
};

const void Help::Run() const
{
    cout << "Help!" << endl;
    return;
}

Help::Help( const string& name = "Help", const uint_t& type = PLG_TYPE_COMMAND ) : Plugin( name, type )
{
    return;
}

Help::~Help()
{
}

extern "C" {
    Plugin* New() { return new Help(); }
    void Delete( Plugin* p ) { delete p; }
}

