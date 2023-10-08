// Prepare Upload for update command


#include "pch.h"
#include "PrepWebCmprCmd.h"
#include "NotePad.h"
#include "SFTP.h"
#include "SFTPDoc.h"
#include "SiteFileDscs.h"
#include "WebFiles.h"
#include "WorkerThrd.h"


static UINT prepUploadThrd(void* param);


PrepWebCmprCmd prepWebCmprCmd;


void PrepWebCmprCmd::start() {

  if (workerThrd.isLocked()) return;

  notePad.clear();   noFiles = 0;

  if (!siteID.login()) return;

  mainFrm()->startPrgBar(curFileDscs.nData());

  workerThrd.start(prepUploadThrd, (void*) &webFiles.root(), ID_PrepUpLdMsg);
  }


UINT prepUploadThrd(void* param) {

  sendWdwScroll();

  if (!webFiles.load(siteID.remoteRoot)) return false;

  prepWebCmprCmd.setChecks();

  prepWebCmprCmd.setCurFileDscs(*(WebNode*) param);

  return 0;
  }


void PrepWebCmprCmd::setChecks() {
FileDscsIter iter(curFileDscs);
SiteFileDsc* dsc;

  for (dsc = iter(); dsc; dsc = iter++) if (!dsc->check) {dsc->check = true; dsc->status = OthSts;}
  }


void PrepWebCmprCmd::setCurFileDscs(WebNode& node) {
String       path = node.path;
WebNodeIter  iter(node);
WebItem*     item;
SiteFileDsc* dsc;

  for (item = iter(); item; item = iter++) if (item->typ == WebFileType) {

    String s = path + item->name;    siteID.webToLocal(s);

    dsc = curFileDscs.find(s);

    if (!dsc)
       {dsc = curFileDscs.addFile(s);   if (dsc) {dsc->check = true;   dsc->status = GetSts;}   continue;}

    if (dsc->status == OthSts) {dsc->check = false;   dsc->status = NilSts;}
    }

  for (item = iter(); item; item = iter++) if (item->typ == WebDirType) setCurFileDscs(*item->node);
  }


LRESULT PrepWebCmprCmd::finishMsg(WPARAM wparam, LPARAM lParam) {
String path;

  mainFrm()->closePrgBar();

  display();

  if      (noFiles <= 0) notePad << _T("No Files");
  else if (noFiles == 1) notePad << noFiles << _T(" file");
  else                   notePad << noFiles << _T(" files");

  notePad << _T(" marked for up/down load because they are absent on the web or in the local files");
  notePad << nCrlf;

  sendDisplayMsg();   Sleep(10);   sendWdwScroll(false);   return 0;
  }


void PrepWebCmprCmd::display() {
FileDscsIter iter(curFileDscs);
SiteFileDsc* dsc;

  notePad << nClrTabs << nSetTab(15);

  for (dsc = iter(); dsc; dsc = iter++) if (dsc->check) {

    switch (dsc->status) {
      case OthSts : dsc->status = PutSts;
      case PutSts : notePad << _T("Upload:");    break;
      case GetSts : notePad << _T("Download:");  break;
      case DelSts : notePad << _T("Delete:");    break;
      default     : notePad << _T("Unknown:");   break;
      }
    notePad << nTab << dsc->path << nCrlf;   noFiles++;   sendDisplayMsg();
    }
  }

