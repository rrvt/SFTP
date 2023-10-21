// Site Files -- A list of all the web site files (filtered) in the local and remote site tree


#pragma once
#include "Date.h"
#include "ExpandableP.h"
#include "IterT.h"

class CSVLex;
class CSVOut;
class Archive;


enum   SiteFlSts {NilSts, WebPutSts, DifPutSts, GetSts, DelSts, OthSts};


class SiteFileDsc {
public:

SiteFlSts status;
bool      check;
bool      updated;

String    name;             // Name of file, <name>.<ext>
String    path;             // Relative to Root Path, key for Sort,
int       size;
Date      date;

  SiteFileDsc() : status(NilSts), check(false), updated(false), size(0), date(Date::MinDate) { }
 ~SiteFileDsc() { }

  SiteFileDsc(SiteFileDsc& siteFile) {copy(siteFile);}                            // copy data
  SiteFileDsc& operator= (SiteFileDsc& siteFile) {copy(siteFile); return *this;}

  // Allows sorted data
  bool   operator>= (SiteFileDsc& siteFile) {return _tcsicmp(path, siteFile.path) >= 0;}
  bool   operator== (SiteFileDsc& siteFile) {return _tcsicmp(path, siteFile.path) == 0;}

  // Required for Binary Search
  bool operator== (TCchar* path) {return _tcsicmp(this->path, path) == 0;}
  bool operator<  (TCchar* path) {return _tcsicmp(this->path, path) <  0;}
  bool operator>  (TCchar* path) {return _tcsicmp(this->path, path) >  0;}

  void   addLclAttr(TCchar* path);

  bool   load(CSVLex& lex);
  void   save(CSVOut& csvOut);
  void   clrSts() {status = NilSts; check = false;   updated = false;}

  void   display();
  void   log();

private:

  void copy(SiteFileDsc& siteFile) {
    status = siteFile.status;  check = siteFile.check;   updated = siteFile.updated;
    name   = siteFile.name;    path  = siteFile.path;
    size   = siteFile.size;    date  = siteFile.date;
    }
  };


class SiteFileDscs;

typedef DatumPtrT<SiteFileDsc> SiteFileP;
typedef IterT<SiteFileDscs, SiteFileDsc> FileDscsIter;          // Iterator Declaration


class SiteFileDscs {

String root;
int    rootLng;
bool   loaded;

ExpandableP<SiteFileDsc, SiteFileP, 2> data;             // List of all files in web site (local and remote)

public:

                SiteFileDscs() : rootLng(0), loaded(false) { }
                SiteFileDscs(SiteFileDscs& sf) {copy(sf);}
               ~SiteFileDscs() {clear();}

  void          clear() {root.clear(); rootLng = 0; data.clear();}                 // XXX
  void          clrSts();

  SiteFileDscs& operator= (SiteFileDscs& sf) {copy(sf);   return *this;}

  bool          isEmpty() {return nData() == 0;}

  void          setRoot(TCchar* path) {root = path;   rootLng = root.length();}    // XXX

  bool          loadFromPC();                                                      // XXX
  SiteFileDsc*  addFile(TCchar* path);

  void          setCheck();
  bool          updateFromPC();

  void          update(SiteFileDsc& uf);
  void          delRcd(SiteFileDsc& uf);
  SiteFileDsc*  addFile(SiteFileDsc& uf);

  bool          loadFromCSV(Archive& ar);
  void          saveCSV(Archive& ar);
//  SiteFileDscs& updatePrv(SiteFileDscs& dscs);

  SiteFileDsc*  find(TCchar* path);      // {return data.bSearch(path);}

  void          display(TCchar* title);

  int           nData() {return data.end();}                 // returns number of data items in array
  SiteFileDsc*  datum(int i) {return 0 <= i && i < nData() ? data[i].p : 0;}

  void          logSelected(TCchar* title);

private:

  void          copy(SiteFileDscs& sf);

  void          loadOneDir(TCchar* path);

  bool          load(CSVLex& lex);

  bool          filterFile(TCchar* path);

  void          checkForDel(SiteFileDscs& newDscs);

  void          removeDatum(int i) {if (0 <= i && i < nData()) data.del(i);}

  friend typename FileDscsIter;
  };


extern SiteFileDscs prvFileDscs;
extern SiteFileDscs curFileDscs;




//  void      startFromWeb(TCchar* path);
//  void      fromWeb(TCchar* path);

// returns either a pointer to data (or datum) at index i in array or zero
//extern SiteFileDscs updateFileDscs;

