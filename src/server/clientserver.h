#ifndef CLIENTSERVER_HEADER
#define CLIENTSERVER_HEADER

#define PROTOCOL_ID 0x4f457403

enum ToClientCommand {
    TOCLIENT_BLOCKDATA = 0x20,
    TOCLIENT_ADDNODE,
    TOCLIENT_REMOVENODE,
    TOCLIENT_PLAYERPOS,
    TOCLIENT_RMPLAYER,
    TOCLIENT_LOGIN,
    TOCLIENT_INIT,
    TOCLIENT_INIT_EMPTY
};

enum ToServerCommand {
    TOSERVER_GETBLOCK = 0x20,
    TOSERVER_ADDNODE,
    TOSERVER_REMOVENODE,
    TOSERVER_PLAYERPOS,
    TOSERVER_RMPLAYER,
    TOSERVER_PLAYERPORT,
    TOSERVER_EXIT,
    TOSERVER_INIT,
    TOSERVER_COLLAPSE
};

#endif

