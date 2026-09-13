

SFTP.chm : *.htm SFTP.hhp SFTP.hhc App.css
  -1 "C:\Program Files (x86)\HTML Help Workshop\hhc" SFTP


Clean.cln    :
  @echo "Cleaning"
  cd $(MAKEDIR)
  if exist SFTP.chm del SFTP.chm


$(TgtFolder)/SFTP.chm : SFTP.chm
  copy $(MAKEDIR)\SFTP.chm  $(MAKEDIR)\..\$(TgtFolder)\SFTP.chm


$(TgtFolder)/Clean.cln    : Clean.cln
  cd $(MAKEDIR)\..\$(TgtFolder)
  if exist SFTP.chm del /f SFTP.chm
  cd $(MAKEDIR)


$(TgtFolder)/Rebuild.rbd : $(TgtFolder)/Clean.cln $(TgtFolder)/SFTP.chm


