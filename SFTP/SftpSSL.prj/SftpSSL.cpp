// SSH FTP Interface


#include "pch.h"
#include "SftpSSL.h"
#include "SftpLog.h"
#include "SftpOps.h"


SftpSSL sftpSSL;


SftpSSL::SftpSSL() : cmd(transport, err), transport(err) {
int wsaErr = WSAStartup(MAKEWORD(2, 2), &wsa);                   // Required in Windows

  if (wsaErr) {err.skt(_T("WSAStartup() failed"), wsaErr);}
  }


bool SftpSSL::open(TCchar* host) {sftpLog(_T("wsa"), wsa.wVersion); return cmd.open(host);}


SftpStore& SftpSSL::fileData() {return transport.sftpOps;}

