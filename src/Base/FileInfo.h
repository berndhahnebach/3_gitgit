/***************************************************************************
 *   (c) J�rgen Riegel (juergen.riegel@web.de) 2005                        *   
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License (LGPL)   *
 *   as published by the Free Software Foundation; either version 2 of     *
 *   the License, or (at your option) any later version.                   *
 *   for detail see the LICENCE text file.                                 *
 *                                                                         *
 *   FreeCAD is distributed in the hope that it will be useful,            *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        * 
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with FreeCAD; if not, write to the Free Software        * 
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  *
 *   USA                                                                   *
 *                                                                         *
 *   Juergen Riegel 2002                                                   *
 ***************************************************************************/


#ifndef BASE_FILEINFO_H
#define BASE_FILEINFO_H

#include <string>


namespace Base
{

/** File name unification
  * This class handles everything related to file names
  */
class BaseExport FileInfo
{
public:
    /// Constrction
    FileInfo (const char* FileName="");
    /// set a new file name
    void setFile(const char* name);
    /// set a new file name
    void setFile(const std::string &name){setFile(name.c_str());}
    /// Does the file exist?
    bool exists () const;
    /// Returns the file name, including the path (which may be absolute or relative).
    std::string filePath () const;
    /// Returns the dir path name (which may be absolute or relative).
    std::string dirPath () const;
    /// Returns the name of the file, excluding the path, including the extension.
    std::string fileName () const;
    /// Returns the name of the file, excluding the path and the extension.
    std::string fileNamePure () const;
    /// Convert the path name into a UCS-2 encoded wide string format.
    std::wstring toStdWString() const;
    /** Returns the file's extension name.
     * If complete is TRUE (the default), extension() returns the string of all characters in the file name after (but not including) the first '.' character.
     * If complete is FALSE, extension() returns the string of all characters in the file name after (but not including) the last '.' character.
     * Example:
     *@code
     *  FileInfo fi( "/tmp/archive.tar.gz" );
     *  std::string ext = fi.extension(true);  // ext = "tar.gz"
     *  ext = fi.extension(false);   // ext = "gz"
     *  ext = fi.extension();   // ext = "gz"
     *@endcode
     */
    std::string extension (bool complete = false) const;
    /// checks for a special extension, NOT case sensetive
    bool hasExtension (const char* Ext) const;

    /// checks if the file exist and is readable
    bool isReadable () const;
    /// checks if the file exist and is writable
    bool isWritable () const;
    bool isFile () const;
    bool isDir () const;
    unsigned int size () const;

    /**
     * Creates a directory. Returns TRUE if successful; otherwise returns FALSE.
     */
    bool createDirectory( const char* ) const;

    /// get the path to the dir which is designeded to temp files
    static const char *getTempPath(void);
    /// get a unique File Name in the temp path
    static std::string getTempFileName(void);
    /// delete the file
    bool deleteFile(void);

protected:
    std::string FileName;
};

} //namespace Base


#endif // BASE_FILEINFO_H

