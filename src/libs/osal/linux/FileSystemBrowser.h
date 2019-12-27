/*
Copyright(c) 2019 Cedric Jimenez

This file is part of cantools.

cantools is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

cantools is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with cantools.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef FILESYSTEMBROWSER_H
#define FILESYSTEMBROWSER_H

#include <string>
#include <vector>

/** \brief Utility class to browse the filesystem */
class FileSystemBrowser
{
    public:


        /** \brief List the files matching a specified extension in a directory */
        static bool listFiles(const std::string& dir_path, const std::string& extension, std::vector<std::string>& files);

};


#endif // FILESYSTEMBROWSER_H
