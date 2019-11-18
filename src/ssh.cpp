#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include "libssh2.h"
#include "ssh.h"

SSH::SSH(string ipAddress, unsigned short tcpPort) : ip(ipAddress),
                                                     port(tcpPort),
                                                     sock(0),
                                                     connected(false),
                                                     session(NULL)
{
    libssh2_init(0);
}

SSH::~SSH(void) 
{
    if(session != NULL)
    {
        libssh2_session_disconnect((LIBSSH2_SESSION*)session, "Normal Shutdown");
        libssh2_session_free((LIBSSH2_SESSION*)session);
        session = NULL;
    }
    
    if(sock > 0)
    {
        close(sock);
        sock = 0;
    }

    connected = false;

    libssh2_exit();
}

bool SSH::connectTo(const string& username, const string& password)
{
    int auth_pw = 0;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);
    sin.sin_addr.s_addr = inet_addr(ip.c_str());
    if(connect(sock, (struct sockaddr*)(&sin),sizeof(struct sockaddr_in)) != 0) 
    {
        LOG_MSG("failed to connect sftp server!\n");
        return false;
    }
 
    session = libssh2_session_init();

    if(!session)
        return false;
 
    libssh2_session_set_blocking((LIBSSH2_SESSION*)session, 1);
 
    int rc = libssh2_session_handshake((LIBSSH2_SESSION*)session, sock);
    if(rc) 
    {
        LOG_MSG("Failure establishing SSH session: %d\n", rc);
        return false;
    }
 
    /* check what authentication methods are available */ 
    char *userauthlist = libssh2_userauth_list((LIBSSH2_SESSION*)session,username.c_str(),username.length());

    LOG_MSG("Authentication methods: %s\n", userauthlist);
    if(strstr(userauthlist, "password") != NULL) 
    {
        auth_pw |= 1;
        //0001
    }
    if(strstr(userauthlist, "keyboard-interactive") != NULL) 
    {
        auth_pw |= 2;
        //0010
    }
    if(strstr(userauthlist, "publickey") != NULL) 
    {
        auth_pw |= 4;
        //0100
    }
 
    if(auth_pw & 1) 
    {
        /* We could authenticate via password */ 
        if(libssh2_userauth_password((LIBSSH2_SESSION*)session, username.c_str(), password.c_str())) 
        {
            LOG_MSG("Authentication by password failed.\n");
            return false;
        }
    }
    else if(auth_pw & 2) 
    {
        /* Or via keyboard-interactive */ 
        if(libssh2_userauth_keyboard_interactive((LIBSSH2_SESSION*)session, username.c_str(),NULL)) 
        {
            LOG_MSG("Authentication by keyboard-interactive failed!\n");
            return false;
        }
        else 
        {
            LOG_MSG("Authentication by keyboard-interactive succeeded.\n");
        }
    }
    else if(auth_pw & 4)
    {
        /* Or by public key */ 
        if(libssh2_userauth_publickey_fromfile((LIBSSH2_SESSION*)session,
                                                username.c_str(),
                                                keyFilePub.c_str(),
                                                keyFileRsa.c_str(),
                                                password.c_str()))
        {
            LOG_MSG("Authentication by public key failed!\n");
            return false;
        }
        else 
        {
            LOG_MSG("Authentication by public key succeeded.\n");
        }
    }
    else 
    {
        LOG_MSG("No supported authentication methods found!\n");
        return false;
    }
    connected = true;
    return true;
}

void SSH::disconnect(void)
{
    if(session != NULL)
    {
        libssh2_session_disconnect((LIBSSH2_SESSION*)session, "Normal Shutdown");
        libssh2_session_free((LIBSSH2_SESSION*)session);
        session = NULL;
    }
    
    if(sock > 0)
    {
        close(sock);
        sock = 0;
    }
    connected = false;
}