//======================================================================================================================
// Project: DoomRunner
//----------------------------------------------------------------------------------------------------------------------
// Author:      Jan Broz (Youda008)
// Description: utilities concerning paths, directories and files
//======================================================================================================================

#include "FileSystemUtils.hpp"

#include "OSUtils.hpp"

#include <QDirIterator>
#include <QFile>
#include <QStringBuilder>
#include <QRegularExpression>
#include <QThread>  // sleep


//======================================================================================================================

static bool tryToWriteFile( const QString & filePath )
{
	QFile file( filePath );
	if (!file.open( QIODevice::WriteOnly ))
	{
		return false;
	}
	file.close();
	file.remove();
	return true;
}

bool isDirectoryWritable( const QString & dirPath )
{
	// Qt does not offer any reliable way to determine if we can write a file into a directory.
	// This is the only working workaround.
	return tryToWriteFile( getPathFromFileName( dirPath, "write_test.txt" ) );
}

QString fixExePath( const QString & exePath )
{
	if (!exePath.contains("/"))  // the file is in the current working directory
	{
		return "./" + exePath;
	}
	return exePath;
}

QString sanitizePath( const QString & path )
{
	QString sanitizedPath = path;
	// Newer engines such as GZDoom 4.x can handle advanced Unicode characters such as emojis,
	// but the old ones are pretty much limited to ASCII, so it's easier to just stick to aS "safe" white-list.
	static const QRegularExpression invalidChars("[^a-zA-Z0-9_ !#$&'\\(\\)+,\\-.;=@\\[\\]\\^~]");
	//sanitizedPath.replace( invalidChars, "#" );
	sanitizedPath.remove( invalidChars );
	return sanitizedPath;
}

QString readWholeFile( const QString & filePath, QByteArray & dest )
{
	QFile file( filePath );
	if (!file.open( QIODevice::ReadOnly ))
	{
		return "Could not open file "%filePath%" for reading: "%file.errorString();
	}

	dest = file.readAll();
	if (file.error() != QFile::NoError)
	{
		return "Error occured while reading a file "%filePath%": "%file.errorString();
	}

	file.close();
	return {};
}

QString updateFileSafely( const QString & origFilePath, const QByteArray & newContent )
{
	// Write to a different file than the original and after it's done and closed, replace the original with the new.
	// This is done to prevent data loss, when the program (or OS) crashes during writing to drive.

	QString newFilePath = origFilePath+".new";
	QFile newFile( newFilePath );
	if (!newFile.open( QIODevice::WriteOnly ))
	{
		return "Could not open file "%newFilePath%" for writing: "%newFile.errorString();
	}

	newFile.write( newContent );
	if (newFile.error() != QFile::NoError)
	{
		return "Could not write to file "%newFilePath%": "%newFile.errorString();
	}

	newFile.close();

	QThread::msleep( 20 );  // desperate attempt to fix user's persisting problem with losing the file due to power-outage

	QFile oldFile( origFilePath );
	if (oldFile.exists())
	{
		if (!oldFile.remove())
		{
			return "Could not delete the previous file "%origFilePath%": "%oldFile.errorString();
		}
	}

	if (!newFile.rename( origFilePath ))
	{
		return "Could not rename the new file "%newFilePath%" back to "%origFilePath%": "%newFile.errorString();
	}

	return {};
}

void traverseDirectory(
	const QString & dir, bool recursively, EntryTypes typesToVisit,
	const PathContext & pathContext, const std::function< void ( const QFileInfo & entry ) > & visitEntry
)
{
	if (dir.isEmpty())
		return;

	QDir dir_( dir );
	if (!dir_.exists())
		return;

	QDirIterator dirIt( dir_ );
	while (dirIt.hasNext())
	{
		QString entryPath = pathContext.convertPath( dirIt.next() );
		QFileInfo entry( entryPath );
		if (entry.isDir())
		{
			QString dirName = dirIt.fileName();  // we need the original entry name including "." and "..", entry is already converted
			if (dirName != "." && dirName != "..")
			{
				if (typesToVisit.isSet( EntryType::DIR ))
					visitEntry( entry );
				if (recursively)
					traverseDirectory( entry.filePath(), recursively, typesToVisit, pathContext, visitEntry );
			}
		}
		else
		{
			if (typesToVisit.isSet( EntryType::FILE ))
				visitEntry( entry );
		}
	}
}
