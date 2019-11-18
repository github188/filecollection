#ifndef _SSH_H_
#define _SSH_H_

#include <string>
#include "logprocess.h"

using namespace std;

class SSH 
{
   private:
       string ip;
       unsigned short port;
       int sock;
       string keyFileRsa;
       string keyFilePub;

       SSH(const SSH&);
       SSH& operator=(const SSH&);
   protected:
       bool connected;
       void* session;

       bool connectTo(const string&, const string&);
       void disconnect(void);
   public:
       SSH(string, unsigned short);
       virtual ~SSH(void);

       virtual bool openSession(const string&, const string&) = 0;
       virtual void closeSession(void) = 0;
};

#endif // _SSH_H_