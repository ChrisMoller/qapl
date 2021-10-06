#include <QRegularExpression>
#include <apl/libapl.h>

#include "mainwindow.h"
#include "aplexec.h"

#define LAMBDA_HEADER "lambda_"
#define expvar "expvarλ"

/*******  WARNING:  TINKER WITH THESE ONLY IF YOU'RE A MASOCHIST!!  *******/
#define CLINE_RE "(([[:alpha:]][[:alnum:]]*)\\s*)"
      // 0 = whole thing
      // 1 = cmd with trailing spaces
      // 2 = cmd

#define STRING_RE \
  "(([^[:space:]'\"]+)[[:space:]]*)|\
('([^']*)'[[:space:]]*)|\
(\"([^\"]*)\"[[:space:]]*)"

static const QRegularExpression cre (CLINE_RE,
                               QRegularExpression::CaseInsensitiveOption);
static const QRegularExpression sre (STRING_RE,
                               QRegularExpression::CaseInsensitiveOption);

QStringList parseCl (QString str)
{
  QStringList args;
  QRegularExpressionMatch match = cre.match (str);
  if (match.hasMatch ()) {
    QStringList matches = match.capturedTexts ();
    int offset = match.capturedLength (0);
    str.remove (0, offset);
#if 1
    args << matches[2];
#else
    real_ed = matches[2];
#endif
    while (!str.isEmpty ()) {
      match = sre.match (str);
      if (match.hasMatch ()) {
	matches = match.capturedTexts ();
	offset = match.capturedLength (0);
	str.remove (0, offset);
	int count = matches.size ();
	args << matches[count - 1];
      }
    }
  }
#if 0
  for (int i = 0; i < args.size (); i++)
    fprintf (stderr, "arg %d \"%s\"\n", i, toCString (args[i]));
#endif
  return args;
}

void  MainWindow::edit_fcn (QString text)
{
  QString outString;
  QString errString;

   bool isLambda = text.startsWith (QString ("∇∇"));
   bool isNew = true;
   text = text.remove (0, (isLambda ? 2 : 1)).trimmed ();

   QString cmd = QString ("⎕cr '%1'").arg(text);
   QStringList list;
   AplExec::aplExec (APL_OP_EXEC, cmd, outString, errString);
   if (!outString.isEmpty ()) {
     list = outString.split (QChar ('\n'));
     isLambda |= list.first ().contains (QString ("λ"));
     isNew = false;
   }
   QString fn;
   if (isLambda) {
     if (isNew || ((list.size () == 3) && (list[2].isEmpty ())))
       fn = QString ("%1/%2%3.apl").
	 arg (tempdir.path (), LAMBDA_HEADER, text);
     else if ((list.size () != 0) && (list.size () != 2)){
       QMessageBox msgBox (QMessageBox::Warning,
			   QString ("Invalid lambda"),
  QString ("A lambda must consist of exactly one expession on one line."));
       msgBox.exec();
       // fixme invalid lambda
       return;
     }	
   }
   else
     fn = QString ("%1/%2.apl").arg (tempdir.path (), text);
   QFile file (fn);
   if  (file.open (QIODevice::WriteOnly | QIODevice::Text)) {
     QTextStream out(&file);
     if (outString.isEmpty ()) {
       if (!isLambda)
	 out << text;                  // boilerplate fcn header
     }
     else {
       if (isLambda && !list.isEmpty ()) {
	 QString fn = list[1];
	 fn = fn.trimmed ();
	 fn.remove (0, 2);
	 out << fn;
       }
       else if (!outString.isEmpty ())
	 out << outString;
     }
     file.close ();

     {
       QStringList args;
       QString real_ed;
       QString editor_copy
	 = editor.isEmpty () ? QString (DEFAULT_EDITOR) : editor;
       args = parseCl (editor_copy);
       if (!args.isEmpty ()) {
	 args << fn;
	 real_ed = args[0];
	 args.removeFirst ();
	 QProcess *edit = new QProcess ();
	 connect (edit, &QProcess::started,
		  [=]() {
		    qint64 pid = edit->processId ();
		    processList.append (pid);
		  });
	 watcher.addPath (fn);
	 edit->start (real_ed, args);
       }
     }
   }
   else {
     // fixme file open error
   }
}

void  MainWindow::show_fcn (QString text)
{
  QString outString;
  QString errString;
  
  outputLog->append ("      " + text);
  QRegularExpression aplsep (APL_VARIABLE);
  aplsep.setPatternOptions (QRegularExpression::CaseInsensitiveOption);
  QRegularExpressionMatchIterator toks = aplsep.globalMatch (text);
  if (toks.hasNext ()) {
    QRegularExpressionMatch match = toks.next();
    QString tok = match.captured(1);
    QString cmd = QString ("⎕cr '%1'").arg(tok);
    AplExec::aplExec (APL_OP_EXEC, cmd, outString, errString);
    update_screen (errString, outString);
  }
}


void
MainWindow::fileChanged(const QString &path)
{
  QFile file(path);
  if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QString outString;
    QString errString;
    QTextStream in(&file);
    QStringList fcn;
    int len = 0;
    while (!in.atEnd()) {
      QString line = in.readLine();
      if (len < line.size ()) len = line.size ();
      fcn += line;
    }

    if (path.contains (LAMBDA_HEADER)) {
      if (fcn.size () == 1) {
        QFileInfo info (file);
        QString name = info.baseName ();
        name.remove (0, QString (LAMBDA_HEADER).size ());
        QString stmt = fcn.last ().trimmed ();
        QString cmd = QString (")erase %1").arg (name);
        AplExec::aplExec (APL_OP_EXEC, cmd, outString, errString);
        cmd = QString ("%1←{%2}").arg (name, stmt);
        AplExec::aplExec (APL_OP_EXEC, cmd, outString, errString);
        update_screen (errString, outString);
      }
      else {
        //fixme invalid lambda
        return;
      }
    }
    else {
      int i;
      QString mtx;
      for (i = 0; i < fcn.size (); i++) {
        QString ln = fcn[i];
        ln.resize (len, QChar (' '));
        mtx.append (ln);
      }
      APL_value aplv = char_vector (toCString (mtx), "qvis");
      set_var_value (expvar, aplv, "qvis");
      QString cmd = QString ("%1←%2 %3ρ%4")
        .arg (expvar).arg (fcn.size ()).arg (len).arg (expvar);
      AplExec::aplExec (APL_OP_EXEC, cmd, outString, errString);
      cmd = QString ("⎕fx %1").arg (expvar);
      AplExec::aplExec (APL_OP_EXEC, cmd, outString, errString);
      update_screen (errString, outString);
      release_value (aplv, "qvis");
      cmd = QString (")erase %1").arg (expvar);
      AplExec::aplExec (APL_OP_EXEC, cmd, outString, errString);
    }
  }
}

