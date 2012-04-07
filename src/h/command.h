#ifndef DEC_COMMAND_H
#define DEC_COMMAND_H

class Command {
    public:
        Command();
        ~Command();

    bool LoadCommand( const string file );

    private:
        string m_name;
        bool m_preempt;
};

#endif
