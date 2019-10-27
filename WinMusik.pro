###############################################################################
# This file is part of WinMusik 3 by Patrick Fedick                           #
# Web: https://www.pfp.de/winmusik/                                           #
###############################################################################
#
# Copyright (c) 2019, Patrick Fedick <patrick@pfp.de>
# All rights reserved.
# 
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
###############################################################################

TEMPLATE = app
TARGET = WinMusik
QT += core \
    gui \
    xml \
    widgets \
    printsupport
    
CONFIG += debug_and_release
CONFIG(debug, debug|release) { 
    win32:TARGET = WinMusik
    unix:TARGET = debug/WinMusik
}
else { 
    win32:TARGET = WinMusik
    unix:TARGET = release/WinMusik
}

DEFINES += PPL6LIB
DEFINES += PPL7LIB
DEFINES += HAVE_CONFIG_H

QMAKE_CC = cc
QMAKE_CXX	= c++
QMAKE_CFLAGS = -I/usr/local/include  -I/usr/local/include  -DHAVE_CONFIG_H  -D_THREAD_SAFE -pthread -I/usr/include 
QMAKE_CFLAGS +=  -I/usr/include -I/usr/local/include -I/usr/local/include
QMAKE_CFLAGS += -I/usr/local/include 
QMAKE_CXXFLAGS	= -g -O2 -I/usr/local/include
LDFLAGS		= -L/usr/local/lib -no-pie 


RESOURCES += resources.qrc
RC_FILE = resource.rc
INCLUDEPATH += include
INCLUDEPATH += widgets
INCLUDEPATH += pplib/include

unix:INCLUDEPATH += /usr/local/include
win32:INCLUDEPATH += C:/mingw/usr/local/include
win32:INCLUDEPATH += D:/jenkins/local/include
win32:QMAKE_LIBDIR += C:/mingw/usr/local/lib
win32:QMAKE_LFLAGS += -static-libgcc

LIBS +=  -D_THREAD_SAFE -pthread  -L/usr/lib -lz
LIBS += -lbz2 -lpcre /usr/local/lib/libiconv.so -Wl,-rpath -Wl,/usr/local/lib
LIBS += -L/usr/lib -lssl -lcrypto -L/usr/local/lib -lcurl
LIBS += -L/usr/local/lib -lcdio -lm -lcam 

win32:LIBS += -lwinmm
CODECFORSRC = UTF-8
CODECFORTR = UTF-8
TRANSLATIONS = winmusik_de.ts \
    winmusik_en.ts

DISTFILES += \
    WinMusik.includes \
    setup.iss \
    setup64.iss \
    HISTORY.TXT \
    build_mingw64.sh


SOURCES += src/asynchronousMessage.cpp \
    src/errors.cpp \
    src/traktor.cpp \
    src/editdevice.cpp \
    src/firststart.cpp \
    src/about.cpp \
    src/CTrackList.cpp \
    src/CTableControl.cpp \
    src/StorageSimpleTable.cpp \
    src/DropArea.cpp \
    src/StorageDevice.cpp \
    src/StorageShortcut.cpp \
    src/config.cpp \
    src/regexpcapture.cpp \
    src/CWmFile.cpp \
    src/CWmClient.cpp \
    src/CHashes.cpp \
    src/searchlists/searchlistdialog.cpp \
    src/searchlists/searchlists.cpp \
    src/searchlists/searchlisttracks.cpp \
    src/searchlists/searchlisttrackdialog.cpp \
    src/searchlists/dupecheckthread.cpp \
    src/menue/menue.cpp \
    src/InitialDatabase.cpp \
    src/colorscheme.cpp \
    src/plugins.cpp \
    src/StorageTitle.cpp \
    src/musickey.cpp \
    src/printdevicedialog.cpp \
    src/functions.cpp \
    src/CStorage.cpp \
    src/CID3TagSaver.cpp \
    src/edittrackdialog.cpp \
    src/wm20import.cpp \
    src/coverviewer.cpp \
    src/main.cpp \
    src/playlist/playlistexport.cpp \
    src/playlist/playlistStatusBar.cpp \
    src/playlist/playlisttracks.cpp \
    src/playlist/setbpmplayed.cpp \
    src/playlist/editstring.cpp \
    src/playlist/playlistedit.cpp \
    src/playlist/playlist.cpp \
    src/search/search.cpp \
    src/devicelist.cpp \
    src/editor/renumberdialog.cpp \
    src/editor/tablesearch.cpp \
    src/editor/massimport.cpp \
    src/editor/edit.cpp \
    src/editor/shortcutdialog.cpp \
    src/editor/oimpinfo.cpp \
    src/editor/cddbimport.cpp \
    src/editor/cddbselect.cpp \
    src/editor/edit-functions.cpp \
    src/splashscreen.cpp \
    src/CSearchlist.cpp \
    src/StorageOriginalImportData.cpp \
    src/langselect.cpp \
    src/properties/properties.cpp \
    src/properties/regexpedit.cpp \
    src/StorageTrack.cpp \
    src/Print.cpp \
    src/backgroundjobs.cpp \
    src/wmtoolbutton.cpp \
    src/coverprinter.cpp \
    src/resultfilter.cpp \
    widgets/fkeys.cpp \
    widgets/KeyWheel.cpp \
    widgets/edittrack.cpp \
    pplib/ppl6/ppl6_AudioCD.cpp \
    pplib/ppl6/ppl6_pplcore.cpp \
    pplib/ppl6/ppl6_CInt.cpp \
    pplib/ppl6/ppl6_CNotifyHandler.cpp \
    pplib/ppl6/ppl6_CList.cpp \
    pplib/ppl6/ppl6_CString.cpp \
    pplib/ppl6/ppl6_AudioInfo.cpp \
    pplib/ppl6/ppl6_CHeap.cpp \
    pplib/ppl6/ppl6_time.cpp \
    pplib/ppl6/ppl6_CTok.cpp \
    pplib/ppl6/ppl6_CTree.cpp \
    pplib/ppl6/ppl6_CCurl.cpp \
    pplib/ppl6/ppl6_CVar.cpp \
    pplib/ppl6/ppl6_functions.cpp \
    pplib/ppl6/ppl6_CFileObject.cpp \
    pplib/ppl6/ppl6_CWString.cpp \
    pplib/ppl6/ppl6_CThread.cpp \
    pplib/ppl6/ppl6_CConfig.cpp \
    pplib/ppl6/ppl6_md5.cpp \
    pplib/ppl6/ppl6_strings.cpp \
    pplib/ppl6/ppl6_CBinary.cpp \
    pplib/ppl6/ppl6_cpu.cpp \
    pplib/ppl6/ppl6_CMemFile.cpp \
    pplib/ppl6/ppl6_mp3.cpp \
    pplib/ppl6/ppl6_CDateTime.cpp \
    pplib/ppl6/ppl6_Exception.cpp \
    pplib/ppl6/ppl6_CIconv.cpp \
    pplib/ppl6/ppl6_CID3Tag.cpp \
    pplib/ppl6/ppl6_CLog.cpp \
    pplib/ppl6/ppl6_CSignal.cpp \
    pplib/ppl6/ppl6_CArray.cpp \
    pplib/ppl6/ppl6_CDebug.cpp \
    pplib/ppl6/ppl6_CFile.cpp \
    pplib/ppl6/ppl6_Cddb.cpp \
    pplib/ppl6/ppl6_MemoryHeap.cpp \
    pplib/ppl6/ppl6_random.cpp \
    pplib/ppl6/ppl6_CMutex.cpp \
    pplib/ppl6/ppl6_CBool.cpp \
    pplib/ppl6/ppl6_CDir.cpp \
    pplib/ppl6/ppl6_compat.cpp \
    pplib/ppl6/ppl6_Errors.cpp \
    pplib/ppl6/ppl6_CAVLTree.cpp \
    pplib/ppl6/ppl6_CMemoryReference.cpp \
    pplib/ppl6/ppl6_CMemory.cpp \
    pplib/ppl6/ppl6_CAssocArray.cpp \
    

HEADERS += widgets/edittrack.h \
    widgets/keywheel.h \
    widgets/fkeys.h \
    src/player/player.h \
    src/searchlists/searchlistdialog.h \
    src/searchlists/searchlists.h \
    src/searchlists/searchlisttrackdialog.h \
    src/searchlists/searchlisttracks.h \
    src/menue/menue.h \
    src/playlist/playlisttracks.h \
    src/playlist/playlistexport.h \
    src/playlist/playlist.h \
    src/playlist/editstring.h \
    src/playlist/playlistStatusBar.h \
    src/playlist/playlistedit.h \
    src/playlist/setbpmplayed.h \
    src/search/search.h \
    src/editor/cddbimport.h \
    src/editor/oimpinfo.h \
    src/editor/cddbselect.h \
    src/editor/massimport.h \
    src/editor/tablesearch.h \
    src/editor/edit.h \
    src/editor/shortcutdialog.h \
    src/editor/renumberdialog.h \
    src/properties/regexpedit.h \
    src/properties/properties.h \
    include/wmstorage.h \
    include/tablecontrol.h \
    include/version.h \
    include/droparea.h \
    include/resultfilter.h \
    include/splashscreen.h \
    include/backgroundjobs.h \
    include/traktor.h \
    include/coverviewer.h \
    include/plugins.h \
    include/musickey.h \
    include/devicelist.h \
    include/coverprinter.h \
    include/winmusik3.h \
    include/colorscheme.h \
    include/printdevicedialog.h \
    include/csearchlist.h \
    include/wmtoolbutton.h \
    include/firststart.h \
    include/wm20import.h \
    include/about.h \
    include/edittrackdialog.h \
    include/editdevice.h \
    include/regexpcapture.h \
    include/langselect.h \
    include/asynchronousMessage.h \
    pplib/include/config.h \
    pplib/include/compat.h \
    pplib/include/prolog.h \
    pplib/include/ppl6-exceptions.h \
    pplib/include/internal.h \
    pplib/include/ppl6-types.h \
    pplib/include/ppl6-sound.h \
    pplib/include/thread.h \
    pplib/include/ppl6.h \
    pplib/include/ppl6-algorithms.h \
    

FORMS += widgets/edittrack.ui \
    widgets/fkeys.ui \
    src/player/player.ui \
    src/searchlists/searchlists.ui \
    src/searchlists/searchlistdialog.ui \
    src/searchlists/searchlisttrackdialog.ui \
    src/menue/menue.ui \
    src/playlist/editstring.ui \
    src/playlist/setbpmplayed.ui \
    src/playlist/playlistexport.ui \
    src/playlist/playlist.ui \
    src/playlist/playlistedit.ui \
    src/search/search.ui \
    src/editor/tablesearch.ui \
    src/editor/oimpinfo.ui \
    src/editor/shortcutdialog.ui \
    src/editor/edit.ui \
    src/editor/massimport.ui \
    src/editor/cddbimport.ui \
    src/editor/renumberdialog.ui \
    src/editor/cddbselect.ui \
    src/properties/regexpedit.ui \
    src/properties/properties.ui \
    src/exporttracks/exporttracks.ui \
    forms/coverprinter.ui \
    forms/asynchronousMessage.ui \
    forms/firststart.ui \
    forms/splashscreen.ui \
    forms/edittrackdialog.ui \
    forms/langselect.ui \
    forms/editdevice.ui \
    forms/printdevicedialog.ui \
    forms/devicelist.ui \
    forms/about.ui \
    forms/coverviewer.ui \
    
