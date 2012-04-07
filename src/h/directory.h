#ifndef DEC_DIRECTORY_H
#define DEC_DIRECTORY_H

class Directory : public Utils {
    public:
        Directory();
        ~Directory();

    // Core
    const void Close();
    bool Open( const string path );

    // Query
    bool iOpen() const { return m_handle; }

    // Manipulate

    private:
        DIR* m_handle;
};

#endif
