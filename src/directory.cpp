#include "h/globals.h"
#include "h/directory.h"

// Core
const void Directory::Close()
{
    return;
}

bool Directory::Open( const string path )
{
    UFLAGS_DE( flags );

    if ( path.empty() )
    {
        LOGSTR( flags, "Directory::Open()-> called with empty path" );
        return false;
    }

    if ( ( m_handle = ::opendir( CSTR( path ) ) ) == NULL )
    {
        LOGFMT( flags, "Directory::Open()->opendir()-> returned errno %d: %s", errno, strerror( errno ) );
        return false;
    }

    return true;
}

// Query

// Manipulate

Directory::Directory()
{
    m_handle = NULL;

    return;
}

Directory::~Directory()
{
    if ( iOpen() )
        ::closedir( m_handle );

    return;
}
