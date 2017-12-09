
#ifndef __Q_WEATHERWIDGET_H__
#define __Q_WEATHERWIDGET_H__

#include <QWidget>
#include <QLabel>
#include "bmkgweatherparser.h"

class QPushButton;
class QGroupBox;
class QComboBox;

class QWeatherWidget : public QWidget
{
	Q_OBJECT

public:
	QWeatherWidget(QWidget * aParent = NULL);
	~QWeatherWidget();
	int Construct();
	void SetWeatherInfo();

private:
	xmlDoc * iXml;

	// bmkg info
	char iValidStartDate[24];
	char iValidEndDate[24];
	SBmkgCity iBmkgCityList[128];
	int iBmkgCityCount;

protected:
	// Zoom
	QPushButton * iZoomInBtn;
	QPushButton * iZoomOutBtn;

	// for big map
	QLabel * iLabel;
	QImage iImage;

	// for city selection
	QComboBox * iComboBox;

	// for group box
	QGroupBox * iGroupBox;

	// for weather info
	QLabel * iMapLbl;
	QLabel * iWeatherLbl;
	QLabel * iTempLbl;
	QLabel * iHumidityLbl;
	QLabel * iValidDateLbl;

	// small map
	char iSmallMapZoom;
	char iCityText[32];

// button event handler
private slots:
	void OnComboIdxChanged(const QString& aText);
	void OnZoomIn();
	void OnZoomOut();
};

#endif // __Q_WEATHERWIDGET_H__

