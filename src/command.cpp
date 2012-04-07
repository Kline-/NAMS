#include "h/globals.h"
#include "h/command.h"

// Core

// Query

// Manipulate

Command::Command()
{
    m_name.clear();
    m_preempt = false;

    return;
}

Command::~Command()
{
    return;
}
