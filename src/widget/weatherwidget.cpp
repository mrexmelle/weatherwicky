
#include <QComboBox>
#include <QGroupBox>
#include <QPainter>
#include <QPushButton>
#include <QMouseEvent>
#include <QVBoxLayout>
#include <QLabel>
#include <QWidget>
#include <iostream>
#include <math.h>
#include <stdio.h>
#include "weatherwidget.h"
#include "goocurl.h"
#include "bmkgweatherparser.h"

using namespace std;

#define	KIndonesiaMapFile	"data/indonesia.png"
#define	KBmkgIndoWeatherFile	"data/indo_weather.xml"

QWeatherWidget::QWeatherWidget(QWidget * aParent)
{
//	setGeometry(QRect(0,0,1200,800));
//	setWindowFlags(Qt::FramelessWindowHint);

	// construct buttons
	iZoomInBtn = new QPushButton("+", this);
	connect(iZoomInBtn, SIGNAL(pressed()), this, SLOT(OnZoomIn()));
	iZoomInBtn->setGeometry(280, 550, 135, 40);

	iZoomOutBtn = new QPushButton("-", this);
	connect(iZoomOutBtn, SIGNAL(pressed()), this, SLOT(OnZoomOut()));
	iZoomOutBtn->setGeometry(415, 550, 135, 40);


	// fill widget with KTransparentColor16 using iImage
	iImage = QImage(540, 240, QImage::Format_RGB888);
	iImage.fill(0xFFFFFF);
	iLabel = new QLabel(this);
	iLabel->setGeometry(10, 10, 540, 240);
	iLabel->setPixmap(QPixmap::fromImage(iImage));
}

QWeatherWidget::~QWeatherWidget()
{
	if(iLabel) { delete iLabel; iLabel=NULL; }
	if(iZoomInBtn) { delete iZoomInBtn; iZoomInBtn=NULL; }
	if(iZoomOutBtn) { delete iZoomOutBtn; iZoomOutBtn=NULL; }
	if(iGroupBox) { delete iGroupBox; iGroupBox=NULL; }
	if(iComboBox) { delete iComboBox; iComboBox=NULL; }
	if(iMapLbl) { delete iMapLbl; iMapLbl=NULL; }

	goocurl_close();

	bmkgweatherparser_close(&iXml);
}

int QWeatherWidget::Construct()
{
	int status;

	// init goocurl
	status=goocurl_init();
	if(status!=0) { return -1; }
	
	printf("QWeatherWidget::Construct - requesting weather...\n");
	status=bmkgcurl_get_indonesia_weather(KBmkgIndoWeatherFile);
	if(status!=0) { return -3; }

	iXml=bmkgweatherparser_open(KBmkgIndoWeatherFile);
	if(iXml==NULL) { return -4; }

	printf("QWeatherWidget::Construct - parsing...\n");

	status=bmkgweatherparser_query_valid_date(iXml, iValidStartDate, iValidEndDate);
//	printf("QWeatherWidget::Construct - status: %d\n", status);
//	printf("QWeatherWidget::Construct - start: %s\n", iValidStartDate);
//	printf("QWeatherWidget::Construct - end: %s\n", iValidEndDate);

	status=bmkgweatherparser_query_cities(iXml, iBmkgCityList);
	if(status>=0) { iBmkgCityCount=status; }

/*
	for(short i=0; i<iBmkgCityCount; i++)
	{
		printf("%s: %s - %d - %d - %d - %d\n",
			iBmkgCityList[i].iKota,
			iBmkgCityList[i].iCuaca,
			(int)iBmkgCityList[i].iSuhuMin,
			(int)iBmkgCityList[i].iSuhuMax,
			(int)iBmkgCityList[i].iKelembapanMin,
			(int)iBmkgCityList[i].iKelembapanMax);
	}
*/

	iComboBox=new QComboBox(this);
	iComboBox->setGeometry(10, 270, 540, 24);
	for(unsigned short i=0; i<iBmkgCityCount; i++)
	{
		iComboBox->addItem(iBmkgCityList[i].iKota);
	}
	connect(iComboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(OnComboIdxChanged(const QString&)));

	iGroupBox=new QGroupBox("Weather Information", this);
	iGroupBox->setGeometry(10, 300, 270, 240);
	QVBoxLayout * layout=new QVBoxLayout();
	
	iWeatherLbl=new QLabel(this);
	iTempLbl=new QLabel(this);
	iHumidityLbl=new QLabel(this);
	iValidDateLbl=new QLabel(this);

	layout->addWidget(iWeatherLbl);
	layout->addWidget(iTempLbl);
	layout->addWidget(iHumidityLbl);
	layout->addWidget(iValidDateLbl);
	
	iGroupBox->setLayout(layout);

	iMapLbl=new QLabel(this);
	iMapLbl->setGeometry(280, 300, 270, 240);

	printf("QWeaterWidget::Construct - setting weather info\n" );

	SetWeatherInfo();

//	printf("QWeatherWidget::Construct - done\n");

	return 0;
}

void QWeatherWidget::OnComboIdxChanged(const QString& aText)
{
	SetWeatherInfo();
}

void QWeatherWidget::OnZoomIn()
{
	int status;

	if(iSmallMapZoom<20) { iSmallMapZoom++; }
	else { return; }
	status=goocurl_request_static_map_by_string(iCityText, iSmallMapZoom, 270, 240,
		KTypeSatellite, KFormatPng32, 0, NULL, KIndonesiaMapFile);
	if(status!=0) { return; }

//	printf("QWeatherWidget::OnZoomIn - rendering image...\n");
	iImage.load(KIndonesiaMapFile, "PNG");
	iMapLbl->setPixmap(QPixmap::fromImage(iImage));

	if(iSmallMapZoom>=20)
	{
		iZoomInBtn->setEnabled(false);
		iZoomOutBtn->setEnabled(true);
	}
	else if(iSmallMapZoom<=8)
	{
		iZoomInBtn->setEnabled(true);
		iZoomOutBtn->setEnabled(false);
	}
	else
	{
		iZoomInBtn->setEnabled(true);
		iZoomOutBtn->setEnabled(true);
	}
}

void QWeatherWidget::OnZoomOut()
{
	int status;

	if(iSmallMapZoom>8) { iSmallMapZoom--; }
	else { return; }
	status=goocurl_request_static_map_by_string(iCityText, iSmallMapZoom, 270, 240,
		KTypeSatellite, KFormatPng32, 0, NULL, KIndonesiaMapFile);
	if(status!=0) { return; }

//	printf("QWeatherWidget::OnZoomIn - rendering image...\n");
	iImage.load(KIndonesiaMapFile, "PNG");
	iMapLbl->setPixmap(QPixmap::fromImage(iImage));

	if(iSmallMapZoom>=20)
	{
		iZoomInBtn->setEnabled(false);
		iZoomOutBtn->setEnabled(true);
	}
	else if(iSmallMapZoom<=8)
	{
		iZoomInBtn->setEnabled(true);
		iZoomOutBtn->setEnabled(false);
	}
	else
	{
		iZoomInBtn->setEnabled(true);
		iZoomOutBtn->setEnabled(true);
	}
}

void QWeatherWidget::SetWeatherInfo()
{
	char text[64];
	char marker[64];
	int status;

	for(unsigned short i=0; i<iBmkgCityCount; i++)
	{
		if(strcasecmp(iComboBox->itemText(iComboBox->currentIndex()).toStdString().c_str(), iBmkgCityList[i].iKota)==0)
		{
			printf("QWeatherWidget::SetWeatherInfo - requesting image...\n");
			
			memset(iCityText, 0x0, 32);
			sprintf(iCityText, "%s,ID",  iBmkgCityList[i].iKota);
			for(char u=0; u<strlen(iBmkgCityList[i].iKota); u++)
			{
				if(iCityText[u]==' ') { iCityText[u]='+'; }
			}

			status=goocurl_get_marker_descriptor_by_string(iCityText, KMarkerSizeTiny, 0xff000000, 'a',
				marker, 32);
			if(status!=0) { return; }

			status=goocurl_request_static_map_by_lat_lon(-1.5, 118.25, 4, 540, 240,
				KTypeSatellite, KFormatPng32, 0, marker, KIndonesiaMapFile);
			if(status!=0) { return; }

			printf("QWeatherWidget::SetWeatherInfo - rendering image...\n");
			iImage.load(KIndonesiaMapFile, "PNG");
			iLabel->setPixmap(QPixmap::fromImage(iImage));

			iSmallMapZoom=14;

			status=goocurl_request_static_map_by_string(iCityText, iSmallMapZoom, 270, 240,
				KTypeSatellite, KFormatPng32, 0, NULL, KIndonesiaMapFile);
			if(status!=0) { return; }

			iZoomInBtn->setEnabled(true);
			iZoomOutBtn->setEnabled(true);

			printf("QWeatherWidget::SetWeatherInfo - rendering image...\n");
			iImage.load(KIndonesiaMapFile, "PNG");
			iMapLbl->setPixmap(QPixmap::fromImage(iImage));

			memset(text, 0x0, 64);
			sprintf(text, "Cuaca:\n%s",  iBmkgCityList[i].iCuaca);
			iWeatherLbl->setText(text);

			memset(text, 0x0, 64);
			sprintf(text, "Suhu:\n%d - %d degC", (int)(iBmkgCityList[i].iSuhuMin), (int)(iBmkgCityList[i].iSuhuMax));
			iTempLbl->setText(text);

			memset(text, 0x0, 64);
			sprintf(text, "Kelembapan:\n%d - %d %%",  (int)(iBmkgCityList[i].iKelembapanMin), (int)(iBmkgCityList[i].iKelembapanMax));
			iHumidityLbl->setText(text);

			memset(text, 0x0, 64);
			sprintf(text, "Berlaku:\n%s - %s\n(BMKG)", iValidStartDate, iValidEndDate);
			iValidDateLbl->setText(text);
		}
	}
}

