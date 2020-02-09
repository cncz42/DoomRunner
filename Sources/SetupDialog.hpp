//======================================================================================================================
// Project: DoomRunner
//----------------------------------------------------------------------------------------------------------------------
// Author:      Jan Broz (Youda008)
// Created on:  13.5.2019
// Description:
//======================================================================================================================

#ifndef SETUP_DIALOG_INCLUDED
#define SETUP_DIALOG_INCLUDED


#include "Common.hpp"

#include "SharedData.hpp"
#include "ItemModels.hpp"

#include <QDialog>
#include <QFileInfo>

class QDir;
class QLineEdit;
class PathHelper;
class QListView;

namespace Ui {
	class SetupDialog;
}


//======================================================================================================================

class SetupDialog : public QDialog {

	Q_OBJECT

	using thisClass = SetupDialog;

 public:

	explicit SetupDialog( QWidget * parent, bool useAbsolutePaths, const QDir & baseDir, const QList< Engine > & engines,
	                      const QList< IWAD > & iwads, bool iwadListFromDir, const QString & iwadDir, bool iwadSubdirs,
	                      const QString & mapDir, const QString & modDir );
	virtual ~SetupDialog() override;

 private:

	virtual void timerEvent( QTimerEvent * event ) override;

 private slots:

	void manageIWADsManually();
	void manageIWADsAutomatically();

	void browseIWADDir();
	void browseMapDir();
	void browseModDir();

	void changeIWADDir( QString text );
	void changeMapDir( QString text );
	void changeModDir( QString text );

	void toggleIWADSubdirs( bool checked );

	void iwadAdd();
	void iwadDelete();
	void iwadMoveUp();
	void iwadMoveDown();

	void engineAdd();
	void engineDelete();
	void engineMoveUp();
	void engineMoveDown();

	void editEngine( const QModelIndex & index );

	void toggleAbsolutePaths( bool checked );

	void closeDialog();

 signals:



 private: // methods

	void toggleAutoIWADUpdate( bool enabled );
	void browseDir( QString dirPurpose, QLineEdit * targetLine );

 private: // members

	Ui::SetupDialog * ui;

	uint tickCount;

 public: // return values from this dialog

	PathHelper pathHelper;

	// engine info
	QList< Engine > engines;
	ReadOnlyListModel< Engine > engineModel;  ///< read-only view model, list content is changed by buttons

	// IWAD info
	QList< IWAD > iwads;
	ReadOnlyListModel< IWAD > iwadModel;  ///< read-only view model, list content is changed by buttons
	bool iwadListFromDir;
	QString iwadDir;
	bool iwadSubdirs;

	// additional paths
	QString mapDir;
	QString modDir;

};


#endif // SETUP_DIALOG_INCLUDED
