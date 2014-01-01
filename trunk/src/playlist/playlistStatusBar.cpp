/*
 * playlistStatusBar.cpp
 *
 *  Created on: 01.01.2014
 *      Author: patrick
 */

#include "playlistStatusBar.h"
#include <QHBoxLayout>
#include "winmusik3.h"


PlaylistStatusBar::PlaylistStatusBar(QWidget *parent)
	: QFrame(parent)
{
	setupUi();
}

PlaylistStatusBar::~PlaylistStatusBar()
{

}

static void addSpacer(QLayout *layout)
{
	QFrame *line = new QFrame();
	line->setGeometry(QRect(320, 150, 118, 3));
	line->setFrameShape(QFrame::VLine);
	line->setFrameShadow(QFrame::Sunken);
	layout->addWidget(line);
}

static QHBoxLayout *createPanel(QLayout *layout)
{
	QHBoxLayout *l=new QHBoxLayout;
	l->setContentsMargins(2,2,2,2);

	QFrame *frame=new QFrame;
	frame->setFrameShape(QFrame::Panel);
	frame->setFrameShadow(QFrame::Sunken);
	frame->setLayout(l);
	if (layout) layout->addWidget(frame);
	return l;
}


void PlaylistStatusBar::setupUi()
{
	QHBoxLayout *panel;
	QHBoxLayout *layout=new QHBoxLayout;
	layout->setContentsMargins(2,2,2,2);
	QLabel *label;

	// Search
	setupSearch(layout);
	layout->addStretch(1);

	// MusicKey selection
	panel=createPanel(layout);

	musicKeyLabel=new QLabel(tr("Musical Key:"));
	panel->addWidget(musicKeyLabel);
	displayMusicKey=new QComboBox();
	displayMusicKey->addItem(tr("musical sharps"));
	displayMusicKey->addItem(tr("open key"));
	displayMusicKey->addItem(wm_main->conf.customMusicKeyName);
	panel->addWidget(displayMusicKey);
	connect(displayMusicKey,SIGNAL(currentIndexChanged(int)),
			this, SLOT(on_displayMusicKey_currentIndexChanged(int)));
	displayMusicKey->setEnabled(false);

	musicKeyDisplay=wm_main->conf.musicKeyDisplay;
	switch (musicKeyDisplay) {
		case musicKeyTypeMusicalSharps: displayMusicKey->setCurrentIndex(0); break;
		case musicKeyTypeOpenKey: displayMusicKey->setCurrentIndex(1); break;
		case musicKeyTypeCustom: displayMusicKey->setCurrentIndex(2); break;
		default: displayMusicKey->setCurrentIndex(1); break;
	}

	// Selection
	panel=createPanel(layout);

	// Selected Tracks
	label=new QLabel(tr("selected tracks:"));
	panel->addWidget(label);
	selectedTracks=new QLabel();
	selectedTracks->setFrameShadow(QFrame::Sunken);
	selectedTracks->setFrameShape(QFrame::StyledPanel);
	selectedTracks->setMinimumWidth(60);
	panel->addWidget(selectedTracks);
	addSpacer(panel);

	// Selected Track length
	label=new QLabel(tr("length:"));
	panel->addWidget(label);
	selectedTrackLength=new QLabel();
	selectedTrackLength->setFrameShadow(QFrame::Sunken);
	selectedTrackLength->setFrameShape(QFrame::StyledPanel);
	selectedTrackLength->setMinimumWidth(80);
	panel->addWidget(selectedTrackLength);
	addSpacer(panel);

	// Selected Mix length
	label=new QLabel(tr("mixLength:"));
	panel->addWidget(label);
	selectedMixLength=new QLabel();
	selectedMixLength->setFrameShadow(QFrame::Sunken);
	selectedMixLength->setFrameShape(QFrame::StyledPanel);
	selectedMixLength->setMinimumWidth(80);
	panel->addWidget(selectedMixLength);


	// Total
	panel=createPanel(layout);

	// Total Tracks
	label=new QLabel(tr("total tracks:"));
	panel->addWidget(label);
	totalTracks=new QLabel();
	totalTracks->setFrameShadow(QFrame::Sunken);
	totalTracks->setFrameShape(QFrame::StyledPanel);
	totalTracks->setMinimumWidth(60);
	panel->addWidget(totalTracks);
	addSpacer(panel);

	// Total Track length
	label=new QLabel(tr("length:"));
	panel->addWidget(label);
	totalTrackLength=new QLabel();
	totalTrackLength->setFrameShadow(QFrame::Sunken);
	totalTrackLength->setFrameShape(QFrame::StyledPanel);
	totalTrackLength->setMinimumWidth(80);
	panel->addWidget(totalTrackLength);
	addSpacer(panel);

	// Total Mix length
	label=new QLabel(tr("mixLength:"));
	panel->addWidget(label);
	totalMixLength=new QLabel();
	totalMixLength->setFrameShadow(QFrame::Sunken);
	totalMixLength->setFrameShape(QFrame::StyledPanel);
	totalMixLength->setMinimumWidth(80);
	panel->addWidget(totalMixLength);


	this->setContentsMargins(0,0,0,0);
	this->setLayout(layout);
}

void PlaylistStatusBar::setupSearch(QLayout *layout)
{
	QHBoxLayout *panel=createPanel(layout);
	QLabel *label;
	panel->setEnabled(false);			// TODO: Enablen, wenn fertig implementiert
	label=new QLabel(tr("search:"));
	panel->addWidget(label);

	searchText=new QLineEdit;
	panel->addWidget(searchText);

	searchBackward=new QToolButton();
	searchBackward->setIcon(QIcon(":/icons/resources/previous.png"));
	panel->addWidget(searchBackward);

	searchForward=new QToolButton();
	searchForward->setIcon(QIcon(":/icons/resources/next.png"));
	panel->addWidget(searchForward);
}

void PlaylistStatusBar::ReloadTranslation()
{

}

void PlaylistStatusBar::setMusicKeySelectionEnabled(bool flag)
{
	musicKeyLabel->setEnabled(flag);
	displayMusicKey->setEnabled(flag);
}

void PlaylistStatusBar::setTotalLength(int sec)
{
	setReadableLength(totalTrackLength,sec);
}

void PlaylistStatusBar::setTotalTracks(int count)
{
	ppl6::CString Tmp;
	Tmp.Setf("%d",count);
	totalTracks->setText(Tmp);
}

void PlaylistStatusBar::setMixLength(int sec)
{
	setReadableLength(totalMixLength,sec);
}

void PlaylistStatusBar::setSelectedLength(int sec)
{
	setReadableLength(selectedTrackLength,sec);
}

void PlaylistStatusBar::setSelectedTracks(int count)
{
	ppl6::CString Tmp;
	Tmp.Setf("%d",count);
	selectedTracks->setText(Tmp);
}

void PlaylistStatusBar::setSelectedMixLength(int sec)
{
	setReadableLength(selectedMixLength,sec);
}


void PlaylistStatusBar::on_displayMusicKey_currentIndexChanged(int)
{
	switch (displayMusicKey->currentIndex()) {
		case 0: musicKeyDisplay=musicKeyTypeMusicalSharps; break;
		case 1: musicKeyDisplay=musicKeyTypeOpenKey; break;
		case 2: musicKeyDisplay=musicKeyTypeCustom; break;
		default: musicKeyDisplay=musicKeyTypeOpenKey; break;
	}
	emit musicKeySelectionChanged(musicKeyDisplay);
}

