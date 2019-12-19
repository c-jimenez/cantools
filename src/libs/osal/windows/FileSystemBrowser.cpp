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

#include "FileSystemBrowser.h"

#include <Windows.h>
#include <sstream>


using namespace std;

/** \brief List the files matching a specified extension in a directory */
bool FileSystemBrowser::listFiles(const std::string& dir_path, const std::string& extension, std::vector<std::string>& files)
{
    bool ret = false;

    files.clear();

    WIN32_FIND_DATAA find_file_data = { 0 };
    stringstream dir_base;
    stringstream find_path;
    dir_base << dir_path;
    if ((dir_path.size() != 0) &&
        (dir_path[dir_path.size() - 1] != '\\'))
    {
        dir_base << "\\";
    }
    find_path << dir_base.str() << "*";
    if (extension != "")
    {
        find_path << extension;
    }
    HANDLE find_handle = FindFirstFileA(find_path.str().c_str(), &find_file_data);
    if (find_handle != INVALID_HANDLE_VALUE)
    {
        do
        {
            if ((find_file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
            {
                string file_name = dir_base.str() + find_file_data.cFileName;
                files.push_back(file_name);
            }
        } 
        while (FindNextFileA(find_handle, &find_file_data) != 0);
        FindClose(find_handle);

        ret = true;
    }

    return ret;
}

