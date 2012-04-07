#ifndef DEC_COMMAND_H
#define DEC_COMMAND_H

class Command : public Utils {
    public:
        Command();
        ~Command();

    // Core
    bool LoadCommand( const string file );

    // Query

    // Manipulate

    private:
        string m_name;
        bool m_preempt;
};

#endif
