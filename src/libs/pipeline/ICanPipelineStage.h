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

#ifndef ICANPIPELINESTAGE_H
#define ICANPIPELINESTAGE_H

#include "ICanControllerListener.h"

#include <string>

/** \brief Interface for CAN message pipeline stages implementations */
class ICanPipelineStage : public ICanControllerListener
{
    public:

        /** \brief Destructor */
        virtual ~ICanPipelineStage() {}


        /** \brief Get the stage's name  */
        virtual const std::string& getName() const = 0;

        /** \brief Set the stage's name  */
        virtual void setName(const std::string& name) = 0;

        /** \brief Enable the stage */
        virtual void enable() = 0;

        /** \brief Disable the stage */
        virtual void disable() = 0;

        /** \brief Retrieve stage's state */
        virtual bool isEnabled() const = 0;

        /** \brief Indicate if the stage outputs CAN mesages */
        virtual bool isOutputStage() const = 0;


        /** \brief Add a child pipeline stage */
        virtual void addChild(ICanPipelineStage& child) = 0;

        /** \brief Remove a child pipeline stage */
        virtual void removeChild(ICanPipelineStage& child) = 0;


};


#endif // ICANPIPELINESTAGE_H
