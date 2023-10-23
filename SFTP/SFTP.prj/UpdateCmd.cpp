// Update Local or Remote Site


#include "pch.h"
#include "UpdateCmd.h"
#include "NotePad.h"
#include "SFTP.h"
#include "SFTPDoc.h"
#include "SiteFileDscs.h"
#include "SiteID.h"
#include "WebFiles.h"
#include "WorkerThrd.h"


static UINT updateThrd(void* param);


UpdateCmd updateCmd;


void UpdateCmd::start() {
FileDscsIter iter(curFileDscs);
SiteFileDsc* dsc;
int          n;

  if (isLocked()) return;

  notePad.clear();   noFiles = 0;

  if (!siteID.login()) return;

  notePad << _T("Update Files") << nCrlf << nCrlf;

  for (dsc = iter(), n = 0; dsc; dsc = iter++) if (dsc->check) n++;

  mainFrm()->startPrgBar(n);

  workerThrd.start(updateThrd, (void*) &curFileDscs, ID_UpdateMsg);
  }


bool UpdateCmd::isLocked(bool prevent) {return doc()->isLocked(prevent);}


UINT updateThrd(void* param) {

  sendWdwScroll();

  if (curFileDscs.isEmpty()) return false;

  return updateCmd.doXfer(*(SiteFileDscs*) param) ? 0 : 1;
  }


bool UpdateCmd::doXfer(SiteFileDscs& dscs) {
FileDscsIter iter(dscs);
SiteFileDsc* dsc;
String       lclPath;
String       webPath;
String       rslt;

  for (dsc = iter(); dsc; dsc = iter++) {

    if (!dsc->check) continue;

    switch (dsc->status) {
      case WebPutSts:
      case DifPutSts: lclPath = siteID.localRoot  + dsc->path;

                      if (!sftpSSL.getLocalFile(lclPath)) continue;

                      webPath = siteID.remoteRoot + dsc->path;

                      if (!sftpSSL.stor(toRemotePath(webPath))) continue;

                      dsc->updated = true;   webFiles.modified();   break;

      case GetSts   : webPath = siteID.remoteRoot + dsc->path;

                      if (!sftpSSL.retr(toRemotePath(webPath))) continue;

                      lclPath = siteID.localRoot + dsc->path;

                      if (!doc()->saveData(StoreSrc, toLocalPath(lclPath))) continue;

                      dsc->updated = true;   dsc->addLclAttr(lclPath);

                      sftpSSL.noop(rslt);    break;

      case DelSts   : webPath = siteID.remoteRoot + dsc->path;

                      if (!sftpSSL.del(toRemotePath(webPath)) && sftpSSL.list(toRemotePath(webPath)))
                                                                                                continue;
                      dsc->updated = true;   webFiles.modified();    break;

      default       : continue;
      }

    sendStepPrgBar();   noFiles++;   continue;
    }

  return true;
  }


LRESULT UpdateCmd::finishMsg(WPARAM wparam, LPARAM lParam) {
String path;

  mainFrm()->closePrgBar();

  display();

  if (noFiles) doc()->saveData(CurSiteSrc, siteID.dbPath(path));

  sendDisplayMsg();   Sleep(1);   sendWdwScroll(false);   return 0;
  }


void UpdateCmd::display() {
FileDscsIter iter(curFileDscs);
SiteFileDsc* dsc;
int          n;

  notePad << nClrTabs << nSetTab(15);

  for (dsc = iter(), n = 0; dsc; dsc = iter++) if (dsc->updated) {

    switch (dsc->status) {
      case WebPutSts:
      case DifPutSts: notePad << _T(" Uploaded:");    break;
      case GetSts   : notePad << _T(" Downloaded:");  break;
      case DelSts   : notePad << _T(" Deleteed:");    break;
      default       : notePad << _T(" Unknown Op:");  break;
      }

    notePad << nTab << dsc->path<< nCrlf;    n++;
    }

  if      (n <= 0) notePad << _T("No Files");
  else if (n == 1) notePad << n << _T(" file");
  else                   notePad << n << _T(" files");

  notePad << _T(" updated") << nCrlf;

  sendDisplayMsg();
  }

