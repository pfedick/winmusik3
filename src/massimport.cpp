/*
 * This file is part of WinMusik 3 by Patrick Fedick
 *
 * $Author: pafe $
 * $Revision: 1.1 $
 * $Date: 2010/11/14 13:20:11 $
 * $Id: playlists.cpp,v 1.1 2010/11/14 13:20:11 pafe Exp $
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
#include "../include/massimport.h"

MassImport::MassImport(QWidget *parent, CWmClient *wm)
    : QDialog(parent)
{
	ui.setupUi(this);
	this->wm=wm;
	searchWindow=NULL;
	currentTrackListItem=NULL;
	position=oldposition=0;

	ui.treeWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
	ui.treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    QString Style="QTreeView::item {\n"
    		"border-right: 1px solid #b9b9b9;\n"
    		"border-bottom: 1px solid #b9b9b9;\n"
    		"}\n"
    		"QTreeView::item:selected {\n"
    		//"border-top: 1px solid #80c080;\n"
    		//"border-bottom: 1px solid #80c080;\n"
    		"background: #000070;\n"
    		"color: rgb(255, 255, 255);\n"
    		"}\n"
    		"";
    ui.treeWidget->setStyleSheet(Style);

	TCVersion.Title=tr("Version");
	TCVersion.Init(this,wm,ui.versionId,ui.version,&wm->VersionStore);
	TCVersion.SetNextWidget(ui.versionApplyButton);

	TCGenre.Title=tr("Genre");
	TCGenre.Init(this,wm,ui.genreId,ui.genre,&wm->GenreStore);
	TCGenre.SetNextWidget(ui.genreApplyButton);

	TCLabel.Title=tr("Label");
	TCLabel.Init(this,wm,ui.labelId,ui.labelName,&wm->LabelStore);
	TCLabel.SetNextWidget(ui.labelApplyButton);

	TCRecordSource.Title=tr("Record Source");
	TCRecordSource.Init(this,wm,ui.recordSourceId,ui.recordSource,&wm->RecordSourceStore);
	TCRecordSource.SetNextWidget(ui.recordingSourceApplyButton);

	InstallFilter(ui.versionId,6);
	InstallFilter(ui.version,7);
	InstallFilter(ui.genreId,8);
	InstallFilter(ui.genre,9);
	InstallFilter(ui.labelId,15);
	InstallFilter(ui.labelName,16);
	InstallFilter(ui.recordSourceId,17);
	InstallFilter(ui.recordSource,18);
	//InstallFilter(ui.recordDeviceId,19);
	//InstallFilter(ui.recordDevice,20);



}

MassImport::~MassImport()
{
}

void MassImport::setSearchWindow(QWidget *widget)
{
	searchWindow=widget;
}

QWidget *MassImport::getSearchWindow() const
{
	return searchWindow;
}


void MassImport::Resize()
{
	int w=ui.treeWidget->width();
	ui.treeWidget->setColumnWidth(0,60);
	ui.treeWidget->setColumnWidth(1,64);
	ui.treeWidget->setColumnWidth(5,80);	// Length
	ui.treeWidget->setColumnWidth(6,50);	// Dupe
	ui.treeWidget->setColumnWidth(7,40);	// Action
	w=w-60-64-80-50-40-5*4;
	if (w<200) w=200;
	ui.treeWidget->setColumnWidth(2,w*55/100);
	ui.treeWidget->setColumnWidth(3,w*30/100);
	ui.treeWidget->setColumnWidth(4,w*15/100);
}

void MassImport::showEvent(QShowEvent * event)
{
	Resize();
	QDialog::showEvent(event);
}

void MassImport::resizeEvent(QResizeEvent * event)
{
	Resize();
	QDialog::resizeEvent(event);
}



void MassImport::ReloadTranslation()
{
	ui.retranslateUi(this);
}

void MassImport::InstallFilter(QObject *object, int id)
{
	object->installEventFilter(this);
	object->setProperty("id",id);
}

bool MassImport::eventFilter(QObject *target, QEvent *event)

{
	if (consumeEvent(target,event)) return true;
	return QDialog::eventFilter(target,event);
}

bool MassImport::consumeEvent(QObject *target, QEvent *event)
{
	ppl6::CString Tmp;
	QKeyEvent *keyEvent=NULL;
	int key=0;
	int modifier=Qt::NoModifier;
	QFocusEvent *focusEvent=NULL;

	// Id auslesen
	int id=target->property("id").toInt();
	int type=event->type();
	if (type==QEvent::KeyPress) {
		keyEvent= static_cast<QKeyEvent *>(event);
		key=keyEvent->key();
		modifier=keyEvent->modifiers();
		//if (on_KeyPress(target,key,modifier)) return true;		// Fkeys und andere Steuerkeys prüfen
	} else if (type==QEvent::FocusIn || type==QEvent::FocusOut) {
		focusEvent=static_cast<QFocusEvent *>(event);
		if (type==QEvent::FocusIn) {
			position=id;
		}
		else if (type==QEvent::FocusOut) {
			oldposition=id;
		}
	}

	if (target==ui.version || target==ui.versionId) {
		return TCVersion.ConsumeEvent(target,event,oldposition,position);
	} else if (target==ui.genre || target==ui.genreId) {
		return TCGenre.ConsumeEvent(target,event,oldposition,position);
	} else if (target==ui.labelName || target==ui.labelId) {
		return TCLabel.ConsumeEvent(target,event,oldposition,position);
	} else if (target==ui.recordSource || target==ui.recordSourceId) {
		return TCRecordSource.ConsumeEvent(target,event,oldposition,position);
	}
	return false;
}


int MassImport::load(ppluint8 DeviceType, ppluint32 DeviceId, ppluint8 Page, ppluint16 StartTrack)
{

	this->DeviceType=DeviceType;
	this->DeviceId=DeviceId;
	this->Page=Page;
	this->StartTrack=StartTrack;

	ppl6::CString Path, Filename, Pattern;
	ppl6::CString MP3Path=wm->conf.DevicePath[DeviceType];

	if (MP3Path.IsEmpty()) return 0;
	Path=MP3Path;
	Path.RTrim("/");
	Path.RTrim("\\");
	Path.Concatf("/%02u/%03u/",(ppluint32)(DeviceId/100),DeviceId);
	Pattern.Setf("*.mp3");
	ppl6::CDir Dir;
	ppl6::CDirEntry *entry;
	int count=0;
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	qApp->processEvents();
	ui.treeWidget->setSortingEnabled(false);

	if (Dir.Open(Path,ppl6::CDir::Sort_Filename_IgnoreCase)) {
		if (wmlog) wmlog->Printf(ppl6::LOG::DEBUG,5,"CWMClient","NextMP3File",__FILE__,__LINE__,"%i Dateien vorhanden, suche nach Pattern...",Dir.Num());
		while ((entry=Dir.GetNextPattern(Pattern,true))) {
			Filename=entry->Filename;
			// Der Dateiname darf nicht mit drei Ziffern und Bindestrich beginnen
			if (!Filename.PregMatch("/^[0-9]{3}\\-.*/")) {
				// Muss aber mit .mp3 enden
				if (Filename.PregMatch("/^.*\\.mp3$/i")) {
					//printf ("%s\n",(const char*)Filename);
					addTrack(entry->File);
					count++;
				}
			}
		}
	}
	ui.treeWidget->sortByColumn(0,Qt::AscendingOrder);
	ui.treeWidget->setSortingEnabled(true);
	QApplication::restoreOverrideCursor();
	if (count) return 1;
	QMessageBox::information(this, tr("WinMusik: Notice"),
			tr("There are no tracks to import"));
	return 0;

}

void MassImport::addTrack(const ppl6::CString Filename)
{
	//printf ("%s\n",(const char*)Filename);
	ppl6::CString Tmp;
	TreeItem *item=new TreeItem;
	item->Filename=Filename;
	item->dupePresumption=0;
	item->import=true;
	getTrackInfoFromFile(item->info,Filename,wm->conf.ReadId3Tag);
	ppl6::CString Key;
	Key=item->info.Artist+" "+item->info.Title+" "+item->info.Version;
	Key.LCase();

	std::set<ppl6::CString>::iterator it;
	it=LocalDupeCheck.find(Key);
	if (it!=LocalDupeCheck.end()) {
		// Den Titel haben wir scheinbar schon
		item->dupePresumption=100;
		item->import=false;
	} else {
		LocalDupeCheck.insert(Key);
		CTitleHashTree Result;
		wm->Hashes.Find(item->info.Artist,item->info.Title,item->info.Version,"","",Result);
		if (Result.Num()>1) {
			item->dupePresumption=100;
			item->import=false;
		} else if (Result.Num()>0) {
			item->dupePresumption=90;
			item->import=false;
		} else {
			wm->Hashes.Find(item->info.Artist,item->info.Title,Result);
			if (Result.Num()>3) {
				item->dupePresumption=70;
				item->import=true;
			} else if (Result.Num()>0) {
				item->dupePresumption=40;
				item->import=true;
			}
		}
	}
	Tmp.Setf("%5i",ui.treeWidget->topLevelItemCount()+1);
	item->setText(0,Tmp);
	renderTrack(item);


	ui.treeWidget->addTopLevelItem(item);
	qApp->processEvents();
}

void MassImport::renderTrack(TreeItem *item)
{
	QBrush Brush(Qt::SolidPattern);
	Brush.setColor("red");

	ppl6::CString Tmp;
	if (item->info.Cover.GetSize()>0) {
		QPixmap pix, icon;
		pix.loadFromData((const uchar*)item->info.Cover.GetPtr(),item->info.Cover.GetSize());
		icon=pix.scaled(64,64,Qt::KeepAspectRatio,Qt::SmoothTransformation);
		item->setIcon(1,icon.copy(0,0,64,16));
	} else {
		item->setIcon(1,QIcon());
	}

	Tmp=item->info.Artist+" - "+item->info.Title;
	item->setText(2,Tmp);
	item->setText(3,item->info.Version);
	item->setForeground(3,Brush);
	item->setText(4,item->info.Genre);

	Tmp.Setf("%4i:%02i",(int)(item->info.Length/60),item->info.Length%60);
	item->setText(5,Tmp);

	Tmp.Setf("%i %%",item->dupePresumption);
	item->setText(6,Tmp);
	if (item->import) item->setIcon(7,QIcon(":/icons/resources/button_ok.png"));
	else item->setIcon(7,QIcon(":/icons/resources/edit-delete.png"));


}

void MassImport::on_treeWidget_customContextMenuRequested ( const QPoint & pos )
{
    QPoint p=ui.treeWidget->mapToGlobal(pos);
    currentTrackListItem=(TreeItem*)ui.treeWidget->itemAt(pos);
    if (!currentTrackListItem) return;
	QMenu *m=new QMenu(this);
	QAction *a=m->addAction (QIcon(":/icons/resources/findmore.png"),tr("Find other versions","trackList Context Menue"),this,SLOT(on_contextFindMoreVersions_triggered()));
	m->addAction (QIcon(":/icons/resources/play.png"),tr("Play Track","trackList Context Menue"),this,SLOT(on_contextPlayTrack_triggered()));
	m->addAction (QIcon(":/icons/resources/edit.png"),tr("Edit Track","trackList Context Menue"),this,SLOT(on_contextEditTrack_triggered()));
	m->addAction (QIcon(":/icons/resources/delete-track.png"),tr("Delete Track","trackList Context Menue"),this,SLOT(on_contextDeleteTrack_triggered()));
	m->popup(p,a);
}

void MassImport::on_treeWidget_itemDoubleClicked ( QTreeWidgetItem * item, int )
{
	TreeItem *ti=(TreeItem*) item;
	if (!ti) return;
	wm->PlayFile(ti->Filename);
}

void MassImport::on_treeWidget_itemClicked ( QTreeWidgetItem * item, int column)
{
	TreeItem *ti=(TreeItem*) item;
	if (!ti) return;
	if (column==7) {
		ti->import=!ti->import;
		if (ti->import) item->setIcon(7,QIcon(":/icons/resources/button_ok.png"));
		else ti->setIcon(7,QIcon(":/icons/resources/edit-delete.png"));

	}
}


void MassImport::on_contextFindMoreVersions_triggered()
{
	if (!currentTrackListItem) return;
	searchWindow=wm->OpenOrReuseSearch(searchWindow,currentTrackListItem->info.Artist,
			currentTrackListItem->info.Title);
}

void MassImport::on_contextPlayTrack_triggered()
{
	if (!currentTrackListItem) return;
	wm->PlayFile(currentTrackListItem->Filename);
}

void MassImport::on_contextEditTrack_triggered()
{
	if (!currentTrackListItem) return;
}

void MassImport::on_contextDeleteTrack_triggered()
{
	if (!currentTrackListItem) return;
}

void MassImport::on_versionApplyButton_clicked()
{
	QList<QTreeWidgetItem *> list;
	TreeItem *item;
	list=ui.treeWidget->selectedItems();
	for (int i = 0; i < list.size(); ++i) {
		item=(TreeItem*)list.at(i);
		if (item) {
			item->info.Version=ui.version->text();
			renderTrack(item);
		}
	}
}

void MassImport::on_genreApplyButton_clicked()
{
	QList<QTreeWidgetItem *> list;
	TreeItem *item;
	list=ui.treeWidget->selectedItems();
	for (int i = 0; i < list.size(); ++i) {
		item=(TreeItem*)list.at(i);
		if (item) {
			item->info.Genre=ui.genre->text();
			renderTrack(item);
		}
	}
}

void MassImport::on_labelApplyButton_clicked()
{
	QList<QTreeWidgetItem *> list;
	TreeItem *item;
	list=ui.treeWidget->selectedItems();
	for (int i = 0; i < list.size(); ++i) {
		item=(TreeItem*)list.at(i);
		if (item) {
			item->info.Label=ui.label->text();
			renderTrack(item);
		}
	}
}

void MassImport::on_recordingSourceApplyButton_clicked()
{
	QList<QTreeWidgetItem *> list;
	TreeItem *item;
	list=ui.treeWidget->selectedItems();
	for (int i = 0; i < list.size(); ++i) {
		item=(TreeItem*)list.at(i);
		if (item) {
			item->info.RecordingSource=ui.recordSource->text();
			renderTrack(item);
		}
	}
}

void MassImport::on_albumApplyButton_clicked()
{
	QList<QTreeWidgetItem *> list;
	TreeItem *item;
	list=ui.treeWidget->selectedItems();
	for (int i = 0; i < list.size(); ++i) {
		item=(TreeItem*)list.at(i);
		if (item) {
			item->info.Album=ui.album->text();
			renderTrack(item);
		}
	}
}

void MassImport::on_tagsApplyButton_clicked()
{
	QList<QTreeWidgetItem *> list;
	TreeItem *item;
	list=ui.treeWidget->selectedItems();
	for (int i = 0; i < list.size(); ++i) {
		item=(TreeItem*)list.at(i);
		if (item) {
			item->info.Tags=ui.tags->text();
			renderTrack(item);
		}
	}
}

void MassImport::on_markImportSelectedTracksButton_clicked()
{
	QList<QTreeWidgetItem *> list;
	TreeItem *item;
	list=ui.treeWidget->selectedItems();
	for (int i = 0; i < list.size(); ++i) {
		item=(TreeItem*)list.at(i);
		if (item) {
			item->import=true;
			renderTrack(item);
		}
	}
}

void MassImport::on_markIgnoreSelectedTracksButton_clicked()
{
	QList<QTreeWidgetItem *> list;
	TreeItem *item;
	list=ui.treeWidget->selectedItems();
	for (int i = 0; i < list.size(); ++i) {
		item=(TreeItem*)list.at(i);
		if (item) {
			item->import=false;
			renderTrack(item);
		}
	}
}

void MassImport::on_deleteSelectedTracksButton_clicked()
{

}

void MassImport::on_importSelectedTracksButton_clicked()
{

}


