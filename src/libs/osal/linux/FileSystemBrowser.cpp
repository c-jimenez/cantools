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

#include <dirent.h>
#include <string.h>
#include <sstream>


using namespace std;

/** \brief List the files matching a specified extension in a directory */
bool FileSystemBrowser::listFiles(const std::string& dir_path, const std::string& extension, std::vector<std::string>& files)
{
    bool ret = false;

    files.clear();

    struct dirent** file_list;
    stringstream dir_base;
    dir_base << dir_path;
    if ((dir_path.size() != 0) &&
        (dir_path[dir_path.size() - 1] != '/'))
    {
        dir_base << "/";
    }
    int count = scandir(dir_path.c_str(), &file_list, nullptr, alphasort);
    if (count > 0)
    {
        for (int i = 0; i < count; i++)
        {
            size_t filename_len = strlen(file_list[i]->d_name);
            if ((filename_len > extension.size()) &&
                (extension.compare(&file_list[i]->d_name[filename_len - extension.size()]) == 0))
            {
                string file_name = dir_base.str() + file_list[i]->d_name;
                files.push_back(file_name);
            }
        }
        free(file_list);

        ret = true;
    }

    return ret;
}

