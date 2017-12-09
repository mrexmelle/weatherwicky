
#include <iostream>
#include <stdio.h>
#include <QApplication>
#include "weatherwidget.h"

using namespace std;

int main(int argc , char * argv[])
{
	QApplication a(argc, argv);
	QWeatherWidget w;
	w.setGeometry(0, 0, 560, 600);
	w.Construct();
 	w.show();
 	return a.exec();
}
