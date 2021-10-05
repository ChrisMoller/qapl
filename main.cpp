#include <apl/libapl.h>

#include "mainwindow.h"
#include "optionstrings.h"

#include <QApplication>
#include <QCommandLineParser>

int main(int argc, char *argv[])
{
  init_libapl ("apl", 0);

  QApplication app(argc, argv);
  QCoreApplication::setOrganizationName(APPLICATION_ORGANISATION);
  QCoreApplication::setApplicationName(APPLICATION_NAME);
  QCoreApplication::setApplicationVersion(APPLICATION_VERSION);

  QCommandLineParser parser;
  parser.setApplicationDescription("APL Environment");
  parser.addHelpOption();
  parser.addVersionOption();

  QCommandLineOption noCONT (OPT_noCONT, "Skip CONTINUE workspace.");
  parser.addOption(noCONT);

  QCommandLineOption loadws(OPT_L, "Workspace to load.", "<ws>");
  parser.addOption(loadws);

  parser.process(app);
  
  MainWindow w (parser);
  w.show();
  return app.exec();
}
