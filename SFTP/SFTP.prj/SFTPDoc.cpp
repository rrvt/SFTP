 // SFTPDoc.cpp : implementation of the SFTPDoc class


#include "pch.h"
#include "SFTPDoc.h"
#include "ClipLine.h"
#include "filename.h"
#include "IniFile.h"
#include "MessageBox.h"
#include "NotePad.h"
#include "Printer.h"
#include "Resource.h"
#include "SFTP.h"
#include "SftpLog.h"
#include "SFTPView.h"
#include "SiteFileDscs.h"
#include "UpdateDlg.h"
#include "WebFiles.h"
#include "workerThrd.h"


static TCchar* LocalWebPath = _T("LocalWebPath");

static UINT fromWebThrd(LPVOID param);


// SFTPDoc

IMPLEMENT_DYNCREATE(SFTPDoc, CDoc)

BEGIN_MESSAGE_MAP(SFTPDoc, CDoc)
  ON_COMMAND(ID_NewSite,          &onNewSite)
  ON_COMMAND(ID_PickSite,         &onPickSite)
  ON_COMMAND(ID_EditSite,         &onEditSite)
  ON_COMMAND(ID_WebCmpr,          &onPrepWebCmprCmd)
  ON_COMMAND(ID_PrevCmpr,         &onPrepPrevCmpr)

  ON_COMMAND(ID_UpdateSite,       &onUpdateSite)

  ON_COMMAND(ID_RemoteDir,        &onRemoteDir)
  ON_COMMAND(ID_DeleteSite,       &onDelSite)

  ON_COMMAND(ID_File_Save,        &OnFileSave)
  ON_COMMAND(ID_EDIT_COPY,        &onEditCopy)

//  ON_COMMAND(ID_CurrentTest,      &onCurrentTest)

//  ON_COMMAND(ID_SftpCmds,         &onSftpAcct)
  ON_COMMAND(ID_SftpAcct,         &onSftpAcct)
  ON_COMMAND(ID_SftpADAT,         &onSftpADAT)
  ON_COMMAND(ID_SftpALLO,         &onSftpALLO)
  ON_COMMAND(ID_SftpAPPE,         &onSftpAPPE)
  ON_COMMAND(ID_SftpAUTH,         &onSftpAUTH)
  ON_COMMAND(ID_SftpAVBL,         &onSftpAVBL)
  ON_COMMAND(ID_SftpCCC,          &onSftpCCC)
  ON_COMMAND(ID_SftpCDUP,         &onSftpCDUP)
  ON_COMMAND(ID_SftpCONF,         &onSftpCONF)
  ON_COMMAND(ID_SftpCSID,         &onSftpCSID)
  ON_COMMAND(ID_SftpCWD,          &onSftpCWD)
  ON_COMMAND(ID_SftpDELE,         &onSftpDELE)
  ON_COMMAND(ID_SftpDSIZ,         &onSftpDSIZ)
  ON_COMMAND(ID_SftpENC,          &onSftpENC)
  ON_COMMAND(ID_SftpFEAT,         &onSftpFEAT)
  ON_COMMAND(ID_SftpHELP,         &onSftpHELP)
  ON_COMMAND(ID_SftpLIST,         &onSftpLIST)
  ON_COMMAND(ID_SftpMDTM,         &onSftpMDTM)
  ON_COMMAND(ID_SftpMFCT,         &onSftpMFCT)
  ON_COMMAND(ID_SftpMIC,          &onSftpMIC)
  ON_COMMAND(ID_SftpMKD,          &onSftpMKD)
  ON_COMMAND(ID_SftpMLSD,         &onSftpMLSD)
  ON_COMMAND(ID_SftpMLST,         &onSftpMLST)
  ON_COMMAND(ID_SftpMODE,         &onSftpMODE)
  ON_COMMAND(ID_SftpNLST,         &onSftpNLST)
  ON_COMMAND(ID_SftpNOOP,         &onSftpNOOP)
  ON_COMMAND(ID_SftpOPTS,         &onSftpOPTS)
  ON_COMMAND(ID_SftpPWD,          &onSftpPWD)
  ON_COMMAND(ID_SftpQUIT,         &onSftpQUIT)
  ON_COMMAND(ID_SftpRETR,         &onSftpRETR)
  ON_COMMAND(ID_SftpRMD,          &onSftpRMD)
  ON_COMMAND(ID_SftpRMDA,         &onSftpRMDA)
  ON_COMMAND(ID_SftpRNFR,         &onSftpRNFR)
  ON_COMMAND(ID_SftpRNTO,         &onSftpRNTO)
  ON_COMMAND(ID_SftpSITE,         &onSftpSITE)
  ON_COMMAND(ID_SftpSIZE,         &onSftpSIZE)
  ON_COMMAND(ID_SftpSMNT,         &onSftpSMNT)
  ON_COMMAND(ID_SftpSPSV,         &onSftpSPSV)
  ON_COMMAND(ID_SftpSTAT,         &onSftpSTAT)
  ON_COMMAND(ID_SftpSTOR,         &onSftpSTOR)
  ON_COMMAND(ID_SftpSTOU,         &onSftpSTOU)
  ON_COMMAND(ID_SftpSYST,         &onSftpSYST)
  ON_COMMAND(ID_SftpTHMB,         &onSftpTHMB)
  ON_COMMAND(ID_SftpTYPE,         &onSftpTYPE)
  ON_COMMAND(ID_SftpUSER,         &onSftpUSER)
  ON_COMMAND(ID_SftpXCUP,         &onSftpXCUP)
  ON_COMMAND(ID_SftpXMKD,         &onSftpXMKD)
  ON_COMMAND(ID_SftpXPWD,         &onSftpXPWD)
  ON_COMMAND(ID_SftpXRMD,         &onSftpXRMD)
  ON_COMMAND(ID_SftpLogout,       &onSftpLogout)

  ON_COMMAND(ID_SftpNotImpl,      &onSftpNotImpl)

END_MESSAGE_MAP()


// SFTPDoc construction/destruction                                   , cmdLock(false)

SFTPDoc::SFTPDoc() noexcept : dataSource(NotePadSrc), savePrv(false) { }

SFTPDoc::~SFTPDoc() {logoutSite();}


void SFTPDoc::defaultSite() {
String sect;

  if (!iniFile.read(GlobalSect, LastSiteKey, sect)) {display();  return;}

  if (!loadSiteID(sect))                            {display();  return;}

  if (!loginSite())                {clearSiteID();   display();   return;}

  if (!loadSiteDescriptors())      {clearSiteID();   display();   return;}

  display();
  }


// Create a new site

void SFTPDoc::onNewSite() {                                   // XXX

  if (workerThrd.isLocked()) return;

  notePad.clear();    clearSiteID();

  iniFile.readString(GlobalSect, LocalWebPath, path);

  getDirPathDlg(_T("Web Site"), path);   fixLocalPath(path);   //lclSite.rootPath = path;

  String pth = ::getPath(path);

  iniFile.writeString(GlobalSect, LocalWebPath, pth);   siteID.localRoot = pth;

  if (!doEditSite()) return;

  loadSiteDescriptors();   display();
  }


void SFTPDoc::onPickSite() {                                  // XXX

  if (workerThrd.isLocked()) return;

  notePad.clear();   pickSite();

  logoutSite();    if (!loginSite()) return;

  loadSiteDescriptors();   display();
  }


void SFTPDoc::onEditSite() {                                  // XXX

  if (workerThrd.isLocked()) return;

  notePad.clear();

  if (!doEditSite()) return;

  loadSiteDescriptors();   display();
  }


void SFTPDoc::onDelSite() {
String msg;

  if (workerThrd.isLocked() || siteID.name.isEmpty()) return;

  msg = _T("Delete Site: ") + siteID.name;

  switch (AfxMessageBox(msg, MB_YESNO)) {

    case IDYES: break;  // The Yes button was selected.
    default   : return;
    }

  notePad << _T("Deleting ") << siteID.name << nCrlf;

  prvFileDscs.clear();   curFileDscs.clear();

  ToAnsi ansi(siteID.dbPath(path));   remove(ansi());    siteID.delSiteData();

  display();
  }


bool SFTPDoc::loadSiteDescriptors() {

  if (!loadPrevSiteData()) prvFileDscs.loadFromPC();

  if (!curFileDscs.loadFromPC())
                            {notePad << siteID.localRoot << _T(" does not exist") << nCrlf; return false;}
  return true;
  }


bool SFTPDoc::doEditSite() {editSite();   logoutSite();    return loginSite();}     // XXX


void SFTPDoc::onPrepPrevCmpr() {
FileDscsIter  iter(curFileDscs);
SiteFileDsc*  dsc;
int           noFiles;

  if (workerThrd.isLocked()) return;

  notePad << nClrTabs << nSetTab(15);

  noFiles = findDeletedFiles();

  for (dsc = iter(); dsc; dsc = iter++) {

    if (dsc->check) continue;

    SiteFileDsc* prv = prvFileDscs.find(dsc->path);

    if (prv && prv->size == dsc->size && prv->date == dsc->date) continue;

    dsc->check = true; dsc->status = PutSts;   dsc->updated = false;

    notePad << _T("Upload:") << nTab << dsc->path << nCrlf;   noFiles++;
    }

  if      (noFiles <= 0) notePad << _T("No Files");
  else if (noFiles == 1) notePad << noFiles << _T(" file");
  else                   notePad << noFiles << _T(" files");

  notePad << _T(" marked for up load or deletion due to a change in the local files") << nCrlf;

  display();
  }


int SFTPDoc::findDeletedFiles() {
FileDscsIter  iter(prvFileDscs);
SiteFileDsc*  dsc;
int           noFiles;

  for (dsc = iter(), noFiles = 0; dsc; dsc = iter++) {

    if (dsc->check) continue;

    SiteFileDsc* cur = curFileDscs.find(dsc->path);   if (cur) continue;

    dsc->check = true;   dsc->status = DelSts;   dsc->updated = false;
    curFileDscs.addFile(*dsc);

    notePad << _T("Delete:") << nTab << dsc->path << nCrlf;   noFiles++;
    }

  return noFiles;
  }


void SFTPDoc::onUpdateSite() {
SiteUpdateDlg dlg(curFileDscs);

  if (workerThrd.isLocked()) return;

  if (dlg.DoModal() == IDOK) updateCmd.start();
  }

#if 0
void SFTPDoc::onRemoteDir() {                                 //XXXX

  if (workerThrd.isLocked()) return;

  notePad.clear();

  sftpSSL.list(_T("/"));

  display();
  }
#endif



void SFTPDoc::saveSite(DataSource src) {
String path;

  dataSource = src;   onSaveDocument(dbPath(path));   dataSource = NotePadSrc;
  }


void SFTPDoc::onEditCopy() {                                  // XXXX

  if (workerThrd.isLocked()) return;

  clipLine.load();
  }


void SFTPDoc::display(DataSource ds) {dataSource = ds; invalidate();}


void SFTPDoc::OnFileSave() {                                  // XXXX

  if (workerThrd.isLocked()) return;

  switch (dataSource) {
    case NotePadSrc : if (setSaveAsPath(pathDlgDsc)) OnSaveDocument(path);   break;
    case StoreSrc   : dataSource = StrTxtSrc;  saveFile(_T("Store"), _T("Str"), _T("txt")); break;
    }

  invalidate();
  }


void SFTPDoc::saveFile(TCchar* title, TCchar* suffix, TCchar* fileType) {
String fileName = path;
int    pos      = fileName.find_last_of(_T('\\'));
String ext      = _T("*."); ext += fileType;
String ttl      = title;    ttl += _T(" Output");

  fileName = fileName.substr(pos+1);   pos = fileName.find_first_of(_T('.'));
  fileName = fileName.substr(0, pos);  fileName += suffix;

  pathDlgDsc(ttl, fileName, fileType, ext);

  if (setSaveAsPath(pathDlgDsc)) OnSaveDocument(path);
  }


// Override the version in CDoc to force creation of file

BOOL SFTPDoc::OnSaveDocument(LPCTSTR lpszPathName) {
String  path = lpszPathName;
Archive ar(path, FileIO::Write | FileIO::Create);   if (!ar.isOpen()) return false;

  serialize(ar);   return true;
  }


// The load and save functions do the necessary things to open files and then call the serialize
// function which switches to the correct operation and function to do the work.  This was the MFC
// way of doing business and I just copied with my own flavor of opening files, etc...

bool SFTPDoc::loadPrevSiteData() {                            // XXX
String path;

  dataSource = PrvSiteSrc;

  return OnOpenDocument(dbPath(path)) && !prvFileDscs.isEmpty();
  }


// UglyDoc serialization

void SFTPDoc::serialize(Archive& ar) {

  if (ar.isStoring())
    switch(dataSource) {
      case NotePadSrc : notePad.archive(ar);      return;
      case CurSiteSrc : curFileDscs.saveCSV(ar);  prvFileDscs = curFileDscs;   curFileDscs.clrSts();
                        return;
      case StoreSrc   : sftpSSL.store(ar);        return;
      default         : return;
      }

  else
    switch(dataSource) {
      case PrvSiteSrc : prvFileDscs.loadFromCSV(ar);   return;
      default         : return;
      }
  }


// SFTPDoc diagnostics

#ifdef _DEBUG
void SFTPDoc::AssertValid() const {         CDocument::AssertValid();}
void SFTPDoc::Dump(CDumpContext& dc) const {CDocument::Dump(dc);}
#endif //_DEBUG





#if 0
//static UINT startDownLoad(LPVOID param);
void SFTPDoc::onDownLoadSite() {

  if (workerThrd.isLocked()) return;

  notePad.clear();

  if (!loginSite()) return;

  webFiles.load(siteID.remoteRoot);

#if 1

  workerThrd.start(startDownLoad, (void*) &webFiles.root(), ID_DwnLdMsg);

#else
  downLoad(webFiles.root());
#endif

  display();
  }


UINT startDownLoad(LPVOID param) {return doc()->downLoad(*(WebNode*) param);}


bool SFTPDoc::downLoad(WebNode& node) {
String      path = node.path;
WebNodeIter iter(node);
WebItem*    item;
int         n;
double      dbl;

  notePad << nSetRTab(65);

  for (item = iter(); item; item = iter++) {
    if (item->typ == WebFileType) {

      String s = path + item->name;

      sftpSSL.get(s);

      dataSource = StoreSrc;   s = siteID.localRoot + s;   OnSaveDocument(toLocalPath(s));

      dbl = n = sftpSSL.nBytes();

      notePad << s << nTab;
      if      (dbl < 1024)                      notePad << n << _T(" bytes");
      else if (dbl < 1048576) {dbl /= 1024;     notePad << nSetPrec(3) << dbl << _T(" KB");}
      else                    {dbl /= 1048576;  notePad << nSetPrec(3) << dbl << _T(" MB");}
      notePad << nCrlf;

      sendMsg(ID_DisplayMsg, 0, 0);
      }
    }

  for (item = iter(); item; item = iter++) if (item->typ == WebDirType) downLoad(*item->node);

  display(); return true;
  }
#endif





#if 0
// Called from thread end or function loadSite to complete loading the site

void SFTPDoc::finLoadSite() {

  siteID.noWebFiles = prvFileDscs.nData();

  prvFileDscs.setRoot(siteID.localRoot);     // Transform list into local file list to facilitate comparisons

  updatePrevSite();

  prvFileDscs.display(_T("Previous"));
  curFileDscs.display(_T("Current"));

  display(NotePadSrc);
  }

void SFTPDoc::onGetSite() {

  if (workerThrd.isLocked()) return;

  notePad.clear();

  pickSite();

  if (!loginSite()) {notePad << _T("Not logged in") << nCrlf; display(NotePadSrc); return;}

  curFileDscs.clear();
  curFileDscs.setRoot(siteID.localRoot);
  curFileDscs.fromPC(siteID.localRoot);

//cmdLock = true;   mainFrm()->openProgress(siteID.noWebFiles);

#if 1
  workerThrd.start(fromWebThrd, (void*) siteID.remoteRoot.str(), ID_GetThrdMsg);
#else
  startWkrThrd(fromWebThrd, ID_GetThrdMsg, siteID.noWebFiles);
#endif
  }

UINT fromWebThrd(LPVOID param) {
#if 1
String root = (TCchar*) param;

  prvFileDscs.startFromWeb(root);   return 0;

#else
HWND hWnd  = mainFrm()->m_hWnd;
uint msgID = (uint) param;
uint rslt;

  try {prvFileDscs.startFromWeb(root, hWnd, msgID);   rslt = TE_Normal;}
  catch (...) {                                              rslt = TE_Exception;}

  ::PostMessage(hWnd, msgID, ID_EndThread, rslt);
#endif
  }


void SFTPDoc::finGetSite(LPARAM lParam) {
FileDscsIter iter(prvFileDscs);
SiteFileDsc* pf;
SiteFileDsc* uf;
String    root = siteID.localRoot;

  if (!finWkrThrd(lParam)) return;

  siteID.noWebFiles = prvFileDscs.nData();    siteID.saveSiteData();

  for (pf = iter(); pf; pf = iter++) {

    uf = updateFileDscs.data = *pf;   uf->status = GetSts;

    String path = root + uf->path;

    uf->addLclAttr(path);
    }

  updateFileDscs.setRoot(siteID.localRoot);    saveSite(UpdSiteSrc);

  dspUpdates();
  }




void SFTPDoc::finLoadSite(LPARAM lParam) {

  if (!finWkrThrd(lParam)) return;

  siteID.noWebFiles = prvFileDscs.nData();

  prvFileDscs.setRoot(siteID.localRoot);     // Transform list into local file list to facilitate comparisons

  if (savePrv) saveSite(PrvSiteSrc);   savePrv = false;

  prvFileDscs.display(_T("Previous"));
  curFileDscs.display(_T("Current"));

  display(NotePadSrc);
  }
#endif
#if 0
LRESULT SFTPDoc::onGetThrdMsg( WPARAM wParam, LPARAM lParam) {
  return 0;
  }

LRESULT SFTPDoc::onPickThrdMsg(WPARAM wParam, LPARAM lParam) {
  return 0;
  }
#endif



#if 0
// Lock the commands and start the progress bar and the thread

void SFTPDoc::startWkrThrd(AFX_THREADPROC thdProc, uint arg, int n)
         {cmdLock = true;   /*mainFrm()->openProgress(n);*/   AfxBeginThread(thdProc, LPVOID(arg), 0, 0);}
#endif
#if 0

void SFTPDoc::finConfirm(LPARAM lParam) {

  if (!finWkrThrd(lParam)) return;

  if (savePrv) saveSite(PrvSiteSrc);
  }


// Finish the thread unlocking the commands, closing the progress bar posting a message about the results

bool SFTPDoc::finWkrThrd(LPARAM lParam) {
bool rslt;

  switch (lParam) {
    case TE_Normal    : notePad << _T("Finished successfully");     rslt = true;  break;
    case TE_Exception : notePad << _T("Thread had an exception");   rslt = false; break;
    default           : notePad << _T("Unknown Thread completion"); rslt = false; break;
    }

  notePad << nCrlf;   /*mainFrm()->closeProgress();*/   display(NotePadSrc);

  return rslt;                                                 /*cmdLock = false;*/
  }
#endif

//  rslt = OnOpenDocument(dbPath(path));
//bool   rslt;
#if 0
void SFTPDoc::onUpLoadSite() {                                // XXXX

  if (workerThrd.isLocked()) return;

  notePad.clear();   dataSource = StoreSrc;

  pathDlgDsc(_T("Ugly Example"), pathDlgDsc.name, _T("txt"), _T("*.txt"));

  if (!setOpenPath(pathDlgDsc)) return;

  pathDlgDsc.name = getMainName(path);

  if (!OnOpenDocument(path)) messageBox(_T(" Not Loaded!"));

  display(StoreSrc);
  }
#endif
#if 0
void SFTPDoc::confirmUpdate() {
FileDscsIter iter(updateFileDscs);
SiteFileDsc* uf;

  savePrv = false;

  for (uf = iter(); uf; uf = iter++) {

    if (!uf->check) continue;

    switch (uf->status) {

      case NilSts : break;

      case PutSts : if (put(*uf)) {prvFileDscs.update(*uf);   savePrv = true;}  break;

      case DelSts : if (del(*uf))
                        {prvFileDscs.delRcd(*uf);   updateFileDscs.delRcd(*uf);   savePrv = true;}   break;

      case GetSts : if (get(*uf))
                           {curFileDscs.update(*uf);   prvFileDscs.update(*uf);   savePrv = true;}   break;

      default     : continue;  // post am error message here
      }

//    ::PostMessage(mainFrm()->m_hWnd, ID_ConfirmMsg, ID_IncProgress, iter.index());
    }
  }

bool SFTPDoc::put(SiteFileDsc& uf) {
String src = siteID.localRoot  + uf.path;
String dst = siteID.remoteRoot + uf.path;

  if (!sftpSSL.sftpTransport().load(src)) return false;

  return sftpSSL.stor(toRemotePath(dst));
  }


bool SFTPDoc::get(SiteFileDsc& uf) {
String src = siteID.remoteRoot + uf.path;
String dst = siteID.localRoot  + uf.path;

  if (!sftpSSL.retr(toRemotePath(src))) return false;

  return sftpSSL.sftpTransport().store(dst);
  }


bool SFTPDoc::del(SiteFileDsc& uf)
                      {String dst = siteID.remoteRoot + uf.path;   return sftpSSL.del(toRemotePath(dst));}
#endif
#if 0
void SFTPDoc::dspUpdates(bool onlyChkd) {
FileDscsIter iter(updateFileDscs);
SiteFileDsc* uf;
SiteFlSts status;
SiteFileDsc* pf;

  notePad << _T("Update List") << nCrlf << nCrlf;

  notePad << nSetRTab(20) << nSetRTab(32) << nSetTab(35);

  for (uf = iter(); uf; uf = iter++) {

    if (onlyChkd && !uf->check) continue;

    status = uf->status;   pf = prvFileDscs.find(uf->path);

    switch (status) {
      case PutSts : notePad << _T("Put File:    "); break;
      case GetSts : notePad << _T("Get File:    "); break;
      case DelSts : notePad << _T("Delete File: "); break;
      default     : notePad << _T("Unchanged:   "); break;
      }

    notePad << nTab << uf->size << nTab << uf->date;
    notePad << nTab << uf->path << nCrlf;

    if (pf && status == PutSts) notePad << nTab << pf->size << nTab << pf->date << nCrlf;
    }

  display(NotePadSrc);
  }
#endif
//  ON_COMMAND(ID_PrepDownLoadSite, &onPrepDownLoadSite)

