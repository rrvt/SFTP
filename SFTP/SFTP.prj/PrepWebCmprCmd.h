// Prepare Upload for update command


#pragma once

class WebNode;


class PrepWebCmprCmd {
int noFiles;

public:

          PrepWebCmprCmd() { }
         ~PrepWebCmprCmd() { }

  void    start();

  LRESULT finishMsg(WPARAM wparam, LPARAM lParam);

private:

  void    setCurFileDscs(WebNode& node);

  void    setChecks();

  void    display();

  friend UINT prepUploadThrd(void* param);
  };


extern PrepWebCmprCmd prepWebCmprCmd;

