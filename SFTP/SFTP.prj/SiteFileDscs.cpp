

#include "pch.h"
#include "SiteFileDscs.h"
#include "Archive.h"
#include "CSVLex.h"
#include "CSVOut.h"
#include "Expandable.h"
#include "FileName.h"
#include "FileSrch.h"
#include "SFTP.h"
#include "Resource.h"
#include "SftpSSL.h"
#include "SftpDataIter.h"
#include "SftpTransport.h"
#include "SiteID.h"
#include "Utilities.h"
#include "WorkerThrd.h"


SiteFileDscs prvFileDscs;
SiteFileDscs curFileDscs;



bool SiteFileDscs::loadFromPC() {
String& path = siteID.localRoot;

  if (path.isEmpty()) return false;

  clear();   setRoot(path);   loadOneDir(path);

  return !isEmpty();
  }

void SiteFileDscs::loadOneDir(TCchar* path) {
Expandable<String, 2> data;
FileSrch              srch;
String                sub;
String                file;
int                   i;
int                   n;

  if (srch.findAllSubDirs(path)) while (srch.getName(sub)) data = sub;

  if (srch.findAllFiles(path)) while (srch.getName(file)) addFile(file);

  for (i = 0, n = data.end(); i < n; i++) loadOneDir(data[i]);

  data.clear();
  }


SiteFileDsc* SiteFileDscs::addFile(TCchar* path) {
SiteFileDsc* sf;
FileIO       lu;
CTime        time;

  if (filterFile(path)) return 0;

  sf  = data.allocate();

  String& pth = sf->path;   pth = path;

  if (pth.find(_T('/')) >= 0) {sftpSSL.size(pth, sf->size);   sftpSSL.date(pth, sf->date);}
  else                         sf->addLclAttr(path);

  pth      = toLocalPath(pth.substr(rootLng));
  sf->name = removePath(pth);   data = sf;

  return sf;
  }


void SiteFileDsc::addLclAttr(TCchar* path) {
FileIO lu;
CTime  time;

  if (lu.open(path, FileIO::Read)) {

    size = lu.getLength();
    if (!lu.getModifiedTime(time)) time = CTime::GetCurrentTime();

    lu.close();
    }

  else {size = 1; time = CTime::GetCurrentTime();}

  date = time;
  }



void SiteFileDscs::update(SiteFileDsc& uf) {
SiteFileDsc* sf = find(uf.path);

  if (sf) {sf ->size = uf.size; sf->date = uf.date;}
  else     data = uf;
  }



static TCchar* filters[] = {_T("_notes\\"), _T("Junk\\"),   _T(".log"),   _T("tmp\\"),  _T(".vpj"),
                            _T(".vpw"),     _T(".vpwhist"), _T(".vtg"),   _T(".xml")
                            };

bool SiteFileDscs::filterFile(TCchar* path) {
String s = toLocalPath(path);
int    i;
int    n;

  for (i = 0, n = noElements(filters); i < n; i++ ) if (s.find(filters[i]) >= 0) return true;

  return false;
  }


SiteFileDsc* SiteFileDscs::find(TCchar* path) {
String s = path;

  if (s.find(root) == 0) s = s.substr(rootLng);

  return data.bSearch(s.str());
  }



void SiteFileDscs::delRcd(SiteFileDsc& uf) {
FileDscsIter iter(*this);
SiteFileDsc* sf;

  for (sf = iter(); sf; sf = iter++) if (sf->path == uf.path) {iter.remove(); return;}
  }



bool SiteFileDscs::loadFromCSV(Archive& ar) {
CSVLex     lex(ar);
CSVtokCode code;
CSVtokCode code1;
int        i;

  clear();    setRoot(siteID.localRoot);

  for (code = lex.get_token(), i = 0; code != EOFToken; code = lex.get_token(), i++) {

    if (code == EolToken) {lex.accept_token(); break;}

    CSVtok& tok = lex.token;   code1 = lex.token1.code;

    if (code == CommaToken && code1 == StringToken)
              {notePad << _T("Unable to find field in line: ") << *tok.psource << nCrlf;  return false;}

    switch (i) {
      case 0  : if (tok.name != siteID.name) return false;
                break;

      case 1  : if (tok.name != siteID.url) {
                  notePad << _T("Web Name differs: ") << siteID.url << _T(" != ") << tok.name;
                  notePad << nCrlf;
                  }
                break;
      case 2  : if (tok.name != siteID.localRoot) {
                  notePad << _T("Path differs: ") << siteID.localRoot << _T(" != ") << tok.name;
                  notePad << nCrlf;
                  }
                break;

      default : notePad << _T("Whoops!") << nCrlf;
      }

    lex.accept_two_tokens();
    }

  return load(lex);
  }


bool SiteFileDscs::load(CSVLex& lex) {
int i;

  data.clear();

  for (i = 0; ; i++) {
    SiteFileDsc siteFile;

    if (!siteFile.load(lex)) return i > 0;

    data = siteFile;
    }
  }


bool SiteFileDsc::load(CSVLex& lex) {
CSVtokCode code;
CSVtokCode code1;
int        i;
uint       x;

  for (code = lex.get_token(), i = 0; code != EOFToken; code = lex.get_token(), i++) {

    if (code == EolToken) {lex.accept_token(); return true;}

    CSVtok& tok = lex.token;   code1 = lex.token1.code;

    if (code != StringToken || code1 != CommaToken)
              {notePad << _T("Unable to find field in line: ") << *tok.psource << nCrlf;  return false;}

    switch (i) {
      case 0  : name = tok.name; break;

      case 1  : path = tok.name; break;

      case 2  : size = tok.name.stoi(x); break;

      case 3  : {ToDate lcldt(tok.name);  date = lcldt();} break;

      case 4  : size = tok.name.stoi(x); break;

      case 5  : {ToDate rmtdt(tok.name);  date = rmtdt();} break;

//    case 6  : remotePath = tok.name; break;

      default : notePad << _T("Whoops!") << nCrlf;
      }
    lex.accept_two_tokens();
    }

  return false;
  }



void SiteFileDscs::saveCSV(Archive& ar) {
CSVOut       csvOut(ar);
FileDscsIter iter(*this);
SiteFileDsc* dsc;

  csvOut << siteID.name << Comma << siteID.url << Comma << siteID.localRoot << Comma << vCrlf;

  for (dsc = iter(); dsc; dsc = iter++) {

    switch (dsc->status) {
      case DelSts : if (!dsc->updated) dsc->save(csvOut);    break;

      case GetSts : if ( dsc->updated) dsc->save(csvOut);    break;

      case PutSts : if (!dsc->updated)
                                  {dsc = prvFileDscs.find(dsc->path);  if (!dsc) {iter.remove();  break;}}

      default     : dsc->save(csvOut); break;
      }
    }
  }


SiteFileDscs& SiteFileDscs::operator= (SiteFileDscs& dscs) {
FileDscsIter iter(dscs);
SiteFileDsc* dsc;

  clear();

  for (dsc = iter(); dsc; dsc = iter++) {

    switch (dsc->status) {
      case DelSts : if (!dsc->updated) addFile(*dsc);   break;

      case GetSts : if ( dsc->updated) addFile(*dsc);   break;

      case PutSts : if (!dsc->updated) break;

      default     : addFile(*dsc);   break;
      }
    }

  return *this;
  }


// Clear status for all entries that have been updated

void SiteFileDscs::clrSts() {
FileDscsIter iter(curFileDscs);
SiteFileDsc* dsc;

  for (dsc = iter(); dsc; dsc = iter++) if (dsc->updated) {

    switch (dsc->status) {
      case DelSts : iter.remove();   break;

      case GetSts : dsc->clrSts();   break;

      case PutSts : dsc->clrSts();   break;

      default     : break;
      }
    }
  }


SiteFileDsc* SiteFileDscs::addFile(SiteFileDsc& uf) {
SiteFileDsc* sf = find(uf.path);

  if (!sf) sf = data.allocate();    *sf = uf;   data = sf;    return sf;
  }



void SiteFileDsc::save(CSVOut& csvOut) {

  csvOut << name << Comma;
  csvOut << path << Comma;
  csvOut << size << Comma;
  csvOut << date << Comma;
  csvOut << vCrlf;
  }


void SiteFileDscs::display(TCchar* title) {
FileDscsIter iter(*this);
SiteFileDsc* fl;

  notePad << nClrTabs << nSetRTab(32) << nSetRTab(43) << nSetTab(45) << nCrlf;

  notePad << nBeginLine << title << _T(" Files -- Path: ") << root;
  notePad << nTab << nData() << nEndLine << nCrlf << nCrlf;

  for (fl = iter(); fl; fl = iter++) fl->display();
  }


void SiteFileDsc::display() {
String d  = date;
String rd = date;

  notePad << name << nTab << size << nTab << d << nTab << path << nCrlf;
  }





// ----------------------------------------

#if 0
void SiteFileDscs::startFromWeb(TCchar* path) {clear();   setRoot(path);   fromWeb(root);}


void SiteFileDscs::fromWeb(TCchar* path) {
SftpDataIter          iter(sftpSSL.fileData());
String*               s;

Expandable<String, 2> data;
int                   i;
int                   n;
int                   pos;
Tchar                 tch;
String                name;

  sftpSSL.dir(path);

  for (s = iter(); s; s = iter++) data.nextData() = *s;

  for (i = 0, n = data.end(); i < n; i++) {
    String& line = data[i];

    pos = line.findLastOf(_T(' '));  if (pos < 0) continue;
    name = line.substr(pos+1);   tch = name[0];   if (tch == _T('.') || tch == _T('_')) continue;

    if (line[0] == _T('d')) {String subDir = path + name + _T('/');   fromWeb(subDir); continue;}

    String fullPath = path + name;

    addFile(fullPath);
    }

  data.clear();
  }
#endif
#if 0
void SiteFileDscs::save(CSVOut& csvOut) {
FileDscsIter iter(*this);
SiteFileDsc* siteFile;

  for (siteFile = iter(); siteFile; siteFile = iter++) {


    siteFile->save(csvOut);
    }
  }
#endif
  //SiteFileDscs updateFileDscs;

