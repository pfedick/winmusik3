/*
 * This file is part of WinMusik 3 by Patrick Fedick
 *
 * $Author: pafe $
 * $Revision: 1.7 $
 * $Date: 2012/01/13 19:45:01 $
 * $Id: edit.h,v 1.7 2012/01/13 19:45:01 pafe Exp $
 *
 *
 * Copyright (c) 2010 Patrick Fedick
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef EDIT_H
#define EDIT_H

#include "winmusik3.h"
#include <ppl6-sound.h>
#include "tablecontrol.h"
#include <QtGui/QDialog>
#include <QRegExpValidator>
#include <QKeyEvent>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include "fkeys.h"
#include "ui_edit.h"
#include "oimpinfo.h"


class Edit;

#define TRACKLIST_TRACK_ROW		0
#define TRACKLIST_COVER_ROW		1
#define TRACKLIST_NAME_ROW		2
#define TRACKLIST_VERSION_ROW		3
#define TRACKLIST_GENRE_ROW		4
#define TRACKLIST_LENGTH_ROW		5
#define TRACKLIST_RATING_ROW		6

class CTitleList : public QTreeWidget
{
	friend class Edit;
	private:
		Edit *edit;
		CWmClient *wm;
		QPoint startPos;

	public:
		CTitleList(QWidget * parent = 0);

	private:
		//void mouseDoubleClickEvent ( QMouseEvent * event);
		//void itemClicked ( QTreeWidgetItem * item, int column);
		//void itemDoubleClicked ( QTreeWidgetItem * item, int column);

		void mousePressEvent ( QMouseEvent * event );
		void mouseReleaseEvent ( QMouseEvent * event );
		void mouseMoveEvent ( QMouseEvent * event );
		//void contextMenuEvent(QContextMenuEvent *event);
		//void customContextMenuRequested ( const QPoint & pos );
		void focusInEvent ( QFocusEvent * event );
};

class Edit;

class AsynchronousTrackUpdate : public ppl6::CThread
{
	public:
		AsynchronousTrackUpdate();
		~AsynchronousTrackUpdate();
		Edit *edit;

		void ThreadMain(void *data);

};

class Edit : public QWidget
{
	friend class CTitleList;
    Q_OBJECT

public:
    Edit(QWidget *parent = 0, CWmClient *wm=NULL, int typ=0);
    ~Edit();

    void UpdateFkeys();
    void UpdateDevice();
    void UpdateTrackListing();
    ppluint32 EditDeviceDialog(ppluint32 id);
    void ClearEditFields();
    void FillEditFields();
    void ReloadTranslation();
    void OpenTrack(ppluint32 deviceId, ppluint8 page=0, ppluint16 track=0);
    void ReloadTracks();
    void hideEditor();
    void showEditor();
    void showEditorWithoutFocusChange();


private:
    AsynchronousTrackUpdate	asyncTrackUpdate;

	CWmClient *wm;
	ppluint8 DeviceType;
	int position, oldposition;;
	ppluint32 DeviceId;
	ppluint8 Page;
	//ppluint32 TitleId;
	ppluint16 TrackNum;
    Ui::EditClass ui;

    DataDevice	datadevice;
    CTrackList	*TrackList;
    DataTrack	Track;
    DataTitle	Ti;
    DataOimp	Oimp;

    CTableControl TCVersion;
    CTableControl TCGenre;
    CTableControl TCLabel;
    CTableControl TCRecordSource;
    CTableControl TCRecordDevice;
    QPixmap			Cover;
    QStringList Artists;
    QStringList Titles;
    QStringList Albums;
    QCompleter *artistCompleter;
    QCompleter *titleCompleter;
    QCompleter *albumCompleter;
    bool		DupeCheck;
    QTimer		*DupeTimer;
    const char	*DupeCheckIcon;

    OimpInfo	*oimpInfo;

    CTitleList	*trackList;
    WMTreeItem	*currentTrackListItem;
    QPoint startPos;	// Für Drag/Drop und multiple Markierungen
    QPoint	ratePos;	// Für Rating-Spalte

    QWidget *searchWindow;



    void SetupTrackList();

    bool eventFilter(QObject *target, QEvent *event);
    bool consumeEvent(QObject *target, QEvent *event);
    void closeEvent(QCloseEvent *event);
    void moveEvent(QMoveEvent *event);

    // Globale Events
    bool on_KeyPress(QObject *target, int key,int modifier);
    void MoveToNextWidget();
    QWidget *GetWidgetFromPosition(int position);
    void FixFocus();

    // Events
    bool on_index_FocusIn();
    bool on_index_KeyPress(QKeyEvent *event,int key,int modifier);
    bool on_page_FocusIn();
    bool on_track_FocusIn();
    bool on_artist_FocusIn();
    bool on_artist_FocusOut();
    bool on_title_FocusIn();
    bool on_title_FocusOut();
    bool on_album_FocusIn();
    bool on_album_FocusOut();
    bool on_length_FocusOut();
    bool on_FocusIn(QLineEdit *widget);
    bool on_f4_Pressed(int position);
    bool on_f5_ShortCut(int modifier);
    bool on_f5_CheckDupes(QObject *target);
    bool on_f6_Pressed(QObject *target, int modifier);
    bool on_f6_MassImport();
    bool on_f7_DeleteTrack();
    bool on_f8_InsertTrack();
    bool on_f9_UpdateAllID3Tags();
    bool on_f10_WritePlaylist();



	void on_esc_clicked();
	void on_f1_clicked();
	void on_f2_clicked();
	void on_f3_clicked();
	void on_f4_clicked();
	void on_f5_clicked();
	void on_f6_clicked();
	void on_f7_clicked();
	void on_f8_clicked();
	void on_f9_clicked();
	void on_f10_clicked();
	void on_f11_clicked();
	void on_f12_clicked();

    bool on_trackList_MousePress(QMouseEvent *event);
    bool on_trackList_MouseRelease(QMouseEvent *event);
    bool on_trackList_MouseMove(QMouseEvent *event);


    bool EditTrack();

    //void SetFocus(QWidget *widget);
    void SetFkey(QToolButton *button,const char *Icon, QString Text, bool enabled=true);
    void InstallFilter(QObject *object, int id);

    void SaveTrack();
    void resizeEvent ( QResizeEvent * event );
    void CopyFromTrackInfo(TrackInfo &info);

    void UpdateCompleters();
    void ShowOimpInfo();
    void CheckDupes();
    void UpdateCover();
    //void index_LostFocus();

public slots:
	void on_trackList_customContextMenuRequested ( const QPoint & pos );
	void on_trackList_itemClicked ( QTreeWidgetItem * item, int column );
	void on_trackList_itemDoubleClicked ( QTreeWidgetItem * item, int column );

	void on_contextFindMoreArtist_triggered();
	void on_contextFindMoreTitle_triggered();
	void on_contextFindMoreVersions_triggered();
	void on_contextPlayTrack_triggered();
	void on_contextEditTrack_triggered();
	void on_contextCopyTrack_triggered();
	void on_contextCopyFile_triggered();
	void on_contextDeleteTrack_triggered();
	void on_contextInsertTrack_triggered();
	void on_DupeTimer_update();

	void on_fkeys_clicked(int num);


	void on_coverCopyButton_clicked();
	void on_coverInsertButton_clicked();
	void on_coverDeleteButton_clicked();
	void on_coverLoadButton_clicked();
	void on_coverSaveButton_clicked();

	void on_coverSearchAmazon_clicked();
	void on_coverSearchGoogle_clicked();
	void on_coverSearchBeatport_clicked();
	void on_coverSearchSoundcloud_clicked();

	void on_hideEditor_clicked();

};

#endif // EDIT_H
