/*
 * This file is part of WinMusik 3 by Patrick Fedick
 *
 * Copyright (c) 2022 Patrick Fedick
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


#include "oimpinfo.h"

OimpInfo::OimpInfo(QWidget* parent, DataOimp* oimp)
	: QWidget(parent)
{
	ui.setupUi(this);
	ui.tabWidget->setCurrentIndex(1);
	if (oimp) CopyData(oimp);

}

OimpInfo::~OimpInfo()
{

}

void OimpInfo::CopyData(DataOimp* oimp)
{
	ppl7::String Tmp;

	ui.filename->setText(oimp->Filename);

	Tmp=oimp->ID3v2.getString("artist");
	ui.v2Artist->setText(Tmp);
	Tmp=oimp->ID3v2.getString("title");
	ui.v2Title->setText(Tmp);
	Tmp=oimp->ID3v2.getString("album");
	ui.v2Album->setText(Tmp);
	Tmp=oimp->ID3v2.getString("year");
	ui.v2Year->setText(Tmp);
	Tmp=oimp->ID3v2.getString("comment");
	ui.v2Comment->setText(Tmp);
	Tmp=oimp->ID3v2.getString("genre");
	ui.v2Genre->setText(Tmp);
	Tmp=oimp->ID3v2.getString("remixer");
	ui.v2Remixer->setText(Tmp);

	Tmp=oimp->ID3v1.getString("artist");
	ui.v1Artist->setText(Tmp);
	Tmp=oimp->ID3v1.getString("title");
	ui.v1Title->setText(Tmp);
	Tmp=oimp->ID3v1.getString("album");
	ui.v1Album->setText(Tmp);
	Tmp=oimp->ID3v1.getString("year");
	ui.v1Year->setText(Tmp);
	Tmp=oimp->ID3v1.getString("comment");
	ui.v1Comment->setText(Tmp);
	Tmp=oimp->ID3v1.getString("genre");
	ui.v1Genre->setText(Tmp);

}

void OimpInfo::on_deleteButton_clicked()
{
	close();
}

void OimpInfo::on_closeButton_clicked()
{
	close();
}
