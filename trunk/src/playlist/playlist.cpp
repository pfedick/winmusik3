/*
 * This file is part of WinMusik 3 by Patrick Fedick
 *
 * $Author: pafe $
 * $Revision: 1.1 $
 * $Date: 2010/11/14 13:20:11 $
 * $Id: Playlist.cpp,v 1.1 2010/11/14 13:20:11 pafe Exp $
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


#include <QClipboard>
#include <QMenu>
#include <QList>
#include <QUrl>
#include <QMimeData>
#include <QMouseEvent>
#include <QFileDialog>
#include <QToolBar>
#include <QKeyEvent>

#include "playlisttracks.h"
#include "playlist.h"
#include "playlistedit.h"
#include "traktor.h"
#include <stdio.h>

Playlist::Playlist(QWidget *parent, CWmClient *wm)
    : QMainWindow(parent)
{
	ui.setupUi(this);
	this->wm=wm;
	currentTreeItem=NULL;
	setAttribute(Qt::WA_DeleteOnClose,true);
	ui.tracks->setPlaylist(this);
	ui.tracks->setAcceptDrops(true);
	//ui.tracks->setMouseTracking(true);
	ui.tracks->installEventFilter(this);

	createMenue();
	createToolbar();
	createStatusBar();

	playlistView=playlistViewNormal;

	recreatePlaylist();
	changed=false;

	ui.tracks->sortByColumn(0,Qt::AscendingOrder);

	restoreGeometry(wm->GetGeometry("playlist"));
}


Playlist::~Playlist()
{
	if (wm) {
		wm->PlaylistClosed(this);
	}
}


void Playlist::createMenue()
{
	QMenu	*menu;
	menu=menuBar()->addMenu(tr("&File"));

	menu->addAction(QIcon(":/icons/resources/filenew.png"),tr("&new Playlist"),this,SLOT(on_menuNew_triggered()));
	menu->addAction(QIcon(":/icons/resources/fileopen.png"),tr("&load Playlist"),this,SLOT(on_menuOpen_triggered()));
	menu->addAction(QIcon(":/icons/resources/filesave.png"),tr("&save Playlist"),this,SLOT(on_menuSave_triggered()));
	menu->addAction(QIcon(":/icons/resources/filesaveas.png"),tr("save Playlist &as"),this,SLOT(on_menuSaveAs_triggered()));
	menu->addSeparator();

	menuRecentPlaylists=menu->addMenu(QIcon(":/icons/resources/fileopen.png"),tr("&recent Playlists"));
	updateRecentPlaylistsMenu();

	menu=menuBar()->addMenu(tr("&View"));
	menu->addAction(QIcon(":/icons/resources/view_playlist.png"),tr("&Playlist"),this,SLOT(on_viewPlaylist_triggered()));
	menu->addAction(QIcon(":/icons/resources/view_dj.png"),tr("&DJ"),this,SLOT(on_viewDJ_triggered()));

}

void Playlist::createToolbar()
{
	QToolBar *tb=new QToolBar();
	tb->setFloatable(false);
	tb->setMovable(false);

	tb->addAction(QIcon(":/icons/resources/filenew.png"),tr("&new Playlist"),this,SLOT(on_menuNew_triggered()));
	tb->addAction(QIcon(":/icons/resources/fileopen.png"),tr("&load Playlist"),this,SLOT(on_menuOpen_triggered()));
	tb->addAction(QIcon(":/icons/resources/filesave.png"),tr("&save Playlist"),this,SLOT(on_menuSave_triggered()));
	tb->addAction(QIcon(":/icons/resources/filesaveas.png"),tr("save Playlist &as"),this,SLOT(on_menuSaveAs_triggered()));
	tb->addSeparator();
	tb->addAction(QIcon(":/icons/resources/view_playlist.png"),tr("view &Playlist"),this,SLOT(on_viewPlaylist_triggered()));
	tb->addAction(QIcon(":/icons/resources/view_dj.png"),tr("view &DJ"),this,SLOT(on_viewDJ_triggered()));

	this->addToolBar(tb);
}

void Playlist::createStatusBar()
{
	QHBoxLayout *layout=new QHBoxLayout;
	layout->setContentsMargins(2,2,2,2);

	QLabel *label;
	QFrame *line;

	// Total Tracks
	label=new QLabel(tr("total tracks:"));
	layout->addWidget(label);

	totalTracks=new QLabel();
	totalTracks->setFrameShadow(QFrame::Sunken);
	totalTracks->setFrameShape(QFrame::StyledPanel);
	layout->addWidget(totalTracks);

	line = new QFrame();
	line->setGeometry(QRect(320, 150, 118, 3));
	line->setFrameShape(QFrame::VLine);
	line->setFrameShadow(QFrame::Sunken);
	layout->addWidget(line);


	// Total Track length
	label=new QLabel(tr("total track length:"));
	layout->addWidget(label);

	totalTrackLength=new QLabel();
	totalTrackLength->setFrameShadow(QFrame::Sunken);
	totalTrackLength->setFrameShape(QFrame::StyledPanel);
	layout->addWidget(totalTrackLength);

	QLabel *min=new QLabel(tr("(hh:mm:ss)"));
	layout->addWidget(min);

	line = new QFrame();
	line->setGeometry(QRect(320, 150, 118, 3));
	line->setFrameShape(QFrame::VLine);
	line->setFrameShadow(QFrame::Sunken);
	layout->addWidget(line);


	// Total Mix length
	label=new QLabel(tr("total mix length:"));
	layout->addWidget(label);

	totalMixLength=new QLabel();
	totalMixLength->setFrameShadow(QFrame::Sunken);
	totalMixLength->setFrameShape(QFrame::StyledPanel);

	layout->addWidget(totalMixLength);
	min=new QLabel(tr("(hh:mm:ss)"));
	layout->addWidget(min);

	QFrame *frame=new QFrame;
	//frame->setFrameShadow(QFrame::Sunken);
	//frame->setFrameShape(QFrame::StyledPanel);
	frame->setContentsMargins(0,0,0,0);
	frame->setLayout(layout);

	this->statusBar()->addPermanentWidget(frame);

}

void Playlist::updateRecentPlaylistsMenu()
{
	menuRecentPlaylists->clear();
	if (wm->conf.LastPlaylists[0].NotEmpty()) menuRecentPlaylists->addAction(QIcon(":/icons/resources/fileopen.png"),wm->conf.LastPlaylists[0],this,SLOT(on_menuOpenRecent0_triggered()));
	if (wm->conf.LastPlaylists[1].NotEmpty()) menuRecentPlaylists->addAction(QIcon(":/icons/resources/fileopen.png"),wm->conf.LastPlaylists[1],this,SLOT(on_menuOpenRecent1_triggered()));
	if (wm->conf.LastPlaylists[2].NotEmpty()) menuRecentPlaylists->addAction(QIcon(":/icons/resources/fileopen.png"),wm->conf.LastPlaylists[2],this,SLOT(on_menuOpenRecent2_triggered()));
	if (wm->conf.LastPlaylists[3].NotEmpty()) menuRecentPlaylists->addAction(QIcon(":/icons/resources/fileopen.png"),wm->conf.LastPlaylists[3],this,SLOT(on_menuOpenRecent3_triggered()));
	if (wm->conf.LastPlaylists[4].NotEmpty()) menuRecentPlaylists->addAction(QIcon(":/icons/resources/fileopen.png"),wm->conf.LastPlaylists[4],this,SLOT(on_menuOpenRecent4_triggered()));

}

void Playlist::closeEvent(QCloseEvent *event)
{
	if (wm) {
		wm->SaveGeometry("playlist",saveGeometry());
	}
	QMainWindow::closeEvent(event);
}


bool Playlist::eventFilter(QObject *target, QEvent *event)
{
	if (consumeEvent(target,event)) return true;
	return QMainWindow::eventFilter(target,event);
}

bool Playlist::consumeEvent(QObject *target, QEvent *event)
{
	int type=event->type();
	//printf ("Event: %i\n",type);
	if (target==ui.tracks) {
		if (type==QEvent::Drop) {
			handleDropEvent(static_cast<QDropEvent *>(event));
			return true;
		} else if (type==QEvent::DragEnter) {
			(static_cast<QDragEnterEvent *>(event))->accept();
			return true;
		} else if (type==QEvent::KeyPress) {
			QKeyEvent *e=static_cast<QKeyEvent *>(event);
			if (e->key()==Qt::Key_Delete) {
				QList<QTreeWidgetItem *>selected=ui.tracks->selectedItems();
				if (selected.count()>1) {
					if (QMessageBox::question (this, tr("Delete Tracks"),
							tr("Do you want to delete all selected tracks from this playlist?"),
							QMessageBox::No|QMessageBox::Yes,
							QMessageBox::No) != QMessageBox::Yes) return false;
				}
				ui.tracks->deleteSelectedItems();
				updateLengthStatus();
				renumberTracks();
				return true;
			}
			return false;
			/*
		} else if (type==QEvent::DragMove) {
			//QPoint p=(static_cast<QDragMoveEvent *>(event))->pos()-ui.tracks->geometry().topLeft();
			//QTreeWidgetItem *item=ui.tracks->itemAt(p);
			//printf ("Move: %i:%i\n",p.x(),p.y());
			//(static_cast<QDragMoveEvent *>(event))->accept();
			return false;
			*/
		}
	}
	return false;
}

bool Playlist::on_tracks_MouseButtonPress(QMouseEvent * event)
{
	//printf ("on_tracks_MouseButtonPress\n");
	if (event->buttons() == Qt::LeftButton) startPos=event->pos();
	ratePos=event->pos();
	return false;
}

bool Playlist::on_tracks_MouseButtonRelease(QMouseEvent *)
{
	//printf ("on_tracks_MouseButtonRelease\n");
	startPos.setX(0);
	startPos.setY(0);
	return false;
}



bool Playlist::on_tracks_MouseMove(QMouseEvent *event)
{
	//printf ("on_tracks_MouseMove\n");
	if (!(event->buttons() == Qt::LeftButton)) return false;

	int distance=(event->pos()-startPos).manhattanLength();
	if (distance<QApplication::startDragDistance()) return false;

	QList<QTreeWidgetItem*> Items=ui.tracks->selectedItems();
	QList<QUrl> list;
    QPixmap Icon;

    //printf ("DragStart\n");

    ppl6::CString xml;
    xml="<winmusikTracklist>\n";
	for (int i=0;i<Items.size();i++) {
		PlaylistItem *item=(PlaylistItem *)Items[i];
		if (Icon.isNull()) {
			Icon.loadFromData((const uchar*)item->CoverPreview.GetPtr(),item->CoverPreview.GetSize());
		}
		xml+=item->exportAsXML(0);

#ifdef _WIN32
			list.append(QUrl::fromLocalFile(item->File));
#else
		list.append(QUrl::fromLocalFile(item->File));
#endif
	}
	//xml.Print(true);

    QDrag *drag = new QDrag(this);
    QMimeData *mimeData = new QMimeData;
    if (Icon.isNull()) Icon.load(":/devices48/resources/tr48x48-0007.png");
    drag->setPixmap(Icon);
    mimeData->setText(xml);
    mimeData->setUrls(list);
    drag->setMimeData(mimeData);
    // start drag
    drag->exec(Qt::CopyAction | Qt::MoveAction);
	startPos.setX(0);
	startPos.setY(0);
	return true;
}

void Playlist::handleDropEvent(QDropEvent *event)
{
	//printf ("Drop Event 1\n");

	event->accept();
	if (event->source()==this) printf ("Quelle identisch\n"); else printf ("Fremdquelle\n");
	const QMimeData *mime=event->mimeData();
	if (!mime) return;

	ui.tracks->setSortingEnabled(false);
	//if (!mime->hasUrls()) return;
	//printf ("Drop Event 2\n");

	QList<QTreeWidgetItem *>selected=ui.tracks->selectedItems();
	ui.tracks->unselectItems();
	QApplication::processEvents();

	//QPoint p=(static_cast<QDragMoveEvent *>(event))->pos()-ui.tracks->geometry().topLeft();
	QPoint p=(static_cast<QDragMoveEvent *>(event))->pos()-ui.tracks->geometry().topLeft();
	//printf ("Drop Position: %i:%i\n",p.x(),p.y());
	QTreeWidgetItem *insertItem=ui.tracks->itemAt(p);
	if (insertItem) {
		//PlaylistItem *p=(PlaylistItem*)insertItem;
		//printf ("Insert Item: %s\n",(const char*)((ppl6::CString)(p->text(0))));
	}


	ppl6::CString xml=mime->text();
	if (xml.Left(18)=="<winmusikTracklist") {
		ppl6::CArray List;
		List.Explode(xml,"</track>");
		//printf ("List: %i\n",List.Num());
		for (int i=0;i<List.Num();i++) {
			PlaylistItem *item=new PlaylistItem;
			ppl6::CString Row=List.GetString(i);
			if (Row.PregMatch("/^\\<titleId\\>(.*?)\\<\\/titleId\\>$/m"))
				item->titleId=ppl6::UnescapeHTMLTags(Row.GetMatch(1)).ToInt();
			loadTrackFromXML(item,xml);
			renderTrack(item);
			if (insertItem) ui.tracks->insertTopLevelItem(ui.tracks->indexOfTopLevelItem(insertItem),item);
			else ui.tracks->addTopLevelItem(item);
			changed=true;
			item->setSelected(true);

		}
		if (event->source()==this) ui.tracks->deleteItems(selected);
		updateLengthStatus();
		renumberTracks();
		ui.tracks->setSortingEnabled(true);
		return;
	}

	ppl6::CString path=wm->conf.DevicePath[7];
	QList<QUrl>	list=mime->urls();
	for (int i=0;i<list.size();i++) {
		QUrl url=list[i];
		QString file=url.toLocalFile();
		PlaylistItem *item=new PlaylistItem;
		ppl6::CString f=file;
		int p=f.Instr(path);
		if (p>=0) {
			f=f.Mid(p);
			f.Replace(path,"");
			if (f.PregMatch("/\\/([0-9]+)\\/([0-9]{3})-.*$/")) {
				int myDeviceId=ppl6::atoi(f.GetMatch(1));
				int myTrack=ppl6::atoi(f.GetMatch(2));
				//printf ("myDeviceId=%i, myTrack=%i\n",myDeviceId,myTrack);
				const DataTrack *tr=wm->GetTrack(7,myDeviceId,1,myTrack);
				if (tr) {
					if (!loadTrackFromDatabase(item,tr->TitleId)) {
						delete item;
						continue;
					} else {
						item->File=file;
					}
				}
			}
		}
		if (item->titleId==0) loadTrackFromFile(item,file);
		renderTrack(item);
		changed=true;
		if (insertItem) ui.tracks->insertTopLevelItem(ui.tracks->indexOfTopLevelItem(insertItem),item);
		else ui.tracks->addTopLevelItem(item);
		item->setSelected(true);
	}
	updateLengthStatus();
	renumberTracks();
	ui.tracks->setSortingEnabled(true);
}

bool Playlist::loadTrackFromDatabase(PlaylistItem *item, ppluint32 titleId)
{
	DataTitle *ti=wm->GetTitle(titleId);
	if (!ti) return false;
	item->DeviceId=ti->DeviceId;
	item->DeviceTrack=ti->Track;
	item->DeviceType=ti->DeviceType;
	item->DevicePage=ti->Page;

	item->titleId=titleId;
	item->Artist=ti->Artist;
	item->Title=ti->Title;
	item->Version=wm->GetVersionText(ti->VersionId);
	item->Genre=wm->GetGenreText(ti->GenreId);
	item->Label=wm->GetLabelText(ti->LabelId);
	item->Album=ti->Album;
	item->musicKey=ti->Key;
	item->bpm=ti->BPM;
	item->rating=ti->Rating;
	item->trackLength=ti->Length;
	item->mixLength=ti->Length;
	item->startPositionSec=0;
	item->endPositionSec=ti->Length;
	for (int z=0;z<5;z++) {
		item->cutStartPosition[z]=0;
		item->cutEndPosition[z]=0;
	}
	item->CoverPreview=ti->CoverPreview;
	item->File=wm->MP3Filename(ti->DeviceId,ti->Page,ti->Track);
	item->useTraktorCues(item->File);
	return true;
}

void Playlist::loadTrackFromXML(PlaylistItem *item, const ppl6::CString &xml)
{
	ppl6::CString Row=xml;
	if (Row.PregMatch("/^\\<Artist\\>(.*?)\\<\\/Artist\\>$/m")) item->Artist=ppl6::UnescapeHTMLTags(Row.GetMatch(1));
	if (Row.PregMatch("/^\\<Title\\>(.*?)\\<\\/Title\\>$/m")) item->Title=ppl6::UnescapeHTMLTags(Row.GetMatch(1));
	if (Row.PregMatch("/^\\<Version\\>(.*?)\\<\\/Version\\>$/m")) item->Version=ppl6::UnescapeHTMLTags(Row.GetMatch(1));
	if (Row.PregMatch("/^\\<Genre\\>(.*?)\\<\\/Genre\\>$/m")) item->Genre=ppl6::UnescapeHTMLTags(Row.GetMatch(1));
	if (Row.PregMatch("/^\\<Label\\>(.*?)\\<\\/Label\\>$/m")) item->Label=ppl6::UnescapeHTMLTags(Row.GetMatch(1));
	if (Row.PregMatch("/^\\<Album\\>(.*?)\\<\\/Album\\>$/m")) item->Album=ppl6::UnescapeHTMLTags(Row.GetMatch(1));
	if (Row.PregMatch("/^\\<File\\>(.*?)\\<\\/File\\>$/m")) item->File=ppl6::UnescapeHTMLTags(Row.GetMatch(1));
	if (Row.PregMatch("/^\\<bpm\\>(.*?)\\<\\/bpm\\>$/m")) item->bpm=ppl6::UnescapeHTMLTags(Row.GetMatch(1)).ToInt();
	if (Row.PregMatch("/^\\<rating\\>(.*?)\\<\\/rating\\>$/m")) item->rating=ppl6::UnescapeHTMLTags(Row.GetMatch(1)).ToInt();
	if (Row.PregMatch("/^\\<trackLength\\>(.*?)\\<\\/trackLength\\>$/m")) item->trackLength=ppl6::UnescapeHTMLTags(Row.GetMatch(1)).ToInt();
	if (Row.PregMatch("/^\\<musicKey\\>(.*?)\\<\\/musicKey\\>$/m")) item->musicKey=DataTitle::keyId(ppl6::UnescapeHTMLTags(Row.GetMatch(1)));

	item->mixLength=item->trackLength;
	item->startPositionSec=0;
	item->endPositionSec=item->trackLength;
	for (int z=0;z<5;z++) {
		item->cutStartPosition[z]=0;
		item->cutEndPosition[z]=0;
	}
	item->updateFromDatabase();

	if (Row.PregMatch("/^\\<musicKey\\>(.*?)\\<\\/musicKey\\>$/m")) item->musicKey=DataTitle::keyId(ppl6::UnescapeHTMLTags(Row.GetMatch(1)));
	item->useTraktorCues(item->File);

	if (Row.PregMatch("/^\\<startPositionSec\\>(.*?)\\<\\/startPositionSec\\>$/m")) item->startPositionSec=ppl6::UnescapeHTMLTags(Row.GetMatch(1)).ToInt();
	if (Row.PregMatch("/^\\<endPositionSec\\>(.*?)\\<\\/endPositionSec\\>$/m")) item->endPositionSec=ppl6::UnescapeHTMLTags(Row.GetMatch(1)).ToInt();
	if (Row.PregMatch("/^\\<cuts\\>(.*?)\\<\\/cuts\\>$/s")) {
		ppl6::CArray cut(Row.GetMatch(1),"</cut>");
		printf ("Number of cuts: %i\n",cut.Num());
		for (int i=0;i<5;i++) {
			ppl6::CString r=cut[i];
			if (r.PregMatch("/^\\<start\\>(.*?)\\<\\/start\\>$/m")) item->cutStartPosition[i]=ppl6::UnescapeHTMLTags(r.GetMatch(1)).ToInt();
			if (r.PregMatch("/^\\<end\\>(.*?)\\<\\/end\\>$/m")) item->cutEndPosition[i]=ppl6::UnescapeHTMLTags(r.GetMatch(1)).ToInt();
		}
	}
	item->updateMixLength();
	item->loadCoverPreview();
}

void Playlist::loadTrackFromFile(PlaylistItem *item, const ppl6::CString &file)
{
	item->File=file;
	TrackInfo info;

	if (!getTrackInfoFromFile(info,file)) return;
	//printf ("Playlist::loadTrackFromFile: %s\n",(const char*)file);
	item->Artist=info.Ti.Artist;
	item->Title=info.Ti.Title;
	item->Album=info.Ti.Album;
	item->Version=info.Version;
	item->Genre=info.Genre;
	item->Label=info.Label;
	item->trackLength=info.Ti.Length;
	item->mixLength=item->trackLength;
	item->endPositionSec=item->trackLength;
	item->bpm=info.Ti.BPM;
	item->rating=info.Ti.Rating;
	item->musicKey=info.Ti.Key;
	item->CoverPreview=info.Ti.CoverPreview;

	item->useTraktorCues(file);
}


void Playlist::Resize()
{
	int w=ui.tracks->width();
	ui.tracks->setColumnWidth(columnTrack,40);
	w-=44;
	ui.tracks->setColumnWidth(columnCover,64);
	w-=68;
	ui.tracks->setColumnWidth(columnLength,60);
	w-=64;
	ui.tracks->setColumnWidth(columnRating,50);
	w-=53;


	if (playlistView==playlistViewNormal) {
	} else if (playlistView==playlistViewDJ) {
		ui.tracks->setColumnWidth(columnBpm,35);
		w-=39;
		ui.tracks->setColumnWidth(columnMusicKey,50);
		w-=54;
		ui.tracks->setColumnWidth(columnStart,60);
		w-=64;
		ui.tracks->setColumnWidth(columnEnd,60);
		w-=64;
		ui.tracks->setColumnWidth(columnCuts,60);
		w-=64;
	}
	ui.tracks->setColumnWidth(columnSource,85);
	w-=89;
	ui.tracks->setColumnWidth(columnTitle,w*80/100);
	ui.tracks->setColumnWidth(columnGenre,w*15/100);

}

void Playlist::recreatePlaylist()
{
	QTreeWidgetItem *item=ui.tracks->headerItem();

	columnTrack=0;
	columnCover=1;
	columnTitle=2;
	columnGenre=3;

	if (playlistView==playlistViewNormal) {
		ui.tracks->setColumnCount(7);
		item->setText(0,tr("Track"));
		item->setText(1,tr("Cover"));
		item->setText(2,tr("Artist - Title (Version)"));
		item->setText(3,tr("Genre"));
		item->setText(4,tr("Length"));
		item->setText(5,tr("Rating"));
		item->setText(6,tr("Source"));
		columnLength=4;
		columnRating=5;
		columnSource=6;

	} else if (playlistView==playlistViewDJ) {
		ui.tracks->setColumnCount(12);
		item->setText(0,tr("Track"));
		item->setText(1,tr("Cover"));
		item->setText(2,tr("Artist - Title (Version)"));
		item->setText(3,tr("Genre"));
		item->setText(4,tr("Bpm"));
		item->setText(5,tr("Key"));
		item->setText(6,tr("Rating"));
		item->setText(7,tr("Start"));
		item->setText(8,tr("End"));
		item->setText(9,tr("Cuts"));
		item->setText(10,tr("Length"));
		item->setText(11,tr("Source"));

		columnLength=10;
		columnRating=6;
		columnSource=11;
		columnBpm=4;
		columnMusicKey=5;
		columnStart=7;
		columnEnd=8;
		columnCuts=9;
	}
	Resize();
	updatePlaylist();
}

void setLength(QLabel *label, int length)
{
	int hh=0;
	int mm=(int)(length/60);
	int ss=length%60;
	if (mm>59) {
		hh=(int)(mm/60);
		mm=mm%60;
	}
	label->setText(ppl6::ToString("%0d:%02d:%02d",hh,mm,ss));
}

void Playlist::updatePlaylist()
{
	for (int i=0;i<ui.tracks->topLevelItemCount();i++) {
		PlaylistItem *item=(PlaylistItem*)ui.tracks->topLevelItem(i);
		renderTrack(item);
		item->setText(0,ppl6::ToString("%5d",i+1));
	}
	updateLengthStatus();
}

void Playlist::renumberTracks()
{
	for (int i=0;i<ui.tracks->topLevelItemCount();i++) {
		PlaylistItem *item=(PlaylistItem*)ui.tracks->topLevelItem(i);
		item->setText(0,ppl6::ToString("%5d",i+1));
	}
}

void Playlist::updateLengthStatus()
{
	int trackLength=0;
	int mixLength=0;
	for (int i=0;i<ui.tracks->topLevelItemCount();i++) {
		PlaylistItem *item=(PlaylistItem*)ui.tracks->topLevelItem(i);
		trackLength+=item->trackLength;
		mixLength+=item->mixLength;
	}
	setLength(totalTrackLength,trackLength);
	setLength(totalMixLength,mixLength);
	totalTracks->setText(ppl6::ToString("%d",ui.tracks->topLevelItemCount()));
}

void Playlist::renderTrack(PlaylistItem *item)
{
	QColor color(0,0,0);
	for (int i=1;i<item->columnCount();i++) {
		item->setText(i,"");
		item->setIcon(i,QIcon());
		item->setTextColor(i,color);
	}
	if (item->CoverPreview.Size()>0) {
		QPixmap pix, icon;
		pix.loadFromData((const uchar*)item->CoverPreview.GetPtr(),item->CoverPreview.Size());
		icon=pix.scaled(64,64,Qt::KeepAspectRatio,Qt::SmoothTransformation);
		item->setIcon(columnCover,icon.copy(0,0,64,16));
	}
	ppl6::CString Tmp;
	Tmp=item->Artist+" - "+item->Title + " ("+item->Version+")";
	item->setText(columnTitle,Tmp);
	item->setText(columnGenre,item->Genre);

	// Rating
	switch (item->rating) {
		case 0: item->setIcon(columnRating,QIcon(":/bewertung/resources/rating-0.png"));
		item->setText(columnRating,"0");
		break;
		case 1: item->setIcon(columnRating,QIcon(":/bewertung/resources/rating-1.png"));
		item->setText(columnRating,"1");
		break;
		case 2: item->setIcon(columnRating,QIcon(":/bewertung/resources/rating-2.png"));
		item->setText(columnRating,"2");
		break;
		case 3: item->setIcon(columnRating,QIcon(":/bewertung/resources/rating-3.png"));
		item->setText(columnRating,"3");
		break;
		case 4: item->setIcon(columnRating,QIcon(":/bewertung/resources/rating-4.png"));
		item->setText(columnRating,"4");
		break;
		case 5: item->setIcon(columnRating,QIcon(":/bewertung/resources/rating-5.png"));
		item->setText(columnRating,"5");
		break;
		case 6: item->setIcon(columnRating,QIcon(":/bewertung/resources/rating-6.png"));
		item->setText(columnRating,"6");
		break;
	}

	// Tonträger
	if (item->titleId) {
		const char Seite[]="?ABCDEFGHIJKLMNOPQRSTUVWXYZ";
		Tmp.Setf("%u %c-%03u", item->DeviceId,Seite[(item->DevicePage<10?item->DevicePage:0)],item->DeviceTrack);
		item->setText(columnSource,Tmp);
		QIcon Icon;
		Tmp.Setf(":/devices16/resources/tr16x16-%04i.png",item->DeviceType);
		Icon.addFile(Tmp);
		item->setIcon(columnSource,Icon);
	} else {
		item->setText(columnSource,item->File);
	}


	if (playlistView==playlistViewNormal) {
		renderTrackViewPlaylist(item);
	} else if (playlistView==playlistViewDJ) {
		renderTrackViewDJ(item);
	}
}

void Playlist::renderTrackViewPlaylist(PlaylistItem *item)
{
	ppl6::CString Tmp;
	Tmp.Setf("%02i:%02i",(int)(item->trackLength/60),(int)item->trackLength%60);
	item->setText(columnLength,Tmp);
}

void Playlist::renderTrackViewDJ(PlaylistItem *item)
{
	ppl6::CString Tmp;
	Tmp.Setf("%i",item->bpm);
	item->setText(columnBpm,Tmp);
	item->setText(columnMusicKey,DataTitle::keyName(item->musicKey));
	Tmp.Setf("%02i:%02i",(int)(item->startPositionSec/60),(int)item->startPositionSec%60);
	item->setText(columnStart,Tmp);
	Tmp.Setf("%02i:%02i",(int)(item->endPositionSec/60),(int)item->endPositionSec%60);
	item->setText(columnEnd,Tmp);
	Tmp.Setf("%02i:%02i",(int)(item->mixLength/60),(int)item->mixLength%60);
	item->setText(columnLength,Tmp);

	int cuts=0;
	for (int i=0;i<5;i++) {
		cuts+=(item->cutEndPosition[i]-item->cutStartPosition[i]);
	}
	Tmp.Setf("%02i:%02i",(int)(cuts/60),(int)cuts%60);
	item->setText(columnCuts,Tmp);

	QColor grey(192,192,192);

	if (item->startPositionSec==0) item->setTextColor(columnStart,grey);
	if (item->endPositionSec==item->trackLength) item->setTextColor(columnEnd,grey);
	if (cuts==0) item->setTextColor(columnCuts,grey);


}

void Playlist::showEvent(QShowEvent * event)
{
	Resize();
	QWidget::showEvent(event);
}
void Playlist::resizeEvent(QResizeEvent * event)
{
	Resize();
	QWidget::resizeEvent(event);
}



void Playlist::ReloadTranslation()
{
	ui.retranslateUi(this);
}

void Playlist::loadPlaylist(ppl6::CString &Filename)
{
	ui.tracks->setSortingEnabled(false);
	if (ui.tracks->load(Filename)) {
		PlaylistFileName=Filename;
		updateLastPlaylist();
		updateRecentPlaylistsMenu();
		ui.playlistName->setText(ui.tracks->getName());
		updatePlaylist();
		changed=false;
	}
	ui.tracks->setSortingEnabled(true);
	ui.tracks->sortByColumn(0,Qt::AscendingOrder);

}

void Playlist::on_menuNew_triggered()
{
	PlaylistFileName.Clear();
	ui.tracks->setName("");
	ui.playlistName->setText("");
	ui.tracks->clear();
	updatePlaylist();
	changed=false;
}

void Playlist::on_menuOpen_triggered()
{
	ppl6::CString Tmp=QFileDialog::getOpenFileName (this, tr("Load Playlist"), wm->conf.LastPlaylistPath,
				tr("Playlists (*.wmp)"));
	if (Tmp.IsEmpty()) return;
	loadPlaylist(Tmp);
}

void Playlist::on_menuOpenRecent0_triggered()
{
	loadPlaylist(wm->conf.LastPlaylists[0]);
}

void Playlist::on_menuOpenRecent1_triggered()
{
	loadPlaylist(wm->conf.LastPlaylists[1]);
}

void Playlist::on_menuOpenRecent2_triggered()
{
	loadPlaylist(wm->conf.LastPlaylists[2]);
}

void Playlist::on_menuOpenRecent3_triggered()
{
	loadPlaylist(wm->conf.LastPlaylists[3]);
}

void Playlist::on_menuOpenRecent4_triggered()
{
	loadPlaylist(wm->conf.LastPlaylists[4]);
}


void Playlist::on_menuSave_triggered()
{
	if (PlaylistFileName.IsEmpty()) {
		on_menuSaveAs_triggered();
		return;
	}
	ui.tracks->setName(ui.playlistName->text());
	ui.tracks->save(PlaylistFileName);
	wm->conf.LastPlaylistPath=ppl6::GetPath(PlaylistFileName);
	updateLastPlaylist();
	updateRecentPlaylistsMenu();
	wm->conf.Save();
	changed=false;
}

void Playlist::on_menuSaveAs_triggered()
{
	ppl6::CString Filename=PlaylistFileName;
	if (Filename.IsEmpty()) Filename=wm->conf.LastPlaylistPath+"/playlist.wmp";

	ppl6::CString Tmp=QFileDialog::getSaveFileName (this, tr("Save WinMusik Playlist"), Filename,
			tr("Playlists (*.wmp)"));
	if (Tmp.IsEmpty()) return;
	PlaylistFileName=Tmp;
	on_menuSave_triggered();
}

void Playlist::updateLastPlaylist()
{
	ppl6::CString Tmp[WM_NUM_LASTPLAYLISTS];
	for (int i=0;i<WM_NUM_LASTPLAYLISTS;i++) Tmp[i]=wm->conf.LastPlaylists[i];
	wm->conf.LastPlaylists[0]=PlaylistFileName;
	int c=1;
	for (int i=0;i<WM_NUM_LASTPLAYLISTS && c<WM_NUM_LASTPLAYLISTS;i++) {
		if(Tmp[i]!=PlaylistFileName) {
			wm->conf.LastPlaylists[c]=Tmp[i];
			c++;
		}
	}
}

void Playlist::on_viewPlaylist_triggered()
{
	if (playlistView!=playlistViewNormal) {
		playlistView=playlistViewNormal;
		recreatePlaylist();
	}
}

void Playlist::on_viewDJ_triggered()
{
	if (playlistView!=playlistViewDJ) {
		playlistView=playlistViewDJ;
		recreatePlaylist();
	}
}

void Playlist::on_tracks_itemDoubleClicked (QTreeWidgetItem * item, int )
{
	Qt::KeyboardModifiers key=QApplication::keyboardModifiers ();
	if (key&Qt::MetaModifier) {
		wm->PlayFile(((PlaylistItem*)item)->File);
		return;
	}

	PlaylistEdit edit(this,wm);
	edit.filloutFields((PlaylistItem*)item);
	if (edit.exec()==1) {
		edit.storeFileds((PlaylistItem*)item);
		changed=true;
		renderTrack((PlaylistItem*)item);
		updateLengthStatus();
	}

}

void Playlist::on_tracks_itemClicked (QTreeWidgetItem * item, int)
{
	Qt::KeyboardModifiers key=QApplication::keyboardModifiers ();

	PlaylistItem *t=(PlaylistItem*)item;
	QClipboard *clipboard = QApplication::clipboard();
	ppl6::CString Text;
	if (key&Qt::MetaModifier) {
		Text.Setf("%s %s",(const char*)t->Artist,(const char*)t->Title);
	} else {
		Text.Setf("%s - %s (%s, %0i:%02i min, %s)",(const char*)t->Artist,(const char*)t->Title,
				(const char*)t->Version, t->trackLength/60,t->trackLength%60, (const char*)t->Genre);
		Text.Concatf(" [%s %u %c-%i]",(const char*)wm->GetDeviceNameShort(t->DeviceType),
				t->DeviceId,(t->DevicePage+'A'-1),t->DeviceTrack);
	}
	clipboard->setText(Text,QClipboard::Clipboard);
	clipboard->setText(Text,QClipboard::Selection);
}
