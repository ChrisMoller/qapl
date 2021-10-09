
#include "mainwindow.h"

#include "aplexec.h"
#include <apl/libapl.h>

static const QRegularExpression whitespace ("[[:space:]]"); 
static const QColor black = QColor (0, 0, 0);

void MainWindow::processLine (bool suppressOppressOutput, QString text)
{
  QString outString;
  QString errString;
  QString outFile;
  QString outExec;
  bool appendFile = false;

  outFile.clear ();
  outExec.clear ();

  if (text.contains (">>>")) {
    QStringList parts = text.split (">>>");
    appendFile = true;
    text = parts[0].trimmed ();
    if (parts.size () == 2) {
      if (!parts[1].isEmpty ())
	outFile = parts[1].trimmed ();
      else printError ("Output filename can't be empty.");
    }
    else printError ("Output filename must exist.");
  }
  
  if (!appendFile && text.contains (">>")) {
    QStringList parts = text.split (">>");
    text = parts[0].trimmed ();
    if (parts.size () == 2) {
      if (!parts[1].isEmpty ())
	outFile = parts[1].trimmed ();
      else printError ("Output filename can't be empty.");
    }
    else printError ("Output filename must exist.");
  }

  if (text.contains ("|>")) {
    QStringList parts = text.split ("|>");
    text = parts[0].trimmed ();
    if (parts.size () == 2) {
      if (!parts[1].isEmpty ())
	outExec = parts[1].trimmed ();
      else printError ("Piped executable string can't be empty.");
    }
    else printError ("Piped executable string must exist.");
  }
  
  LIBAPL_error rc = AplExec::aplExec (APL_OP_EXEC, text, outString, errString);

  if (suppressOppressOutput) return;
  
  if (rc != LAE_NO_ERROR) {
    QString emsg =
      QString ("APL error %1").arg ((int)rc, 8, 16, QLatin1Char('0'));
    printError (emsg, errString);
  }

  if (!outFile.isEmpty ()) {
    if ((outFile.startsWith ("'") && outFile.endsWith ("'")) ||
	(outFile.startsWith ("\"") && outFile.endsWith ("\""))) {
      outFile.chop (1);
      outFile.remove (0, 1);
    }
    if (!outFile.isEmpty ()) {
      QFile file(outFile);
      if (0 < outString.size ()) {
	QIODevice::OpenMode mode = QIODevice::ReadWrite;
	if (appendFile) mode |= QIODevice::Append;
	if (file.open(mode)) {
	  QTextStream stream(&file);
	  stream << outString;
	  file.close ();
	}
      }
      else file.resize (0);
    }
    else printError ("Output filename can't be null.");
  }
  else if (!outExec.isEmpty ()) {
    /***
	<expr> |> cmd op op op...
	
	or
	  
	<expr> |> z ←[?] cmd op op op...

	or
	  
	<expr> |> z ←[+] 'cmd op op op...'
    ***/

    QString outTarget;
    bool wasQuoted = false;
    bool extended  = false;
    if (outExec.contains ("←")) {
      QStringList parts = outExec.split ("←");
      outTarget = parts[0].trimmed ();
      outExec   = parts[1].trimmed ();
      if (outExec.startsWith ("+")) {
	extended = true;
	outExec.remove (0,1);
	outExec = outExec.trimmed ();
      }
    }
    if (outExec.startsWith ("'") && outExec.endsWith ("'")) {
      outExec.chop (1);
      outExec.remove (0, 1);
      wasQuoted = true;
    }

    if (!outExec.isEmpty ()) {
      QStringList args = parseCl (outExec);
      if (0 < args.size ()) {
	QString exec = args[0];
	args.removeFirst ();
	QProcess *proc = new QProcess ();
	connect (proc,
	 QOverload<QProcess::ProcessError>::of(&QProcess::errorOccurred),
		 [=](
		     QProcess::ProcessError error __attribute__((unused))
		     ) {
		   printError ("Error starting external process.");
		 });
	connect (proc, &QProcess::started,
		 [=]() {
		   if (0 < outString.size ())
		     proc->write (toCString (outString));
		   proc->closeWriteChannel ();
		 });
	connect(proc,
		QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
		[=](
		    int exitCode,
		    QProcess::ExitStatus exitStatus __attribute__((unused))
		    ){
		  QByteArray qby = proc->readAllStandardOutput();
		  if (0 < qby.size ()) {
		    QString qs (qby);
		    if (outTarget.isEmpty ())
		      outputLog->append (qs);
		    else {
		      if (qs.endsWith ("\n")) qs.chop (1);
		      if (wasQuoted) {
			qs.prepend (QChar ('\''));
			qs.append (QChar ('\''));
		      }
		      QString cmd =
			extended
			? QString ("%1←%2,⊂%3")
			.arg (outTarget). arg (exitCode). arg (qs)
			: QString ("%1←%2").arg (outTarget, qs);
		      processLine (false, cmd);
		    }
		  }
		});
	for (int i = 0; i < args.size (); i++) {
	  if (args[i].startsWith ("`") && args[i].endsWith ("`")) {
	    args[i].chop (1);
	    args[i].remove (0, 1);
	    QString os;
	    QString es;
	    LIBAPL_error rc = AplExec::aplExec (APL_OP_EXEC, args[i], os, es);
	    if (rc != LAE_NO_ERROR)
	      printError ("Error expanding argument.");
	    else {
	      if (os.endsWith ("\n")) os.chop (1);
	      if (os.contains (whitespace)) {
		os.prepend (QChar ('"'));
		os.append (QChar ('"'));
	      }
	      args[i] = os;
	    }
	      
	  }
	}
	proc->start (exec, args);
      }
      else printError ("Empty executable string.");
    }


    else printError ("Invalid executable string.");
  }
  else if (outString.size () > 0) {
    outputLog->setTextColor (black);
    outputLog->append (outString);		// not captured
  }

  outputLog->moveCursor (QTextCursor::EndOfBlock, QTextCursor::MoveAnchor);
  outputLog->ensureCursorVisible();
  QScrollBar *sb = outputLog->verticalScrollBar();
  sb->setValue (sb->maximum ());
}
